/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkView3D.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkView3D.h"

namespace igstk{

/** Constructor */
View3D::View3D( int x, int y, int w, int h, const char *l ) : View(x,y,w,h,l)
{
}

/** Destructor */
View3D::~View3D()
{
}

/** Main FLTK event handler */
int View3D::handle( int event ) 
{
  return View::handle( event );
}


} // end namespace igstk
