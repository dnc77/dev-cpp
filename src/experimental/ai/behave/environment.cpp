/*
Date: 26 Nov 2019 11:25:12.848372225
File: environment.cpp

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

Purpose:

Version control
26 Nov 2019 Duncan Camilleri           Initial development
*/

#include <memory.h>
#include <list>
#include <string>
#include <cstdint>
#include "objref.h"
#include "stringlist.h"
#include "serializer.h"
#include "mood.h"
#include "action.h"
#include "being.h"
#include "gathering.h"
#include "environment.h"

using namespace std;

// 
//                 o                              |
// ,---.,---..    ,.,---.,---.,---.,-.-.,---.,---.|---
// |---'|   | \  / ||    |   ||   || | ||---'|   ||
// `---'`   '  `'  ``    `---'`   '` ' '`---'`   '`---'
// 
// An environment is a space in BehaveWorld within which beings can interact
// with each other. Beings cannot interact with each other unless they are
// not in the same environment.
// Some examples of environments can be: a room, a football ground, a park,
// a phone call, a lift, a train etc...
// Some actions can be performed in an environment and some actions cannot.
//

//
// Construction
//

Environment::Environment()
{
   reset();
}

Environment::Environment(const Environment& env)
{
   *this = env;
}

Environment::Environment(Environment&& env)
{
   *this = env;
}

Environment::~Environment()
{
   reset();
}

//
// Assignments
//

Environment& Environment::operator=(const Environment& env)
{
   if (&env == this)
      return *this;

   mId = env.mId;
   memcpy(mName, env.mName, 32);
   mActionRefs = env.mActionRefs;
   mBeingRefs = env.mBeingRefs;
   mGatheringRefs = env.mGatheringRefs;

   // Done.
   return *this;
}

Environment& Environment::operator=(Environment&& env)
{
   if (&env == this)
      return *this;

   *this = const_cast<const Environment&>(env);
   env.reset();

   // Done.
   return *this;
}

Environment& Environment::operator=(const EnvironmentNode& node)
{
   if (&node == this)
      return *this;

   // Assign to this.
   const Environment& e = dynamic_cast<const Environment&>(node);
   *this = e;

   // Return.
   return *this;
}

//
// Data
//

void Environment::reset()
{
   mId = 0;
   memset(mName, 0, 32);
   mActionRefs.clear();
   mBeingRefs.clear();
   mGatheringRefs.clear();
}

//
// Naming
//

void Environment::name(const uint64_t id, const char* const name)
{
   mId = id;
   strncpy(mName, name, 31);
}

// 
//                 o                              |    ,   .         |
// ,---.,---..    ,.,---.,---.,---.,-.-.,---.,---.|--- |\  |,---.,---|,---.
// |---'|   | \  / ||    |   ||   || | ||---'|   ||    | \ ||   ||   ||---'
// `---'`   '  `'  ``    `---'`   '` ' '`---'`   '`---'`  `'`---'`---'`---'
// 
// A serializable environment that implements serialization functionality on top
// of Environment.
//

// Construction
EnvironmentNode::EnvironmentNode(Node& node)
: mpNode(&node)
{

}

EnvironmentNode::EnvironmentNode(const Environment& env, Node& node)
: mpNode(&node)
{
   *this = env;
}

//
// Assignment
//

EnvironmentNode& EnvironmentNode::operator=(const Environment& env)
{
   Environment& target = dynamic_cast<Environment&>(*this);
   target = env;

   return *this;
}

//
// From node
//
// Reads a node into environment.
// Format of node is as follows:
// <env>name
//    <id>id</id>
//    <possibleactions>
//       <actionid>id</actionid>...<actionid>id</actionid>
//    </possibleactions>
//    <beings>
//       <beingid>id</beingid>...<beingid>id</beingid>
//    </beings>
//    <gatherings>
//       <gatheringid>id</gatheringid>...<gatheringid>id</gatheringid>
//    </gatherings>
// </env>

bool EnvironmentNode::fromNode()
{
   if (!mpNode) return false;

   // Name of node should be 'env' and should have a value.
   const char* const pName = mpNode->getName();
   const char* const pValue = mpNode->getValue();
   if (!pName || !pValue) return false;
   if (strncmp(pName, "env", 3) != 0) return false;

   reset();

   // Set failure function in case things go wrong.
   auto fail = [&]() -> bool {
      reset();
      return false;
   };

   // Get value as name.
   strncpy(mName, pValue, 31);

   // Process simple children here but complex children will be processed
   // through another function call.
   std::list<Node>& children = mpNode->getChildren();
   for (Node& child : children) {
      const char* const pName = child.getName();
      const char* const pValue = child.getValue();
      if (!pName) continue;

      if (strncmp(pName, "id", 2) == 0) {
         // id.
         if (!pValue) return fail();
         mId = child.getUint64(pValue);
      } else if (strncmp(pName, "possibleactions", 15) == 0) {
         if (!fromPossibleActionsNode(child)) {
            return fail();
         }
      } else if (strncmp(pName, "beings", 6) == 0) {
         if (!fromBeingsNode(child)) {
            return fail();
         }
      } else if (strncmp(pName, "gatherings", 10) == 0) {
         if (!fromGatheringsNode(child)) {
            return fail();
         }
      }
   }

   // Complete.
   return true;
}

// To node
// Writes a node from environment.
// Format of node is as described in EnvironmentNode::fromNode().
bool EnvironmentNode::toNode()
{
   if (!mpNode) return false;
   mpNode->empty();

   // Set value first.
   if (!mpNode->setValue("env", mName)) return false;

   // Fail function.
   auto fail = [&]() -> bool {
      mpNode->empty();
      return false;
   };

   // Spawn children and set their value.
   Node* pId = mpNode->spawnChild();
   Node* pPossibleActions = mpNode->spawnChild();
   Node* pBeings = mpNode->spawnChild();
   Node* pGatherings = mpNode->spawnChild();
   if (nullptr == pId || nullptr == pPossibleActions) return fail();
   if (nullptr == pBeings || nullptr == pGatherings) return fail();

   // Set Id.
   if (!pId->setValue("id", mId)) {
      return fail();
   }

   // Doable actions.
   if (!pPossibleActions->setValue("possibleactions", "")) {
      return fail();
   }

   for (ObjRef<const Action>& actionref : mActionRefs) {
      // Create a node for an object reference.
      Node* pChild = pPossibleActions->spawnChild();
      if (!pChild) return fail();

      // Store action object reference only.
      if (!pChild->setValue("actionid", actionref.getId())) {
         return fail();
      }
   }

   // Beings.
   if (!pBeings->setValue("beings", "")) {
      return fail();
   }

   for (ObjRef<const Being>& beingref : mBeingRefs) {
      // Create a node for an object reference.
      Node* pChild = pBeings->spawnChild();
      if (!pChild) return fail();

      // Store being object reference only.
      if (!pChild->setValue("beingid", beingref.getId())) {
         return fail();
      }
   }

   // Gatherings.
   if (!pGatherings->setValue("gatherings", "")) {
      return fail();
   }

   for (ObjRef<const Gathering>& gatheringref : mGatheringRefs) {
      // Create a node for an object reference.
      Node* pChild = pGatherings->spawnChild();
      if (!pChild) return fail();

      // Store gathering object reference only.
      if (!pChild->setValue("gatheringid", gatheringref.getId())) {
         return fail();
      }
   }

   // Node created.
   return true;
}

//
// From node privates
//

// <possibleactions>
//    <actionid>id</actionid>...<actionid>id</actionid>
// </possibleactions>
bool EnvironmentNode::fromPossibleActionsNode(Node& child)
{
   try {
      std::list<Node>& children = child.getChildren();
      for (Node& child : children) {
         const char* const pName = child.getName();
         const char* const pValue = child.getValue();
         if (!pName) continue;

         if (strncmp(pName, "actionid", 8) == 0) {
            ObjRef<const Action> actionref(child.getUint64(pValue));
            mActionRefs.push_back(actionref);
         }
      }
   } catch (std::exception& e) {
      return false;
   }

   // Done.
   return true;
}

// <beings>
//    <beingid>id</beingid>...<beingid>id</beingid>
// </beings>
bool EnvironmentNode::fromBeingsNode(Node& child)
{
   try {
      std::list<Node>& children = child.getChildren();
      for (Node& child : children) {
         const char* const pName = child.getName();
         const char* const pValue = child.getValue();
         if (!pName) continue;

         if (strncmp(pName, "beingid", 7) == 0) {
            ObjRef<const Being> beingref(child.getUint64(pValue));
            mBeingRefs.push_back(beingref);
         }
      }
   } catch (std::exception& e) {
      return false;
   }

   // Done.
   return true;
}

// <gatherings>
//    <gatheringid>id</gatheringid>...<gatheringid>id</gatheringid>
// </gatherings>
bool EnvironmentNode::fromGatheringsNode(Node& child)
{
   try {
      std::list<Node>& children = child.getChildren();
      for (Node& child : children) {
         const char* const pName = child.getName();
         const char* const pValue = child.getValue();
         if (!pName) continue;

         if (strncmp(pName, "gatheringid", 7) == 0) {
            ObjRef<const Gathering> gatheringref(child.getUint64(pValue));
            mGatheringRefs.push_back(gatheringref);
         }
      }
   } catch (std::exception& e) {
      return false;
   }

   // Done.
   return true;
}
