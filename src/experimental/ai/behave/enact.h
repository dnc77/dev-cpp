/*
Date: 09 Dec 2019 12:51:11.852345965
File: enact.h

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
17 Jan 2020 Duncan Camilleri           Introduced generic randomize function

*/

#ifndef __ENACT_H_694DF6A9040A307F58D229F7FB958694__
#define __ENACT_H_694DF6A9040A307F58D229F7FB958694__

#if not defined _GLIBCXX_CSTDINT
#error "enact.h: missing include - cstdint"
#elif not defined _GLIBCXX_LIST
#error "enact.h: missing include - list"
#elif not defined _GLIBCXX_STRING
#error "enact.h: missing include - string"
#elif not defined _GLIBCXX_MUTEX
#error "enact.h: missing include - mutex"
#elif not defined __OBJREF_H_508BC7820E23F9ECDC1F73EB27A6B36E__
#error "enact.h: missing include - objref.h"
#elif not defined __STRINGLIST_H_3F91B722978DC8F00D1E31EF68DC35C8__
#error "enact.h: missing include - stringlist.h"
#elif not defined __SERIALIZER_H_B51368F9355D0A6C0E780E1F8D197E39__
#error "enact.h: missing include - serializer.h"
#elif not defined __MOOD_H_AAB57A66720F9EDAC5D0668C603393AB__
#error "enact.h: missing include - mood.h"
#elif not defined __ACTION_H_BA0DD2641DF3D0A449B1FA2732A6432E__
#error "enact.h: missing include - action.h"
#elif not defined __ENVIRONMENT_H_98216C8B541FBFB5FD5CA3DC5B6355BF__
#error "enact.h: missing include - environment.h"
#elif not defined __BEING_H_B969CD487F2BA6EDED6F3DC728739CDC__
#error "enact.h: missing include - being.h"
#elif not defined __GATHERING_H_7C194EDF338C8266B57A8FB4606C6495__
#error "enact.h: missing include - gathering.h"
#elif not defined __RELATIONSHIP_H_F256603D5EE1002DE62E21BDC7758768__
#error "enact.h: missing include - relationship.h"
#elif not defined __BEHAVEFACTORY_H_F15CB7E7A7397E6132CB1BCC3C95F0B9__
#error "enact.h: missing include - behavefactory.h"
#endif

typedef struct _EnactCallbacks {
   bool (*actionDecided)(const Action& action,
      const Being* pInstigator, const Being* pRecipient,
      const Relationship* pRelationship, bool willAct);
   bool (*actionTaken)(const Action& action,
      const Being* pInstigator, const Being* pRecipient,
      const Relationship* pRelationship);
} EnactCallbacks;

//
// Enact class
//

class Enact
{
public:
   // Construction
   Enact(BehaveFactory& factory);
   virtual ~Enact();

   // Accessors
   void setCallbacks(EnactCallbacks* pecb);

   // Action
   bool enact(const Action& action,
      Being* pInstigator, Being* pRecipient = nullptr);
   void enactAll();

protected:
   EnactCallbacks mCallbacks;
   BehaveFactory& mFactory;
   std::mutex mEnactSync;

   bool enactWithBeings(const Action& a, Being& instigator);

private:
   // To library.
   bool randomize(std::list<ObjRef<const Action>>& lst, int times = 2);
};


#endif   // __ENACT_H_694DF6A9040A307F58D229F7FB958694__

