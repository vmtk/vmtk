## Program: VMTK
## Language:  Python
## Date:      January 10, 2018
## Version:   1.4

##   Copyright (c) Richard Izzo, Luca Antiga, All rights reserved.
##   See LICENCE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

## Note: this code was contributed by
##       Richard Izzo (Github @rlizzo)
##       University at Buffalo

# This script is designed to take data from an input directory, read all
# files, and write a file  with the name "ifile.ext" md5 hash to a seperate
# file (keeping the relative directory structure the same) in a seperate
# directory with the file name "ifile.ext.md5"

import argparse
import os
import sys 
import glob
import hashlib
import shutil

parser = argparse.ArgumentParser()
parser.add_argument("idir", type=str, 
    help="the input directory for all data files to hash")
parser.add_argument("odir", type=str, 
    help="the output directory to store hashed file digests")
args = parser.parse_args()


# handle relative path names.
if args.idir.startswith('./'):
    args.idir = args.idir.replace('./', '')
    args.idir = os.path.join(os.getcwd(), args.idir)
elif args.idir.startswith('../'):
    args.idir = args.idir.replace('../', '')
    # join dir name one directory up from calling directory
    args.idir = os.path.join(os.path.dirname(os.getcwd()), args.idir)

#handle relative path names
if args.odir.startswith('./'):
    args.odir = args.odir.replace('./', '')
    args.odir = os.path.join(os.getcwd(), args.odir)
elif args.odir.startswith('../'):
    args.odir = args.odir.replace('../', '')
    # join dir name one directory up from calling directory
    args.odir = os.path.join(os.path.dirname(os.getcwd()), args.odir)

'''
this next block finds the full path to files, and a relative path for the same file to place in the output directory.
file in hidden (dot) directories are ignored, as are mark-down files

Examples:
inFilepath: /Users/rick/projects/vmtk/vmtk-test-data/input/aorta-surface.stl
relpath: input/aorta-surface.stl.md5
outFilepath: /Users/rick/projects/vmtk/vmtk/tests/data/input/aorta-surface.stl.md5

inFilepath: /Users/rick/projects/vmtk/vmtk-test-data/surfacereference/test_vmtksurfacesmoothing_test_laplace_change_relaxation.vtp
relpath: surfacereference/test_vmtksurfacesmoothing_test_laplace_change_relaxation.vtp.md5
outFilepath: /Users/rick/projects/vmtk/vmtk/tests/data/surfacereference/test_vmtksurfacesmoothing_test_laplace_change_relaxation.vtp.md5
''' 
for root, dirs, files in os.walk(args.idir):
    # exclude directories and files in hidden folders ('dot' directories)
    files = [f for f in files if not f[0] == '.']
    dirs[:] = [d for d in dirs if not d[0] == '.']
    for ifile in files:
        # full path to input file
        inFilepath = os.path.join(root, ifile) 
        if inFilepath.lower().endswith('.md'):
            continue
        else:
            outFilepath = os.path.join(args.odir, '.sha512')

        with open(inFilepath, 'rb') as f:
            data = f.read()
            inFilehash = hashlib.sha512(data).hexdigest()

        outFilepath = os.path.join(outFilepath, inFilehash)

        if not os.path.exists(os.path.dirname(outFilepath)):
            os.makedirs(os.path.dirname(outFilepath))

        shutil.copyfile(inFilepath, outFilepath)

        print(f'processed file {inFilepath} to {outFilepath}')