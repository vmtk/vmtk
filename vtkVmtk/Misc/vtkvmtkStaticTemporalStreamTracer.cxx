/*=========================================================================

Program:   Visualization Toolkit
Module:    vtkvmtkStaticTemporalStreamTracer.cxx

Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
All rights reserved.
See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkvmtkStaticTemporalStreamTracer.h"

#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkCompositeDataIterator.h"
#include "vtkCompositeDataSet.h"
#include "vtkDoubleArray.h"
#include "vtkGenericCell.h"
#include "vtkIdList.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkSmartPointer.h"
#include "vtkVersion.h"
#include "vtkInterpolatedVelocityField.h"

#include "vtkvmtkStaticTemporalInterpolatedVelocityField.h"

#include "vtkTable.h"

vtkStandardNewMacro(vtkvmtkStaticTemporalStreamTracer);
vtkCxxSetObjectMacro(vtkvmtkStaticTemporalStreamTracer, TimeStepsTable, vtkTable);

vtkvmtkStaticTemporalStreamTracer::vtkvmtkStaticTemporalStreamTracer()
{
  this->SeedTime = 0.0;
  this->SeedTimesArrayName = NULL;
  this->Periodic = 0;
  this->VelocityScale = 1.0;
  this->TimeStepsTable = NULL;
  this->UseVectorComponents = 0;
  this->VectorPrefix = NULL;
  this->Component0Prefix = NULL;
  this->Component1Prefix = NULL;
  this->Component2Prefix = NULL;
}

vtkvmtkStaticTemporalStreamTracer::~vtkvmtkStaticTemporalStreamTracer()
{
  if (this->SeedTimesArrayName) {
    delete[] this->SeedTimesArrayName;
    this->SeedTimesArrayName = NULL;
  }

  if (this->VectorPrefix) {
    delete[] this->VectorPrefix;
    this->VectorPrefix = NULL;
  }

  if (this->Component0Prefix) {
    delete[] this->Component0Prefix;
    this->Component0Prefix = NULL;
  }

  if (this->Component1Prefix) {
    delete[] this->Component1Prefix;
    this->Component1Prefix = NULL;
  }

  if (this->Component2Prefix) {
    delete[] this->Component2Prefix;
    this->Component2Prefix = NULL;
  }

  this->SetTimeStepsTable(NULL);
}

void vtkvmtkStaticTemporalStreamTracer::InitializeDefaultInterpolatorPrototype()
{
  vtkvmtkStaticTemporalInterpolatedVelocityField* staticTemporalInterpolator = vtkvmtkStaticTemporalInterpolatedVelocityField::New();

  staticTemporalInterpolator->SetUseVectorComponents(this->UseVectorComponents);
  staticTemporalInterpolator->SetVectorPrefix(this->VectorPrefix);
  staticTemporalInterpolator->SetComponent0Prefix(this->Component0Prefix);
  staticTemporalInterpolator->SetComponent1Prefix(this->Component1Prefix);
  staticTemporalInterpolator->SetComponent2Prefix(this->Component2Prefix);
  staticTemporalInterpolator->SetTimeStepsTable(this->TimeStepsTable);
  staticTemporalInterpolator->SetPeriodic(this->Periodic);
  staticTemporalInterpolator->SetVelocityScale(this->VelocityScale);

  this->SetInterpolatorPrototype(staticTemporalInterpolator);

  staticTemporalInterpolator->Delete();
}

void vtkvmtkStaticTemporalStreamTracer::InitializeSeeds(vtkDataArray*& seeds,
                                                        vtkIdList*& seedIds,
                                                        vtkDoubleArray*& startTimes,
                                                        vtkIntArray*& integrationDirections,
                                                        vtkDataSet *source)
{
  this->Superclass::InitializeSeeds(seeds,seedIds,integrationDirections,source);

  startTimes = vtkDoubleArray::New();

  if (!source)
    {
    return;
    }

  vtkDataArray* sourceSeedTimes = source->GetPointData()->GetArray(this->SeedTimesArrayName);

  if (sourceSeedTimes)
    {
    startTimes->DeepCopy(sourceSeedTimes);
    }
  else
    {
    startTimes->SetNumberOfTuples(source->GetNumberOfPoints());
    startTimes->FillComponent(0,this->SeedTime);
    }
}

int vtkvmtkStaticTemporalStreamTracer::CheckInputs(vtkAbstractInterpolatedVelocityField*& func,
                                                   int* maxCellSize)
{
  if (!this->InputData)
    {
    return VTK_ERROR;
    }

  vtkCompositeDataIterator* iter = this->InputData->NewIterator();
  vtkSmartPointer<vtkCompositeDataIterator> iterP(iter);
  iter->Delete();

  iterP->GoToFirstItem();
  if (iterP->IsDoneWithTraversal())
    {
    return VTK_ERROR;
    }

  // Set the function set to be integrated
  if ( !this->InterpolatorPrototype )
    {
    func = vtkvmtkStaticTemporalInterpolatedVelocityField::New();

    // turn on the following segment, in place of the above line, if an
    // interpolator equipped with a cell locator is dedired as the default
    //
    // func = vtkCellLocatorInterpolatedVelocityField::New();
    // vtkSmartPointer< vtkModifiedBSPTree > locator =
    // vtkSmartPointer< vtkModifiedBSPTree >::New();
    // vtkCellLocatorInterpolatedVelocityField::SafeDownCast( func )
    //   ->SetCellLocatorPrototype( locator.GetPointer() );
    }
  else
    {
    func = this->InterpolatorPrototype->NewInstance();
    func->CopyParameters(this->InterpolatorPrototype);
    }

  //vtkDataArray *vectors = this->GetInputArrayToProcess(
  //  0,iterP->GetCurrentDataObject());
  //if (!vectors)
  //  {
  //  return VTK_ERROR;
  //  }
  //const char *vecName = vectors->GetName();
  //func->SelectVectors(vecName);

  // Add all the inputs ( except source, of course ) which
  // have the appropriate vectors and compute the maximum
  // cell size.
  int numInputs = 0;
  iterP->GoToFirstItem();
  while (!iterP->IsDoneWithTraversal())
    {
    vtkDataSet* inp = vtkDataSet::SafeDownCast(iterP->GetCurrentDataObject());
    if (inp)
      {
      //if (!inp->GetPointData()->GetVectors(vecName))
      //  {
      //  vtkDebugMacro("One of the input blocks does not contain a "
      //                "velocity vector.");
      //  iterP->GoToNextItem();
      //  continue;
      //  }
      int cellSize = inp->GetMaxCellSize();
      if ( cellSize > *maxCellSize )
        {
        *maxCellSize = cellSize;
        }
      vtkInterpolatedVelocityField::SafeDownCast(func)->AddDataSet(inp);
      numInputs++;
      }
    iterP->GoToNextItem();
    }
  if ( numInputs == 0 )
    {
    vtkDebugMacro("No appropriate inputs have been found. Can not execute.");
    return VTK_ERROR;
    }
  return VTK_OK;
}

int vtkvmtkStaticTemporalStreamTracer::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  if (!this->SetupOutput(inInfo, outInfo))
    {
    return 0;
    }

  vtkInformation *sourceInfo = inputVector[1]->GetInformationObject(0);
  vtkDataSet *source = 0;
  if (sourceInfo)
    {
    source = vtkDataSet::SafeDownCast(
      sourceInfo->Get(vtkDataObject::DATA_OBJECT()));
    }
  vtkPolyData *output = vtkPolyData::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

  if (this->InterpolatorPrototype == 0)
    {
    this->InitializeDefaultInterpolatorPrototype();
    }

  vtkDataArray* seeds = 0;
  vtkIdList* seedIds = 0;
  vtkIntArray* integrationDirections = 0;
  vtkDoubleArray* startTimes = 0;
  this->InitializeSeeds(seeds, seedIds, startTimes, integrationDirections, source);

  if (seeds)
    {
    double lastPoint[3];
    vtkAbstractInterpolatedVelocityField * func;
    int maxCellSize = 0;
    if (this->CheckInputs(func, &maxCellSize) != VTK_OK)
      {
      vtkDebugMacro("No appropriate inputs have been found. Can not execute.");
      func->Delete();
      seeds->Delete();
      startTimes->Delete();
      integrationDirections->Delete();
      seedIds->Delete();
      this->InputData->UnRegister(this);
      return 1;
      }

    vtkCompositeDataIterator* iter = this->InputData->NewIterator();
    vtkSmartPointer<vtkCompositeDataIterator> iterP(iter);
    iter->Delete();

    iterP->GoToFirstItem();
    vtkDataSet* input0 = 0;
    if (!iterP->IsDoneWithTraversal())
      {
      input0 = vtkDataSet::SafeDownCast(iterP->GetCurrentDataObject());
      }

    double propagation = 0;
    vtkIdType numSteps = 0;
    this->Integrate(input0, output,
                    seeds, seedIds,
                    startTimes,
                    integrationDirections,
                    lastPoint, func,
                    maxCellSize,
                    propagation, numSteps);
    func->Delete();
    seeds->Delete();
    }

  startTimes->Delete();
  integrationDirections->Delete();
  seedIds->Delete();

  this->InputData->UnRegister(this);
  return 1;
}

void vtkvmtkStaticTemporalStreamTracer::Integrate(vtkDataSet *input0,
                                                  vtkPolyData* output,
                                                  vtkDataArray* seedSource,
                                                  vtkIdList* seedIds,
                                                  vtkDoubleArray* startTimes,
                                                  vtkIntArray* integrationDirections,
                                                  double lastPoint[3],
                                                  vtkAbstractInterpolatedVelocityField* func,
                                                  int maxCellSize,
                                                  double& inPropagation,
                                                  vtkIdType& inNumSteps)
{
  int i;
  vtkIdType numLines = seedIds->GetNumberOfIds();
  double propagation = inPropagation;
  vtkIdType numSteps = inNumSteps;

  // Useful pointers
  vtkDataSetAttributes* outputPD = output->GetPointData();
  vtkDataSetAttributes* outputCD = output->GetCellData();
  vtkPointData* inputPD;
  vtkDataSet* input;
  //TODO: this one will potentially change at every evaluation.

  int direction=1;

  double* weights = 0;
  if ( maxCellSize > 0 )
    {
    weights = new double[maxCellSize];
    }

  if (this->GetIntegrator() == 0)
    {
    vtkErrorMacro("No integrator is specified.");
    return;
    }

  // Used in GetCell()
  vtkGenericCell* cell = vtkGenericCell::New();

  // Create a new integrator, the type is the same as Integrator
  vtkInitialValueProblemSolver* integrator =
    this->GetIntegrator()->NewInstance();
  integrator->SetFunctionSet(func);

  // Since we do not know what the total number of points
  // will be, we do not allocate any. This is important for
  // cases where a lot of streamers are used at once. If we
  // were to allocate any points here, potentially, we can
  // waste a lot of memory if a lot of streamers are used.
  // Always insert the first point
  vtkPoints* outputPoints = vtkPoints::New();
  vtkCellArray* outputLines = vtkCellArray::New();

  // We will keep track of integration time in this array
  vtkDoubleArray* time = vtkDoubleArray::New();
  time->SetName("IntegrationTime");

  // This array explains why the integration stopped
  vtkIntArray* retVals = vtkIntArray::New();
  retVals->SetName("ReasonForTermination");

  vtkDoubleArray* cellVectors = 0;
  vtkDoubleArray* velocityArray = 0;
  vtkDoubleArray* speedArray = 0;
  vtkDoubleArray* vorticity = 0;
  vtkDoubleArray* rotation = 0;
  vtkDoubleArray* angularVel = 0;

  velocityArray = vtkDoubleArray::New();
  velocityArray->SetName("Velocity");
  velocityArray->SetNumberOfComponents(3);

  speedArray = vtkDoubleArray::New();
  speedArray->SetName("Speed");

  if (this->ComputeVorticity)
    {
    cellVectors = vtkDoubleArray::New();
    cellVectors->SetNumberOfComponents(3);
    cellVectors->Allocate(3*VTK_CELL_SIZE);

    vorticity = vtkDoubleArray::New();
    vorticity->SetName("Vorticity");
    vorticity->SetNumberOfComponents(3);

    rotation = vtkDoubleArray::New();
    rotation->SetName("Rotation");

    angularVel = vtkDoubleArray::New();
    angularVel->SetName("AngularVelocity");
    }

  // We will interpolate all point attributes of the input on each point of
  // the output (unless they are turned off). Note that we are using only
  // the first input, if there are more than one, the attributes have to match.
  //
  // Note: We have to use a specific value (safe to employ the maximum number
  //       of steps) as the size of the initial memory allocation here. The
  //       use of the default argument might incur a crash problem (due to
  //       "insufficient memory") in the parallel mode. This is the case when
  //       a streamline intensely shuttles between two processes in an exactly
  //       interleaving fashion --- only one point is produced on each process
  //       (and actually two points, after point duplication, are saved to a
  //       vtkPolyData in vtkDistributedStreamTracer::NoBlockProcessTask) and
  //       as a consequence a large number of such small vtkPolyData objects
  //       are needed to represent a streamline, consuming up the memory before
  //       the intermediate memory is timely released.

  // TODO: this one should be avoided - or at least we should interpolate all vectors that are not in the list of temporal vector fields
  //outputPD->InterpolateAllocate( input0->GetPointData(),
  //                               this->MaximumNumberOfSteps );

  vtkIdType numPtsTotal=0;
  double velocity[3];

  int shouldAbort = 0;

  for(int currentLine = 0; currentLine < numLines; currentLine++)
    {
    double progress = static_cast<double>(currentLine)/numLines;
    this->UpdateProgress(progress);

    switch (integrationDirections->GetValue(currentLine))
      {
      case FORWARD:
        direction = 1;
        break;
      case BACKWARD:
        direction = -1;
        break;
      }

    // temporary variables used in the integration
    double point1[3], point2[3], pcoords[3], vort[3], omega;
    vtkIdType index, numPts=0;

    // Clear the last cell to avoid starting a search from
    // the last point in the streamline
    func->ClearLastCellId();

    double startTime = startTimes->GetValue(currentLine);

    // Initial point
    seedSource->GetTuple(seedIds->GetId(currentLine), point1);
    memcpy(point2, point1, 3*sizeof(double));

    double point1t[4];
    memcpy(point1t, point1, 3*sizeof(double)); 
    point1t[3] = startTime;

    if (!func->FunctionValues(point1t, velocity))
      {
      continue;
      }

    if ( propagation >= this->MaximumPropagation ||
         numSteps    >  this->MaximumNumberOfSteps)
      {
      continue;
      }

    numPts++;
    numPtsTotal++;
    vtkIdType nextPoint = outputPoints->InsertNextPoint(point1);

    time->InsertNextValue(startTime);

    // We will always pass an arc-length step size to the integrator.
    // If the user specifies a step size in cell length unit, we will
    // have to convert it to arc length.
    IntervalInformation stepSize;  // either positive or negative
    stepSize.Unit  = LENGTH_UNIT;
    stepSize.Interval = 0;
    IntervalInformation aStep; // always positive
    aStep.Unit = LENGTH_UNIT;
    double step, minStep=0, maxStep=0;
    double stepTaken, accumTime=startTime;
    double speed;
    double cellLength;
    int retVal=OUT_OF_LENGTH, tmp;

    // Make sure we use the dataset found by the vtkAbstractInterpolatedVelocityField
    input = func->GetLastDataSet();
    inputPD = input->GetPointData();

    // Convert intervals to arc-length unit
    input->GetCell(func->GetLastCellId(), cell);
    cellLength = sqrt(static_cast<double>(cell->GetLength2()));
    speed = vtkMath::Norm(velocity);
    // Never call conversion methods if speed == 0
    if ( speed != 0.0 )
      {
      this->ConvertIntervals( stepSize.Interval, minStep, maxStep,
                              direction, cellLength );
      }

    // Interpolate all point attributes on first point
    func->GetLastWeights(weights);
    //TODO: avoid this at least for time vectors
    //outputPD->InterpolatePoint(inputPD, nextPoint, cell->PointIds, weights);

    velocityArray->InsertNextTuple(velocity);
    speedArray->InsertNextValue(speed);

    // Compute vorticity if required
    // This can be used later for streamribbon generation.
    if (this->ComputeVorticity)
      {
      //TODO: for vorticity to work, inVectors should be updated with the vector field at the correct time step
      //inVectors->GetTuples(cell->PointIds, cellVectors);
      func->GetLastLocalCoordinates(pcoords);
      vort[0] = vort[1] = vort[2] = 0.0;
      //vtkStreamTracer::CalculateVorticity(cell, pcoords, cellVectors, vort);
      vorticity->InsertNextTuple(vort);
      // rotation
      // local rotation = vorticity . unit tangent ( i.e. velocity/speed )
      if (speed != 0.0)
        {
        omega = vtkMath::Dot(vort, velocity);
        omega /= speed;
        omega *= this->RotationScale;
        }
      else
        {
        omega = 0.0;
        }
      angularVel->InsertNextValue(omega);
      rotation->InsertNextValue(0.0);
      }

    double error = 0;
    // Integrate until the maximum propagation length is reached,
    // maximum number of steps is reached or until a boundary is encountered.
    // Begin Integration
    while ( propagation < this->MaximumPropagation )
      {

      if (numSteps > this->MaximumNumberOfSteps)
        {
        retVal = OUT_OF_STEPS;
        break;
        }

      if ( numSteps++ % 1000 == 1 )
        {
        progress =
          ( currentLine + propagation / this->MaximumPropagation ) / numLines;
        this->UpdateProgress(progress);

        if (this->GetAbortExecute())
          {
          shouldAbort = 1;
          break;
          }
        }

      // Never call conversion methods if speed == 0
      if ( (speed == 0) || (speed <= this->TerminalSpeed) )
        {
        retVal = STAGNATION;
        break;
        }

      // If, with the next step, propagation will be larger than
      // max, reduce it so that it is (approximately) equal to max.
      aStep.Interval = fabs( stepSize.Interval );

      if ( ( propagation + aStep.Interval ) > this->MaximumPropagation )
        {
        aStep.Interval = this->MaximumPropagation - propagation;
        if ( stepSize.Interval >= 0 )
          {
          stepSize.Interval = this->ConvertToLength( aStep, cellLength );
          }
        else
          {
          stepSize.Interval = this->ConvertToLength( aStep, cellLength ) * ( -1.0 );
          }
        maxStep = stepSize.Interval;
        }
      this->LastUsedStepSize = stepSize.Interval;

      // Calculate the next step using the integrator provided
      // Break if the next point is out of bounds.
      func->SetNormalizeVector( true );
      tmp = integrator->ComputeNextStep( point1, point2, accumTime, stepSize.Interval,
                                         stepTaken, minStep, maxStep,
                                         this->MaximumError, error );
      func->SetNormalizeVector( false );
      if ( tmp != 0 )
        {
        retVal = tmp;
        memcpy(lastPoint, point2, 3*sizeof(double));
        break;
        }

      // It is not enough to use the starting point for stagnation calculation
      // Use delX/stepSize to calculate speed and check if it is below
      // stagnation threshold
      double disp[3];
      for (i=0; i<3; i++)
        {
        disp[i] = point2[i] - point1[i];
        }
      if ( (stepSize.Interval == 0) ||
           (vtkMath::Norm(disp) / fabs(stepSize.Interval) <= this->TerminalSpeed) )
        {
        retVal = STAGNATION;
        break;
        }

      accumTime += stepTaken / speed;
      // Calculate propagation (using the same units as MaximumPropagation
      propagation += fabs( stepSize.Interval );

      // This is the next starting point
      for(i=0; i<3; i++)
        {
        point1[i] = point2[i];
        }

      double point2t[4];
      memcpy(point2t, point2, 3*sizeof(double)); 
      point2t[3] = accumTime;

      // Interpolate the velocity at the next point
      if ( !func->FunctionValues(point2t, velocity) )
        {
        retVal = OUT_OF_DOMAIN;
        memcpy(lastPoint, point2, 3*sizeof(double));
        break;
        }
      // Make sure we use the dataset found by the vtkAbstractInterpolatedVelocityField
      input = func->GetLastDataSet();
      inputPD = input->GetPointData();

      // Point is valid. Insert it.
      numPts++;
      numPtsTotal++;
      nextPoint = outputPoints->InsertNextPoint(point1);
      time->InsertNextValue(accumTime);

      // Calculate cell length and speed to be used in unit conversions
      input->GetCell(func->GetLastCellId(), cell);
      cellLength = sqrt(static_cast<double>(cell->GetLength2()));

      velocityArray->InsertNextTuple(velocity);

      speed = vtkMath::Norm(velocity);

      speedArray->InsertNextValue(speed);

      // Interpolate all point attributes on current point
      func->GetLastWeights(weights);
      //TODO: avoid this at least for time vectors
      //outputPD->InterpolatePoint(inputPD, nextPoint, cell->PointIds, weights);

      // Compute vorticity if required
      // This can be used later for streamribbon generation.
      if (this->ComputeVorticity)
        {
        //TODO
        //inVectors->GetTuples(cell->PointIds, cellVectors);
        func->GetLastLocalCoordinates(pcoords);
        vort[0] = vort[1] = vort[2] = 0.0;
        //vtkStreamTracer::CalculateVorticity(cell, pcoords, cellVectors, vort);
        vorticity->InsertNextTuple(vort);
        // rotation
        // angular velocity = vorticity . unit tangent ( i.e. velocity/speed )
        // rotation = sum ( angular velocity * stepSize )
        omega = vtkMath::Dot(vort, velocity);
        omega /= speed;
        omega *= this->RotationScale;
        index = angularVel->InsertNextValue(omega);
        rotation->InsertNextValue(rotation->GetValue(index-1) +
                                  (angularVel->GetValue(index-1) + omega)/2 *
                                  (accumTime - time->GetValue(index-1)));
        }

      // Never call conversion methods if speed == 0
      if ( (speed == 0) || (speed <= this->TerminalSpeed) )
        {
        retVal = STAGNATION;
        break;
        }

      // Convert all intervals to arc length
      this->ConvertIntervals( step, minStep, maxStep, direction, cellLength );


      // If the solver is adaptive and the next step size (stepSize.Interval)
      // that the solver wants to use is smaller than minStep or larger
      // than maxStep, re-adjust it. This has to be done every step
      // because minStep and maxStep can change depending on the cell
      // size (unless it is specified in arc-length unit)
      if (integrator->IsAdaptive())
        {
        if (fabs(stepSize.Interval) < fabs(minStep))
          {
          stepSize.Interval = fabs( minStep ) *
                                stepSize.Interval / fabs( stepSize.Interval );
          }
        else if (fabs(stepSize.Interval) > fabs(maxStep))
          {
          stepSize.Interval = fabs( maxStep ) *
                                stepSize.Interval / fabs( stepSize.Interval );
          }
        }
      else
        {
        stepSize.Interval = step;
        }

      // End Integration
      }

    if (shouldAbort)
      {
      break;
      }

    if (numPts > 1)
      {
      outputLines->InsertNextCell(numPts);
      for (i=numPtsTotal-numPts; i<numPtsTotal; i++)
        {
        outputLines->InsertCellPoint(i);
        }
      retVals->InsertNextValue(retVal);
      }

    // Initialize these to 0 before starting the next line.
    // The values passed in the function call are only used
    // for the first line.
    inPropagation = propagation;
    inNumSteps = numSteps;

    propagation = 0;
    numSteps = 0;
    }

  if (!shouldAbort)
    {
    // Create the output polyline
    output->SetPoints(outputPoints);
    outputPD->AddArray(time);
    outputPD->AddArray(velocityArray);
    outputPD->AddArray(speedArray);
    if (vorticity)
      {
      outputPD->AddArray(vorticity);
      outputPD->AddArray(rotation);
      outputPD->AddArray(angularVel);
      }

    vtkIdType numPts = outputPoints->GetNumberOfPoints();
    if ( numPts > 1 )
      {
      // Assign geometry and attributes
      output->SetLines(outputLines);
      if (this->GenerateNormalsInIntegrate)
        {
        ////////////////////////////// TODO //////////////////////////////
        //this->GenerateNormals(output, 0, vecName);
        }

      outputCD->AddArray(retVals);
      }
    }

  velocityArray->Delete();
  speedArray->Delete();

  if (vorticity)
    {
    vorticity->Delete();
    rotation->Delete();
    angularVel->Delete();
    }

  if (cellVectors)
    {
    cellVectors->Delete();
    }
  retVals->Delete();

  outputPoints->Delete();
  outputLines->Delete();

  time->Delete();


  integrator->Delete();
  cell->Delete();

  delete[] weights;

  output->Squeeze();
  return;
}

void vtkvmtkStaticTemporalStreamTracer::PrintSelf(std::ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "Seed time: " << this->SeedTime
     << " unit: time." << endl;
}

