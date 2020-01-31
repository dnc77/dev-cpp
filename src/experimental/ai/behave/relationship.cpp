/*
Date: 29 Nov 2019 12:43:28.242995939
File: relationship.cpp

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

Purpose: A relationship is defined by two parties, party A, and party B.
         Each party has their own interpretation of how the relationship
         between the two works so there are different emotions per party's
         point of view. Actions taken between the two parties will have an
         impact over the relationship. In summary, the relationship overall
         mood can be quantified by averaging the two different perspectives
         together.

Version control
29 Nov 2019 Duncan Camilleri           Initial development
09 Dec 2019 Duncan Camilleri           getPartner() and impact()
09 Jan 2020 Duncan Camilleri           impact() exponential diminish

*/

#include <string.h>
#include <cstdint>
#include <list>
#include <string>
#include "objref.h"
#include "stringlist.h"
#include "serializer.h"
#include "mood.h"
#include "action.h"
#include "environment.h"
#include "being.h"
#include "relationship.h"

// 
//           |         |    o               |    o
// ,---.,---.|    ,---.|--- .,---.,---.,---.|---..,---.
// |    |---'|    ,---||    ||   ||   |`---.|   |||   |
// `    `---'`---'`---^`---'``---'`   '`---'`   '`|---'
//                                                |
// 

intensity Relationship::mCfgRelationshipImpactingMoodMinIntensity = -0.0020;
intensity Relationship::mCfgRelationshipImpactingMoodMaxIntensity = 0.0020;

//
// Construction
//

Relationship::Relationship()
{
   reset();
}

Relationship::Relationship(RelationshipData& rd)
{
   mRelData = rd;
}

Relationship::Relationship(const Relationship& rel)
{
   *this = rel;
}

Relationship::~Relationship()
{
   reset();
}

//
// Accessors
//

const uint64_t& Relationship::id() const
{
   return mId;
}

const RelationshipData& Relationship::getData() const
{
   return mRelData;
}

const Being* Relationship::getPartner(const Being& counterpart)
{
   if (counterpart.id() == mRelData.mBeingRefA.getId())
      return mRelData.mBeingRefB.getObj();
   if (counterpart.id() == mRelData.mBeingRefB.getId())
      return mRelData.mBeingRefA.getObj();

   return nullptr;
}

//
// Assignments
//
Relationship& Relationship::operator=(const Relationship& rel)
{
   if (&rel == this)
      return *this;

   mRelData = rel.mRelData;
   return *this;

}

Relationship& Relationship::operator=(const RelationshipData& reldata)
{
   mRelData = reldata;
   return *this;
}

Relationship& Relationship::operator=(const RelationshipNode& node)
{
   if (&node == this)
      return *this;

   // Assign to this.
   const Relationship& rel = dynamic_cast<const Relationship&>(node);
   *this = rel;

   // Return.
   return *this;
}

//
// Data
//

void Relationship::reset()
{
   mRelData.mBeingRefA.setId(0);
   mRelData.mBeingRefB.setId(0);
   mRelData.mMoodA.neutralize();
   mRelData.mMoodB.neutralize();
}

//
// Naming
//

void Relationship::name(const uint64_t id, const char* const name)
{
   mId = id;
   if (name == nullptr) {
      memset(mName, 0, 32);
   } else {
      strncpy(mName, name, 31);
   }
}

//
// Mood shifting
//
void Relationship::impact(const Being& actor, const Action& action)
{
   // Ensure actor is valid.
   const Being* recipient = getPartner(actor);
   if (!recipient) return;

   // Distinguish actor vs recipient in relationship.
   Mood* pActorMood = &mRelData.mMoodA;
   Mood* pRecipientMood = &mRelData.mMoodB;
   if (actor.id() == mRelData.mBeingRefB.getId()) {
      pActorMood = &mRelData.mMoodB;
      pRecipientMood = &mRelData.mMoodA;
   }

   // Impacting moods (instigator and recipient impacting moods).
   const Mood& instigateImpact = action.getInstigateReactions();
   const Mood& recipientImpact = action.getRecipientReactions();
   for (unsigned short emotion = 0; emotion < Mood::plutchikCount; ++emotion) {
      Mood::Plutchik e = (Mood::Plutchik)emotion;
      
      // Impact instigator of action's view on relationship.
      pActorMood->intensify(e,
         Mood::scaleIntensity(mCfgRelationshipImpactingMoodMinIntensity,
            mCfgRelationshipImpactingMoodMaxIntensity, 
            instigateImpact.get(e)),
         false
      );

      // Impact recipient of action's view on relationship.
      pRecipientMood->intensify(e,
         Mood::scaleIntensity(mCfgRelationshipImpactingMoodMinIntensity,
            mCfgRelationshipImpactingMoodMaxIntensity, 
            recipientImpact.get(e)),
         false
      );
   }
}

// 
//           |         |    o               |    o     ,   .         |
// ,---.,---.|    ,---.|--- .,---.,---.,---.|---..,---.|\  |,---.,---|,---.
// |    |---'|    ,---||    ||   ||   |`---.|   |||   || \ ||   ||   ||---'
// `    `---'`---'`---^`---'``---'`   '`---'`   '`|---'`  `'`---'`---'`---'
//                                                |
// 

RelationshipNode::RelationshipNode(Node& node)
: mpNode(&node)
{

}

RelationshipNode::RelationshipNode(const Relationship& rel, Node& node)
: mpNode(&node)
{
   *this = rel;
}

//
// Assignment
//

RelationshipNode& RelationshipNode::operator=(const Relationship& rel)
{
   Relationship& target = dynamic_cast<Relationship&>(*this);
   target = rel;

   return *this;
}

//
// From node
//

// Reads a node into relationship.
// Format of node is as follows:
// <relationship>name
//    <id>id</id>
//    <partya>
//       <beingid>id</beingid>
//       <mood>relationshipmood moodcontent</mood>
//    </partya>
//    <partyb>
//       <beingid>id</beingid>
//       <mood>relationshipmood moodcontent</mood>
//    </partyb>
// </relationship>
bool RelationshipNode::fromNode()
{
   if (!mpNode) return false;

   // Name of node should be 'relationship' and doesn't need a value.
   const char* const pName = mpNode->getName();
   const char* const pValue = mpNode->getValue();
   if (!pName) return false;
   if (strncmp(pName, "relationship", 12) != 0) return false;

   reset();

   // Set failure function in case things go wrong.
   auto fail = [&]() -> bool {
      reset();
      return false;
   };

   // Get value as being name.
   strncpy(mName, pValue, 31);

   // Get data.
   std::list<Node>& children = mpNode->getChildren();
   for (Node& child : children) {
      const char* const pName = child.getName();
      const char* const pValue = child.getValue();
      if (!pName) continue;

      if (strncmp(pName, "id", 2) == 0) {
         // id.
         if (!pValue) return fail();
         mId = child.getUint64(pValue);
      } else if (strncmp(pValue, "partya", 6) == 0) {
         // partya.
         if (!fromPartyA(child))
            return fail();
      } else if (strncmp(pValue, "partyb", 6) == 0) {
         // partyb.
         if (!fromPartyB(child))
            return fail();
      }
   }

   // Complete.
   return true;
}

// To node
bool RelationshipNode::toNode()
{
   if (!mpNode) return false;
   mpNode->empty();

   // Set value first.
   if (!mpNode->setValue("relationship", mName)) return false;

   // Fail function.
   auto fail = [&]() -> bool {
      mpNode->empty();
      return false;
   };

   // Spawn children and set their value.
   Node* pId = mpNode->spawnChild();
   Node* pPartyA = mpNode->spawnChild();
   Node* pPartyB = mpNode->spawnChild();
   if (nullptr == pId || nullptr == pPartyA || nullptr == pPartyB)
      return fail();

   Node* pPartyAId = pPartyA->spawnChild();
   Node* pPartyAMood = pPartyA->spawnChild();
   Node* pPartyBId = pPartyB->spawnChild();
   Node* pPartyBMood = pPartyB->spawnChild();

   // Set Id.
   if (!pId->setValue("id", mId)) {
      return fail();
   }

   // Party A.
   if (!pPartyAId->setValue("beingid", mRelData.mBeingRefA.getId())) {
      return fail();
   }
   MoodNode partyA(mRelData.mMoodA, *pPartyAMood);
   if (!partyA.toNode()) {
      return fail();
   }

   // Party B.
   if (!pPartyAId->setValue("beingid", mRelData.mBeingRefB.getId())) {
      return fail();
   }
   MoodNode partyB(mRelData.mMoodB, *pPartyAMood);
   if (!partyB.toNode()) {
      return fail();
   }

   // Node created.
   return true;
}

//
// From node privates
//

bool RelationshipNode::fromPartyA(Node& node)
{
   std::list<Node>& children = node.getChildren();
   for (Node& child : children) {
      const char* const pName = child.getName();
      const char* const pValue = child.getValue();
      if (!pName) continue;

      if (strncmp(pName, "beingid", 8) == 0) {
         mRelData.mBeingRefA.setId(child.getUint64(pValue));
      } else if (strncmp(pName, "mood", 4) == 0) {
         MoodNode n(child);
         if (!n.fromNode())
            return false;

         mRelData.mMoodA = n;
      }
   }

   // Done.
   return true;
}

bool RelationshipNode::fromPartyB(Node& node)
{
   std::list<Node>& children = node.getChildren();
   for (Node& child : children) {
      const char* const pName = child.getName();
      const char* const pValue = child.getValue();
      if (!pName) continue;

      if (strncmp(pName, "beingid", 8) == 0) {
         mRelData.mBeingRefB.setId(child.getUint64(pValue));
      } else if (strncmp(pName, "mood", 4) == 0) {
         MoodNode n(child);
         if (!n.fromNode())
            return false;

         mRelData.mMoodB = n;
      }
   }

   // Done.
   return true;
}
