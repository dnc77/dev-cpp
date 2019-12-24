/*
Date: 28 Nov 2019 10:29:20.981886842
File: actiondecider.cpp

Copyright Notice
This document is protected by the GNU General Public License v3.0.

This allows for commercial use, modification, distribution, patent and private
use of this software only when the GNU General Public License v3.0 and this
copyright notice are both attached in their original form.

For developer and author protection, the GPL clearly explains that there is no
warranty for this free software and that any source code alterations are to be
shown clearly to identify the original author as well as any subsequent changes
made and by who.
g
For any questions or ideas, please contact:
github:  https://github(dot)com/dnc77
email:   dnc77(at)hotmail(dot)com
web:     http://www(dot)dnc77(dot)com

Copyright (C) 2000-2025 Duncan Camilleri, All rights reserved.
End of Copyright Notice

Purpose: Establishes a decision making process based on a being's current mood
         and bias to determine whether an action is bound to happen on the basis
         of its trigger mood. When an action is deemed as 'plausible' a fuzzy
         implement of human behaviour may trigger the action and have an
         emotional response on the being's mood performing the action as well as
         any recipient beings and environments.
         The components to the decider are as follows:
         -  An action which needs to be decided on based on it's 
            triggers (Action.mTriggers) and also the actor's bias (Being.mBias)
            and current mood (Being.mMood).
         -  A being taking the action (referred to as the instigator).
         -  An optional being that is being acted upon (recipient) whose moods
            will be impacted by the action (causing potential subsequent actions
            to take place at a later stage) (Being.impact()). This being will
            be referenced if the action requires a relationship. This
            relationship should be spawned within BehaveFactory but at this
            stage is not a required element. This is because certain actions
            can forge relationships should they happen. When actions with beings
            complete and no relationship for the two being exists, a
            relationship is spawned if the action allows for forging
            relationships. On a separate note, an action cannot be performed if
            a relationship does not exist between two beings and such action
            does not forge relationships.            
         -  An optional relationship which would have spawned by BehaveFactory
            only. This relationship will have the right moods set up according
            to the time of spawning and would also have experienced various
            actions. A relationship need not be provided if two beings are
            but a relationship should not be provided when one being is missing.
            Finally a relationship should only be between the two parties.
            The relationship is used to calculate the action potential below.
         Note: A being can also be a gathering in that a group of people may
               collectively perform an action on one being or many. Call it a
               war zone if you like :)

         Additional concepts
         -  ActionPotential - The action potential is a Mood that defines the
            potential for the action to take place. This is needed because it
            defines a basis for an action to take place. An actor won't
            typically punch someone if he or she feels happy. Said actor is more
            liable to punch someone when angry. Now an actor (Being) has two
            moods that define this potential to act. The bias is the long term
            basis and is the undertone of the character. This is formed over a
            long period of time through many actions taking place on said actor.
            On the other hand, the current mood is going to have a stronger
            influence on the actor. If an actor is currently very angry, and
            his undertone is usually peaceful, the chances of said actor
            punching someone are significantly higher. As a result, these two
            moods need to be combined in such a way such that the
            ActionPotential defines such results. This is done by applying 
            influence intensifiers on the current and bias moods, then averaging
            them to determine an ActionPotential mood.
            An environment's ambience mood also plays a subtle part in this.
            If a relationship exists, then the action will be taken on the
            other party. In that case, the relationship's mood from the actor's
            perspective will also need to be taken into account. Typically,
            the current mood has the strongest influence, followed by
            relationship views and then the bias. The environment may also have
            a slight influence. These intensifiers are applied in
            calcActionPotential() returning a Mood which defines the mood that
            should be used to determine whether an action will trigger or not.
         -  Triggerable - Each action has a trigger mood (Action.mTriggers).
            This mood defines levels at which the action has the potential to
            'trigger'. Any emotion intensity that exceeds the trigger level is
            considered to qualify as an emotional trigger. If a being's
            ActionPotential mood exceeds that action's trigger mood level on all
            emotions, then the action is considered triggerable.
         -  WeightingFactor - When determining whether an action should be taken
            or not, a weighting factor is used to generate a random number which
            determines the outcome (action taken or not). This weighting factor
            is used with WeightedBinary. The weighted factor is found by
            finding the difference between the ActionPotential mood and the
            action's trigger mood for each emotion. All the emotions of the
            difference will then be averaged. That will become the weighting
            factor.

Version control
28 Nov 2019 Duncan Camilleri           Initial development
17 Dec 2019 Duncan Camilleri           Support for relationship forging actions
19 Dec 2019 Duncan Camilleri           Support emotions without trigger actions

*/

#include <assert.h>
#include <string.h>
#include <sys/time.h>
#include <cstdint>
#include <list>
#include <string>
#include <random>
#include "helpers.h"
#include "objref.h"
#include "weightedbinary.h"
#include "stringlist.h"
#include "serializer.h"
#include "mood.h"
#include "action.h"
#include "environment.h"
#include "being.h"
#include "gathering.h"
#include "relationship.h"
#include "actiondecider.h"

using namespace std;

//
// Construction
//

ActionDecider::ActionDecider(const Action& action, const Being* pInstigator,
      const Being* pRecipient /*= nullptr*/,
      const Relationship* pRel /*= nullptr*/)
: mAction(action), mpInstigator(pInstigator), mpRecipient(pRecipient),
   mpRel(pRel)
{
}

ActionDecider::~ActionDecider()
{
}

//
// Actor-Relationship specific
//

// The action potential
Mood ActionDecider::calcActionPotential()
{
   const intensity biasInfluence = 0.70;
   const intensity currentInfluence = 1.20;
   const intensity ambienceInfluence = 0.10;
   const intensity relInfluence = 1.15;
   Mood actionPotential[3];
   Mood average;
   // 0 - biasMood with a slightly lower influence.
   mpInstigator->getBiasMood().intensify(actionPotential[0], biasInfluence);
   // 1 - currentMood with stronger influence.
   mpInstigator->getCurrentMood().intensify(
      actionPotential[1], currentInfluence);
   // 2 - relationship mood of actor.
   if (mpRel != nullptr) {
      if (mpInstigator->id() == mpRel->getData().mBeingRefA.getId()) {
         // Include relationship modifier.
         mpRel->getData().mMoodA.intensify(actionPotential[2], relInfluence);

         average = Mood::average(actionPotential, 3);
      } else if (mpInstigator->id() == mpRel->getData().mBeingRefB.getId()) {
         // Include relationship modifier.
         mpRel->getData().mMoodB.intensify(actionPotential[2], relInfluence);

         average = Mood::average(actionPotential, 3);
      } else {
         // Find resultant potential - no valid relationship.
         average = Mood::average(actionPotential, 2);
      }
   } else {
      // Find resultant potential - no valid relationship.
      average = Mood::average(actionPotential, 2);
   }

   // Established base line mood for actor. Now consider the environment.
   Environment* pEnv = const_cast<Being*>(mpInstigator)->getEnvironment();
   if (!pEnv)
      return average;
   
   Mood ambience = pEnv->getAmbienceMood();
   ambience.intensify(ambience, ambienceInfluence);

   // We want to apply a subtle influence of the environment's ambience
   // mood to the baseline mood of the actor to conclude the action potential
   // calculation.
   return average + ambience;
}

//
// Actor-Action relationship
//

// Establishes a decision to act versus not to act.
// If acting is the decision, true is returned. Otherwise false.
bool ActionDecider::decide()
{
   if (!isActable()) return false;

   double weightingFactor = 0.0;
   if (!isTriggerable(weightingFactor))
      return false;

   // Ok at this stage, the action may take place. Use randomization to
   // determine fuzzy behaviour.
   if (weightingFactor != mWeightingFactor.getRatio()) {
      mWeightingFactor.setRatio(weightingFactor);
   }

   // Determines state as decision to act vs not act.
   return mWeightingFactor.changeState();
}

// Can actor perform action?
// Check if the action exists in the actor's actable actions.
bool ActionDecider::isActable()
{
   // If the action requires a relationship, then at least, there should be
   // both an instigator and a recipient. If not, the action cannot be performed
   // as it requires both. No need to worry about the presence of a relationship
   // since an action can spawn a relationship if it does not exist.
   if (mAction.requiresRelationship()) {
      if (nullptr == mpInstigator || nullptr == mpRecipient)
         return false;

      // Ensure common environment. Two actors cannot work together if they are
      // in different environments.
      if (mpInstigator->getEnvironmentId() != mpRecipient->getEnvironmentId()) {
         return false;
      }

      // If a relationship is present, ensure it's validity.
      if (mpRel) {
         const RelationshipData& data = mpRel->getData();
         if (data.mBeingRefA.getId() !=  mpInstigator->id() &&
            data.mBeingRefA.getId() != mpRecipient->id()) {
            return false;
         }
         if (data.mBeingRefB.getId() !=  mpInstigator->id() &&
            data.mBeingRefB.getId() != mpRecipient->id()) {
            return false;
         }
      } else {
         // If no relationship present and action cannot spawn a relationship,
         // then, action is not actable.
         if (!mAction.forgesRelationship()) {
            return false;
         }
      }
   }

   // Does this action belong to the actor's actable actions?
   const list<ObjRef<const Action>> actableActions =
      mpInstigator->getActableActions();
   for (ObjRef<const Action> actionref : actableActions) {
      if (actionref.getId() == mAction.id()) {
         return true;
      }
   }

   // Valid action but cannot be actioned by the actor.
   return false;
}

// Is actor inclined to act based on mood?
// This goes through each actor's emotions and compares them with the
// ActionPotential for the scenario.
// While going through each of the emotions, one weightingFactor is
// introduced for randomness. This will be used on a WeightedBinary
// to establish a weighted random probability which effectively
// determines whether the action should be taken or not.
bool ActionDecider::isTriggerable(double& weightingFactor)
{
   Mood ap = calcActionPotential();
   const Mood& triggers = mAction.getTriggers();

   // Go through all trigger emotions. Any that are not 0 are emotions that can
   // trigger the action. If the action potential emotion is lower than the
   // trigger emotion at any point in time, then it can be deduced that the
   // action is not triggerable in this instance.
   unsigned short n = 0;
   unsigned short nEnabledTriggers = 0;
   weightingFactor = 0.0;
   for (; n < Mood::plutchikCount; ++n) {
      const intensity& emotion = triggers.get((Mood::Plutchik)n);
      if (emotion != 0 && ap.get((Mood::Plutchik)n) < emotion)
         return false;

      // Weighting factor calculation.
      // This is done by averaging the difference between the action potential
      // intensities of each emotion and the *enabled* trigger emotion.
      // So we can say that given Trigger mood T[e], and Action potential AP[e],
      // the weighting factor would be:
      // avg(summation((AP[e] - T[e]) / 2) where e = 1 to 8 (for each emotion).
      if (emotion != 0) {
         nEnabledTriggers++;
         weightingFactor += (ap.get((Mood::Plutchik)n) - emotion);
      }
   }

   // There is enough emotion to make it plausible for this action to happen.
   if (nEnabledTriggers > 0) {
      weightingFactor = weightingFactor / (nEnabledTriggers * 2);
      return true;
   } else if (nEnabledTriggers == 0) {
      weightingFactor = 0.5;
      return true;
   }

   // No trigger emotions enabled.
   return false;
}
