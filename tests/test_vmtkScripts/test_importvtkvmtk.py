## Program: VMTK
## Language:  Python
## Date:      January 10, 2018
## Version:   1.4

##   Copyright (c) Richard Izzo, Luca Antiga, All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

## Note: this code was contributed by
##       Richard Izzo (Github @rlizzo)
##       University at Buffalo

import pytest
import unittest
import os


@pytest.fixture(scope='function')
def vmtk_vtkvmtk_attributes():
    attributes =  [
        'vtk',
        'vtkvmtkActiveTubeFilter',
        'vtkvmtkAnisotropicDiffusionImageFilter',
        'vtkvmtkAnnularCapPolyData',
        'vtkvmtkAppendFilter',
        'vtkvmtkBoundaryConditions',
        'vtkvmtkBoundaryLayerGenerator',
        # 'vtkvmtkBoundaryLayerGenerator2',
        'vtkvmtkBoundaryReferenceSystems',
        'vtkvmtkBoundedReciprocalImageFilter',
        'vtkvmtkCapPolyData',
        'vtkvmtkCardinalSpline',
        # 'vtkvmtkCellDimensionFilter',
        'vtkvmtkCenterlineAttributesFilter',
        'vtkvmtkCenterlineBifurcationReferenceSystems',
        'vtkvmtkCenterlineBifurcationVectors',
        'vtkvmtkCenterlineBranchExtractor',
        'vtkvmtkCenterlineBranchGeometry',
        'vtkvmtkCenterlineEndpointExtractor',
        'vtkvmtkCenterlineGeometry',
        # 'vtkvmtkCenterlineInterpolateArray',
        'vtkvmtkCenterlineReferenceSystemAttributesOffset',
        'vtkvmtkCenterlineSmoothing',
        'vtkvmtkCenterlineSphereDistance',
        'vtkvmtkCenterlineSplitExtractor',
        'vtkvmtkCenterlineSplittingAndGroupingFilter',
        'vtkvmtkCenterlineUtilities',
        'vtkvmtkCollidingFrontsImageFilter',
        # 'vtkvmtkConcaveAnnularCapPolyData',
        'vtkvmtkCurvedMPRImageFilter',
        'vtkvmtkCurvesLevelSetImageFilter',
        'vtkvmtkDanielssonDistanceMapImageFilter',
        'vtkvmtkDataSetItem',
        'vtkvmtkDataSetItems',
        'vtkvmtkDirichletBoundaryConditions',
        'vtkvmtkDolfinWriter',
        # 'vtkvmtkDolfinWriter2',
        'vtkvmtkDoubleVector',
        'vtkvmtkEllipticProblem',
        'vtkvmtkEmptyNeighborhood',
        'vtkvmtkEmptyStencil',
        'vtkvmtkFDNEUTReader',
        'vtkvmtkFDNEUTWriter',
        'vtkvmtkFEAssembler',
        'vtkvmtkFEShapeFunctions',
        'vtkvmtkFWHMFeatureImageFilter',
        'vtkvmtkFastMarchingDirectionalFreezeImageFilter',
        'vtkvmtkFastMarchingUpwindGradientImageFilter',
        'vtkvmtkFluentWriter',
        'vtkvmtkGaussQuadrature',
        'vtkvmtkGeodesicActiveContourLevelSet2DImageFilter',
        'vtkvmtkGeodesicActiveContourLevelSetImageFilter',
        'vtkvmtkGradientMagnitudeImageFilter',
        'vtkvmtkGradientMagnitudeRecursiveGaussian2DImageFilter',
        'vtkvmtkGradientMagnitudeRecursiveGaussianImageFilter',
        'vtkvmtkGrayscaleMorphologyImageFilter',
        'vtkvmtkITKArchetypeImageSeriesReader',
        'vtkvmtkITKArchetypeImageSeriesScalarReader',
        'vtkvmtkITKImageWriter',
        'vtkvmtkITKVersion',
        'vtkvmtkImageBoxPainter',
        'vtkvmtkImagePlaneWidget',
        'vtkvmtkInteractorStyleTrackballCamera',
        'vtkvmtkInternalTetrahedraExtractor',
        'vtkvmtkItem',
        'vtkvmtkItems',
        'vtkvmtkIterativeClosestPointTransform',
        'vtkvmtkLaplacianSegmentationLevelSetImageFilter',
        'vtkvmtkLevelSetSigmoidFilter',
        'vtkvmtkLinearSystem',
        'vtkvmtkLinearSystemSolver',
        'vtkvmtkLinearToQuadraticMeshFilter',
        'vtkvmtkLinearToQuadraticSurfaceMeshFilter',
        'vtkvmtkLinearizeMeshFilter',
        'vtkvmtkMath',
        'vtkvmtkMedialCurveFilter',
        'vtkvmtkMergeCenterlines',
        'vtkvmtkMeshLambda2',
        'vtkvmtkMeshProjection',
        'vtkvmtkMeshVelocityStatistics',
        'vtkvmtkMeshVorticity',
        'vtkvmtkMeshWallShearRate',
        'vtkvmtkMinHeap',
        'vtkvmtkNeighborhood',
        'vtkvmtkNeighborhoods',
        'vtkvmtkNonManifoldFastMarching',
        'vtkvmtkNonManifoldSteepestDescent',
        'vtkvmtkNormalizeImageFilter',
        'vtkvmtkObjectnessMeasureImageFilter',
        'vtkvmtkOpenNLLinearSystemSolver',
        'vtkvmtkOtsuMultipleThresholdsImageFilter',
        'vtkvmtkPNGWriter',
        'vtkvmtkPolyBall',
        'vtkvmtkPolyBallLine',
        # 'vtkvmtkPolyBallLine2',
        'vtkvmtkPolyBallModeller',
        'vtkvmtkPolyDataAreaWeightedUmbrellaStencil',
        'vtkvmtkPolyDataBifurcationProfiles',
        'vtkvmtkPolyDataBifurcationSections',
        'vtkvmtkPolyDataBoundaryExtractor',
        'vtkvmtkPolyDataBranchSections',
        'vtkvmtkPolyDataBranchUtilities',
        'vtkvmtkPolyDataCenterlineAbscissaMetricFilter',
        'vtkvmtkPolyDataCenterlineAngularMetricFilter',
        'vtkvmtkPolyDataCenterlineGroupsClipper',
        'vtkvmtkPolyDataCenterlineMetricFilter',
        'vtkvmtkPolyDataCenterlineProjection',
        'vtkvmtkPolyDataCenterlineSections',
        'vtkvmtkPolyDataCenterlines',
        'vtkvmtkPolyDataClampedSmoothingFilter',
        'vtkvmtkPolyDataCylinderHarmonicMappingFilter',
        # 'vtkvmtkPolyDataDijkstraDistanceToPoints',
        'vtkvmtkPolyDataDiscreteElasticaFilter',
        'vtkvmtkPolyDataDistanceToCenterlines',
        # 'vtkvmtkPolyDataDistanceToSpheres',
        'vtkvmtkPolyDataFEGradientAssembler',
        'vtkvmtkPolyDataFELaplaceAssembler',
        'vtkvmtkPolyDataFELaplaceBeltramiStencil',
        'vtkvmtkPolyDataFVFELaplaceBeltramiStencil',
        'vtkvmtkPolyDataFlowExtensionsFilter',
        # 'vtkvmtkPolyDataGeodesicRBFInterpolation',
        'vtkvmtkPolyDataGradientFilter',
        'vtkvmtkPolyDataGradientStencil',
        'vtkvmtkPolyDataHarmonicMappingFilter',
        'vtkvmtkPolyDataKiteRemovalFilter',
        'vtkvmtkPolyDataLaplaceBeltramiStencil',
        'vtkvmtkPolyDataLineEmbedder',
        'vtkvmtkPolyDataLocalGeometry',
        'vtkvmtkPolyDataManifoldExtendedNeighborhood',
        'vtkvmtkPolyDataManifoldNeighborhood',
        'vtkvmtkPolyDataManifoldStencil',
        'vtkvmtkPolyDataMeanCurvature',
        'vtkvmtkPolyDataMultipleCylinderHarmonicMappingFilter',
        'vtkvmtkPolyDataNeighborhood',
        'vtkvmtkPolyDataNetworkExtraction',
        'vtkvmtkPolyDataNormalPlaneEstimator',
        'vtkvmtkPolyDataPatchingFilter',
        'vtkvmtkPolyDataPotentialFit',
        'vtkvmtkPolyDataReferenceSystemBoundaryMetricFilter',
        'vtkvmtkPolyDataRigidSurfaceModelling',
        # 'vtkvmtkPolyDataSampleFunction',
        'vtkvmtkPolyDataScissors',
        'vtkvmtkPolyDataSizingFunction',
        'vtkvmtkPolyDataStencilFlowFilter',
        'vtkvmtkPolyDataStretchMappingFilter',
        'vtkvmtkPolyDataSurfaceRemeshing',
        'vtkvmtkPolyDataToUnstructuredGridFilter',
        'vtkvmtkPolyDataUmbrellaStencil',
        'vtkvmtkRBFInterpolation',
        # 'vtkvmtkRBFInterpolation2',
        'vtkvmtkRecursiveGaussian2DImageFilter',
        'vtkvmtkRecursiveGaussianImageFilter',
        'vtkvmtkReferenceSystemUtilities',
        'vtkvmtkSatoVesselnessMeasureImageFilter',
        'vtkvmtkSigmoidImageFilter',
        'vtkvmtkSimpleCapPolyData',
        'vtkvmtkSimplifyVoronoiDiagram',
        'vtkvmtkSmoothCapPolyData',
        'vtkvmtkSparseMatrix',
        'vtkvmtkSparseMatrixRow',
        'vtkvmtkStaticTemporalInterpolatedVelocityField',
        'vtkvmtkStaticTemporalStreamTracer',
        'vtkvmtkSteepestDescentLineTracer',
        'vtkvmtkSteepestDescentShooter',
        'vtkvmtkStencil',
        'vtkvmtkStencils',
        'vtkvmtkStreamlineClusteringFilter',
        'vtkvmtkStreamlineOsculatingCentersFilter',
        'vtkvmtkStreamlineToParticlesFilter',
        'vtkvmtkSurfaceDistance',
        # 'vtkvmtkSurfaceProjectCellArray',
        'vtkvmtkSurfaceProjection',
        'vtkvmtkTetGenReader',
        'vtkvmtkTetGenWrapper',
        'vtkvmtkTetGenWriter',
        'vtkvmtkThresholdSegmentationLevelSetImageFilter',
        'vtkvmtkTopologicalSeamFilter',
        'vtkvmtkUnstructuredGridCenterlineGroupsClipper',
        'vtkvmtkUnstructuredGridCenterlineSections',
        'vtkvmtkUnstructuredGridFEGradientAssembler',
        'vtkvmtkUnstructuredGridFELaplaceAssembler',
        'vtkvmtkUnstructuredGridFEVorticityAssembler',
        'vtkvmtkUnstructuredGridGradientFilter',
        'vtkvmtkUnstructuredGridHarmonicMappingFilter',
        'vtkvmtkUnstructuredGridNeighborhood',
        'vtkvmtkUnstructuredGridTetraFilter',
        'vtkvmtkUnstructuredGridVorticityFilter',
        'vtkvmtkUpwindGradientMagnitudeImageFilter',
        'vtkvmtkVesselEnhancingDiffusionImageFilter',
        'vtkvmtkVesselnessMeasureImageFilter',
        'vtkvmtkVoronoiDiagram3D',
        'vtkvmtkXdaReader',
        'vtkvmtkXdaWriter'
        ]
    return attributes


def test_import_vtkvmtk(vmtk_vtkvmtk_attributes):
    from vmtk import vtkvmtk
    print(vmtk_vtkvmtk_attributes)
    for name in vmtk_vtkvmtk_attributes:
        assert hasattr(vtkvmtk, name)
        # assert importlib.__import__(name)

def test_itk_version():
    from vmtk.vtkvmtk import vtkvmtkITKVersion
    print(vtkvmtkITKVersion.GetITKVersion())
    print(vtkvmtkITKVersion.GetITKMajorVersion())
    print(vtkvmtkITKVersion.GetITKMinorVersion())
    print(vtkvmtkITKVersion.GetITKBuildVersion())
    print(vtkvmtkITKVersion.GetITKSourceVersion())