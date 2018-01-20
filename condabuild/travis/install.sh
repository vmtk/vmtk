# Install and set up miniconda.
if [ $TRAVIS_OS_NAME == "linux" ]; then sudo apt-get update && sudo apt-get install -yqq git wget libxt-dev libgl1-mesa-glx libgl1-mesa-dev libglapi-mesa libosmesa-dev build-essential; fi
if [ $TRAVIS_OS_NAME == "linux" ]; then wget http://repo.continuum.io/miniconda/Miniconda3-latest-Linux-x86_64.sh -O miniconda.sh; fi
if [ $TRAVIS_OS_NAME == "osx" ]; then wget http://repo.continuum.io/miniconda/Miniconda3-latest-MacOSX-x86_64.sh -O miniconda.sh; fi
if [ $TRAVIS_OS_NAME == "osx" ]; then git clone https://github.com/phracker/MacOSX-SDKs.git; fi
bash miniconda.sh -b -p $CONDA_INSTALL_LOCN
export PATH=${CONDA_INSTALL_LOCN}/bin:$PATH
conda config --set always_yes true

conda install --quiet -y conda conda-build anaconda-client

# set the ordering of additional channels
conda config --prepend channels vmtk/label/dev

# To ease debugging, list installed packages
conda info -a
conda list
# - |
#   Only upload if this is NOT a pull request.
#   if [ "$TRAVIS_PULL_REQUEST" = "false" ] && \
#      [ $TRAVIS_REPO_SLUG = "bccp/conda-channel-bccp" ] && \
#      [ "$TRAVIS_BRANCH" == "master" ]; then
#       UPLOAD="--user $DESTINATION_CONDA_CHANNEL --token $BINSTAR_TOKEN";
#       conda config --set anaconda_upload true
#       echo "Uploading enabled";
#   else
#       echo "Uplading disabled";
#       UPLOAD="";
#   fi