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
URL                 = "https://github.com/daron1337/jBessel"
DOWNLOAD_URL        = "http://pypi.python.org/pypi/jBessel"
LICENSE             = "BSD"
CLASSIFIERS         = CLASSIFIERS
AUTHOR              = "Luca Antiga"
AUTHOR_EMAIL        = "luca.antiga@orobix.com"
PLATFORMS           = "Linux/MacOsX"
ISRELEASED          = True
VERSION             = '1.2'

def list_files(directory):
    '''A specialized version of os.walk() that list files only in the current directory
       and ignores files whose start with a leading period and cmake files.'''
    for root, dirs, files in os.walk(directory):
        if root == directory:
            return [x for x in files if not (x.startswith('.')) and not (x.endswith('cmake'))]

class vmtk_build(_build):

    def run(self):
        #finding absolute path 
        vmtk_path = os.path.abspath(VMTKPATH)
        
        #copying install directory 
        try:
            shutil.copytree(vmtk_path+'/bin','bin', ignore=shutil.ignore_patterns('Python'))
        except OSError:
            shutil.rmtree('bin')
            shutil.copytree(vmtk_path+'/bin','bin', ignore=shutil.ignore_patterns('Python'))
              
        try:
            shutil.copytree(vmtk_path+'/bin/Python/vtk','vtk')
        except OSError:
            shutil.rmtree('vtk')
            shutil.copytree(vmtk_path+'/bin/Python/vtk','vtk')
        try:
            shutil.copytree(vmtk_path+'/lib/vmtk/vmtk','vmtk')
        except OSError:
            shutil.rmtree('vmtk')
            shutil.copytree(vmtk_path+'/lib/vmtk/vmtk','vmtk')
        
        shutil.copytree(vmtk_path+'/lib', 'vmtk/lib', symlinks=True, ignore=shutil.ignore_patterns('cmake'))
        
        for file_to_move in list_files('vmtk/lib/vmtk'):
            shutil.copy('vmtk/lib/vmtk/'+file_to_move, 'vmtk/lib/'+file_to_move)
        shutil.rmtree('vmtk/lib/vmtk')
        
        for file_to_move in list_files('vmtk/lib/vtk-5.10'):
            shutil.copy('vmtk/lib/vtk-5.10/'+file_to_move, 'vmtk/lib/'+file_to_move)
        shutil.rmtree('vmtk/lib/vtk-5.10')
        
        for file_to_move in list_files('vmtk'):
            if file_to_move.endswith('.so'):
                shutil.move('vmtk/'+file_to_move, 'vmtk/lib/'+file_to_move)

        for file_to_unlink in list_files('vmtk/lib'):
            if file_to_unlink == 'hints':
                os.remove('vmtk/lib/'+file_to_unlink)
            if os.path.islink('vmtk/lib/'+file_to_unlink):
                os.unlink('vmtk/lib/'+file_to_unlink)
                        
class vmtk_install(_install):
    def run(self):
        _install.run(self)
        home = os.path.expanduser("~")
        package_name = 'vmtk-'+VERSION+'-py2.7.egg-info'
        package_path = self.install_lib+package_name
        
        print "Please run this script with sudo\n"
        print "chmod+x on binary scripts..."
        bin_path = package_path+'/bin'      
        os.system('chmod -R +x '+bin_path)
        
        vmtkHomeEnvironmentVariable="VMTKHOME=%s" % package_path
        vmtkPathEnvironmentVariable="export PATH=$VMTKHOME/bin:$PATH"
        vmtkPythonPath="export PYTHONPATH=$VMTKHOME/vmtk/lib:$PYTHONPATH"
        append_decision = raw_input('Do you want to append vmtk environment variables in your .bash_profile? YES/n: ')
        while True:
            if append_decision.lower() == 'y' or append_decision.lower() == 'yes':
                if sys.platform == 'darwin': 
                    ldEnvironmentVariable="export DYLD_LIBRARY_PATH=$VMTKHOME/vmtk/lib:$DYLD_LIBRARY_PATH"
                    with open(home+'/.bash_profile','aw') as bash_profile:
                        bash_profile.write('\n#VMTK\n')
                        bash_profile.write(vmtkHomeEnvironmentVariable+'\n')
                        bash_profile.write(vmtkPathEnvironmentVariable+'\n')
                        bash_profile.write(ldEnvironmentVariable+'\n')
                        bash_profile.write(vmtkPythonPath+'\n')
                elif sys.platform == 'linux2':
                    ldEnvironmentVariable="export LD_LIBRARY_PATH=$VMTKHOME/vmtk/lib:$LD_LIBRARY_PATH"
                    with open(home+'/.bashrc','aw') as bashrc:
                        bashrc.write('\n#VMTK\n')
                        bashrc.write(vmtkHomeEnvironmentVariable+'\n')
                        bashrc.write(vmtkPathEnvironmentVariable+'\n')
                        bashrc.write(ldEnvironmentVariable+'\n')
                        bashrc.write(vmtkPythonPath+'\n')
                else:
                    #WINDOWS
                    pass
                break
            elif append_decision.lower() == 'n' or append_decision.lower() == 'no':
                break
            else:
                append_decision = raw_input('Do you want to append vmtk environment variables in your .bash_profile? YES/n: ')


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
      cmdclass={'install':vmtk_install, 'vmtk_build':vmtk_build},
      packages = find_packages(),
      zip_safe=False,
      data_files=[('bin', [f for f in glob.glob(os.path.join("bin", '*'))])],
      package_data = {
         "vmtk": ["lib/*.so*","lib/*.*lib*"],
         "vtk": ["lib/*.so*"],
        },
      scripts = ['vmtk_post_install.py']
    )