/*
Date: 09 Dec 2019 12:51:17.066440184
File: enact.cpp

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

Purpose: Combines beings, actions, environments and relationships together in a
         BehaveFactory in order to get some behaviour happening.

Version control
09 Dec 2019 Duncan Camilleri           Initial development
*/

#include <exception>
#include <cstdint>
#include <string>
#include <list>
#include <map>
#include <random>
#include <mutex>
#include <algorithm>
#include <sys/time.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include <helpers.h>
#include <cycbuf.h>
#include "weightedbinary.h"
#include "objref.h"
#include "stringlist.h"
#include "serializer.h"
#include "mood.h"
#include "action.h"
#include "environment.h"
#include "being.h"
#include "gathering.h"
#include "relationship.h"
#include "behavefactory.h"
#include "actiondecider.h"
#include "enact.h"

using namespace std;

//
// Construction
//

Enact::Enact(BehaveFactory& factory)
: mFactory(factory)
{
   memset(&mCallbacks, 0, sizeof(mCallbacks));
}

Enact::~Enact()
{
}

//
// Accessors
//

void Enact::setCallbacks(EnactCallbacks* pecb)
{
   mCallbacks = *pecb;
}

//
// Action
//

// Will use ActionDecider to decide if an action can take place given the
// arguments provided. take an action. If a decision to act has been made, then
// all beings, relationships and the environment will be impacted accordingly.
// Will call any callbacks that are present.
// If the action gets performed, the recipient will have a time slot to perform
// any response actions that exist in the chain responses. This will keep going
// on until the last actor does not take a subsequent action.
// If no action has been taken, false is returned.
bool Enact::enact(const Action& action, Being* pInstigator,
   Being* pRecipient /*= nullptr*/)
{
   if (!pInstigator) return false;

   // Does a relationship exist between the two beings?
   Relationship* pRel = nullptr;
   if (pRecipient) {
      pRel = mFactory.findRel(pInstigator->id(), pRecipient->id());
   }

   // Make a decision.
   ActionDecider ad(action, pInstigator, pRecipient, pRel);
   const Being* pConstInstigator = const_cast<const Being*>(pInstigator);
   const Being* pConstRecipient = const_cast<const Being*>(pRecipient);
   const Relationship* pConstRel = const_cast<const Relationship*>(pRel);
   bool decision = ad.decide();
   if (mCallbacks.actionDecided) {
      mCallbacks.actionDecided(action,
         pConstInstigator, pConstRecipient,
         pConstRel, decision
      );
   }

   // If no action should be taken, exit.
   if (!decision) return false;

   // Since the decision is made to act, have an impact on all relevent items.
   // Lock all data for impact modifications. Cannot guarantee actor/recipient
   // not being used elsewhere in another instance of EnactEntities here.
   unique_lock<mutex> lock(mEnactSync, defer_lock);
   lock.lock();

   // Relationship checking/spawning.
   // Note that the ActionDecider would have already done some crucial
   // validation checks and at this point, these need not be done again.
   if (action.requiresRelationship()) {
      // Forge a relationship if the action requires a relationship but
      // none exist at the moment.
      if (!pRel && action.forgesRelationship()) {
         // Spawn a relationship as required. If this fails, stop and fail the
         // action.
         pRel = mFactory.spawnRelationship(nullptr, action,
            *pConstInstigator, *pConstRecipient);
         if (!pRel) {
            lock.unlock();
            return false;
         }
      }
   }

   // Impact the actor.
   pInstigator->impact(action, true);

   // Impact the environment.
   Environment* pEnv = pInstigator->getEnvironment();
   if (pEnv) {
      pEnv->impact(action);
   }

   // Impact the recipient if there was a relationship.
   if (pRecipient && pRel) {
      pRecipient->impact(action, false);

      // Impact the relationship.
      pRel->impact(*pConstInstigator, action);
   }

   // Release lock.
   lock.unlock();

   // Action taken - callback.
   if (mCallbacks.actionTaken) {
      mCallbacks.actionTaken(action,
         pConstInstigator, pConstRecipient,
         pConstRel);
   }

   // Once all the relevant parties have been impacted, chainAct again this time
   // having the recipient as the instigator. Consider all reaction actions.
   // Once a response has been taken, stop. Note recursive behaviour as actor
   // and recipient keep swapping roles.
   if (pRecipient && pRel) {
      list<ObjRef<const Action>>& chainResponses =
         const_cast<list<ObjRef<const Action>>&>(
            action.getChainResponses()
         );

      for (ObjRef<const Action> obj : chainResponses) {
         Action* pAction = const_cast<Action*>(obj.getObj());
         if (pAction) {
            if (enact(*pAction, pRecipient, pInstigator)) {
               return true;
            }
         }
      }
   }

   // Done!
   return true;
}

// This will go through all beings in the behave factory, and try to enact one
// action by themselves or with any others in the environment
// whom they have a relationship with. They may even try to initiate
// conversation and establish new relationships.
void Enact::enactAll()
{
   // Go through each being in the factory.
   for (Being& b : mFactory.mBeings) {
      // Go through each action, and perform.
      const std::list<ObjRef<const Action>>& actions = b.getActableActions();
      for (ObjRef<const Action> actionref : actions) {
         // Solo action?
         const Action* pAction = actionref.getObj();
         if (!pAction) continue;

         // If action does not require a relationship, try to perform it.
         if (!pAction->requiresRelationship()) {
            // Enact solo actions.
            if (enact(*pAction, &b)) {
               // Once one being has acted, move on ot the next being.
               break;
            }
         }

         // Enact action with a being that is in the same environment.
         if (enactWithBeings(*pAction, b)) {
            // Same - do not act more than once.
            break;
         }
      }
   }
}

// Performs action by instigator on all other beings.
bool Enact::enactWithBeings(const Action& a, Being& instigator)
{
   // Go through each being in the factory.
   for (Being& b : mFactory.mBeings) {
      // Do not process on self!
      if (instigator.id() == b.id())
         continue;

      // Do nothing if both actors are not in the same environment.
      if (instigator.getEnvironmentId() != b.getEnvironmentId())
         continue;

      // Try perform action and if successful, exit.
      if (enact(a, &instigator, &b))
         return true;
   }

   // No Action taken!
   return false;
}
