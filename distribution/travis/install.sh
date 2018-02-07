## Program: VMTK
## Language:  Python
## Date:      January 30, 2018
## Version:   1.4
##
##   Copyright (c) Richard Izzo, Luca Antiga, All rights reserved.
##   See LICENCE file for details.
##
##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.
##
## Note: this code was contributed by
##       Richard Izzo (Github @rlizzo)
##       University at Buffalo

# Install and set up miniconda.
if [ $TRAVIS_OS_NAME == "linux" ]; then wget http://repo.continuum.io/miniconda/Miniconda3-latest-Linux-x86_64.sh -O miniconda.sh; fi
if [ $TRAVIS_OS_NAME == "osx" ]; then wget http://repo.continuum.io/miniconda/Miniconda3-latest-MacOSX-x86_64.sh -O miniconda.sh; fi
if [ $TRAVIS_OS_NAME == "osx" ]; then cd $HOME && git clone https://github.com/phracker/MacOSX-SDKs.git && cd $TRAVIS_BUILD_DIR; fi
bash miniconda.sh -b -p $CONDA_INSTALL_LOCN
export PATH=${CONDA_INSTALL_LOCN}/bin:$PATH
conda config --set always_yes true

conda install --quiet -y conda conda-build anaconda-client

# set the ordering of additional channels
conda config --prepend channels vmtk

# To ease debugging, list installed packages
conda info -a
conda list