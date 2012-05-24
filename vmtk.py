#!/usr/bin/env python

import sys
import os

if __name__ == '__main__':

    if sys.platform == 'darwin':
    	ldEnvironmentVariable = "DYLD_LIBRARY_PATH"
    
    elif sys.platform == 'win32':
    	ldEnvironmentVariable = "PATH"
    
    else:
    	ldEnvironmentVariable = "LD_LIBRARY_PATH"
    
    currentEnviron = dict()
    currentEnviron[ldEnvironmentVariable] = ""
    currentEnviron["PYTHONPATH"] = ""
    
    if os.environ.has_key(ldEnvironmentVariable):
    	currentEnviron[ldEnvironmentVariable] = os.environ[ldEnvironmentVariable]
    
    if os.environ.has_key("PYTHONPATH"):
    	currentEnviron["PYTHONPATH"] = os.environ["PYTHONPATH"]
    
    newEnviron = {}
    
    vmtkhome = os.path.dirname(os.path.abspath(__file__))
    
    if vmtkhome.endswith('bin'):
        vmtkhome = os.path.join(os.path.dirname(os.path.abspath(__file__)),"..")
    else:
        vmtkhome = os.path.join(os.path.dirname(os.path.abspath(__file__)),"..","..","..")
    
    vtkdir = [el for el in os.listdir(os.path.join(vmtkhome,"lib")) if el.startswith('vtk')][0]
    
    newEnviron[ldEnvironmentVariable] = os.path.join(vmtkhome,"bin") + os.path.pathsep + \
    								os.path.join(vmtkhome,"lib",vtkdir) + os.path.pathsep + \
    								os.path.join(vmtkhome,"lib","vmtk") + os.path.pathsep + \
    								os.path.join(vmtkhome,"lib","InsightToolkit")
    
    os.environ[ldEnvironmentVariable] = newEnviron[ldEnvironmentVariable] + os.path.pathsep + currentEnviron[ldEnvironmentVariable]
    
    sys.path.append(os.path.join(vmtkhome,"bin","Python"))
    sys.path.append(os.path.join(vmtkhome,"lib",vtkdir))
    sys.path.append(os.path.join(vmtkhome,"lib","vmtk"))
    
    from vmtk import pypes
    
    vmtkOptions = ['--help', '--ui', '--file']
    
    if len(sys.argv) > 1 and sys.argv[1] not in vmtkOptions:
        arguments = sys.argv[1:]
        print "Executing", ' '.join(arguments)
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
        print 'Usage: \tvmtk [--ui pad|console]\t\tStart in interactive mode\n\tvmtk [PYPE]\t\t\tExecute the pype [PYPE]\n\tvmtk --file [FILE]\t\tExecute the content of file [FILE]'
        sys.exit(0)
    else:
        ui = 'pad'
        if '--ui' in sys.argv and sys.argv.index('--ui') != len(sys.argv)-1:
            ui = sys.argv[sys.argv.index('--ui')+1]
    
        if ui == 'pad':
            try:
                from vmtk import pypepad
            except ImportError:
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
                    inputString = raw_input("vmtk> ")
                except EOFError:
                    sys.stdout.write('\n')
                    sys.exit(0)
                if not inputString:
                    continue
                print "Executing", inputString
                splitString = inputString.split()
                pipe = pypes.Pype()
                pipe.ExitOnError = 0
                pipe.Arguments = inputString.split()
                pipe.ParseArguments()
                try: 
                    pipe.Execute() 
                except Exception:
                    continue
    
