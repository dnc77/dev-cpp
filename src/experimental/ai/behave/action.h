/*
Date: 02 Oct 2019 11:45:19.781576114
File: action.h

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
04 Nov 2019 Duncan Camilleri           Introduced ActionNode and ActionQtyNode
13 Nov 2019 Duncan Camilleri           Added name retrieval
25 Nov 2019 Duncan Camilleri           ActionQty now uses ObjRef
05 Dec 2019 Duncan Camilleri           Added relationship requirement for action
06 Dec 2019 Duncan Camilleri           Differentiate actor/recipient reactions
06 Dec 2019 Duncan Camilleri           Initiated chain reactions
16 Dec 2019 Duncan Camilleri           Introduced 'formsRel'
20 Dec 2019 Duncan Camilleri           Actor terminology changed to instigator

*/

#ifndef __ACTION_H_BA0DD2641DF3D0A449B1FA2732A6432E__
#define __ACTION_H_BA0DD2641DF3D0A449B1FA2732A6432E__

// Check for missing includes.
#if not defined _GLIBCXX_CSTDINT
#error "action.h: missing include - cstdint"
#elif not defined _GLIBCXX_LIST
#error "action.h: missing include - list"
#elif not defined _GLIBCXX_STRING
#error "action.h: missing include - string"
#elif not defined __OBJREF_H_508BC7820E23F9ECDC1F73EB27A6B36E__
#error "action.h: missing include - objref.h"
#elif not defined __STRINGLIST_H_3F91B722978DC8F00D1E31EF68DC35C8__
#error "mood.h: missing include - stringlist.h"
#elif not defined __SERIALIZER_H_B51368F9355D0A6C0E780E1F8D197E39__
#error "mood.h: missing include - serializer.h"
#elif not defined __MOOD_H_AAB57A66720F9EDAC5D0668C603393AB__
#error "action.h: missing include - mood.h"
#endif

//
// Action
//

class ActionNode;
class Action
{
public:
   Action();
   Action(const Action& action);
   Action(const Mood& triggers,
      const Mood& actorReactions, const Mood& recipientReactions);
    virtual ~Action();

   // Assignment
   Action& operator=(const Action& action);
   Action& operator=(const ActionNode& node);
   void reset();

   // Naming
   void name(const uint64_t id, const char* const name);

   // Accessors
   const uint64_t& id() const                   { return mId;                 }
   const char* const name() const               { return mName;               }
   const Mood& getTriggers() const              { return mTriggers;           }
   const Mood& getInstigateReactions() const    { return mInstigateReactions; }
   const Mood& getRecipientReactions() const    { return mRecipientReactions; }
   bool forgesRelationship() const              { return mForgesRel;          }
   bool requiresRelationship() const            { return mRequireRel;         }
   const std::list<ObjRef<const Action>>& getChainResponses() const;

protected:
   // Name of action.
   uint64_t mId;
   char mName[32];
   // Identify different intensities of emotions at which this action may get
   // triggered by default.
   Mood mTriggers;
   // Identifies an intensifier change on emotions when the action is taken
   // by an actor.
   Mood mInstigateReactions;
   // Identifies an intensifier change on emotions of recipients when the action
   // is taken by an actor.
   Mood mRecipientReactions;
   // Determines whether an action will form a relationship after completed.
   bool mForgesRel;
   // Determines whether an action requires at least two beings in a
   // relationship for it to take effect.
   bool mRequireRel;

   // Chain responses
   // Chain responses are potential actions that a recipient may trigger due to
   // an action taken on him/her. For example, if someone starts a fight with
   // someone, a potential chain response is to fight back. This will be
   // processed like a normal action only this time, the receiver of the
   // previous action will be the one initiating the following action.
   // This will trigger a chain of responses as two actors keep acting on each
   // other alternately. Once an actor has acted on a recipient, the recipient
   // becomes the actor and performs one or more of the chain responses
   // available. This keeps alternating between actor and recipient until
   // one of the actors does not trigger any action.
   std::list<ObjRef<const Action>> mChainResponses;
};

//
// ActionNode
//

class ActionNode : public Action
{
public:
   ActionNode(Node& node);
   ActionNode(const Action& action, Node& node);
   virtual ~ActionNode();

   // Assignment
   ActionNode& operator=(const Action& action);

   // Access
   Node& getNode()                     { return *mpNode; }

   // From node
   bool fromNode();

   // To node
   bool toNode();

protected:
   Node* mpNode;

   // From node privates
   bool fromChainResponses(Node& node);
};

//
// ActionQty
// Keeps track of the number of actions present in an environment.
// This is typically used on beings to distinguish a quantity for the
// number of times an action has been acted upon (or vice versa).
//

class ActionQtyNode;
class ActionQty 
{
public:
   ActionQty(const uint64_t id) : mActionRef(id), mQty(1) { }
   ActionQty(const ActionQty& qty);
   ActionQty(const Action& a) : mActionRef(&a, a.id()), mQty(1) { }
   virtual ~ActionQty() { }

   // Data
   void reset();

   // Operators
   ActionQty& operator=(const ActionQty& qty);
   ActionQty& operator=(const ActionQtyNode& node);
   const ActionQty& operator++();
   const ActionQty& operator++(int postfix);

   // Accessors
   const Action* getAction() { return mActionRef.getObj(); }

protected:
   ObjRef<const Action> mActionRef;
   unsigned int mQty;
};

//
// ActionQtyNode
//

class ActionQtyNode : public ActionQty
{
public:
   ActionQtyNode(Node& node);
   ActionQtyNode(const ActionQty& aqty, Node& node);
   virtual ~ActionQtyNode();

   // Assignment
   ActionQtyNode& operator=(const ActionQty& aqty);

   // Access
   Node& getNode()                     { return *mpNode; }

   // From node
   bool fromNode();

   // To node
   bool toNode();

protected:
   Node* mpNode;
};

#endif   // __ACTION_H_BA0DD2641DF3D0A449B1FA2732A6432E__
