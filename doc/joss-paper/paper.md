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
stroke, and aneurysm rupture [@RN478, @RN83]. Analysis of diagnostic medical
scans designed to highlight vascular structures (such as CT Angiography or MR
Angiography) allows for the detailed characterization of vascular structures
*in vivo*. Such analyses generate quantitative measures which can be used in
order to understand both disease characteristics across a population, and the
efficacy of potential treatments [@RN550, @RN83].

``The Vascular Modeling Toolkit (VMTK)`` is a collection of python-wrapped C++
methods which enable the efficient segmentation, geometric characterization,
network analysis, heomdynamic modeling, and visualization of vascular
structures from medical images. Image segmentation is performed via a 3D
gradient based level set algorithm which is initialized from user defined
seeds. Surface editing, mesh generation, and geometric characterization is
largely automated, relying heavily on the centerline definitions calculated
from segmented structures.  User interaction is facilitated by a unique system of
unix-inspired ``Pypes``. This interface allows for the composable creation and
execution of entire analyses from simple terminal commands, provinding a
flexible framework for high-level coding, both from the user’s and from the
developer’s point of view.

``VMTK`` is a mature package with an active development team and user
community. It can be used via it's standalone interface, included as a Python
or C++ library, or as an extension to the medical image processing platform 3D
Slicer. It has recieved 27 citations in scientific publications since its first
release in 2004. The library relies upon two major open source frameworks for
building highly performant and well validated image analysis algorithms and
visualizations: the Visualization Toolkit (``VTK``) and the Insight
Segmentation and Registration Toolkit (``ITK``).  Thorough tutorials and
documentation are avialable on the project webpage: www.vmtk.org.  

# Acknowledgements

We would like to thank Kitware for their contributions and support. Continued
development efforts have been partially funded by Orobix srl. (www.orobix.com)

# References
