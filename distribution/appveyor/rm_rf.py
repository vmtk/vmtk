## Program: VMTK
## Language:  Python
## Date:      January 30, 2018
## Version:   1.4

##   Copyright (c) Richard Izzo, Luca Antiga, All rights reserved.
##   See LICENCE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

## Note: this code was contributed by
##       Richard Izzo (Github @rlizzo)
##       University at Buffalo

from __future__ import print_function
import os
import sys
import stat
import shutil

def remove_readonly(func, path, excinfo):
    os.chmod(path, stat.S_IWRITE)
    func(path)

def main():
    print(sys.executable)
    try:
        shutil.rmtree(sys.argv[1], onerror=remove_readonly)
    except Exception as e:
        print("Error")
        print(e)

if __name__ == '__main__':
    main()