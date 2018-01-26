# Install and set up miniconda.
# if [ $TRAVIS_OS_NAME == "linux" ]; then sudo apt-get update && sudo apt-get install -yqq git wget libxt-dev libgl1-mesa-glx libgl1-mesa-dev libglapi-mesa libosmesa-dev build-essential; fi
if [ $TRAVIS_OS_NAME == "linux" ]; then wget http://repo.continuum.io/miniconda/Miniconda3-latest-Linux-x86_64.sh -O miniconda.sh; fi
if [ $TRAVIS_OS_NAME == "osx" ]; then wget http://repo.continuum.io/miniconda/Miniconda3-latest-MacOSX-x86_64.sh -O miniconda.sh; fi
if [ $TRAVIS_OS_NAME == "osx" ]; then cd $HOME && git clone https://github.com/phracker/MacOSX-SDKs.git && cd $TRAVIS_BUILD_DIR; fi
bash miniconda.sh -b -p $CONDA_INSTALL_LOCN
export PATH=${CONDA_INSTALL_LOCN}/bin:$PATH
conda config --set always_yes true

conda install --quiet -y conda conda-build anaconda-client

# set the ordering of additional channels
conda config --prepend channels vmtk/label/dev

# To ease debugging, list installed packages
conda info -a
conda list