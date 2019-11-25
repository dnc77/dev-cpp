/*
Date: 31 Oct 2019 19:51:23.464577967
File: serializer.cpp

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
         is stored as two maps, one for child nodes and another for
         attribute key value pairs.

Version control
31 Oct 2019 Duncan Camilleri           Initial development
03 Nov 2019 Duncan Camilleri           spawnChild()
04 Nov 2019 Duncan Camilleri           setValue() can't have nullptr as name
07 Nov 2019 Duncan Camilleri           setValue() for uint32_t
21 Nov 2019 Duncan Camilleri           toString checks length as adding to list
22 Nov 2019 Duncan Camilleri           removed value ''
23 Nov 2019 Duncan Camilleri           setValue bugfix with conflicting key vars
*/

#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <cstdint>
#include <cstdlib>
#include <list>
#include <exception>
#include "stringlist.h"
#include "serializer.h"

using namespace std;

Node::Node(const Node& node)
: mData(node.mData)
{
   *this = node;
}

Node::Node(Node&& node)
: mData(node.mData)
{
   *this = node;
}

Node::Node(StringList<slsiz::large>& data)
: mData(data)
{
   empty();
}

Node::Node(StringList<slsiz::large>& data, const char* const name)
: mData(data)
{
   mName = mData.addString(name, strlen(name));
   mValue = sloffsetbad;
   mChildren.clear();
}

Node::~Node()
{
   empty();
}

Node& Node::operator=(const Node& node)
{
   if (&node == this)
      return *this;

   mData = node.mData;
   mName = node.mName;
   mValue = node.mValue;
   mChildren = node.mChildren;

   return *this;
}

Node& Node::operator=(Node&& node)
{
   if (&node == this)
      return *this;

   mData = node.mData;
   mName = node.mName;
   mValue = node.mValue;
   mChildren = node.mChildren;

   return *this;
}

//
// Access
//

const char* const Node::getName() const
{
   if (sloffsetbad == mName) return nullptr;
   return mData[mName];
}

const char* const Node::getValue() const
{
   if (sloffsetbad == mValue) return nullptr;
   return mData[mValue];
}

// Get the uint32_t equivalent of value. If value is nullptr,
// returns 0.
uint32_t Node::getUint32(const char* const value) const
{
   if (!value) return 0;
   return atoi(value);
}

// Get the uint64_t equivalent of value. If value is nullptr,
// returns 0.
uint64_t Node::getUint64(const char* const value) const
{
   if (!value) return 0;

   // atoll will return a long long here but an implicit cast here
   // will be ok since the bit representation for both types is
   // interchangeable.
   return atoll(value);
}

// Get the double equivalent of value. If value is nullptr,
// returns 0.
double Node::getDouble(const char* const value) const
{
   if (!value) return 0.0;
   return atof(value);
}

std::list<Node>& Node::getChildren()
{
   return mChildren;
}

//
// Data
//

void Node::empty()
{
   mName = sloffsetbad;
   mValue = sloffsetbad;
   mChildren.clear();

   // Do not clear the string list because it may be needed by another node
   // in the same Node tree
}

Node* Node::spawnChild()
{
   try {
      const Node child(mData);
      mChildren.push_back(child);
   } catch (std::exception& e) {
      return nullptr;
   }

   return &mChildren.back();
}

bool Node::setValue(const char* const name, const char* const value)
{
   // Validate.
   if (nullptr == name) return false;

   // Locate or create key in string list.
   sloffset slkey = mData.search(name);
   if (sloffsetbad == slkey) {
      slkey = mData.addString(name, strlen(name));
      if (sloffsetbad == slkey) {
         return false;
      }
   }

   // Locate or create value.
   sloffset slvalue = sloffsetbad;
   if (value != nullptr) {
      slvalue = mData.search(value);
      if (sloffsetbad == slvalue) {
         slvalue = mData.addString(value, strlen(value));
         if (sloffsetbad == slvalue) {
            return false;
         }
      }
   }

   // done.
   mName = slkey;
   mValue = slvalue;
   return true;
}

bool Node::setValue(const char* const name, const uint32_t value)
{
   // Convert value to string.
   char cVal[64];
   memset(cVal, 0, 64);
   sprintf(cVal, "%du", value);

   return setValue(name, cVal);
}

bool Node::setValue(const char* const name, const uint64_t value)
{
   // Convert value to string.
   char cVal[64];
   memset(cVal, 0, 64);
   sprintf(cVal, "%llu", value);

   return setValue(name, cVal);
}

bool Node::setValue(const char* const name, const double value)
{
   // Convert value to string.
   char cVal[64];
   memset(cVal, 0, 64);
   sprintf(cVal, "%f", value);

   return setValue(name, cVal);
}


// Conversion
sloffset Node::toString(StringList<slsiz::large>& target)
{
   const char open = '<';
   const char* term = "</";
   const char close = '>';

   // Fail tasks.
   auto fail = [&]() -> sloffset {
      target.reset();
      return sloffsetbad;
   };

   // Open node '<name>' - '<'.
   sloffset start = target.joinWithLast(&open, 1);
   if (sloffsetbad == start) return fail();

   // Open node '<name>' - name.
   sloffset sl = sloffsetbad;
   size_t len = strlen(mData[mName]);
   if (len > 0) {
      sl = target.joinWithLast(mData[mName], len);
      if (sloffsetbad == sl) return fail();
   }

   // Open node '<name>' - '>'.
   if (sloffsetbad == target.joinWithLast(&close, 1)) {
      return fail();
   }

   // Store value (if it exists).
   if (sloffsetbad != mValue) {
      len = strlen(mData[mValue]);
      if (len > 0) {
         sl = target.joinWithLast(mData[mValue], len);
         if (sloffsetbad == sl) {
            return fail();
         }
      }
   }

   // Children.
   for (auto i : mChildren) {
      if (sloffsetbad == i.toString(target)) {
         return sloffsetbad;  // reset already called!
      }
   }

   // Close node '</name>' - '</'.
   sloffset closeNodeStart = target.joinWithLast(term, 2);
   if (sloffsetbad == closeNodeStart) return fail();

   // Close node '</name>' - name.
   len = strlen(mData[mName]);
   if (len > 0) {
      sl = target.joinWithLast(mData[mName], len);
      if (sloffsetbad == sl) return fail();
   }

   // Close node '</name>' - '>'.
   if (sloffsetbad == target.joinWithLast(&close, 1)) {
      return fail();
   }

   // Done!
   return start;
}

// Parses the string and fills target node. target will be emptied first.
// Note (target's stringlist will be used).
// Will return const char* at the end of the node read in.
// On failure, will return nullptr.
const char* Node::fromString(Node& target, const char* const str)
{
   const char open = '<';
   const char* term = "</";
   const char close = '>';
   StringList<slsiz::large>& data = target.mData;

   // Fail tasks.
   auto fail = [&]() -> const char* {
      target.mName = sloffsetbad;
      target.mValue = sloffsetbad;
      target.mChildren.clear();

      return nullptr;
   };

   // Initialize target first (calling fail() without return does this).
   fail();

   // <name>
   // Locate first open quote.
   const char* pOpen = strchr(str, open);
   if (!pOpen) return nullptr;

   // Locate first close quote to establish name.
   const char* pClose = strchr(pOpen, '>');
   if (!pClose) return nullptr;

   // Name length.
   size_t namelen = pClose - (pOpen + 1);
   target.mName = data.addString(pOpen + 1, namelen);
   if (target.mName == sloffsetbad)
      return nullptr;

   // Establish current pointer (at start just after open).
   const char* pCurrent = pOpen + namelen + 2;

   // <</name>
   // Add closing string.
   sloffset closingString = data.addString(term, 2);
   if (sloffsetbad == closingString) {
      return fail();
   }
   if (sloffsetbad == data.joinWithLast(data[target.mName], namelen)) {
      return fail();
   }
   if (sloffsetbad == data.joinWithLast(&close, 1)) {
      return fail();
   }

   // Locate close.
   const char* pEnd = strstr(pCurrent, data[closingString]);
   if (!pEnd) {
      return fail();
   }

   // Now we have a buffer formatted as follows:
   // <name>...</name>
   // We also have:
   // pOpen -> <name>
   // target.mName -> name
   // pCurrent -> ...
   // pEnd -> </name>

   // Find first '<' followed by '>' before pEnd.
   while (pCurrent < pEnd) {
      const char* pNextChild = strchr(pCurrent, open);
      const char* pNextClose = strchr(pNextChild + 1, close);
      const char* pChildOpen = pNextChild;
      // No more children!
      if (pNextChild == pEnd) {
         int len = pEnd - pCurrent;
         if (target.mValue == sloffsetbad && len > 0) {
            target.mValue = target.mData.addString(pCurrent, len);
         }

         break;
      } else if (pNextChild < pEnd) {
         int len = pNextChild - pCurrent;
         if (target.mValue == sloffsetbad && len > 0) {
            target.mValue = target.mData.addString(pCurrent, len);
         }
      }

      // Ensure there is no other later open brace before close.
      while (pChildOpen < pNextClose && pChildOpen != 0) {
         pChildOpen = strchr(pNextChild + 1, open);
         if (pChildOpen && pChildOpen < pNextClose)
            pNextChild = pChildOpen;
      }

      // Found child?
      if (pNextChild && pNextChild < pEnd) {
         Node n(target.mData);
         pCurrent = fromString(n, pCurrent);
         if (pCurrent == 0)
            return fail();

         // Add Child.
         target.mChildren.push_back(n);
      }
   }

   // Success - return end position of node.
   return pEnd + namelen + 3;
}

bool Node::toFile(const char* const filename)
{
   StringList<slsiz::large> strlist;
   sloffset xml = toString(strlist);
   if (sloffsetbad == xml)
      return false;

   // Open file.
   FILE* f = fopen(filename, "wb");
   if (!f) return false;

   // Write to file.
   const char* pCurrent = strlist[xml];
   size_t total = strlen(pCurrent);
   size_t remaining = total;
   
   // Write bytes.
   while (remaining > 0) {
      size_t written;
      written = fwrite(pCurrent, 1, remaining, f);

      // If written for some reason is negative, don't trust the process
      // and exit.
      if (written <= 0 || ferror(f)) {
         fclose(f);
         return false;
      }

      // Update state.
      remaining -= written;
      pCurrent += written;
   }

   // Close!
   fclose(f);
   return true;
}

bool Node::fromFile(Node& target, const char* const filename)
{
   char buf[1024];
   StringList<slsiz::large> strlist;

   // Open file.
   FILE* f = fopen(filename, "rb");
   if (!f) return false;

   // Fail tasks.
   auto fail = [&]() -> bool {
      fclose(f);
      strlist.reset();
      return false;
   };

   // Read in to stringlist.
   sloffset start = sloffsetbad;
   while (!feof(f)) {
      memset(buf, 0, 1024);
      size_t read = fread(buf, 1, 1024, f);

      // Check file read.
      if (ferror(f)) {
         return fail();
      }

      // Store buffer. Assumed to be string valid always.
      sloffset pos = strlist.joinWithLast(buf, read);
      if (start == sloffsetbad)
         start = pos;

      if (sloffsetbad == pos) {
         return fail();
      }
   }

   // Done!
   fclose(f);
   return fromString(target, strlist[start]);
}
