/*=========================================================================

Original Method Developed By:
	Copyright : (C)opyright (Pompeu Fabra University) 2007++
				See COPYRIGHT statement in top level directory.
	Authors   : Ignacio Larrabide && Xavier Mellado
	Modified  : $Author: $
	Purpose   : Application for computing of medial curve inside a surface
				from the distance map representation of a surface.
	Date      : $Date: $
	Version   : $Revision: $

Modifications Made To Support Integration Into:
	The Vascular Modeling Toolkit (VMTK)
	www.vmtk.org

Modification Authors:
	Richard Izzo
		University at Buffalo
		Github: rlizzo
		Date: April 12, 2018
	Luca Antiga
		Orobix Srl.
		Github: lantiga
		Date: April 12, 2018

=========================================================================*/

#include "vtkvmtkMedialCurveFilter.h"
#include "vtkImageData.h"
#include "vtkObjectFactory.h"
#include "vtkPolyDataAlgorithm.h"
#include "vtkPolyData.h"
#include "vtkPointData.h"
#include "vtkVersion.h"
#include "vtkSmartPointer.h"
#include "vtkPolyDataToImageStencil.h"
#include "vtkImageStencil.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"

#include "vtkvmtkITKFilterUtilities.h"

#include "itkImage.h"
#include "itkRecursiveGaussianImageFilter.h"
#include "itkGradientImageFilter.h"
#include "itkAverageOutwardFluxImageFilter.h"
#include "itkMedialCurveImageFilter.h"
#include "itkApproximateSignedDistanceMapImageFilter.h"

vtkStandardNewMacro(vtkvmtkMedialCurveFilter);

vtkvmtkMedialCurveFilter::vtkvmtkMedialCurveFilter()
{
	this->InputSurface = NULL;
	this->BinaryImage = NULL;
	this->DistanceImage = NULL;
	this->OutputImage = NULL;
	this->Sigma = 0.5;
	this->Threshold = 0.0;
	this->PolyDataToImageDataSpacingX = 0.5;
	this->PolyDataToImageDataSpacingY = 0.5;
	this->PolyDataToImageDataSpacingZ = 0.5;
}

vtkvmtkMedialCurveFilter::~vtkvmtkMedialCurveFilter()
{
	if (this->InputSurface)
	{
		this->InputSurface->Delete();
		this->InputSurface = NULL;
	}

	if (this->BinaryImage)
	{
		this->BinaryImage->Delete();
		this->BinaryImage = NULL;
	}

	if (this->DistanceImage)
	{
		this->DistanceImage->Delete();
		this->DistanceImage = NULL;
	}

	if (this->OutputImage)
	{
		this->OutputImage->Delete();
		this->OutputImage = NULL;
	}
}

vtkCxxSetObjectMacro(vtkvmtkMedialCurveFilter,InputSurface,vtkPolyData);
vtkCxxSetObjectMacro(vtkvmtkMedialCurveFilter,OutputImage,vtkImageData);
vtkCxxSetObjectMacro(vtkvmtkMedialCurveFilter,DistanceImage,vtkImageData);
vtkCxxSetObjectMacro(vtkvmtkMedialCurveFilter,BinaryImage,vtkImageData);

void vtkvmtkMedialCurveFilter::PolyDataToBinaryImageData()
{
	vtkSmartPointer<vtkImageData> whiteImage = vtkSmartPointer<vtkImageData>::New();
	double bounds[6];
	this->InputSurface->GetBounds(bounds);
	double spacing[3]; // desired volume spacing
	spacing[0] = this->PolyDataToImageDataSpacingX;
	spacing[1] = this->PolyDataToImageDataSpacingY;
	spacing[2] = this->PolyDataToImageDataSpacingZ;
	whiteImage->SetSpacing(spacing);

	// compute dimensions
	int dim[3];
	for (int i = 0; i < 3; i++)
	{
		dim[i] = static_cast<int>(ceil((bounds[i * 2 + 1] - bounds[i * 2]) / spacing[i]));
	}
	whiteImage->SetDimensions(dim);
	whiteImage->SetExtent(0, dim[0] - 1, 0, dim[1] - 1, 0, dim[2] - 1);

	double origin[3];
	origin[0] = bounds[0] + spacing[0] / 2;
	origin[1] = bounds[2] + spacing[1] / 2;
	origin[2] = bounds[4] + spacing[2] / 2;

	whiteImage->SetOrigin(origin);
	whiteImage->AllocateScalars(VTK_UNSIGNED_CHAR, 1);
	// fill the image with foreground voxels:
	unsigned char inval = 255;
	unsigned char outval = 0;
	vtkIdType count = whiteImage->GetNumberOfPoints();
	for (vtkIdType i = 0; i < count; ++i)
	{
		whiteImage->GetPointData()->GetScalars()->SetTuple1(i, inval);
	}

	// polygonal data --> image stencil:
	vtkSmartPointer<vtkPolyDataToImageStencil> pol2stenc = vtkSmartPointer<vtkPolyDataToImageStencil>::New();
	pol2stenc->SetInputData(this->InputSurface);
	pol2stenc->SetOutputOrigin(origin);
	pol2stenc->SetOutputSpacing(spacing);
	pol2stenc->SetOutputWholeExtent(whiteImage->GetExtent());
	pol2stenc->Update();

	// cut the corresponding white image and set the background:
	vtkSmartPointer<vtkImageStencil> imgstenc = vtkSmartPointer<vtkImageStencil>::New();
	imgstenc->SetInputData(whiteImage);
	imgstenc->SetStencilConnection(pol2stenc->GetOutputPort());
	imgstenc->ReverseStencilOff();
	imgstenc->SetBackgroundValue(outval);
	imgstenc->Update();

	this->BinaryImage = imgstenc->GetOutput();
}

void vtkvmtkMedialCurveFilter::BinaryImageToSignedDistanceMapImage()
{
	typedef itk::Image<unsigned char, 3> UnsignedCharImageType;
	typedef itk::Image<float, 3> FloatImageType;

	UnsignedCharImageType::Pointer image = UnsignedCharImageType::New();
	vtkvmtkITKFilterUtilities::VTKToITKImage<UnsignedCharImageType>(this->BinaryImage, image);

	typedef itk::ApproximateSignedDistanceMapImageFilter<UnsignedCharImageType, FloatImageType> ApproximateSignedDistanceMapImageFilterType;
	ApproximateSignedDistanceMapImageFilterType::Pointer approximateSignedDistanceMapImageFilter = ApproximateSignedDistanceMapImageFilterType::New();
	approximateSignedDistanceMapImageFilter->SetInput(image);
	approximateSignedDistanceMapImageFilter->SetInsideValue(255);
	approximateSignedDistanceMapImageFilter->SetOutsideValue(0);

	vtkvmtkITKFilterUtilities::ITKToVTKImage<itk::Image<float, 3>>(approximateSignedDistanceMapImageFilter->GetOutput(), this->DistanceImage);
}	

void vtkvmtkMedialCurveFilter::CalculateCenterline()
{
	typedef float PixelType;
	typedef itk::Image<PixelType, 3> ImageType;

	ImageType::Pointer distance = ImageType::New();
	vtkvmtkITKFilterUtilities::VTKToITKImage<ImageType>(this->DistanceImage, distance);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 1.	Compute the associated average outward flux
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// To have a good quality gradient of the distance map, perform a light smooth over it. Define
	// convolution kernels in each direction and use them recursively.
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	typedef itk::RecursiveGaussianImageFilter<ImageType, ImageType> RecursiveGaussianFilterType;
	RecursiveGaussianFilterType::Pointer gaussianFilterX = RecursiveGaussianFilterType::New();
	RecursiveGaussianFilterType::Pointer gaussianFilterY = RecursiveGaussianFilterType::New();
	RecursiveGaussianFilterType::Pointer gaussianFilterZ = RecursiveGaussianFilterType::New();

	gaussianFilterX->SetDirection(0);
	gaussianFilterY->SetDirection(1);
	gaussianFilterZ->SetDirection(2);

	gaussianFilterX->SetOrder(RecursiveGaussianFilterType::ZeroOrder);
	gaussianFilterY->SetOrder(RecursiveGaussianFilterType::ZeroOrder);
	gaussianFilterZ->SetOrder(RecursiveGaussianFilterType::ZeroOrder);

	gaussianFilterX->SetNormalizeAcrossScale(false);
	gaussianFilterY->SetNormalizeAcrossScale(false);
	gaussianFilterZ->SetNormalizeAcrossScale(false);

	gaussianFilterX->SetInput(distance);
	gaussianFilterY->SetInput(gaussianFilterX->GetOutput());
	gaussianFilterZ->SetInput(gaussianFilterY->GetOutput());

	gaussianFilterX->SetSigma(this->Sigma);
	gaussianFilterY->SetSigma(this->Sigma);
	gaussianFilterZ->SetSigma(this->Sigma);

	// Compute the gradient.
	typedef itk::GradientImageFilter<ImageType, PixelType, PixelType> GradientFilterType;
	GradientFilterType::Pointer gradientFilter = GradientFilterType::New();
	gradientFilter->SetInput(gaussianFilterZ->GetOutput());
	gradientFilter->SetInput(gaussianFilterY->GetOutput());

	// Compute the average outward flux.
	typedef itk::AverageOutwardFluxImageFilter<ImageType, PixelType, GradientFilterType::OutputImageType::PixelType> AOFFilterType;
	AOFFilterType::Pointer aofFilter = AOFFilterType::New();
	aofFilter->SetInput(distance);
	aofFilter->SetGradientImage(gradientFilter->GetOutput());

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 2. Compute the skeleton
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	typedef itk::MedialCurveImageFilter<ImageType> MedialCurveFilter;
	MedialCurveFilter::Pointer medialFilter = MedialCurveFilter::New();
	medialFilter->SetInput(distance);
	medialFilter->SetAverageOutwardFluxImage(aofFilter->GetOutput());
	medialFilter->SetThreshold(this->Threshold);
	medialFilter->Update();

	vtkvmtkITKFilterUtilities::ITKToVTKImage<MedialCurveFilter::TOutputImage>(medialFilter->GetOutput(), this->OutputImage);
}

int vtkvmtkMedialCurveFilter::RequestData(vtkInformation *vtkNotUsed(request),
										  vtkInformationVector **inputVector,
										  vtkInformationVector *outputVector)
{
	vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  	vtkInformation *outInfo = outputVector->GetInformationObject(0);
	vtkPolyData *input = vtkPolyData::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
	vtkImageData *output = vtkImageData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

	this->PolyDataToBinaryImageData();
	this->BinaryImageToSignedDistanceMapImage();
	this->CalculateCenterline();

	output = this->OutputImage;

	return 1;
}

void vtkvmtkMedialCurveFilter::PrintSelf(ostream &os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);
}