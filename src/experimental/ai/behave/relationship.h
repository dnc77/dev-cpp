/*
Date: 29 Nov 2019 12:43:21.815697577
File: relationship.h

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
13 Jan 2020 Duncan Camilleri           Impact model changes

*/

#ifndef __RELATIONSHIP_H_F256603D5EE1002DE62E21BDC7758768__
#define __RELATIONSHIP_H_F256603D5EE1002DE62E21BDC7758768__

// Check for missing includes.
#if not defined _GLIBCXX_CSTDINT
#error "relationship.h: missing include - cstdint"
#elif not defined __OBJREF_H_508BC7820E23F9ECDC1F73EB27A6B36E__
#error "relationship.h: missing include - objref.h"
#elif not defined __STRINGLIST_H_3F91B722978DC8F00D1E31EF68DC35C8__
#error "relationship.h: missing include - stringlist.h"
#elif not defined __SERIALIZER_H_B51368F9355D0A6C0E780E1F8D197E39__
#error "relationship.h: missing include - serializer.h"
#elif not defined __MOOD_H_AAB57A66720F9EDAC5D0668C603393AB__
#error "relationship.h: missing include - mood.h"
#elif not defined __ACTION_H_BA0DD2641DF3D0A449B1FA2732A6432E__
#error "relationship.h: missing include - action.h"
#elif not defined __BEING_H_B969CD487F2BA6EDED6F3DC728739CDC__
#error "relationship.h: missing include - being.h"
#elif not defined _GLIBCXX_LIST
#error "relationship.h: missing include - list"
#endif

//
// Relationship
//

typedef struct _reldata {
   ObjRef<const Being> mBeingRefA;
   ObjRef<const Being> mBeingRefB;
   Mood mMoodA;
   Mood mMoodB;
} RelationshipData;

class RelationshipNode;
class Relationship
{
public:
   // Construction
   Relationship();
   Relationship(RelationshipData& rd);
   Relationship(const Relationship& rel);
   virtual ~Relationship();

   // Accessors
   const uint64_t& id() const;
   const RelationshipData& getData() const;
   const Being* getPartner(const Being& counterpart);

   // Assignments
   Relationship& operator=(const Relationship& rel);
   Relationship& operator=(const RelationshipData& reldata);
   Relationship& operator=(const RelationshipNode& node);

   // Data
   void reset();

   // Naming
   void name(const uint64_t id, const char* const name);

   // Mood shifting
   void impact(const Being& actor, const Action& action);

protected:
   uint64_t mId;
   char mName[32];
   RelationshipData mRelData;

public:
   static intensity mCfgRelationshipImpactingMoodMinIntensity;
   static intensity mCfgRelationshipImpactingMoodMaxIntensity;
};

//
// RelationshipNode
//

class RelationshipNode : public Relationship
{
public:
   RelationshipNode(Node& node);
   RelationshipNode(const Relationship& rel, Node& node);

   // Assignment
   RelationshipNode& operator=(const Relationship& rel);

   // Access
   Node& getNode()                     { return *mpNode; }

   // From node
   bool fromNode();

   // To node
   bool toNode();

protected:
   Node* mpNode;

   // From node privates
   bool fromPartyA(Node& node);
   bool fromPartyB(Node& node);
};

#endif   // __RELATIONSHIP_H_F256603D5EE1002DE62E21BDC7758768__
