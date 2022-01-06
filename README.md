## [VMTK - the Vascular Modeling Toolkit](http://vmtk.org)

[![Build Status](https://travis-ci.org/vmtk/vmtk.svg?branch=master)](https://travis-ci.org/vmtk/vmtk)
[![Build status](https://ci.appveyor.com/api/projects/status/3u6nupu2r47qbesq/branch/master?svg=true)](https://ci.appveyor.com/project/rlizzo/vmtk-bh6uc/branch/master)
[![DOI](http://joss.theoj.org/papers/10.21105/joss.00745/status.svg)](https://doi.org/10.21105/joss.00745)


Introduction
============

The Vascular Modeling Toolkit is a collection of libraries and tools for 3D
reconstruction, geometric analysis, mesh generation and surface data analysis
for image-based modeling of blood vessels. VMTK can be used via its standalone
interface, included as a Python or C++ library, or as an extension to the
medical image processing platform [3D Slicer](https://www.slicer.org/). By
providing multiple user interfaces with various requirments of technical
ability, VMTK aims to be usable by anyone with an interest in medical image
processing; be they clinicians, researchers, industries, or educational
institutions.

Getting Started
===============

Tutorials, development instructions, and the general information is available at https://www.vmtk.org 

[Installation](http://www.vmtk.org/documentation/installation.html)
- How to install VMTK. 

[Getting Started](http://www.vmtk.org/documentation/getting-started.html) 
- Learn how to open your dataset in vmtk, navigate into a 3D volume and set up your image for further processing. 

[Pypes](http://www.vmtk.org/documentation/pypes.html)
- Learn how to use Pypes.

[Tutorials](http://www.vmtk.org/tutorials)
- Tutorials will guide you through the main features.

[vmtkScripts](http://www.vmtk.org/documentation/vmtkscripts.html)
- Automatically generated python class references

[C++ Scripts](http://www.vmtk.org/doc/html/index.html)
- Automatically generated C++ class reference

[Screenshots](http://www.vmtk.org/documentation/screenshots.html)
- Screenshots from VMTK examples.

[Presentations](http://www.vmtk.org/documentation/presentations.html)
- Presentations about VMTK. 


Features
==============

## Gradient-based 3D level sets segmentation

Take a look into the [Level Set Segmentation](http://www.vmtk.org/tutorials/ImageBasedModeling.html) tutorial to learn how to reconstruct the 3D surface of a vascular segment from CT or MR images using level sets.

```
// Image segmentation
vmtklevelsetsegmentation -ifile image_volume_voi.vti -ofile level_sets.vti
//You can specify different parameters, for example:
-levelsetsfile in order to start from an existing levelset segmentation
-featureimagetype to change featureimage, for example the upwind modality
-featurederivativesigma to use a Gaussian derivative convolution
```

</figure>
<div class="hidden-bullet-wrapper" style="width: 500px">
<figure class="code bullet-description2" style="margin-top: 10px;">
  <img src="https://github.com/vmtk/vmtk/blob/master/doc/img/home_levelset.png" width="500px"> 
</figure>
</div>

A new gradient computation modality based on upwind finite differences 
allows the segmentation of small (down to 1.2 pixels/diameter) vessels.
Interactive level sets initialization based on the `Fast Marching Method`. 
This includes a brand new method for selecting a vascular segment comprised 
between two points automatically ignoring side branches, no parameters involved. 
Segmenting a complex vascular tract comes down to selecting the endpoints of a 
branch, letting `level sets` by attracted to gradient peaks with the sole 
advection term turned on, repeating the operation for all the branches and merging 
everything in a single model.


## Computing centerlines

Take a look into the [Computing Centerlines](http://www.vmtk.org/tutorials/Centerlines.html) tutorial to learn how to compute centerlines of a vascular segment.

```
// Computing centerlines
vmtkcenterlines -ifile foo.vtp -ofile foo_centerlines.vtp
//Look the resulting centerlines
vmtksurfacereader -ifile foo.vtp --pipe vmtkcenterlines --pipe vmtkrenderer --pipe vmtksurfaceviewer -opacity 0.25 --pipe vmtksurfaceviewer -i @vmtkcenterlines.o -array MaximumInscribedSphereRadius
//Inspect the voronoi diagram
vmtksurfacereader -ifile foo.vtp --pipe vmtkcenterlines --pipe vmtkrenderer --pipe vmtksurfaceviewer -opacity 0.25 --pipe vmtksurfaceviewer -i @vmtkcenterlines.voronoidiagram -array MaximumInscribedSphereRadius --pipe vmtksurfaceviewer -i @vmtkcenterlines.o
```

</figure>
<div class="hidden-bullet-wrapper" style="width: 500px">
<figure class="code bullet-description2" style="margin-top: 10px;">
    <img src="https://github.com/vmtk/vmtk/blob/master/doc/img/home_centerlines.png" width="500px">
</figure>
</div>

`Centerlines` are powerful descriptors of the shape of vessels and 
are determined as weighted shortest paths traced between two extremal points. 
In order to ensure that the final lines are in fact central, the paths cannot 
lie anywhere in space, but are bound to run on the `Voronoi diagram` of the vessel model,
considered as the place where the centers of maximal inscribed spheres are defined.
Centerlines are determined as the paths defined on Voronoi diagram sheets that minimize
the integral of the radius of `maximal inscribed spheres` along the path, which is equivalent to finding the shortest paths in the radius metric.


## Geometric analysis

Take a look into the [Geometric analysis](http://www.vmtk.org/tutorials/GeometricAnalysis.html) tutorial to learn how to analyze the 3D geometry of a vascular segment and into the [Preparing a Surface for Meshing](http://www.vmtk.org/tutorials/SurfaceForMeshing.html) tutorial to learn how to prepare a surface for mesh generation.

```
// Generate a vtp file containing the data on the bifurcation vectors
vmtksurfacereader -ifile foo.vtp --pipe vmtkcenterlines -seedselector openprofiles --pipe vmtkbranchextractor --pipe vmtkbifurcationreferencesystems --pipe vmtkbifurcationvectors -ofile foo_bv.vtp
//Compute curvature and torsion
vmtkcenterlinegeometry -ifile foo_cl.vtp -smoothing 1 -ofile foo_clgm.vtp
//Smoothing a surface
vmtksurfacesmoothing -ifile foo.vtp -passband 0.1 -iterations 30 -ofile foo_sm.vtp 
//Adding flow extensions
vmtksurfacereader -ifile foo.vtp --pipe vmtkcenterlines -seedselector openprofiles --pipe vmtkflowextensions -adaptivelength 1 -extensionratio 20 -normalestimationratio 1 -interactive 0 --pipe vmtksurfacewriter -ofile foo_ex.vtp
```

</figure>
<div class="hidden-bullet-wrapper" style="width: 500px">
<figure class="code bullet-description2" style="margin-top: 10px;">
    <img src="https://github.com/vmtk/vmtk/blob/master/doc/img/home_geometry.png" width="500px">
</figure>
</div>

Quantifying geometric features of the vascular segment, 
those associated to `bifurcations`, such as bifurcation planes 
and bifurcation angles, and those associated to branches, such 
as `curvature and torsion`. Curvature and torsion are tightly linked 
to the definition of the Frenet line frame, constituted by a tangent, 
a normal and the binormal.

Increase surface smoothness prior to building the mesh. 
Image segmentation can result in bumpy surfaces, especially if the 
image quality is not high and one didn’t use any curvature term in level 
sets evolution. `Flow extensions` are cylindrical extensions added to the 
inlets and outlets of a model. They are important for ensuring that the 
flow entering and leaving the computational domain is fully developed, so 
that fully developed boundary conditions aren’t forcing the solution in the 
actual vessel.


## Generating a mesh

Take a look into the [Generating a Mesh](http://www.vmtk.org/tutorials/MeshGeneration.html) tutorial to learn how to generate a mesh from a surface and into the [Meshing based on centerlines](http://www.vmtk.org/tutorials/CenterlinesGmsh.html) tutorial to learn how to generate tetrahedral or <strong>mixed hexahedral</strong> meshes using vmtk coupled to Gmsh. Contributed by Emilie Marchandise, U. Louvain.

```
// generating a uniform element mesh
vmtkmeshgenerator -ifile foo.vtp -ofile foo.vtu -edgelength 0.5
//Generating a radius-adaptive element mesh
vmtksurfacereader -ifile foo.vtp --pipe vmtkcenterlines -endpoints 1 -seedselector openprofiles --pipe vmtkdistancetocenterlines -useradius 1 --pipe vmtkmeshgenerator -elementsizemode edgelengtharray -edgelengtharray DistanceToCenterlines -edgelengthfactor 0.3 -ofile foo.vtu 
//Adding a boundary layer
vmtksurfacereader -ifile foo.vtp --pipe vmtkcenterlines -endpoints 1 -seedselector openprofiles --pipe vmtkdistancetocenterlines -useradius 1 --pipe vmtkmeshgenerator -elementsizemode edgelengtharray -edgelengtharray DistanceToCenterlines -edgelengthfactor 0.3 -boundarylayer 1 -ofile foo.vtu
```

</figure>
<div class="hidden-bullet-wrapper" style="width: 500px">
<figure class="code bullet-description2" style="margin-top: 10px;">
    <img src="https://github.com/vmtk/vmtk/blob/master/doc/img/home_mesh.png" width="500px">
</figure>
</div>

Surface `remeshing` is performed under the assumption that 
the surface requires improvement before being used for CFD.
After the surface has been remeshed the volume is filled with 
a combination of `tetrahedral` and `prismatic` elements.
We can heighten the `density` of the mesh near the wall by generating
the `boundary layer`.


## Mapping and patching

Take a look into the [Mapping and patching](http://www.vmtk.org/tutorials/MappingAndPatching.html) tutorial to
learn how to map the surface of a population of vessels onto the same parametric space and enable statistical 
analyses of surface-based quantities

```
// Longitudinal and circumferential metrics
vmtkbranchmetrics -ifile aorta_clipped.vtp -centerlinesfile aorta_cl.vtp -abscissasarray Abscissas -normalsarray ParallelTransportNormals -groupidsarray GroupIds -centerlineidsarray CenterlineIds -tractidsarray TractIds -blankingarray Blanking -radiusarray MaximumInscribedSphereRadius -ofile aorta_clipped_metrics.vtp
//Metrics mapping to branches
vmtkbranchmapping -ifile aorta_clipped_metrics.vtp -centerlinesfile aorta_cl.vtp -referencesystemsfile aorta_cl_rs.vtp -normalsarray ParallelTransportNormals -abscissasarray Abscissas -groupidsarray GroupIds -centerlineidsarray CenterlineIds -tractidsarray TractIds -referencesystemsnormalarray Normal -radiusarray MaximumInscribedSphereRadius -blankingarray Blanking -angularmetricarray AngularMetric -abscissametricarray AbscissaMetric -ofile aorta_clipped_mapping.vtp
//Patching of surface mesh and attributes
vmtkbranchpatching -ifile aorta_clipped_mapping.vtp -groupidsarray GroupIds -longitudinalmappingarray StretchedMapping -circularmappingarray AngularMetric -longitudinalpatchsize 0.5 -circularpatches 12 -ofile aorta_clipped_patching.vtp
```

</figure>
<div class="hidden-bullet-wrapper" style="width: 500px">
<figure class="code bullet-description2" style="margin-top: 10px;">
   <img src="https://github.com/vmtk/vmtk/blob/master/doc/img/home_mappatch.png" width="500px">
</figure>
</div>

A common application is `mapping and patching` of fluid dynamics variables, 
such as wall shear stress `(WSS)` or oscillatory shear index `(OSI)`, obtained 
on the surface mesh typically by means of a CFD simulation.</br>
By construction of a harmonic function over each vascular segment, 
vmtkbranchmapping maps and stretches the longitudinal metric to correctly 
account for the presence of insertion regions at `bifurcations`; the additional 
StretchedMapping array is added to the surface.


## Pypes

Take a look into the [Basic PypeS](http://www.vmtk.org/tutorials/PypesBasic.html) tutorial to learn how to effectively pipe vmtk scripts together, the [Use PypeS Programmatically](http://www.vmtk.org/tutorials/PypesProgrammatically.html) tutorial to learn how to interactively work with PypeS objects and into the [Advanced PypeS](http://www.vmtk.org//tutorials/PypesAdvanced.html) tutorial to learn how to write your own PypeS modules.

```
vmtkmarchingcubes --help
Creating vmtkMarchingCubes instance.
Automatic piping vmtkmarchingcubes
Parsing options vmtkmarchingcubes
vmtkmarchingcubes : generate an isosurface of given level from a 3D image
Input arguments:
	-id Id(int,1); default=0: script id
	-handle Self (self,1): handle to self
	-disabled Disabled (bool,1); default=0: disable execution and piping
	-i Image (vtkImageData,1): the input image
	-ifile ImageInputFileName(str,1): filename for the default Image Reader
	-array ArrayName (str,1): name of the array to work with
	-l Level(float,1); default=0.0: graylevel to generate the isosurface at
	-connectivity Connectivity (bool,1); default=0: only output the largest connected region of the isosurface
	-ofile SurfaceOutputFileName (str,1): filename for the default Surface writer
Output arguments:
	-id Id (int,1); default= 0: script id
	-handle Self (self,1): handle to self
	-o Surface (vtkPolyData,1): the output surface
```
```
// We can use vmtkmarchingcubes as a stand-alone script by using the built-in I/O functionality
vmtkmarchingcubes -ifile foo.vti -ofile foo.vtp
//or we can build a pype that does the same thing
vmtkimagereader -ifile foo.vti --pipe vmtkmarchingcubes --pipe vmtksurfacewriter -ofile foo.vtp @vmtkcenterlines.o -array MaximumInscribedSphereRadius
//Say we want to read two images and extract a surface with Marching Cubes with a level of 20 for both. We can either write
vmtkmarchingcubes -ifile foo1.vti -l 20 --pipe vmtkmarchingcubes -ifile foo2.vti -l 20
//or push the input argument -l along to the second vmtkmarchingcubes this way
vmtkmarchingcubes -ifile foo1.vti -l@ 20 --pipe vmtkmarchingcubes -ifile foo2.vti
```

Writing classes implementing `algorithms` and writing actual tools 
to be used for everyday work are two distinct tasks. </br>Very often a well-designed 
object-oriented library ends up to be used in ever-growing collections of shell, 
Python or Tcl scripts or small C programs, each with its own argument parsing and 
I/O sections. Very often high-level code is duplicated to provide slightly different 
`functionality`. On the other side, writing a GUI is a time-consuming task, and adding 
new functionality requires time, which might deter experimentation. `PypeS` goes in the 
direction of providing a flexible `framework` for `high-level` code, both from the user’s 
and from the developer’s points of view. The user wants to get things done minimizing 
the work required and the amount of intermediate data generated. The coder wants to limit 
the amount of code, she/he has to cut and paste (and maintain), and to quickly add new 
functionality and make it interact with what she/he’s ever written before.

Funding
==============

Development of VMTK is supported by [Orobix Srl.](http://www.orobix.com/)

Contact
==============

If you have any questions or comments [contact the VMTK community](http://www.vmtk.org/community).
