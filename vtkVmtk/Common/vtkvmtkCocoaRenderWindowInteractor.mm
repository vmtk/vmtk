/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkCocoaRenderWindowInteractor.mm,v $
  Language:  C++
  Date:      $Date: 2010/01/08 16:46:43 $
  Version:   $Revision: 1.4 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#import "vtkvmtkCocoaServer.h"
#import "vtkvmtkCocoaRenderWindowInteractor.h"
#import "vtkObjectFactory.h"

vtkCxxRevisionMacro(vtkvmtkCocoaRenderWindowInteractor, "$Revision: 1.70 $");
vtkStandardNewMacro(vtkvmtkCocoaRenderWindowInteractor);

vtkvmtkCocoaRenderWindowInteractor::vtkvmtkCocoaRenderWindowInteractor()
{
  this->CloseWindowOnTerminateApp = 0;
}

vtkvmtkCocoaRenderWindowInteractor::~vtkvmtkCocoaRenderWindowInteractor()
{
}

void vtkvmtkCocoaRenderWindowInteractor::Close()
{
  vtkCocoaRenderWindow *renWin = vtkCocoaRenderWindow::SafeDownCast(this->RenderWindow);
  if (renWin)
    {
    vtkCocoaServer *server = reinterpret_cast<vtkCocoaServer *>(this->GetCocoaServer());
    [server close];
    this->SetCocoaServer(NULL);
    }
}

void vtkvmtkCocoaRenderWindowInteractor::TerminateApp()
{
  vtkCocoaRenderWindow *renWin = vtkCocoaRenderWindow::SafeDownCast(this->RenderWindow);
  if (renWin)
    {
    int windowCreated = renWin->GetWindowCreated();
    if (windowCreated)
      {
      vtkCocoaServer *server = reinterpret_cast<vtkCocoaServer *>(this->GetCocoaServer());
      if (this->CloseWindowOnTerminateApp)
        {
        [server close];
        this->SetCocoaServer(NULL);
        }
      else
        {
        [server stop];
        }
      }
    else
      {
      [NSApp terminate:NSApp];
      }
    }
}

void vtkvmtkCocoaRenderWindowInteractor::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

