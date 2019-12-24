/*
Date: 04 Nov 2019 14:37:44.733536989
File: 

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

Purpose: Defines an action that follows on from Plutchik's model of emotions
         modelled in mood.h. As a result requires mood.h.
         An action has a number of emotions and their corresponding intensity
         levels at which point it will trigger causing a reaction onto something
         which will in turn affect it's mood.
         
         An object has a mood which is affected by reactions to it.

         An action is something that may get triggered on the basis of some
         emotional change and such action can also have an impact or
         emotional reaction on one or more recipient.

Version control
02 Oct 2019 Duncan Camilleri           Initial development
06 Oct 2019 Duncan Camilleri           Removed shortname
03 Nov 2019 Duncan Camilleri           Introduced private constructor
04 Nov 2019 Duncan Camilleri           from() of ActionNode and ActionQtyNode
25 Nov 2019 Duncan Camilleri           ActionQty now uses ObjRef
05 Dec 2019 Duncan Camilleri           Added relationship requirement for action
06 Dec 2019 Duncan Camilleri           Differentiate actor/recipient reactions
16 Dec 2019 Duncan Camilleri           Introduced 'forgesRel'
18 Dec 2019 Duncan Camilleri           Name assignment with nullptr supported
20 Dec 2019 Duncan Camilleri           Actor terminology changed to instigator

*/

#include <memory.h>
#include <string>
#include <cstdint>
#include <list>
#include "objref.h"
#include "stringlist.h"
#include "serializer.h"
#include "mood.h"
#include "action.h"

using namespace std;

// 
// ,---.     |    o
// |---|,---.|--- .,---.,---.
// |   ||    |    ||   ||   |
// `   '`---'`---'``---'`   '
// 

Action::Action()
: mId(0)
{
   reset();
}

Action::Action(const Action& action)
{
   *this = action;
}

Action::Action(const Mood& triggers,
   const Mood& instigateReactions, const Mood& recipientReactions)
: mId(0), mTriggers(triggers), mInstigateReactions(instigateReactions),
   mRecipientReactions(recipientReactions)
{
   memset(mName, 0, 32);
   mForgesRel = false;
   mRequireRel = false;
}

Action::~Action()
{
   reset();
}

//
// Assignment
//

Action& Action::operator=(const Action& action)
{
   if (&action == this)
      return *this;

   mId = action.mId;
   memcpy(mName, action.mName, 32);
   mTriggers = action.mTriggers;
   mInstigateReactions = action.mInstigateReactions;
   mRecipientReactions = action.mRecipientReactions;
   mForgesRel = action.mForgesRel;
   mRequireRel = action.mRequireRel;
   mChainResponses = action.mChainResponses;

   return *this;
}

Action& Action::operator=(const ActionNode& node)
{
   if (&node == this)
      return *this;

   // Assign to this.
   const Action& a = dynamic_cast<const Action&>(node);
   *this = a;

   // Return.
   return *this;
}

void Action::reset()
{
   memset(mName, 0, 32);
   mTriggers.neutralize();
   mInstigateReactions.neutralize();
   mRecipientReactions.neutralize();
   mForgesRel = false;
   mRequireRel = false;
   mChainResponses.clear();
}

//
// Naming
//

void Action::name(const uint64_t id, const char* const name)
{
   mId = id;
   if (name == nullptr) {
      memset(mName, 0, 32);
   } else {
      strncpy(mName, name, 31);
   }
}

//
// Accessors
//

const std::list<ObjRef<const Action>>& Action::getChainResponses() const
{
   return mChainResponses;
}

// 
// ,---.     |    o          ,   .         |
// |---|,---.|--- .,---.,---.|\  |,---.,---|,---.
// |   ||    |    ||   ||   || \ ||   ||   ||---'
// `   '`---'`---'``---'`   '`  `'`---'`---'`---'
// 

ActionNode::ActionNode(Node& node)
: mpNode(&node)
{
}

ActionNode::ActionNode(const Action& action, Node& node)
: mpNode(&node)
{
   *this = action;
}

ActionNode::~ActionNode()
{
   mpNode = nullptr;
}

//
// Assignment
//

ActionNode& ActionNode::operator=(const Action& action)
{
   Action& target = dynamic_cast<Action&>(*this);
   target = action;

   return *this;
}

//
// From node
//

// Reads a node into action.
// Format of node is as follows:
// <action>name
//    <id>id</id>
//    <mood>triggers...</mood>
//    <mood>instigatereactions...</mood>
//    <mood>recipientreactions...</mood>
//    <requiresrelationship>true/false</requiresrelationship>
//    <chainresponses>
//       <actionid>id</actionid>...<actionid>id</actionid>
//    </chainresponses>
// </action>
// Note: Moods must be named 'triggers' and 'reactions' respectively.
bool ActionNode::fromNode()
{
   if (!mpNode) return false;

   // Name of node should be 'action' and value should exist.
   const char* const pName = mpNode->getName();
   const char* const pValue = mpNode->getValue();
   if (!pName || !pValue) return false;
   if (strncmp(pName, "action", 6) != 0) return false;

   reset();

   // Prepare fail function.
   auto fail = [&]() -> bool {
      reset();
      return false;
   };

   // Copy value.
   strncpy(mName, pValue, 31);

   // Child nodes.
   std::list<Node>& children = mpNode->getChildren();
   for (Node& child : children) {
      // id
      const char* const pName = child.getName();
      const char* const pValue = child.getValue();

      // Both a name and value must exist for each child.
      if (!pName && !pValue) continue;

      // id.
      if (strncmp("id", pName, 2) == 0) {
         mId = child.getUint64(pValue);
      } else if (strncmp("forgesrelationship", pName, 17) == 0) {
         mForgesRel = child.getBoolValue();
      } else if (strncmp("requiresrelationship", pName, 20) == 0) {
         mRequireRel = child.getBoolValue();
      } else if (strncmp("chainresponses", pName, 20) == 0) {
         if (!fromChainResponses(child)) {
            return fail();
         }
      } else if (strncmp("triggers", pValue, 8) == 0) {
         MoodNode m(child);
         if (!m.fromNode()) {
            return fail();
         }

         // Demote.
         mTriggers = m;
      } else if (strncmp("instigatereactions", pValue, 14) == 0) {
         MoodNode m(child);
         if (!m.fromNode()) {
            return fail();
         }

         // Demote.
         mInstigateReactions = m;
      } else if (strncmp("recipientreactions", pValue, 18) == 0) {
         MoodNode m(child);
         if (!m.fromNode()) {
            return fail();
         }

         // Demote.
         mRecipientReactions = m;
      }
   } 

   // Done.
   return true;
}

//
// To node
//

// Writes a node from action.
// Format of node is as described in ActionQtyNode::fromNode().
bool ActionNode::toNode()
{
   if (!mpNode) return false;
   mpNode->empty();

   // Set value first.
   if (!mpNode->setValue("action", mName)) return false;

   // Fail function.
   auto fail = [&]() -> bool {
      mpNode->empty();
      return false;
   };

   // Spawn children and set their value.
   Node* pId = mpNode->spawnChild();
   Node* pTriggers = mpNode->spawnChild();
   Node* pInstigateReactions = mpNode->spawnChild();
   Node* pRecipientReactions = mpNode->spawnChild();
   Node* pFormsRel = mpNode->spawnChild();   
   Node* pRequireRel = mpNode->spawnChild();
   Node* pChainResponses = mpNode->spawnChild();
   if (nullptr == pId || nullptr == pTriggers) return fail();
   if (nullptr == pFormsRel || nullptr == pRequireRel) return fail();
   if (nullptr == pChainResponses) return fail();
   if (nullptr == pInstigateReactions || nullptr == pRecipientReactions)
      return fail();

   // Id.
   if (!pId->setValue("id", mId)) {
      return fail();
   }

   // Triggers.
   MoodNode mnTriggers(mTriggers, *pTriggers);
   if (!mnTriggers.toNode("triggers")) {
      return fail();
   }

   // Actor Reactions.
   MoodNode mnInstigateReactions(mInstigateReactions, *pInstigateReactions);
   if (!mnInstigateReactions.toNode("instigatereactions")) {
      return fail();
   }

   // Recipient Reactions.
   MoodNode mnRecipientReactions(mRecipientReactions, *pRecipientReactions);
   if (!mnRecipientReactions.toNode("recipientreactions")) {
      return fail();
   }

   // Relationship forming.
   if (!pFormsRel->setValue("forgesrelationship", mForgesRel)) {
      return fail();
   }

   // Relationship requirement.
   if (!pRequireRel->setValue("requiresrelationship", mRequireRel)) {
      return fail();
   }

   // Chain responses.
   for (ObjRef<const Action>& chainResponse : mChainResponses) {
      // Spawn child node.
      Node* pChainResponse = pChainResponses->spawnChild();
      if (nullptr == pChainResponse)
         return fail();

      // Assign value.
      if (!pChainResponse->setValue("actionid", chainResponse.getId())) {
         return fail();
      }
   }

   // Succeed.
   return true;
}

//
// From node privates
//

bool ActionNode::fromChainResponses(Node& node)
{
   try {
      // Child nodes.
      std::list<Node>& children = node.getChildren();
      for (Node& child : children) {
         // id
         const char* const pName = child.getName();
         const char* const pValue = child.getValue();

         // Both a name and value must exist for each child.
         if (!pName && !pValue) continue;

         // actionid.
         if (strncmp("actionid", pName, 8) == 0) {
            ObjRef<const Action> obj(child.getUint64(pValue));
            mChainResponses.push_back(obj);
         }
      }
   } catch (exception& e) {
      return false;
   }

   // Done.
   return true;
}

// 
// ,---.     |    o          ,---.|
// |---|,---.|--- .,---.,---.|   ||--- ,   .
// |   ||    |    ||   ||   ||   ||    |   |
// `   '`---'`---'``---'`   '`---\`---'`---|
//                                     `---'
// Keeps track of the number of actions present in an environment.
// This is typically used on beings to distinguish a quantity for the
// number of times an action has been acted upon (or vice versa).
//

ActionQty::ActionQty(const ActionQty& qty)
{
   *this = qty;
}

// Data
void ActionQty::reset()
{
   // When resetting, clear everything so qty is 1.
   mQty = 1;
}

//
// Operators
//

// Assignment.
ActionQty& ActionQty::operator=(const ActionQty& qty)
{
   if (&qty == this)
      return *this;

   mActionRef = qty.mActionRef;
   mQty = qty.mQty;
}

ActionQty& ActionQty::operator=(const ActionQtyNode& node)
{
   if (&node == this)
      return *this;

   // Assign to this.
   const ActionQty& a = dynamic_cast<const ActionQty&>(node);
   *this = a;

   // Return.
   return *this;
}

// Pre increment.
const ActionQty& ActionQty::operator++()
{
   mQty++;
   return *this;
}

// Post increment.
const ActionQty& ActionQty::operator++(int postfix)
{
   ActionQty& ret = (*this);
   mQty++;
   return ret;   
}

// 
// ,---.     |    o          ,---.|         ,   .         |
// |---|,---.|--- .,---.,---.|   ||--- ,   .|\  |,---.,---|,---.
// |   ||    |    ||   ||   ||   ||    |   || \ ||   ||   ||---'
// `   '`---'`---'``---'`   '`---\`---'`---|`  `'`---'`---'`---'
//                                     `---'
// 

ActionQtyNode::ActionQtyNode(Node& node)
: ActionQty(1), mpNode(&node)
{
}

ActionQtyNode::ActionQtyNode(const ActionQty& aqty, Node& node)
: ActionQty(aqty)
{
   mpNode = &node;
   *this = aqty;
}

ActionQtyNode::~ActionQtyNode()
{
   mpNode = nullptr;
}

//
// Assignment
//

ActionQtyNode& ActionQtyNode::operator=(const ActionQty& aqty)
{
   ActionQty& target = dynamic_cast<ActionQty&>(*this);
   target = aqty;

   return *this;
}

//
// From node
//

// Reads a node into action.
// Format of node is as follows:
// <actionqty>
//    <actionid></actionid>
//    <aqty>qty</aqty>
// </actionqty>
bool ActionQtyNode::fromNode()
{
   if (!mpNode) return false;

   // Name of node should be 'actionqty'.
   const char* const pName = mpNode->getName();
   if (!pName || strncmp(pName, "actionqty", 9) != 0) {
      return false;
   }

   // Value not required for an actionqty. Get child nodes.
   std::list<Node>& children = mpNode->getChildren();
   for (Node& child : children) {
      const char* const pName = child.getName();
      const char* const pValue = child.getValue();

      // Child nodes must have both name and value.
      if (!pName || !pValue) return false;

      // Get id and quantity.
      if (strncmp(pName, "actionid", 8) == 0) {
         mActionRef.setId(child.getUint64(pValue));
      } else if (strncmp(pName, "aqty", 4) == 0) {
         mQty = child.getUint64(pValue); 
      }
   }

   return true;
}

//
// To node
//

// Writes a node from action.
// Format of node is as described in ActionQtyNode::fromNode().
bool ActionQtyNode::toNode()
{
   if (!mpNode) return false;
   mpNode->empty();

   // Set value first.
   if (!mpNode->setValue("actionqty", "")) return false;

   // Fail function.
   auto fail = [&]() -> bool {
      mpNode->empty();
      return false;
   };

   // Spawn children and set their value.
   Node* pActionId = mpNode->spawnChild();
   Node* pAQty = mpNode->spawnChild();
   if (nullptr == pActionId || nullptr == pAQty) {
      return fail();
   }

   // Action Id.
   if (!pActionId->setValue("actionid", mActionRef.getId())) {
      return fail();
   }

   // Action quantity.
   if (!pAQty->setValue("aqty", mQty)) {
      return fail();
   }

   // Succeed.
   return true;
}
