/*
Date: 02 Oct 2019 15:15:10.160223217
File: being.cpp

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
02 Oct 2019 Duncan Camilleri           Initial development
06 Oct 2019 Duncan Camilleri           Removed shortname
24 Nov 2019 Duncan Camilleri           Doable actions converted to ObjRef
25 Nov 2019 Duncan Camilleri           Bias mood in operator= was missing

*/

#include <assert.h>
#include <string.h>
#include <cstdint>
#include <list>
#include <string>
#include "objref.h"
#include "stringlist.h"
#include "serializer.h"
#include "mood.h"
#include "action.h"
#include "being.h"

using namespace std;

// 
// |         o
// |---.,---..,---.,---.
// |   ||---'||   ||   |
// `---'`---'``   '`---|
//                 `---'
// 

//
// Construction
//

Being::Being()
{
}

Being::Being(const Being& being)
{
   *this = being;
}

Being::~Being()
{
   reset();
}

//
// Assignment
//

Being& Being::operator=(const Being& being)
{
   if (&being == this)
      return *this;

   mId = being.mId;
   memcpy(mName, being.mName, 32);
   mBias = being.mBias;
   mMood = being.mMood;
   mDoableActionRefs = being.mDoableActionRefs;
   mImpactActions = being.mImpactActions;

   return *this;
}

Being& Being::operator=(const BeingNode& node)
{
   if (&node == this)
      return *this;

   // Assign to this.
   const Being& b = dynamic_cast<const Being&>(node);
   *this = b;

   // Return.
   return *this;
}

//
// Data
//

void Being::reset()
{
   mId = 0;
   memset(mName, 0, 32);
   mBias.neutralize();
   mMood.neutralize();
   mDoableActionRefs.clear();
   mImpactActions.clear();
}

//
// Naming
//

void Being::name(const uint64_t id, const char* const name)
{
   mId = id;
   strncpy(mName, name, 31);
}

//
// Mood shifting
//

void Being::forceBias(const Mood& bias)
{
   mBias = bias;
}

void Being::forceMood(const Mood& mood)
{
   mMood = mood;
}

// Impact the being's current mood based on the action a.
// Action a has been acted upon the being so the being's emotions are affected.
// This may also have an impact on the being's bias mood if the action is
// significant enough  or has happened too frequently.
void Being::impact(const Action& a)
{
   mMood += a.getReactions();

   // Locate ActionQty with a.name and if found increment quantity, otherwise
   // add a new ActionQty item for this action.
   list<ActionQty>::iterator i = mImpactActions.begin();
   for (; i != mImpactActions.end(); ++i) {
      const Action* pAction = (*i).getAction();
      if (a.id() == pAction->id()) {
         (*i)++;
         return;
      }
   }

   // TODO: Logic to impact the bias node as well.

   // Action has not been added as a previous impact action yet.
   ActionQty qty(a);
   mImpactActions.push_back(qty);
}

// Actions
bool Being::supportAction(const Action& a)
{
   try {
      // Shouldn't allow duplicate actions.
      for (ObjRef<const Action>& actionref : mDoableActionRefs) {
         if (actionref.getId() == a.id()) {
            // Treat action as already supported.
            return true;
         }
      }

      // Action doesn't exist. Push it on to the list.
      ObjRef<const Action> actionref(&a, a.id());
      mDoableActionRefs.push_back(actionref);
   } catch (exception& e) {
      // push_back failures can happen.
      return false;
   }

   // Done.
   return true;
}

// 
// |         o          ,   .         |
// |---.,---..,---.,---.|\  |,---.,---|,---.
// |   ||---'||   ||   || \ ||   ||   ||---'
// `---'`---'``   '`---|`  `'`---'`---'`---'
//                 `---'
// 

BeingNode::BeingNode(Node& node)
: mpNode(&node)
{
}

BeingNode::BeingNode(const Being& being, Node& node)
: mpNode(&node)
{
   *this = being;
}

//
// Assignment
//

BeingNode& BeingNode::operator=(const Being& being)
{
   Being& target = dynamic_cast<Being&>(*this);
   target = being;

   return *this;
}

//
// From node
//

// Reads a node into being.
// Format of node is as follows:
// <being>name
//    <id>id</id>
//    <mood>bias...</mood>
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
// </being>
bool BeingNode::fromNode()
{
   if (!mpNode) return false;

   // Name of node should be 'being' and should have a value.
   const char* const pName = mpNode->getName();
   const char* const pValue = mpNode->getValue();
   if (!pName || !pValue) return false;
   if (strncmp(pName, "being", 5) != 0) return false;

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
      } else if (strncmp(pValue, "bias", 4) == 0) {
         MoodNode n(child);
         if (!n.fromNode())
            return fail();

         // Demote.
         mBias = n;
      } else if (strncmp(pName, "doableactions", 13) == 0) {
         if (!fromDoableActionsNode(child)) {
            return fail();
         }
      } else if (strncmp(pName, "impactactions", 13) == 0) {
         if (!fromImpactActionsNode(child)) {
            return fail();
         }
      }
   }

   // Complete.
   return true;
}

// To node
// Writes a node from being.
// Format of node is as described in BeingNode::fromNode().
bool BeingNode::toNode()
{
   if (!mpNode) return false;
   mpNode->empty();

   // Set value first.
   if (!mpNode->setValue("being", mName)) return false;

   // Fail function.
   auto fail = [&]() -> bool {
      mpNode->empty();
      return false;
   };

   // Spawn children and set their value.
   Node* pId = mpNode->spawnChild();
   Node* pBias = mpNode->spawnChild();
   Node* pMood = mpNode->spawnChild();
   Node* pDoableActions = mpNode->spawnChild();
   Node* pImpactActions = mpNode->spawnChild();
   if (nullptr == pId || nullptr == pBias || nullptr == pMood) return fail();
   if (nullptr == pDoableActions || nullptr == pImpactActions) return fail();

   // Set Id.
   if (!pId->setValue("id", mId)) {
      return fail();
   }

   // Bias mood.
   MoodNode mnBias(mBias, *pBias);
   if (!mnBias.toNode("bias")) {
      return fail();
   }

   MoodNode mnMood(mMood, *pMood);
   if (!mnMood.toNode("currentmood")) {
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
   if (!pImpactActions->setValue("impactactions", "")) {
      return fail();
   }

   for (ActionQty& aqty : mImpactActions) {
      Node* pChild = pImpactActions->spawnChild();
      if (!pChild) return fail();

      // Store action node.
      ActionQtyNode an(aqty, *pChild);
      if (!an.toNode())
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
bool BeingNode::fromDoableActionsNode(Node& node)
{
   try {
      std::list<Node>& children = node.getChildren();
      for (Node& child : children) {
         const char* const pName = child.getName();
         const char* const pValue = child.getValue();
         if (!pName) continue;

         if (strncmp(pName, "actionid", 8) == 0) {
            ObjRef<const Action> actionref(child.getUint64(pValue));
            mDoableActionRefs.push_back(actionref);
         }
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
bool BeingNode::fromImpactActionsNode(Node& node)
{
   try {
      std::list<Node>& children = node.getChildren();
      for (Node& child : children) {
         // Ensure any invalid nodes are omitted first.
         const char* const pName = child.getName();
         const char* const pValue = child.getValue();
         if (!pName) continue;

         if (strncmp(pName, "actionqty", 9) == 0) {
            // Read a normal actionqty here.
            ActionQtyNode an(child);
            if (!an.fromNode())
               return false;

            // Loaded. Demote to list.
            mImpactActions.push_back(an);
         }
      }
   } catch (exception& e) {
      return false;
   }

   return true;
}

