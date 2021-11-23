#!${PYTHON_SHEBANG}

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import sys
import os
import shutil
home = os.path.expanduser("~")

VERSION = "1.3"

if __name__ == '__main__':

    if sys.platform == "win32":
        try:
            import pkg_resources
            package_path = pkg_resources.get_distribution("vmtk").location
        except:
            package_path = [f for f in sys.path if 'Python27\\lib\\site-packages' in f][0]
        vmtk_exe_path = os.path.join(package_path,"vmtk","bin","vmtk-exe.py")
        DESKTOP_FOLDER = get_special_folder_path("CSIDL_DESKTOPDIRECTORY")
        target = vmtk_exe_path
        description = "vmtk-launcher"
        filename = "vmtk-%s.lnk" % VERSION
        iconpath = os.path.join(package_path,"vmtk","bin","vmtk-icon.ico")
        create_shortcut(target,description,filename,'','',iconpath)
        #move shortcut to Desktop
        shutil.move(os.path.join(os.getcwd(),filename),os.path.join(DESKTOP_FOLDER,filename))
        file_created(os.path.join(DESKTOP_FOLDER,filename))
