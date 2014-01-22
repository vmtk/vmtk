import sys
import os
import glob
import shutil
from setuptools import setup, find_packages
from setuptools.command.install import install as _install
from setuptools.command.build_py import build_py as _build

VMTKPATH = "../../../vmtk-build/Install"

CLASSIFIERS = ["Development Status :: 5 - Production/Stable",
               "Environment :: Console",
               "Intended Audience :: Science/Research",
               "License :: OSI Approved :: BSD License",                
               "Operating System :: MacOS",
               "Operating System :: POSIX :: Linux",
               "Operating System :: Microsoft :: Windows :: Windows 7",
               "Programming Language :: C++",
               "Programming Language :: Python :: 2 :: Only",
               "Topic :: Scientific/Engineering"]

# Description
description = "vmtk - The Vascular Modeling Toolkit"
fid = file('README', 'r')
long_description = fid.read()
fid.close()
idx = max(0, long_description.find("vmtk - The Vascular Modeling Toolkit"))
long_description = long_description[idx:]

NAME                = 'vmtk'
MAINTAINER          = "Luca Antiga"
MAINTAINER_EMAIL    = "luca.antiga@orobix.com"
DESCRIPTION         = description
LONG_DESCRIPTION    = long_description
URL                 = "https://github.com/vmtk/vmtk"
DOWNLOAD_URL        = "http://pypi.python.org/pypi/vmtk"
LICENSE             = "BSD"
CLASSIFIERS         = CLASSIFIERS
AUTHOR              = "Luca Antiga"
AUTHOR_EMAIL        = "luca.antiga@orobix.com"
PLATFORMS           = "Linux/MacOSX/Windows"
ISRELEASED          = True
VERSION             = '1.2'

def list_files(directory):
    '''A specialized version of os.walk() that list files only in the current directory
       and ignores files whose start with a leading period and cmake files.'''
    for root, dirs, files in os.walk(directory):
        if root == directory:
            return [x for x in files if not (x.startswith('.')) and not (x.endswith('cmake'))]

class vmtk_build(_build):
    '''Build vmtk libraries'''
    def run(self):
        #finding absolute path 
        vmtk_path = os.path.abspath(VMTKPATH)
        
        #copying install directory
        try:
            shutil.copytree(os.path.join(vmtk_path,'lib','vmtk','vmtk'), 'vmtk')
        except OSError:
            shutil.rmtree('vmtk')
            shutil.copytree(os.path.join(vmtk_path,'lib','vmtk','vmtk'), 'vmtk')
        try:
            shutil.copytree(os.path.join(vmtk_path,'bin'), os.path.join('vmtk','bin'), ignore=shutil.ignore_patterns('Python'))
        except OSError:
            shutil.rmtree('bin')
            shutil.copytree(os.path.join(vmtk_path,'bin'), os.path.join('vmtk','bin'), ignore=shutil.ignore_patterns('Python'))
        
        try:
            shutil.copytree(os.path.join(vmtk_path,'bin','Python','vtk'), os.path.join('vmtk','vtk'))
        except OSError:
            shutil.rmtree('vtk')
            shutil.copytree(os.path.join(vmtk_path,'bin','Python','vtk'), os.path.join('vmtk','vtk'))
        
        shutil.copytree(os.path.join(vmtk_path,'lib'), os.path.join('vmtk','lib'), symlinks=True, ignore=shutil.ignore_patterns('cmake'))
        
        for file_to_move in list_files(os.path.join('vmtk','lib','vmtk')):
            shutil.copy(os.path.join('vmtk','lib','vmtk',file_to_move),os.path.join('vmtk','lib',file_to_move))
        shutil.rmtree(os.path.join('vmtk','lib','vmtk'))
        
        for file_to_move in list_files(os.path.join('vmtk','lib','vtk-5.10')):
            shutil.copy(os.path.join('vmtk','lib','vtk-5.10',file_to_move),os.path.join('vmtk','lib',file_to_move))
        shutil.rmtree(os.path.join('vmtk','lib','vtk-5.10'))
        
        for file_to_move in list_files(os.path.join('vmtk')):
            if file_to_move.endswith('.so') or file_to_move.endswith('.pyd'):
                shutil.move(os.path.join('vmtk',file_to_move),os.path.join('vmtk','lib',file_to_move))
        
        for file_to_unlink in list_files(os.path.join('vmtk','lib')):
            if file_to_unlink == 'hints':
                os.remove(os.path.join('vmtk','lib',file_to_unlink))
            if os.path.islink(os.path.join('vmtk','lib',file_to_unlink)):
                os.unlink(os.path.join('vmtk','lib',file_to_unlink))

        #copy favicon
        shutil.copy(os.path.join(os.getcwd(),'vmtk-icon.ico'),os.path.join('vmtk','bin','vmtk-icon.ico'))
        
setup(name=NAME,
      maintainer=MAINTAINER,
      maintainer_email=MAINTAINER_EMAIL,
      description=DESCRIPTION,
      long_description=LONG_DESCRIPTION,
      url=URL,
      download_url=DOWNLOAD_URL,
      license=LICENSE,
      classifiers=CLASSIFIERS,
      author=AUTHOR,
      author_email=AUTHOR_EMAIL,
      platforms=PLATFORMS,
      version=VERSION,
      cmdclass={'vmtk_build':vmtk_build},
      packages = find_packages(),
      zip_safe=False,
      package_data = {
         'vmtk': ["lib/*.so*","lib/*.*lib*","lib/*.pyd*","bin/*"],
         'vtk': ["lib/*.so*","lib/*.pyd*"],
        },
      scripts = ['vmtk_post_install.py']
    )
