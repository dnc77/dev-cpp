/*
Date: 05 Nov 2019 20:19:17.990153498
File: objref.cpp

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

Purpose: An object reference is just an id for an object which may or may not
         yet be loaded in memory. The purpose of this class is to allow for
         objects to be referenced before they are actually loaded in memory.
         A load function can also be provided to allow for later loading. The
         purpose of this is to maintain a lot of instances of the same object
         that may not yet be loaded. This is different from a smart pointer in
         that an object may not exist and the object reference is not
         responsible for destroying the object.

Version control
05 Nov 2019 Duncan Camilleri           Initial development
25 Nov 2019 Duncan Camilleri           Added default constructor

*/

#include <cstdint>
#include "objref.h"

template <class T>
ObjRef<T>::ObjRef()
: mId(0), mpObj(nullptr), mLoader(nullptr), mpUserdata(nullptr)
{
}

template <class T>
ObjRef<T>::ObjRef(uint64_t id)
: mId(id), mpObj(nullptr), mLoader(nullptr), mpUserdata(nullptr)
{
}

template <class T>
ObjRef<T>::ObjRef(T* pObj, uint64_t id)
: mId(id), mpObj(pObj), mLoader(nullptr), mpUserdata(nullptr)
{
}

template <class T>
ObjRef<T>::ObjRef(const ObjRef<T>& objref)
{
   *this = objref;
}

//
// Operators
//

template <class T>
ObjRef<T>& ObjRef<T>::operator=(const ObjRef<T>& objref)
{
   if (&objref == this)
      return *this;

   mId = objref.mId;
   mpObj = objref.mpObj;
   mpUserdata = objref.mpUserdata;
   mLoader = objref.mLoader;

   return *this;
}

//
// Accessors
//

template <class T>
void ObjRef<T>::setLoadObjRef(LoadObjRef<T> load /*= nullptr*/,
   void* pUserdata /*= nullptr*/)
{
   mLoader = load;
}

// When changing the id of the contained object, all member parameters
// need to be reset as dependent data may need to change as well.
template <class T>
void ObjRef<T>::setId(uint64_t id)
{
   uint64_t oldId = mId;
   mId = id;

   // Cleanup if id changed.
   if (oldId != mId && mpObj != nullptr) {
      mpObj = nullptr;
      mpUserdata = nullptr;
      mLoader = nullptr;
   }
}

template <class T>
uint64_t ObjRef<T>::getId() const
{
   return mId;
}

// Returns the loaded object. If the reference knows about it.
// If a loader function exists, tries to load the object and
// returns it's loaded object (if it was successful). Otherwise,
// returns nullptr when object is not present.
template <class T>
T* ObjRef<T>::getObj()
{
   // If object exists, just return it.
   if (mpObj) return mpObj;

   // If object does not exist and a loader function exists, try to load.
   if (mLoader != nullptr) {
      return mLoader(mId, mpUserdata);
   }

   // No object to return - nullptr.
   return nullptr;
}
