#!/usr/bin/env python

## Program:   PypeS
## Module:    pypemain.py
## Language:  Python

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

"""Entry point for the "vmtk" command installed by the PyPI package.

This provides the same behavior as the vmtk launcher script at the root of
the source tree (vmtk.py), as an importable function that can be declared
as a console_scripts entry point in setup.py:

  vmtk <pype>          execute a pype (e.g. "vmtk vmtkimageviewer -ifile image.vti")
  vmtk --file FILE     execute the pypes listed in FILE
  vmtk [--ui pad]      start the interactive PypePad user interface
  vmtk --ui console    start an interactive console
  vmtk --help          print usage information
"""

from __future__ import print_function, absolute_import

import sys


def main():
    from vmtk import pypes

    vmtkOptions = ['--help', '--ui', '--file']

    if len(sys.argv) > 1 and sys.argv[1] not in vmtkOptions:
        arguments = sys.argv[1:]
        print("Executing", ' '.join(arguments))
        pipe = pypes.Pype()
        pipe.ExitOnError = 0
        pipe.Arguments = arguments
        pipe.ParseArguments()
        pipe.Execute()
        return 0

    if len(sys.argv) > 1 and '--file' in sys.argv:
        fileindex = sys.argv.index('--file') + 1
        if fileindex < len(sys.argv):
            with open(sys.argv[fileindex], 'r') as inputfile:
                physicalLines = [line for line in inputfile.readlines()
                                 if line and line.strip() and line.strip()[0] != '#']
            lines = []
            for line in physicalLines:
                if lines and lines[-1].endswith('\\\n'):
                    lines[-1] = lines[-1][:-2] + line
                else:
                    lines.append(line)
            for line in lines:
                pipe = pypes.Pype()
                pipe.ExitOnError = 0
                pipe.Arguments = line.split()
                pipe.ParseArguments()
                pipe.Execute()
        return 0

    if '--help' in sys.argv:
        print('Usage: \tvmtk [--ui pad|console]\t\tStart in interactive mode'
              '\n\tvmtk [PYPE]\t\t\tExecute the pype [PYPE]'
              '\n\tvmtk --file [FILE]\t\tExecute the content of file [FILE]')
        return 0

    ui = 'pad'
    if '--ui' in sys.argv and sys.argv.index('--ui') != len(sys.argv) - 1:
        ui = sys.argv[sys.argv.index('--ui') + 1]

    if ui == 'pad':
        try:
            from vmtk import pypepad
        except ImportError:
            print("error")
            ui = 'console'
        else:
            pypepad.RunPypeTkPad()

    if ui == 'console':
        try:
            import readline
        except ImportError:
            pass
        else:
            readline.parse_and_bind("tab: complete")

        while 1:
            try:
                inputString = input("vmtk> ")
            except EOFError:
                sys.stdout.write('\n')
                return 0
            if not inputString:
                continue
            print("Executing", inputString)
            pipe = pypes.Pype()
            pipe.ExitOnError = 0
            pipe.Arguments = inputString.split()
            pipe.ParseArguments()
            try:
                pipe.Execute()
            except Exception:
                continue

    return 0


if __name__ == '__main__':
    sys.exit(main())
