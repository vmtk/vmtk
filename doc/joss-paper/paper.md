---
title: 'The Vascular Modeling Toolkit: A Python Library for the Analysis of Tubular Structures in Medical Images'
tags:
  - Python
  - C++
  - vascular modeling
  - image processing
  - medical imaging
  - image segmentation
  - computational geometry
  - surface extraction
  - CFD Meshing
  - hemodynamics
  - mesh generation
authors:
  - name: Richard Izzo
    orcid: 0000-0002-0811-6513
    affiliation: "1, 2"
  - name: Luca Antiga
    # orcid: 0000-0000-0000-0000 NEED THIS
    affiliation: 3
affiliations:
 - name: Biomedical Engineering, University at Buffalo
   index: 1
 - name: Fellow, Prentice Family Foundation
   index: 2
 - name: Orobix Srl. 
   index: 3
date: 23 April 2018
bibliography: paper.bib
---

# Summary

Vascular disease is the leading cause of death in the developed world [@RN569].
There is a wide body of evidence demonstrating that the geometric structure and
layout of the vascular network has a major impact on hemodynamics and the
associated severity / progression of vascular diseases such as heart attack,
stroke, and aneurysm rupture [@RN478, @RN83].  Analysis of diagnostic medical
scans designed to highlight vascular structures (such as CT Angiography or MR
Angiography) allows for the detailed characterization of these structures *in
vivo*.  Such analyses generate quantitative measures which can be used in order
to understand both disease characteristics across a population, and the
efficacy of potential treatments [@RN550, @RN83].

``The Vascular Modeling Toolkit (VMTK)`` is a collection of python-wrapped C++
methods which enable the efficient segmentation, geometric characterization,
network analysis, hemodynamic modeling, and visualization of vascular
structures from medical images.  Image segmentation is performed via a 3D
gradient based level set algorithm which is initialized from user defined
seeds. Surface editing, mesh generation, and geometric characterization is
largely automated, relying heavily on the centerline definitions calculated
from segmented structures.  User interaction is facilitated by a unique system
of unix-inspired ``Pypes``.  This interface allows for the composable creation
and execution of entire analyses from simple terminal commands, providing a
flexible framework for high-level coding, both from the user’s and from the
developer’s point of view.

``VMTK`` is a mature package with an active development team and user
community.  It can be used via it's standalone interface, included as a Python
or C++ library, or as an extension to the medical image processing platform 3D
Slicer.  It has received 41 citations in scientific publications since its
first release in 2004. The library relies upon two major open source frameworks
for building highly performant and well validated image analysis algorithms and
visualizations: the Visualization Toolkit (``VTK``) and the Insight
Segmentation and Registration Toolkit (``ITK``).  Thorough tutorials and
documentation are available on the project webpage: www.vmtk.org.

# VMTK In Action

## Generating a Surface from an Image via Level Set Evolution

Segmenting a complex vascular tract comes down to selecting the endpoints of a
branch, letting level sets by attracted to gradient peaks with the sole
advection term turned on, repeating the operation for all the branches and
merging everything in a single model.

``vmtklevelsetsegmentation -ifile foo.dcm --pipe vmtkmarchingcubes -i @.o
--pipe vmtksurfacewriter -ofile foo.vtp``

\begin{figure}[h!]\centering
  {\includegraphics[width=.3\textwidth]{levelset-seed-placement.png}}
  {\includegraphics[width=.3\textwidth]{levelset-isosurface-from-seeds.png}}
  {\includegraphics[width=.3\textwidth]{levelset-isosurface-evolution.png}}
  \caption{The process of placing seeds on an image (left), initializing an
  isosurface from the seeds using the colliding fronts methods (center), and
  finally evolving the isosurface through the level set equations (right).}
\end{figure}

## Generating Centerlines from a Surface

Centerlines are determined as the paths defined on Voronoi diagram sheets that
minimize the integral of the radius of maximal inscribed spheres along the
path, which is equivalent to finding the shortest paths in the radius metric.

``vmtkcenterlines -ifile foo.vtp -ofile foo_centerlines.vtp``

\begin{figure}[h!]\centering
  {\includegraphics[width=.3\textwidth]{centerlines1.png}}
  {\includegraphics[width=.3\textwidth]{centerlines2.png}}
  {\includegraphics[width=.3\textwidth]{centerlines3.png}}
  \caption{The input surface representation (left), a visualization of the
  internal subset of the voronoi diagram where each sheet represents a maximum
  inscribed sphere radius centered at some point in the surface (center), the
  centerline extracted from the voronoi diagram rendered as in it's position
  within the input surface (right).} 
\end{figure}

## Splitting a Surface from it's Centerlines

Surface properties can be analyzed, and the surface can be split by analyzing
the surface-centerline tube containment relationships.

`` vmtksurfacereader -ifile foo.vtp --pipe vmtkcenterlines --pipe
vmtkbranchextractor --pipe vmtkbranchclipper -groupids 0 -insideout 1 -ofile
foo_sp.vtp``

\begin{figure}[h!]\centering
  {\includegraphics[width=.45\textwidth]{splitting1.png}}
  {\includegraphics[width=.45\textwidth]{splitting2.png}}
  \caption{Illustration of the centerline-surface tube containment
  relationships (left), visualization of the surface being split into
  independent groups based on the surfaces membership in a centerline
  tract/group (right).}
\end{figure}

## Generating a Volumetric Mesh from a Surface

Tetrahedral, mixed tetrahedral, and boundary layer meshes can be generated from
a surface and it's centerlines. 

Generating a radius-adaptive element mesh with a boundary layer
``vmtksurfacereader -ifile foo.vtp --pipe vmtkcenterlines --pipe
vmtkdistancetocenterlines -useradius 1 --pipe vmtkmeshgenerator
-elementsizemode edgelengtharray -edgelengtharray DistanceToCenterlines
-edgelengthfactor 0.3 -boundarylayer 1 -ofile foo.vtu``

\begin{figure}[h!]\centering
  {\includegraphics[width=.2\textwidth]{mesh1.png}}
  {\includegraphics[width=.2\textwidth]{mesh2.png}}
  {\includegraphics[width=.45\textwidth]{mesh3.png}}
  \caption{The input surface representation (left), a visualization of the
  internal mesh (center), a visualization of a boundary layer internal mesh
  (right).}
\end{figure}

## composable Scripting With PypeS

``PypeS`` is the glue among vmtk scripts. It allows new scripts to be written
easily and have a common interface, but, most of all, it allows single vmtk
scripts to interact with each other, making ``VMTK`` modular and flexible.
Pypes can be used from the command line, a python interpreter, or from the
custom ``PypePad`` user interface

\begin{figure}[h!]\centering
  {\includegraphics[width=.8\textwidth]{pypepad.png}}
  \caption{The VMTK PypePad user interface.}
\end{figure}

# Acknowledgements & Funding

The Vascular Modeling Toolkit is a community project which is open-source and
free of charge. We would especially like to recognize significant
infrastructure contributions from the team at Kitware Inc. (www.kitware.com).
In addition we would like to acknowledge Orobix Srl. (www.orobix.com) for
partially fundinging continued development efforts of ``VMTK``.

# References
