/*
Date: 19 Oct 2019 08:07:42.546097875
File: behavefactory.cpp

Copyright Notice
This document is protected by the GNU General Public License v3.0.

This allows for commercial use, modification, distribution, patent and private
use of this software only when the GNU General Public License v3.0 and this
copyright notice are both attached in their original form.

For developer and author protection, the GPL clearly explains that there is no
warranty for this free software and that any source code alterations are to be
shown clearly to identify the original author as well as any subsequent changes
made and by who.

For any questions or ideas, please contact:
github:  https://github(dot)com/dnc77
email:   dnc77(at)hotmail(dot)com
web:     http://www(dot)dnc77(dot)com

Copyright (C) 2000-2025 Duncan Camilleri, All rights reserved.
End of Copyright Notice

Purpose: Container and instantiator for all Behave related objects.

Version control
05 Oct 2019 Duncan Camilleri           Initial development
19 Oct 2019 Duncan Camilleri           Moved from bwfactory.h
02 Nov 2019 Duncan Camilleri           Introduced node
08 Nov 2019 Duncan Camilleri           Basic serialization complete

*/

#include <exception>
#include <cstdint>
#include <string>
#include <list>
#include <map>
#include <algorithm>
#include <sys/time.h>
#include <string.h>
#include <stdio.h>

#include <helpers.h>
#include <cycbuf.h>
#include "objref.h"
#include "stringlist.h"
#include "serializer.h"
#include "mood.h"
#include "action.h"
#include "being.h"
#include "gathering.h"
#include "behavefactory.h"

using namespace std;

//
// CONSTRUCTION
//
BehaveFactory::BehaveFactory()
{
}

BehaveFactory::~BehaveFactory()
{
}

//
// INSTANTIATE
//

Action* BehaveFactory::spawnAction(const char* const name,
   const Mood& triggers, const Mood& reactions)
{
   try {
      // Create an action and return it.
      Action a(triggers, reactions);
      a.name(++sidAction, name);
      mActions.push_back(a);

      return &mActions.back();
   } catch (exception& e) {
      // push_back failure.
      return nullptr;
   }
}

Being* BehaveFactory::spawnBeing(const char* const name)
{
   try {
      // Create a being and return it.
      Being b;
      b.name(++sidBeing, name);
      mBeings.push_back(b);

      return &mBeings.back();
   } catch (exception& e) {
      // push_back failure.
      return nullptr;
   }
}

Gathering* BehaveFactory::spawnGathering(const char* const name)
{
   try {
      // Create a gathering and return it.
      Gathering g;
      g.name(++sidGathering, name);
      mGatherings.push_back(g);

      return &mGatherings.back();
   } catch (exception& e) {
      // push_back failure.
      return nullptr;
   }
}

//
// SEARCH
//

// Will find an action that has mId = id.
// If found, a will be populated with that action and true is returned.
// Otherwise false is returned.
bool BehaveFactory::find(Action& a, const uint64_t id)
{
   // Compares action's id with passed id.
   auto compare = [&](const Action& iter) -> bool {
      return iter.id() == id;
   };

   // Find object.
   list<Action>::const_iterator i = std::find_if(
      mActions.begin(), mActions.end(), compare);

   if (i == mActions.end()) return false;

   // Found!
   a = (*i);
   return true;
}

// Will find a being that has mId = id.
// If found, b will be populated with that being and true is returned.
// Otherwise false is returned.
bool BehaveFactory::find(Being& b, const uint64_t id)
{
   // Compares action's id with passed id.
   auto compare = [&](const Being& iter) -> bool {
      return iter.id() == id;
   };

   // Find object.
   list<Being>::const_iterator i = std::find_if(
      mBeings.begin(), mBeings.end(), compare);

   if (i == mBeings.end()) return false;

   // Found!
   b = (*i);
   return true;   
}

// Will find a gathering that has mId = id.
// If found, g will be populated with that being and true is returned.
// Otherwise false is returned.
bool BehaveFactory::find(Gathering& g, const uint64_t id)
{
   // Compares action's id with passed id.
   auto compare = [&](const Gathering& iter) -> bool {
      return iter.id() == id;
   };

   // Find object.
   list<Gathering>::const_iterator i = std::find_if(
      mGatherings.begin(), mGatherings.end(), compare);

   if (i == mGatherings.end()) return false;

   // Found!
   g = (*i);
   return true;   
}

//
// SERIALIZATION
//

// Loads a factory from the specified file.
bool BehaveFactory::load(const char* const filename)
{
   // Read xml into a Node tree as a Node map.
   Node root(mNodeData);
   if (!Node::fromFile(root, filename)) {
      return false;
   }

   // Root is always <behavefactory>.
   if (strncmp(root.getName(), "behavefactory", 13) != 0)
      return false;

   // We have a behave factory loaded. Get each child.
   list<Node>& children = root.getChildren();
   for (Node& child : children) {
      uint64_t* pnSidUpdate = nullptr;
      uint64_t nid = 0;

      // Each child node could only be one of
      // <action>, <being>, or <gathering>.
      if (strncmp(child.getName(), "action", 6) == 0) {
         // Read node - if something goes wrong, break out.
         // (Logging ideal at this level).
         if (!loadActionNode(child)) {
            return false;
         }
      } else if (strncmp(child.getName(), "being", 5) == 0) {
         if (!loadBeingNode(child)) {
            return false;
         }
      } else if (strncmp(child.getName(), "gathering", 9) == 0) {
         if (!loadGatheringNode(child)) {
            return false;
         }
      }
   }
}


bool BehaveFactory::loadActionNode(Node& child)
{
   try {
      ActionNode action(child);
      if (!action.fromNode()) {
         return false;
      }

      // Add action - this should create a new action in the
      // list and not reference the current action node.
      // See operator overloading in Action.
      mActions.push_back(const_cast<const ActionNode&>(action));

      // Update sid.
      if (sidAction < action.id())
         sidAction = action.id();
   } catch (exception& e) {
      // Catch push_back exceptions (if any).
      return false;
   }

   // Done.
   return true;
}

bool BehaveFactory::loadBeingNode(Node& child)
{
   try {
      BeingNode being(child);
      if (!being.fromNode()) {
         return false;
      }

      // Add being - this should create a new being in the
      // list and not reference the current being node.
      // See operator overloading in Being.
      mBeings.push_back(const_cast<const BeingNode&>(being));

      // Update sid.
      if (sidBeing < being.id())
         sidBeing = being.id();
   } catch (exception& e) {
      // Catch push_back exceptions (if any).
      return false;
   }

   // Done.
   return true;
}
bool BehaveFactory::loadGatheringNode(Node& child)
{
   try {
      GatheringNode gathering(child);
      if (!gathering.fromNode()) {
         return false;
      }

      // Add action - this should create a new gathering in the
      // list and not reference the current gathering node.
      // See operator overloading in Gathering.
      mGatherings.push_back(const_cast<const GatheringNode&>(gathering));

      // Update sid.
      if (sidGathering < gathering.id())
         sidGathering = gathering.id();
   } catch (exception& e) {
      // Catch push_back exceptions (if any).
      return false;
   }

   // Done.
   return true;
}

bool BehaveFactory::save(const char* const filename)
{
   // First clear the local node data and assign to a root node.
   mNodeData.reset();
   Node root(mNodeData);

   // Set behavefactory root node.
   if (!root.setValue("behavefactory", "")) {
      return false;
   }

   // Set fail function.
   auto fail = [&]() -> bool {
      return false;
   };

   // Go through each action and serialize into the node.
   for (Action& a : mActions) {
      // Spawn child node.
      Node* pChild = root.spawnChild();
      if (!pChild) {
         return fail();
      }

      // Create an action node and populate.
      ActionNode an(a, *pChild);
      if (!an.toNode()) {
         return fail();
      }
   }

   // Go through each being and serialize into the node.
   for (Being& b : mBeings) {
      // Spawn child node.
      Node* pChild = root.spawnChild();
      if (!pChild) {
         return fail();
      }

      // Create a being node and populate.
      BeingNode bn(b, *pChild);
      if (!bn.toNode()) {
         return fail();
      }
   }

   // Go through each gathering and serialize into the node.
   for (Gathering& g : mGatherings) {
      // Spawn child node.
      Node* pChild = root.spawnChild();
      if (!pChild) {
         return fail();
      }

      // Create a gathering node and populate.
      GatheringNode gn(g, *pChild);
      if (!gn.toNode()) {
         return fail();
      }
   }

   // Save file.
   if (!root.toFile(filename)) {
      return fail();
   }

   // File saved.
   return true;
}

//
// INFO
//

void BehaveFactory::toStdout()
{
   auto moodout = [&](const Mood& m) {
      printf("mood: joy: %0.4f, trust: %0.4f, fear: %0.4f, surprise: %0.4f, "
         "sadness: %0.4f, disgust: %0.4f, anger: %0.4f, anticipation: %0.4f",
         m.get(Mood::joy), m.get(Mood::trust),
         m.get(Mood::fear), m.get(Mood::surprise),
         m.get(Mood::sadness), m.get(Mood::disgust),
         m.get(Mood::anger), m.get(Mood::anticipation)
      );
   };

   printf("actions:\n");
   for (Action& a : mActions) {
      printf("   action: %s\n", a.name());
      printf("      triggers: ");
      moodout(a.getTriggers());
      printf("\n");
      printf("      reactions: ");
      moodout(a.getReactions());
      printf("\n");
   }
}


//
// DESTROY
//

void BehaveFactory::destroy()
{
   mGatherings.clear();
   mBeings.clear();
   mActions.clear();
   sidGathering = 0;
   sidBeing = 0;
   sidAction = 0;
}
