#include "vtkvmtkCurvedMPRImageFilter.h"
#include "vtkObjectFactory.h"
#include "vtkVersion.h"

//------------------------------------------------------------------------------


vtkStandardNewMacro(vtkvmtkCurvedMPRImageFilter);

vtkvmtkCurvedMPRImageFilter::vtkvmtkCurvedMPRImageFilter()
{
  this->Centerline = NULL;
  this->InplaneOutputSpacing[0] = 1.0;
  this->InplaneOutputSpacing[1] = 1.0;
  this->InplaneOutputSize[0] = 100;
  this->InplaneOutputSize[1] = 100;
  this->ReslicingBackgroundLevel = 0.0;

  for (int i = 0;i<6;i++)
    {
    this->OutputExtent[i] = 0;
    }
  for (int i = 0;i<3;i++)
    {
    this->OutputOrigin[i] = 0.0;
    }
  for (int i = 0;i<3;i++)
    {
    this->OutputSpacing[i] = 0.0;
    }
  
  this->FrenetTangentArrayName = NULL;
  this->ParallelTransportNormalsArrayName = NULL;
}


vtkvmtkCurvedMPRImageFilter::~vtkvmtkCurvedMPRImageFilter()
{
  if (this->Centerline)
    {
    this->Centerline->Delete();
    this->Centerline = NULL;
    }

  if (this->FrenetTangentArrayName)
    {
    delete[] this->FrenetTangentArrayName;
    this->FrenetTangentArrayName = NULL;
    }
  
  if (this->ParallelTransportNormalsArrayName)
    {
    delete[] this->ParallelTransportNormalsArrayName;
    this->ParallelTransportNormalsArrayName = NULL;
    }
}

//This is called by the superclass
//----------------------------------------------------------------------------
int vtkvmtkCurvedMPRImageFilter::RequestUpdateExtent (
  vtkInformation * vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *vtkNotUsed( outputVector ))
{
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);

  // always request the whole extent
  inInfo->Set(vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT(),
              inInfo->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT()),6);
  return 1;
}

//----------------------------------------------------------------------------
// Compute new extent
int vtkvmtkCurvedMPRImageFilter::RequestInformation (
  vtkInformation * vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  // get the info objects
  vtkInformation* outInfo = outputVector->GetInformationObject(0);
  vtkImageData *outputImage = vtkImageData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkInformation *inInfoImage = inputVector[0]->GetInformationObject(0);
  vtkImageData *inputImage = vtkImageData::SafeDownCast(inInfoImage->Get(vtkDataObject::DATA_OBJECT()));

  if (!this->Centerline)
    {
    vtkErrorMacro(<<"Centerline not set");
    return 1;
    }

  //Check if the inputs are NULL 
  if (inputImage == NULL || this->Centerline  == NULL || outputImage==NULL)
    {
    if (outputImage)
      {
      int outputExtent[6] = {0,-1,0,-1,0,-1};
      outputImage->SetExtent(outputExtent);
#if (VTK_MAJOR_VERSION <= 5)
      outputImage->SetWholeExtent(outputExtent);
      outputImage->SetUpdateExtent(outputExtent);
      outputImage->AllocateScalars();
#elif (VTK_MAJOR_VERSION < 7)
      this->UpdateInformation();
      vtkStreamingDemandDrivenPipeline::SetUpdateExtent(outInfo, outputExtent);
      this->Update();
      outputImage->AllocateScalars(outInfo);
#else
      if (this->GetOutputInformation(0))
        {
        this->GetOutputInformation(0)->Set(
          vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT(),
        this->GetOutputInformation(0)->Get(
          vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT()), 6);
        }
      outputImage->AllocateScalars(outInfo);
#endif

      }
    if ( inputImage == NULL)
      {
      vtkErrorMacro(<< "RequestInformation: InputImageData is not set.");
      return 1;
      }
    else //Centerline==NULL
      {
      vtkErrorMacro(<< "RequestInformation: Centerline is not set.");
      return 1;
      }
    }

  int firstLineInCells;
  vtkCell* line = NULL;
  //get the first line in the vktPolyData
  for (firstLineInCells=0; firstLineInCells < this->Centerline->GetNumberOfCells(); firstLineInCells++)
    {
    line = this->Centerline->GetCell(firstLineInCells);
    if (line->GetCellType() == VTK_LINE || line->GetCellType() == VTK_POLY_LINE)
      {
      break;
      }
    }
  if (!line || (!(firstLineInCells < (int) this->Centerline->GetNumberOfCells())))
    {
    vtkErrorMacro(<< "RequestInformation: no line in input vtkPolyData.");
    return 1;
    }

  vtkPoints* linePoints = line->GetPoints(); 

  int numberOfLinePoints = linePoints->GetNumberOfPoints();

  if (numberOfLinePoints < 2)
    {
    vtkErrorMacro(<< "RequestInformation: Too few points in Centerline.");
    return 1;
    }

  int inDataExtent[6];
  double inDataOrigin[3];
#if (VTK_MAJOR_VERSION <= 5)
  inputImage->GetWholeExtent(inDataExtent);
#else
  inInfoImage->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), inDataExtent);
#endif


  inputImage->GetSpacing(this->OutputSpacing);
  inputImage->GetOrigin(inDataOrigin);
  
  this->OutputExtent[0] = 0;
  this->OutputExtent[1] = this->InplaneOutputSize[0]-1;
  this->OutputExtent[2] = 0; 
  this->OutputExtent[3] = this->InplaneOutputSize[1]-1;
  this->OutputExtent[4] = 0;
  this->OutputExtent[5] = numberOfLinePoints-1;
  
  double firstPoint[3], secondPoint[3], zspace;
  linePoints->GetPoint(0,firstPoint);
  linePoints->GetPoint(1,secondPoint);
  
  zspace = sqrt(vtkMath::Distance2BetweenPoints(firstPoint,secondPoint));
  
  this->OutputSpacing[0]= this->InplaneOutputSpacing[0];
  this->OutputSpacing[1]= this->InplaneOutputSpacing[1];
  this->OutputSpacing[2]= zspace;

  this->OutputOrigin[0]=-1*((this->OutputExtent[1]-this->OutputExtent[0])/2)*this->OutputSpacing[0];
  this->OutputOrigin[1]=-1*((this->OutputExtent[3]-this->OutputExtent[2])/2)*this->OutputSpacing[1];
  this->OutputOrigin[2]=0;

  outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(),this->OutputExtent,6);
  outInfo->Set(vtkDataObject::SPACING(),this->OutputSpacing,3);
  outInfo->Set(vtkDataObject::ORIGIN(),this->OutputOrigin,3);

  return 1;
}

template <class T>
void vtkvmtkCurvedMPRImageFilter::FillSlice(T* outReslicePtr, T* outputImagePtr, int* resliceUpdateExtent, int* outExtent, vtkIdType* outputInc, int slice)
{
  //write result to the new dataset
  int size = (outExtent[1]-outExtent[0]+1)*(outExtent[3]-outExtent[2]+1);
  for(int i=0; i<size; i++)
    {
    outputImagePtr[i + (slice-outExtent[4]) * outputInc[2]] = outReslicePtr[i];
    }
}

//----------------------------------------------------------------------------
int vtkvmtkCurvedMPRImageFilter::RequestData(
  vtkInformation* vtkNotUsed( request ),
  vtkInformationVector** inputVector,
  vtkInformationVector* outputVector)
{
  // get the info objects
  vtkInformation* outInfo = outputVector->GetInformationObject(0);
  vtkImageData *outputImage = vtkImageData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkInformation *inInfoImage = inputVector[0]->GetInformationObject(0);
  vtkImageData *inputImage = vtkImageData::SafeDownCast(inInfoImage->Get(vtkDataObject::DATA_OBJECT()));
 
  if (!this->Centerline)
    {
    vtkErrorMacro(<<"Centerline not set");
    return 1;
    }

  if (inputImage == NULL)
    {
    vtkErrorMacro(<< "RequestData: InputImageData is not set.");
    return 1;
    }

  //check if everything is allright
  if (inputImage == NULL || outputImage==NULL)
    {
    if (outputImage)
      {
      int outputExtent[6] = {0,-1,0,-1,0,-1};
      outputImage->SetExtent(outputExtent);
#if (VTK_MAJOR_VERSION <= 5)
      outputImage->SetWholeExtent(outputExtent);
      outputImage->SetUpdateExtent(outputExtent);
      outputImage->AllocateScalars();
#elif (VTK_MAJOR_VERSION < 7)
      this->UpdateInformation();
      vtkStreamingDemandDrivenPipeline::SetUpdateExtent(outInfo, outputExtent);
      this->Update();
      outputImage->AllocateScalars(outInfo);
#else
      if (this->GetOutputInformation(0))
        {
        this->GetOutputInformation(0)->Set(
          vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT(),
          this->GetOutputInformation(0)->Get(
          vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT()), 6);
        }
      outputImage->AllocateScalars(outInfo);
#endif
      }
   
    }

  //check if FrenetTangent s are available
  if ( this->FrenetTangentArrayName  == NULL)
    {
    vtkErrorMacro(<< "RequestData: FrenetTangentArrayName is not set.");
    return 1;
    }

  vtkDataArray* frenetArray = this->Centerline->GetPointData()->GetArray(this->FrenetTangentArrayName);
  if (!frenetArray)
    {
    vtkErrorMacro(<< "RequestData: InputPolyData does not contain an array with the specified FrenetTangentArrayName.");
    return 1;
    }

  //check if ParallelTransportNormals are available
  if ( this->ParallelTransportNormalsArrayName  == NULL)
    {
    vtkErrorMacro(<< "RequestData: ParallelTransportNormalsArrayName is not set.");
    return 1;
    }

  //check if a Centerline is available
  vtkDataArray* parallelArray = this->Centerline->GetPointData()->GetArray(this->ParallelTransportNormalsArrayName);
  if (!parallelArray)
    {
    vtkErrorMacro(<< "RequestData: InputPolyData does not contain an array with the specified ParallelTransportNormalsArrayName.");
    return 1;
    }

  int firstLineInCells;
  vtkCell* line = NULL;
  //get the first line in the vktPolyData
  for (firstLineInCells=0; firstLineInCells<this->Centerline->GetNumberOfCells(); firstLineInCells++)
    {
    line = this->Centerline->GetCell(firstLineInCells);
    if (line->GetCellType() == VTK_LINE || line->GetCellType() == VTK_POLY_LINE)
      {
      break;
      }
    }

  if (!line || (!(firstLineInCells < (int) this->Centerline->GetNumberOfCells())))
    {
    vtkErrorMacro(<< "RequestData: no line in input vtkPolyData.");
    return 1;
    }

  if (line->GetNumberOfPoints() < 2)
    {
    vtkErrorMacro(<< "RequestData: Too few points in Centerline.");
    return 1;
    }

  vtkPoints* linePoints = line->GetPoints(); 

  int outExtent[6];
#if (VTK_MAJOR_VERSION <= 5)
  outputImage->GetUpdateExtent(outExtent);
#else
  outInfo->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), outExtent);
#endif

  outputImage->SetExtent(outExtent);
#if (VTK_MAJOR_VERSION <= 5)
  outputImage->SetWholeExtent(outExtent);
  outputImage->SetUpdateExtent(outExtent);
  outputImage->AllocateScalars();
#elif (VTK_MAJOR_VERSION < 7)
  this->UpdateInformation();
  vtkStreamingDemandDrivenPipeline::SetUpdateExtent(outInfo, outExtent);
  this->Update();  
  outputImage->AllocateScalars(outInfo);
#else
if (this->GetOutputInformation(0))
  {
  this->GetOutputInformation(0)->Set(
    vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT(),
    this->GetOutputInformation(0)->Get(
    vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT()), 6);
  }
#endif

  vtkDataArray* frenetTangentArray = this->Centerline->GetPointData()->GetArray(this->FrenetTangentArrayName); 
  vtkDataArray* parallelTransportNormalsArray = this->Centerline->GetPointData()->GetArray(this->ParallelTransportNormalsArrayName); 
 
  //start computing
  int inExtent[6];
#if (VTK_MAJOR_VERSION <= 5)
  inputImage->GetWholeExtent(inExtent);
#else
  inInfoImage->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), inExtent);
#endif

  // if the input extent is empty then exit
  if (inExtent[1] < inExtent[0] ||
      inExtent[3] < inExtent[2] ||
      inExtent[5] < inExtent[4])
    {
    return 1;
    }

  vtkImageReslice* reslice = vtkImageReslice::New();
  reslice->SetOutputDimensionality(2);
#if (VTK_MAJOR_VERSION <= 5)
  reslice->SetInput(inputImage);
#else
  reslice->SetInputData(inputImage);
#endif
  reslice->SetInterpolationModeToCubic();
  //turn off transformation of the input spacin, origin and extent, so we can define what we want
  reslice->TransformInputSamplingOff();
  //set the value of the voxels that are out of the input data
  reslice->SetBackgroundLevel(this->ReslicingBackgroundLevel);
  //set the outputspacing
  //reslice->SetOutputSpacing(this->OutputSpacing[0],this->OutputSpacing[1],this->OutputSpacing[2]);
  reslice->SetOutputSpacing(this->OutputSpacing);
// going to MPR
  for (int slice=outExtent[4]; slice<(outExtent[5] + 1);slice++) 
    {// for each slice (or point on the line)
    double centerSlice[3];
    linePoints->GetPoint(slice,centerSlice);

    // To calculate the outputorigin & the necessarry transform 
    // the vectors are retreived from the array's
    //t is the vector in the direction of the Centerline, so along the z-axis in the MPR volume
    double t[3];
    frenetTangentArray->GetTuple(slice,t);

    //p is a normal of the Centerline, directed to the 'North' direction of the inputvolume,in the MPR volume this will be along the x-axis 
    double p[3];
    parallelTransportNormalsArray->GetTuple(slice,p);

    double tp[3];
    //tp is the crossproduct of  t and p, and will be directed to the 'West' direction of the inputvolume,in the MPR volume this will be along the y-axis 
    tp[0] = (t[1]*p[2]- t[2]*p[1]); 
    tp[1] = (t[2]*p[0]- t[0]*p[2]); 
    tp[2] = (t[0]*p[1]- t[1]*p[0]); 

    //set the axis of the first slice according to the vectors of the first point in the line
    reslice->SetResliceAxesDirectionCosines(p[0],p[1],p[2],tp[0],tp[1],tp[2],t[0],t[1],t[2]);
    // the firstPoint on the Centerline coresponds to the origin of the output axes unit vectors
    reslice->SetResliceAxesOrigin(centerSlice[0],centerSlice[1],centerSlice[2]); 
    //the outputextent will be one slice
    reslice->SetOutputExtent(this->OutputExtent[0],this->OutputExtent[1],this->OutputExtent[2],this->OutputExtent[3],0,0);
    //the outputorigin in the input coordinate system is set to zero
    double outputorigin[3];
    outputorigin[0] = 0.0;
    outputorigin[1] = 0.0;
    outputorigin[2] = 0.0;

    reslice->SetOutputOrigin(this->OutputOrigin);
    reslice->Update();
        
    int resliceUpdateExtent[6];
    resliceUpdateExtent[0] = outExtent[0];
    resliceUpdateExtent[1] = outExtent[1];
    resliceUpdateExtent[2] = outExtent[2];
    resliceUpdateExtent[3] = outExtent[3];
    resliceUpdateExtent[4] = 0;
    resliceUpdateExtent[5] = 0;

    vtkIdType outputInc[3];
    outputImage->GetIncrements(outputInc);

    int scalarType =  reslice->GetOutput()->GetScalarType();
    switch (scalarType)
      {
      vtkTemplateMacro(
        FillSlice(
          static_cast<VTK_TT*>(reslice->GetOutput()->GetScalarPointerForExtent(resliceUpdateExtent)),
          static_cast<VTK_TT*>(outputImage->GetScalarPointerForExtent(outExtent)),
          resliceUpdateExtent, outExtent, outputInc, slice));
      } 
    }// for each slice

  reslice->Delete();
  return 1;
}

void vtkvmtkCurvedMPRImageFilter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  
  os << indent << "Centerline: " << this->Centerline << "\n";

  os << indent << "InplaneOutputSpacing: (" << this->InplaneOutputSpacing[0] << " " << this->InplaneOutputSpacing[1] << ")\n";
  os << indent << "InplaneOutputSize: (" << this->InplaneOutputSize[0] << " " << this->InplaneOutputSize[1] << ")\n";
  os << indent << "OutputSpacing: (" << this->OutputSpacing[0] << ", "
     << this->OutputSpacing[1] << ", " << this->OutputSpacing[2] << ")\n";
  os << indent << "ReslicingBackgroundLevel: (" << this->ReslicingBackgroundLevel << ")\n";
  os << indent << "OutputExtent: (" << this->OutputExtent[0] << ", "
     << this->OutputExtent[1] << ", " << this->OutputExtent[2] << ")\n";
  os << indent << "OutputExtent(3-5): (" << this->OutputExtent[3] << ", "
     << this->OutputExtent[4] << ", " << this->OutputExtent[5] << ")\n";
  os << indent << "OutputOrigin: (" << this->OutputOrigin[0] << ", "
     << this->OutputOrigin[1] << ", " << this->OutputOrigin[2] << ")\n";
  os << indent << "OutputSpacing: (" << this->OutputSpacing[0] << ", "
     << this->OutputSpacing[1] << ", " << this->OutputSpacing[2] << ")\n";
}


