/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkMacros.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstk_Macros_h_
#define __igstk_Macros_h_

#include "itkmacro.h"

/**
  igstkMacro.h defines standard system-wide macros, constants, and other 
  common parameters in the IGstk Library. It includes the Get, Set and
  Log macros.
*/

namespace igstk
{

#define igstkLogMacro( x, y)  \
{         \
    if (this->GetLogger() && (this->GetLogger()->GetPriorityLevel()>= x) ) \
    {  \
        *(this->GetLogger()) << y; \
    } \
}


/** Set built-in type.  Creates member Set"name"() (e.g., SetTimeStep(time)); */
#define  SetMacro(name,type) \
virtual void Set##name (const type _arg) \
{ \
    if (this->m_##name != _arg) \
    { \
        this->m_##name = _arg; \
    } \
} 


/** Get built-in type.  Creates member Get"name"() (e.g., GetTimeStep(time)); */
#define GetMacro(name,type) \
virtual type Get##name () \
{ \
    return this->m_##name; \
}






}
#endif // __igstk_Macros_h_