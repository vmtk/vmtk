if [ ! "$APPVEYOR" = true ] ; then
    echo 'This script is only intended for appveyor images. You have to manually remove this check.'
	echo 'DO NOT RUN OUTSIDE OF A DISPOSABLE VM.'
	exit 1
fi
sudo apt update -yqq
sudo apt install -y libxext-dev libgl1-mesa-dev xvfb
export DISPLAY=:99.0
which Xvfb
Xvfb :99 -screen 0 1024x768x24 > /dev/null 2>&1 &

sudo cp -r /usr/lib/x86_64-linux-gnu/* /usr/lib/ &>/dev/null


wget http://repo.continuum.io/miniconda/Miniconda3-latest-Linux-x86_64.sh -O miniconda.sh
bash miniconda.sh -b -p ${HOME}/miniconda
source ${HOME}/miniconda/bin/activate base

conda config --set always_yes true
conda install --quiet -y conda conda-build anaconda-client
conda config --prepend channels conda-forge


conda info -a
conda list