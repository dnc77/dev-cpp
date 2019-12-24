/*
Date: 05 Nov 2019 20:19:11.012123215
File: objref.h

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
11 Dec 2019 Duncan Camilleri           Alterations to load and added unload
18 Dec 2019 Duncan Camilleri           Added set()

*/

#ifndef __OBJREF_H_508BC7820E23F9ECDC1F73EB27A6B36E__
#define __OBJREF_H_508BC7820E23F9ECDC1F73EB27A6B36E__

#if not defined _GLIBCXX_CSTDINT
#error "objref.h: missing include - cstdint"
#endif

// Function pointers to allow ObjRef to load/unload an object of a specific
// id using user related data as needed. This is to be provided by the
// user if ObjRef is to load any objects in memory or return any already loaded
// objects. Any memory allocated with the load function should be freed with the
// unload function. Typically, the objects may already be loaded by other
// mechanisms, in which case, the unload function UnloadObjRef need not be set.
// User is responsible for making the right calls to UnloadObjRef and ensuring
// all ObjRef do not dereference any freed data.
template <class T> class ObjRef;
template <class T>
using LoadObjRef = T*(*)(uint64_t id);
template <class T>
using UnloadObjRef = void(*)(ObjRef<T>& obj);

template <class T>
class ObjRef
{
public:
   ObjRef();
   ObjRef(uint64_t id);
   ObjRef(T* pObj, uint64_t id);
   ObjRef(const ObjRef& objref);

   // Operators
   ObjRef& operator=(const ObjRef& objref);

   // Accessors
   void set(T* pObj, uint64_t id);
   void setId(uint64_t id);
   void dereference();
   uint64_t getId() const;
   T* getObj();

protected:
   uint64_t mId;
   T* mpObj;

public:
   // Static object loading functions.
   static LoadObjRef<T> mLoader;
   static UnloadObjRef<T> mUnloader;
   static void* mpUserdata;
};

#endif   // __OBJREF_H_508BC7820E23F9ECDC1F73EB27A6B36E__
