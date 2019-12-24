/*
Date: 07 Nov 2019 11:40:14.507053915
File: objrefinstances.cpp

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

Purpose: This is a temporary file which gathers all ObjRef instances
         together. Since ObjRef is a template class it requires instantiation
         for each type. This is usually done at the top of the CPP but since
         dependent classes of ObjRef can't be determined at ObjRef, this has
         to be done outside. Usually this is done in the cpp file containing
         the object of instantiation however this is not working here so this
         file is created for ObjRef specifically and temporarily. Usually this
         can be resolved by changing order the files get compiled, but in this
         case the linker still complains about missing references. Later, when
         everything is split into separate libraries, it is envisaged the issue
         will be resolved.

Version control
07 Nov 2019 Duncan Camilleri           Initial development
16 Dec 2019 Duncan Camilleri           Environments added

*/

#include <assert.h>
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
#include "gathering.h"

// Include source files here.
#include "objref.cpp"
template class ObjRef<Action>;
template class ObjRef<const Action>;
template class ObjRef<Being>;
template class ObjRef<const Being>;
template class ObjRef<Gathering>;
template class ObjRef<const Gathering>;
template class ObjRef<Environment>;
template class ObjRef<const Environment>;
