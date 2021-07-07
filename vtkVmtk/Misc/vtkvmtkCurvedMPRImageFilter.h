// Note: this class was contributed by 
// Hugo Gratama van Andel
// Academic Medical Centre - University of Amsterdam
// Dept. Biomedical Engineering  & Physics
// .NAME vtkvmtkCurvedMPRImageFilter - creates a multiplanar reconstruction of an image along a centerline path.
// .SECTION Description
// ...


#ifndef __vtkvmtkCurvedMPRImageFilter_h
#define __vtkvmtkCurvedMPRImageFilter_h

#include "vtkImageAlgorithm.h"
#include "vtkvmtkWin32Header.h"

#include "vtkImageData.h"
#include "vtkImageChangeInformation.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkTransform.h"
#include "vtkImageReslice.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkCell.h"
#include "vtkMath.h"

class VTK_VMTK_MISC_EXPORT vtkvmtkCurvedMPRImageFilter : public vtkImageAlgorithm
{
public:
  static vtkvmtkCurvedMPRImageFilter *New();

  vtkTypeMacro(vtkvmtkCurvedMPRImageFilter,vtkImageAlgorithm);

  void PrintSelf(ostream& os, vtkIndent indent) override;

  // Description: 
  // Set/Get the centerline along which the MPR should be made
  vtkSetObjectMacro(Centerline,vtkPolyData);
  vtkGetObjectMacro(Centerline,vtkPolyData);
 
  // Description: 
  // Set/Get the Inplane Output Spacing
  vtkSetVector2Macro(InplaneOutputSpacing,double);
  vtkGetVector2Macro(InplaneOutputSpacing,double);
  
  // Description:
  // Set/Get the Inplane Output Size
  vtkSetVector2Macro(InplaneOutputSize,int);
  vtkGetVector2Macro(InplaneOutputSize,int);
      
  // Description:
  // Set/Get the Back Ground Level of the Resliced Data
  vtkSetMacro(ReslicingBackgroundLevel,double);
  vtkGetMacro(ReslicingBackgroundLevel,double);
 
   // Description:
  // Set/Get the name of the FrenetTangentArray
  vtkSetStringMacro(FrenetTangentArrayName);
  vtkGetStringMacro(FrenetTangentArrayName); 

   // Description:
  // Set/Get the name of the TransportNormalsArray
  vtkSetStringMacro(ParallelTransportNormalsArrayName);
  vtkGetStringMacro(ParallelTransportNormalsArrayName); 

   // Description:
  // Get the new OutputExtent, the inplane output extent is set by SetInplaneOutputSize, 
  // the out-of-plane extent is defined by the number of centerline-points
  vtkGetVectorMacro(OutputExtent,int,6);

   // Description:
  // Get the output origin. The point (0,0,0) is situated in the middle of the first MPR image 
  // (at the place of the centerline)The Output Origin is defined by 
  // the InplaneOutputSpacing and the InplaneOutputSize
  vtkGetVectorMacro(OutputOrigin,double,3);

   // Description:
  // Get the output spacing, the inplane output spacing is set by SetInplaneOutputSpacing, 
  // the out-of-plane spacing is defined by the distance between the first two centerline-points
  vtkGetVectorMacro(OutputSpacing,double,3);

 protected:
  vtkvmtkCurvedMPRImageFilter();
  ~vtkvmtkCurvedMPRImageFilter();
//BTX
  template<class T>
  void FillSlice(T* outReslicePtr, T* outputImagePtr, int* resliceUpdateExtent, int* outExtent, vtkIdType* outputInc, int slice);
//ETX
  // Description:
  // This method is called by the superclass and sets the update extent of the input image to the wholeextent 
  virtual int RequestUpdateExtent (vtkInformation *, 
                                   vtkInformationVector **, 
                                   vtkInformationVector *) override;
  // Description:
  // This method is called by the superclass and performs the actual computation of the MPR image
  virtual int RequestData(vtkInformation *, 
                          vtkInformationVector **, 
                          vtkInformationVector *) override;
   // Description:
  // This method is called by the superclass and compute the output extent, origin and spacing
  virtual int RequestInformation  (vtkInformation * vtkNotUsed(request),
                                  vtkInformationVector **inputVector,
                                  vtkInformationVector *outputVector) override;

  vtkPolyData * Centerline;
  double InplaneOutputSpacing[2];
  int InplaneOutputSize[2];
  double ReslicingBackgroundLevel;
  int OutputExtent[6];
  double OutputOrigin[3];
  double OutputSpacing[3];
  char * FrenetTangentArrayName;
  char * ParallelTransportNormalsArrayName;

private:
  vtkvmtkCurvedMPRImageFilter(const vtkvmtkCurvedMPRImageFilter&);  // Not implemented.
  void operator=(const vtkvmtkCurvedMPRImageFilter&);  // Not implemented.
};
#endif
