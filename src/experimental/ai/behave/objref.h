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

*/

#ifndef __OBJREF_H_508BC7820E23F9ECDC1F73EB27A6B36E__
#define __OBJREF_H_508BC7820E23F9ECDC1F73EB27A6B36E__

#if not defined _GLIBCXX_CSTDINT
#error "objref.h: missing include - cstdint"
#endif

// Function pointer to allow ObjRef to load an object of a specific
// id using user related data as needed. This is to be provided by the
// user if ObjRef is to load any objects in memory. User is responsible
// for freeing any loaded objects.
template <class T>
using LoadObjRef = T*(*)(uint64_t id, void* userptr);

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
   void setLoadObjRef(LoadObjRef<T> load = nullptr, void* pUserdata = nullptr);
   void setId(uint64_t id);
   uint64_t getId() const;
   T* getObj();

protected:
   uint64_t mId;
   T* mpObj;
   void* mpUserdata;

   // Object load function.
   LoadObjRef<T> mLoader = nullptr;
};

#endif   // __OBJREF_H_508BC7820E23F9ECDC1F73EB27A6B36E__
