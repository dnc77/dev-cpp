/*
Date: 04 Oct 2019 06:47:38.167959370
File: gathering.cpp

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

Purpose: Defines a gathering between two or more beings.

Version control
03 Oct 2019 Duncan Camilleri           Initial development
04 Oct 2019 Duncan Camilleri           Transformed from relation.cpp
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

using namespace std;

// Declare being reference as it's being used here.
template class ObjRef<Being>;

// 
//           |    |              o
// ,---.,---.|--- |---.,---.,---..,---.,---.
// |   |,---||    |   ||---'|    ||   ||   |
// `---|`---^`---'`   '`---'`    ``   '`---|
// `---'                               `---'
// 
// A gathering is treated as another entity in it's own right so it
// will take the properties of a being.
// A gathering has a mood which defines the emotions involved in
// as one group of beings. A gathering is between two or more seperate beings.
// Actions can also impact a gathering.
// Beings together can perform actions as one gather impacting one or more
// further entities.
//

Gathering::Gathering()
{
   mBeingRefs.clear();
}

Gathering::Gathering(const Gathering& gathering)
{
   *this = gathering;
}

Gathering::~Gathering()
{
   mBeingRefs.clear();
}

//
// Assignments
//

Gathering& Gathering::operator=(const Gathering& gathering)
{
   if (&gathering == this)
      return *this;

   // Copy references.
   mBeingRefs = gathering.mBeingRefs;

   // Copy super.
   Being& bThis = dynamic_cast<Being&>(*this);
   const Being& bGathering = dynamic_cast<const Being&>(gathering);

   // Assign being (TODO: Test).
   bThis = bGathering;

   // Done.
   return *this;
}

//
// Data
//

void Gathering::reset()
{
   Being::reset();
   mBeingRefs.clear();
}


// 
//           |    |              o                        |
// ,---.,---.|--- |---.,---.,---..,---.,---.,---.,---.,---|,---.
// |   |,---||    |   ||---'|    ||   ||   ||   ||   ||   ||---'
// `---|`---^`---'`   '`---'`    ``   '`---|`   '`---'`---'`---'
// `---'                               `---'
// 

GatheringNode::GatheringNode(Node& node)
: mpNode(&node)
{
}

GatheringNode::GatheringNode(const Gathering& gathering, Node& node)
: mpNode(&node)
{
   *this = gathering;
}

//
// Assignment
//

GatheringNode& GatheringNode::operator=(const Gathering& gathering)
{
   Gathering& target = dynamic_cast<Gathering&>(*this);
   target = gathering;

   return *this;
}

//
// From node
//

// Reads a node into being.
// Format of node is as follows:
// <gathering>name
//    <id>id</id>
//    <mood>currentmood moodcontent</mood>
//    <doableactions>
//       <actionid>id</actionid>...<actionid>id</actionid>
//    </doableactions>
//    <impactactions>
//       <actionqty>
//          <actionid>id</actionid>
//          <quantity>qty</quantity>
//       </actionqty>
//       ...
//       <actionqty>
//          <actionid>id</actionid>
//          <quantity>qty</quantity>
//       </actionqty>
//    </impactactions>
//    <members>
//       <beingid>id</beingid>
//       ...
//       <beingid>id</beingid>
//    </members>
// </gathering>
bool GatheringNode::fromNode()
{
   if (!mpNode) return false;

   // Name of node should be 'gathering' and should have a value.
   const char* const pName = mpNode->getName();
   const char* const pValue = mpNode->getValue();
   if (!pName || !pValue) return false;
   if (strncmp(pName, "gathering", 9) != 0) return false;

   reset();

   // Set failure function in case things go wrong.
   auto fail = [&]() -> bool {
      reset();
      return false;
   };

   // Get value as being name.
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
      } else if (strncmp(pValue, "currentmood", 11) == 0) {
         MoodNode n(child);
         if (!n.fromNode())
            return fail();

         // Demote.
         mMood = n;
      } else if (strncmp(pName, "doableactions", 13) == 0) {
         if (!fromDoableActionsNode(child))
            return fail();
      } else if (strncmp(pName, "impactactions", 13) == 0) {
         if (!fromImpactActionsNode(child)) {
            return fail();
         }
      } else if (strncmp(pName, "beingid", 7) == 0) {
         try {
            ObjRef<Being> beingref(child.getUint64(pValue));
            mBeingRefs.push_back(beingref);
         } catch (std::exception& e) {
            return fail();
         }
      }
   }

   // Complete.
   return true;
}

// To node
// Writes a node from gathering.
// Format of node is as described in GatheringNode::fromNode().
bool GatheringNode::toNode()
{
   if (!mpNode) return false;
   mpNode->empty();

   // Set value first.
   if (!mpNode->setValue("gathering", mName)) return false;

   // Fail function.
   auto fail = [&]() -> bool {
      mpNode->empty();
      return false;
   };

   // Spawn children and set their value.
   Node* pId = mpNode->spawnChild();
   Node* pMood = mpNode->spawnChild();
   Node* pDoableActions = mpNode->spawnChild();
   Node* pImpactActions = mpNode->spawnChild();
   Node* pMembers = mpNode->spawnChild();
   if (nullptr == pId || nullptr == pMood) return fail();
   if (nullptr == pDoableActions || nullptr == pImpactActions) return fail();
   if (nullptr == pMembers) return fail();

   // Set Id and Mood.
   if (!pId->setValue("id", mId)) {
      return fail();
   }
   MoodNode mnMood(mMood, *pMood);
   if (!mnMood.toNode()) {
      return fail();
   }

   // Doable actions.
   if (!pDoableActions->setValue("doableactions", "")) {
      return fail();
   }

   for (ObjRef<const Action>& actionref : mDoableActionRefs) {
      // Create a node for an object reference.
      Node* pChild = pDoableActions->spawnChild();
      if (!pChild) return fail();

      // Store action object reference only.
      if (!pChild->setValue("actionid", actionref.getId())) {
         return fail();
      }
   }

   // Impact actions.
   for (ActionQty& aqty : mImpactActions) {
      Node* pChild = pImpactActions->spawnChild();
      if (!pChild) return fail();

      // Store actionqty node.
      ActionQtyNode an(aqty, *pChild);
      if (!an.toNode())
         return fail();
   }

   // Members.
   for (ObjRef<Being>& b : mBeingRefs) {
      Node* pChild = pMembers->spawnChild();
      if (!pChild) return fail();

      // Store being node.
      if (!pChild->setValue("beingid", b.getId()))
         return fail();
   }

   // Node created.
   return true;
}

//
// From node privates
//

// <doableactions>
//    <actionid>id</actionid>...<actionid>id</actionid>
// </doableactions>
bool GatheringNode::fromDoableActionsNode(Node& node)
{
   try {
      std::list<Node>& children = node.getChildren();
      for (Node& child : children) {
         ObjRef<const Action> actionref(child.getUint64("id"));
         mDoableActionRefs.push_back(actionref);
      }
   } catch (std::exception& e) {
      return false;
   }

   // Done.
   return true;
}

// Reads impact actions node with a list of actionqty objects as follows:
// <impactactions>
//    <actionqty>
//       <actionid>id</actionid>
//       <quantity>qty</quantity>
//    </actionqty>
//    ...
//    <actionqty>
//       <actionid>id</actionid>
//       <quantity>qty</quantity>
//    </actionqty>
// </impactactions>
bool GatheringNode::fromImpactActionsNode(Node& node)
{
   try {
      std::list<Node>& children = mpNode->getChildren();
      for (Node& child : children) {
         // Read a normal actionqty here.
         ActionQtyNode an(child);
         if (!an.fromNode())
            return false;

         // Loaded. Demote to list.
         mImpactActions.push_back(an);
      }
   } catch (exception& e) {
      return false;
   }

   return true;
}
