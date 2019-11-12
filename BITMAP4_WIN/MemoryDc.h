/* 
   MemoryDc.h

   Copyright (C) 2002-2004 Ren� Nyffenegger

   This source code is provided 'as-is', without any express or implied
   warranty. In no event will the author be held liable for any damages
   arising from the use of this software.

   Permission is granted to anyone to use this software for any purpose,
   including commercial applications, and to alter it and redistribute it
   freely, subject to the following restrictions:

   1. The origin of this source code must not be misrepresented; you must not
      claim that you wrote the original source code. If you use this source code
      in a product, an acknowledgment in the product documentation would be
      appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
      misrepresented as being the original source code.

   3. This notice may not be removed or altered from any source distribution.

   Ren� Nyffenegger rene.nyffenegger@adp-gmbh.ch
*/


#ifndef _MEM_DC_H_
#define _MEM_DC_H_

class Bitmap;

#include "Dc.h"

/*! The class MemoryDc encapsulates a memory device context.  */
class MemoryDc : public Dc{
  public:
    /*! This constructor creates a memory device context that is compatible
        with the screen, using CreateCompatibleDC(0), which is described at
        http://msdn.microsoft.com/library/en-us/gdi/devcons_499f.asp?frame=true */
    MemoryDc();

    using Dc::Select;
    Bitmap Select(Bitmap&);

   virtual ~MemoryDc();
};

#endif

