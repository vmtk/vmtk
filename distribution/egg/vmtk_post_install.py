#!/usr/bin/env python

import sys
import os
import pkg_resources
home = os.path.expanduser("~")

VERSION = "1.2"

if __name__ == '__main__':
    
    if sys.platform == "win32":
        
        #=======================================================================
        # target = # target is the path to the program to be started by the shortcut
        # description = 'vmtk-launcher' #description is the description of the shortcut
        # filename = 'vmtk-'+VERSION #filename is the title of the shortcut that the user will see
        # arguments = # arguments specifies the command line arguments
        # workdir = #workdir is the working directory for the program
        # iconpath = #iconpath is the file containing the icon for the shortcut
        # iconindex = #iconindex is the index of the icon in the file iconpath    
        # 
        # create_shortcut(target, description, filename[, arguments[, workdir[, iconpath[, iconindex]]]])
        # 
        #=======================================================================
        #windows
        pass
    else:
        package_name = 'vmtk-'+VERSION+'-py2.7.egg-info'
        package_path = pkg_resources.get_distribution("vmtk").location
        print "Please run this script with sudo\n"
        bin_path = os.path.join(package_path,'vmtk','bin')
        print "chmod+x on binary scripts...%s"% bin_path  
        os.system('chmod -R +x '+bin_path)   
        vmtkHomeEnvironmentVariable="VMTKHOME=%s" % package_path
        vmtkPathEnvironmentVariable="export PATH=$VMTKHOME/vmtk/bin:$PATH"
        vmtkPythonPath="export PYTHONPATH=$VMTKHOME/vmtk/lib:$VMTKHOME/vmtk:$PYTHONPATH"
        if sys.platform == 'darwin':
            append_decision = raw_input('Do you want to append vmtk environment variables in your .bash_profile? yes/no: ')
        elif sys.platform == 'linux2':
            append_decision = raw_input('Do you want to append vmtk environment variables in your .bashrc? yes/no: ')
        else:
            sys.exit("Unknown platform.")
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
                    sys.exit("Unknown platform.")
                break
            elif append_decision.lower() == 'n' or append_decision.lower() == 'no':
                break
            else:
                if sys.platform == 'darwin':
                    append_decision = raw_input('Do you want to append vmtk environment variables in your .bash_profile? yes/no: ')
                elif sys.platform == 'linux2':
                    append_decision = raw_input('Do you want to append vmtk environment variables in your .bashrc? yes/no: ')
                else:
                    sys.exit("Unknown platform.")