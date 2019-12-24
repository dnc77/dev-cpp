/*
Date: 31 Oct 2019 19:51:13.194153595
File: serializer.h

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

Purpose: Node is an xml storage and retrieval class. It uses a common
         StringList between all nodes to store all contents of the xml buffer.
         Each node (xml parent) can generate another stringlist that will
         contain itself as a whole string if requested. In memory, a node
         has a name, a value and a potential number of child nodes.

Version control
31 Oct 2019 Duncan Camilleri           Initial development
03 Nov 2019 Duncan Camilleri           spawnChild()
07 Nov 2019 Duncan Camilleri           setValue() for uint32_t
05 Dec 2019 Duncan Camilleri           bool value support

*/

#ifndef __SERIALIZER_H_B51368F9355D0A6C0E780E1F8D197E39__
#define __SERIALIZER_H_B51368F9355D0A6C0E780E1F8D197E39__

#if not defined _GLIBCXX_CSTDINT
#error "serializer.h: missing include - cstdint"
#elif not defined _GLIBCXX_LIST
#error "serializer.h: missing include - list"
#elif not defined __STRINGLIST_H_3F91B722978DC8F00D1E31EF68DC35C8__
#error "serializer.h: missing include - stringlist.h"
#endif

class Node
{
public:
   Node(const Node& node);
   Node(Node&& node);
   Node(StringList<slsiz::large>& data);
   Node(StringList<slsiz::large>& data, const char* const name);
   virtual ~Node();

   Node& operator=(const Node& node);
   Node& operator=(Node&& node);

   // Access
   const char* const getName() const;
   const char* const getValue() const;
   bool getBoolValue() const;
   uint32_t getUint32(const char* const value) const;
   uint64_t getUint64(const char* const value) const;
   double getDouble(const char* const value) const;

   std::list<Node>& getChildren();

   // Data
   void empty();
   Node* spawnChild();
   bool setValue(const char* const name, const char* const value);
   bool setValue(const char* const name, const bool value);
   bool setValue(const char* const name, const uint32_t value);
   bool setValue(const char* const name, const uint64_t value);
   bool setValue(const char* const name, const double value);
   

   // Conversion
   sloffset toString(StringList<slsiz::large>& target);
   static const char* fromString(Node& target, const char* const str);
   bool toFile(const char* const filename);
   static bool fromFile(Node& target, const char* const filename);

protected:
   StringList<slsiz::large>& mData;

   sloffset mName;
   sloffset mValue;
   std::list<Node> mChildren;
};

#endif   // __SERIALIZER_H_B51368F9355D0A6C0E780E1F8D197E39__
