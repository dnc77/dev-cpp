/*
Date: 28 Nov 2019 10:29:15.607596363
File: actiondecider.h

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
*/

#ifndef __ACTIONDECIDER_H_0B55FD06241AC776A0B7DAE608EC724F__
#define __ACTIONDECIDER_H_0B55FD06241AC776A0B7DAE608EC724F__

// Check for missing includes.
#if not defined _GLIBCXX_CSTDINT
#error "actiondecider.h: missing include - cstdint"
#elif not defined __STRINGLIST_H_3F91B722978DC8F00D1E31EF68DC35C8__
#error "actiondecider.h: missing include - stringlist.h"
#elif not defined __SERIALIZER_H_B51368F9355D0A6C0E780E1F8D197E39__
#error "actiondecider.h: missing include - serializer.h"
#elif not defined __MOOD_H_AAB57A66720F9EDAC5D0668C603393AB__
#error "actiondecider.h: missing include - mood.h"
#elif not defined __ACTION_H_BA0DD2641DF3D0A449B1FA2732A6432E__
#error "actiondecider.h: missing include - action.h"
#elif not defined __ENVIRONMENT_H_98216C8B541FBFB5FD5CA3DC5B6355BF__
#error "actiondecider.h: missing include - environment.h"
#elif not defined __BEING_H_B969CD487F2BA6EDED6F3DC728739CDC__
#error "actiondecider.h: missing include - being.h"
#elif not defined __RELATIONSHIP_H_F256603D5EE1002DE62E21BDC7758768__
#error "actiondecider.h: missing include - relationship.h"
#elif not defined __WEIGHTEDBINARY_H_B80BAFE8712B57100CB74EB5451E5437__
#error "actiondecider.h: missing include - weightedbinary.h"
#elif not defined _GLIBCXX_LIST
#error "actiondecider.h: missing include - list"
#endif

class ActionDecider
{
public:
   // Construction
   ActionDecider(const Action& action, const Being* pInstigator,
      const Being* pRecipient = nullptr, const Relationship* pRel = nullptr);
   virtual ~ActionDecider();

   // Actor-Action relationship
   bool decide();                               // act or not act.

protected:
   WeightedBinary mWeightingFactor;
   const Action& mAction;   
   const Being* mpInstigator;
   const Being* mpRecipient = nullptr;
   const Relationship* mpRel = nullptr;

   // Actor-Action relationship (private)
   bool isActable();                            // Can actor perform action?
   bool isTriggerable(double& weightingFactor); // Is actor inclined to act?

   // Actor-Relationship specific
   Mood calcActionPotential();
};

#endif   // __ACTIONDECIDER_H_0B55FD06241AC776A0B7DAE608EC724F__
