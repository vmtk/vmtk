## Module:    $RCSfile: vmtknumpywriter.py,v $
## Language:  Python
## Date:      June 10, 2017
## Version:   1.4

##   Copyright (c) Richard Izzo, Luca Antiga, David Steinman. All rights reserved.
##   See LICENCE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

## Note: this class was contributed by
##       Richard Izzo (Github @rlizzo)
##       University at Buffalo
##       The Jacobs Institute

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
import sys

from vmtk import vtkvmtk
from vmtk import vmtkrenderer
from vmtk import pypes
import pickle
import os

try:
    import numpy as np
except ImportError:
    raise ImportError('Unable to Import vmtknumpytosurface module, numpy is not installed')

class vmtkNumpyWriter(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.InputDict = None
        self.OutputFileName = ''
        self.Format = 'pickle'

        self.SetScriptName('vmtkNumpyWriter')
        self.SetScriptDoc('Writes a dictionary containing numpy array data to a file')

        self.SetInputMembers([
            ['ArrayDict','i','dict',1,'','the input dictionary'],
            ['OutputFileName','ofile','str',1,'','the output file name'],
            ['Format','format','str',1,'["pickle","hdf5"]','write files as pickled object or hdf5 file format']])
        self.SetOutputMembers([])

    def WritePickledObjectFile(self):
        self.PrintLog('Writing Pickled Object File')
        pickleFileName = self.OutputFileName + '.pickle'
        with open(pickleFileName, 'wb') as outfile:
            pickle.dump(self.ArrayDict, outfile, protocol=pickle.HIGHEST_PROTOCOL)

    def WriteHDF5File(self): # dic, filename, objectname=None):
        """
        Save a dictionary to an HDF5 file.
        """

        try:
            import h5py
        except ImportError:
            self.PrintError('ImportError: Unable to Write to hdf5. h5py module not installed')
            raise ImportError('Unable to Write to hdf5. h5py module not installed')

        def recursively_save_dict_contents_to_group(h5file, path, dic):
            """
            Take an already open HDF5 file and insert the contents of a dictionary
            at the current path location. Can call itself recursively to fill
            out HDF5 files with the contents of a dictionary.
            """
            for key, item in dic.items():
                if isinstance(item, dict):
                    recursively_save_dict_contents_to_group(h5file, path + key + '/', item)
                else:
                    h5file[path + key] = item

        self.PrintLog('Writing HDF5 File')
        hdf5FileName = self.OutputFileName + '.hdf5'
        with h5py.File(hdf5FileName, 'w') as h5file:
            recursively_save_dict_contents_to_group(h5file, '/', self.ArrayDict)

    def Execute(self):

        if self.ArrayDict == None:
            self.PrintError('Error: no input dictionary')

        if self.OutputFileName == '':
            self.PrintError('Error: no output file name specified')

        if self.OutputFileName == 'BROWSER':
            import tkinter.filedialog
            import os.path
            initialDir = pypes.pypeScript.lastVisitedPath
            self.OutputFileName = tkinter.filedialog.asksaveasfilename(title="Output Dictionary",initialdir=initialDir)
            pypes.pypeScript.lastVisitedPath = os.path.dirname(self.OutputFileName)
            if not self.OutputFileName:
                self.PrintError('Error: no Output File Name.')

        self.OutputFileName = self.OutputFileName.rsplit( ".", 1 )[ 0 ]

        self.PrintLog('Writing File')
        if self.Format == 'pickle':
            self.WritePickledObjectFile()
        elif self.Format == 'hdf5':
            self.WriteHDF5File()
        else:
            self.PrintError('Error: unsupported format '+ self.Format + '.')


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()