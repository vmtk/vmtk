#!/usr/bin/env python

from __future__ import print_function, absolute_import # NEED TO STAY AS TOP IMPORT
import sys
import os


if __name__ == '__main__':

    # hack for code compatibility in python 2 and 3 as there is no __future__ module which remaps the new
    # python 3 style input to the equivalent raw_input function in python 2
    try:
        input = raw_input
    except NameError:
        pass

    if sys.platform == 'win32':

        vmtkhome = os.path.dirname(os.path.abspath(__file__))

        if vmtkhome.endswith('bin'):
            vmtkhome = os.path.join(os.path.dirname(os.path.abspath(__file__)),"..")
            os.environ["PYTHONPATH"] = os.path.join(vmtkhome)
        else:
            vmtkhome = os.path.join(os.path.dirname(os.path.abspath(__file__)),"..","..","..")
            os.environ["PYTHONPATH"] = os.path.join(vmtkhome,"lib","${VMTK_PYTHON_VERSION}","site-packages")

        sys.path.append(os.path.join(vmtkhome,"bin"))
        sys.path.append(os.environ["PYTHONPATH"])
        os.environ["PATH"] += os.path.pathsep + os.path.join(vmtkhome,"bin")

    import vtk
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
        sys.exit(0)
    elif len(sys.argv) > 1 and '--file' in sys.argv:
        fileindex = sys.argv.index('--file')+1
        if fileindex < len(sys.argv):
            inputfile = open(sys.argv[fileindex],'r')
            physicalLines = [line for line in inputfile.readlines() if line and line.strip() and line.strip()[0]!='#']
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
    elif '--help' in sys.argv:
        print('Usage: \tvmtk [--ui pad|console]\t\tStart in interactive mode\n\tvmtk [PYPE]\t\t\tExecute the pype [PYPE]\n\tvmtk --file [FILE]\t\tExecute the content of file [FILE]')
        sys.exit(0)
    else:
        ui = 'pad'
        if '--ui' in sys.argv and sys.argv.index('--ui') != len(sys.argv)-1:
            ui = sys.argv[sys.argv.index('--ui')+1]

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
                    sys.exit(0)
                if not inputString:
                    continue
                print("Executing", inputString)
                splitString = inputString.split()
                pipe = pypes.Pype()
                pipe.ExitOnError = 0
                pipe.Arguments = inputString.split()
                pipe.ParseArguments()
                try:
                    pipe.Execute()
                except Exception:
                    continue
