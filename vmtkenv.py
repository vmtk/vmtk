#!/usr/bin/python

import sys
import os
import os.path

if not os.environ.has_key("VMTKHOME"):
    vmtkhome = os.path.join(os.path.dirname(os.path.abspath(sys.argv[0])),os.path.pardir)

    os.environ["VMTKHOME"] = vmtkhome
    
    currentEnviron = {"PATH":"", "LD_LIBRARY_PATH":"", "DYLD_LIBRARY_PATH":"", "PYTHONPATH":""}
    
    if os.environ.has_key("PATH"):
        currentEnviron["PATH"] = os.environ["PATH"]
    
    if os.environ.has_key("LD_LIBRARY_PATH"):
        currentEnviron["LD_LIBRARY_PATH"] = os.environ["LD_LIBRARY_PATH"]
    
    if os.environ.has_key("DYLD_LIBRARY_PATH"):
        currentEnviron["DYLD_LIBRARY_PATH"] = os.environ["DYLD_LIBRARY_PATH"]
    
    if os.environ.has_key("PYTHONPATH"):
        currentEnviron["PYTHONPATH"] = os.environ["PYTHONPATH"]

    newEnviron = {}
    
    newEnviron["PATH"] = os.path.join(vmtkhome,'bin')    

    newEnviron["LD_LIBRARY_PATH"] = os.path.join(vmtkhome,"lib") + os.path.pathsep + \
                                    os.path.join(vmtkhome,"lib/vmtk") + os.path.pathsep + \
                                    os.path.join(vmtkhome,"lib/InsightToolkit")
    
    newEnviron["DYLD_LIBRARY_PATH"] = os.path.join(vmtkhome,"lib") + os.path.pathsep + \
                                      os.path.join(vmtkhome,"lib/vmtk") + os.path.pathsep + \
                                      os.path.join(vmtkhome,"lib/InsightToolkit")
    
    newEnviron["PYTHONPATH"] =  os.path.join(vmtkhome,"lib/python2.3/site-packages") + os.path.pathsep + \
                                os.path.join(vmtkhome,"lib/vmtk")

    if len(sys.argv) > 1:

        os.environ["PATH"] = newEnviron["PATH"] + os.path.pathsep + currentEnviron["PATH"]
        os.environ["LD_LIBRARY_PATH"] = newEnviron["LD_LIBRARY_PATH"] + os.path.pathsep + currentEnviron["LD_LIBRARY_PATH"]
        os.environ["DYLD_LIBRARY_PATH"] = newEnviron["DYLD_LIBRARY_PATH"] + os.path.pathsep + currentEnviron["DYLD_LIBRARY_PATH"]
        os.environ["PYTHONPATH"] = newEnviron["PYTHONPATH"] + os.path.pathsep + currentEnviron["PYTHONPATH"]
        os.system(" ".join(sys.argv))
        raise SystemExit

    else:

        print "PATH=%s:$PATH" % newEnviron["PATH"]
        print "LD_LIBRARY_PATH=%s:$LD_LIBRARY_PATH" % newEnviron["LD_LIBRARY_PATH"]
        print "DYLD_LIBRARY_PATH=%s:$DYLD_LIBRARY_PATH" % newEnviron["DYLD_LIBRARY_PATH"]
        print "PYTHONPATH=%s:$PYTHONPATH" % newEnviron["PYTHONPATH"]

os.system(" ".join(sys.argv[1:]))

