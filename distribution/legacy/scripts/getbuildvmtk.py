#!/usr/bin/env python

"""
Simple build script for VTK, ITK, and vmtk.
"""


from __future__ import print_function, absolute_import # NEED TO STAY AS TOP IMPORT
import os
import sys
import platform
import urllib.request, urllib.parse, urllib.error
import tarfile
import atexit
import multiprocessing

from stat import S_IRUSR, S_IWUSR, S_IXUSR

BUILD_TYPE = "Release"
OSX_ARCHITECTURES = "i386"  # "i386;x86_64"
MINGW = False
WORK_DIR = os.path.abspath(os.path.curdir)
VTK_BUILD_DIR = os.path.join(WORK_DIR, 'vtk-build')
ITK_BUILD_DIR = os.path.join(WORK_DIR, 'itk-build')
VMTK_BUILD_DIR = os.path.join(WORK_DIR, 'vmtk-build')
PARALLEL_JOBS = multiprocessing.cpu_count() - 1

VERBOSE = True
LOGFILE = True
log_file = None


def log(msg):
    if VERBOSE:
        print(msg)
    if LOGFILE:
        global log_file
        if not log_file:
            log_file = open(os.path.join(WORK_DIR, 'logfile.log'), 'w+')

            def log_close():
                if log_file is not None:
                    log_file.close()
            atexit.register(log_close)
        log_file.write(msg)


def on_windows():
    return platform.system() == 'Windows'


def on_osx():
    return platform.system() == 'Darwin'


def on_linux():
    return platform.system() == 'Linux'


def on_x86():
    return platform.architecture()[0] == '32bit'


def on_x64():
    return platform.architecture()[0] == '64bit'


def download(url, filename):
    log("Downloading %s..." % filename)
    urllib.request.urlretrieve(url, filename)


def tarextract(filename, path='.'):
    tar = tarfile.open(filename)
    log("Extracting %s..." % filename)
    tar.extractall(path)


def getstatusoutput(cmd):
    """Return (status, output) of executing cmd in a shell."""
    if on_windows():
        pipe = os.popen(cmd + ' 2>&1', 'r')
    else:
        pipe = os.popen('{ ' + cmd + '; } 2>&1', 'r')
    text = pipe.read()
    sts = pipe.close()
    if sts is None: sts = 0
    if text[-1:] == '\n': text = text[:-1]
    return sts, text


def create_do_configure(build_dir, cmd):
    """Create a do-configure(.bat) for later possible tweaking."""
    do_configure = os.path.join(build_dir, 'do-configure')
    if on_windows():
        do_configure += '.bat'
        lines = ' ^\n '.join(cmd.split())
    else:
        lines = "#!/bin/bash \n\n"
        lines += ' \\\n '.join(cmd.split())
    lines += '\n'
    open(do_configure, 'w').writelines(lines)
    if not on_windows():
        os.chmod(do_configure, S_IRUSR|S_IWUSR|S_IXUSR)


def build_cmake_project(build_dir, source_dir, options="", install=False):
    if not os.path.isdir(build_dir):
        os.mkdir(build_dir)
    os.chdir(build_dir)
    cmd = "cmake"
    if MINGW:
        cmd += ' -G "MinGW Makefiles"'
    cmd += " %s %s" % (options, source_dir)
    create_do_configure(build_dir, cmd)
    log("Running %s..." % cmd)
    failure, output = getstatusoutput(cmd)
    log(output)
    if failure:
        msg = "Unable to run '%s'\nError message: %s" % (cmd, output)
        raise Exception(msg)
    log(output)

    if on_windows():
        cmd = "nmake"
    else:
        cmd = "make -j%s" % PARALLEL_JOBS
    log("Running %s in %s..." % (cmd, build_dir))
    failure, output = getstatusoutput(cmd)
    log(output)
    if failure:
        msg = "Unable to run '%s'\nError message: %s" % (cmd, output)
        raise Exception(msg)

    if install:
        cmd = cmd + "install"
        log("Running %s in %s..." % (cmd, build_dir))
        failure, output = getstatusoutput(cmd)
        log(output)
        if failure:
            msg = "Unable to run '%s'\nError message: %s" % (cmd, output)
            raise Exception(msg)
    os.chdir(WORK_DIR)


def build_vtk():
    filename = "VTK-7.1.1.tar.gz"
    url = "http://www.vtk.org/files/release/7.1/" + filename
    source_dir = os.path.join(WORK_DIR, 'VTK-7.1.1')
    install_dir = os.path.join(WORK_DIR, 'vtk-bin')
    if not os.path.exists(filename):
        download(url, filename)
    if not os.path.isdir(source_dir):
        tarextract(filename)
    # minor fix for installing VTK Python Wrappers (avoid using setuptools)
    setup_py = os.path.join(source_dir, 'Wrapping', 'Python', 'compile_all_vtk.py.in')
    lines = open(setup_py, 'r').readlines()
    newlines = []
    for line in lines:
        newlines.append(line)
        if 'has_setup_tools = 1' in line:
            newlines.append('has_setup_tools = 0\n')
    os.chmod(setup_py, S_IRUSR|S_IWUSR)  # make the file read/writable
    open(setup_py, 'w').writelines(newlines)

    options = ['-DCMAKE_BUILD_TYPE:STRING=' + BUILD_TYPE,
               '-DCMAKE_INSTALL_PREFIX:PATH=' + repr(install_dir)[1:-1],
               '-DBUILD_TESTING:BOOL=OFF',
               '-DBUILD_SHARED_LIBS:BOOL=ON',
               '-DVTK_WRAP_PYTHON:BOOL=ON',
               '-DVTK_WRAP_TCL:BOOL=OFF',
               '-DVTK_INSTALL_PYTHON_USING_CMAKE:BOOL=ON',
               '-VMTK_USE_VTK8:BOOL=ON'
               ]
    if on_osx():
        options.extend(['-DCMAKE_OSX_ARCHITECTURES:STRING="%s"' % OSX_ARCHITECTURES,
                        '-DCMAKE_OSX_SYSROOT:PATH=/Developer/SDKs/MacOSX10.6.sdk',
                        '-DCMAKE_OSX_DEPLOYMENT_TARGET:STRING=10.5',
                        '-DVTK_USE_TK:BOOL=ON',
                        ])
    if on_windows():
        py_ver_major, py_ver_minor = sys.version_info[0:2]
        python_inc_dir = os.path.join(os.path.dirname(sys.executable), 'include')
        if MINGW:
            options.append('-DCMAKE_USE_PTHREADS:BOOL=OFF')
            python_lib = 'libpython%d%d.a' % (py_ver_major, py_ver_minor)
        else:
            python_lib = 'python%d%d.lib' % (py_ver_major, py_ver_minor)
        python_lib = os.path.join(os.path.dirname(sys.executable), 'libs', python_lib)
        options.extend(['-DVTK_USE_TK:BOOL=OFF',
                        '-DPYTHON_EXECUTABLE:FILEPATH=' + sys.executable,
                        '-DPYTHON_INCLUDE_DIR:PATH=' + python_inc_dir,
                        '-DPYTHON_LIBRARY:FILEPATH=' + python_lib])
    options = ' '.join(options)

    build_cmake_project(VTK_BUILD_DIR, source_dir, options)


def build_itk():
    filename = "InsightToolkit-4.10.1.tar.gz"
    url = "http://downloads.sourceforge.net/project/itk/itk/4.10/" + filename
    source_dir = os.path.join(WORK_DIR, 'InsightToolkit-4.10.1')
    install_dir = os.path.join(WORK_DIR, 'itk-bin')
    if not os.path.exists(filename):
        download(url, filename)
    if not os.path.isdir(source_dir):
        tarextract(filename)

    options = ['-DCMAKE_BUILD_TYPE:STRING=' + BUILD_TYPE,
               '-DCMAKE_INSTALL_PREFIX:PATH=' + repr(install_dir)[1:-1],
               '-DBUILD_TESTING:BOOL=OFF',
               '-DBUILD_SHARED_LIBS:BOOL=ON',
               ]
    if on_osx():
        options.extend(['-DCMAKE_OSX_ARCHITECTURES:STRING="%s"' % OSX_ARCHITECTURES,
                        '-DCMAKE_OSX_SYSROOT:PATH=/Developer/SDKs/MacOSX10.6.sdk',
                        '-DCMAKE_OSX_DEPLOYMENT_TARGET:STRING=10.5',
                        ])
    if on_windows() and MINGW:
        options.append('-DCMAKE_USE_PTHREADS:BOOL=OFF')
    options = ' '.join(options)

    build_cmake_project(ITK_BUILD_DIR, source_dir, options)


def build_vmtk():
    source_dir = WORK_DIR # os.path.join(WORK_DIR, 'vmtk-packaging')
    install_dir = os.path.join(WORK_DIR, 'vmtk-bin')
    # if not os.path.isdir(source_dir):
    #     cmd = "bzr branch lp:vmtk-packaging"
    #     failure, output = getstatusoutput(cmd)
    #     log(output)
    #     if failure:
    #         msg = "Unable to run '%s'\nError message: %s" % (cmd, output)
    #         raise Exception(msg)

    options = ['-DCMAKE_BUILD_TYPE:STRING=' + BUILD_TYPE,
               '-DCMAKE_INSTALL_PREFIX:PATH=' + repr(install_dir)[1:-1],
               '-DBUILD_SHARED_LIBS:BOOL=ON',
               '-DVMTK_USE_SUPERBUILD:BOOL=OFF'
               '-DITK_DIR:PATH=' + repr(ITK_BUILD_DIR)[1:-1],
               '-DVTK_DIR:PATH=' + repr(VTK_BUILD_DIR)[1:-1],
               '-DVTK_VMTK_WRAP_PYTHON:BOOL=ON',
               '-DVTK_VMTK_WRAP_TCL:BOOL=OFF',
               '-DVTK_VMTK_BUILD_TETGEN:BOOL=ON',
               '-DVMTK_WITH_LIBRARY_VERSION:BOOL=ON',
               '-DVMTK_ENABLE_DISTRIBUTION:BOOL=ON',
               ]
    if on_osx():
        options.extend(['-DCMAKE_OSX_ARCHITECTURES:STRING="%s"' % OSX_ARCHITECTURES,
                        '-DCMAKE_OSX_SYSROOT:PATH=/Developer/SDKs/MacOSX10.6.sdk',
                        '-DCMAKE_OSX_DEPLOYMENT_TARGET:STRING=10.5',
                        ])
    if on_windows():
        options.extend(['-DVMTK_INSTALL_PYTHON:BOOLD=ON',])
        if not MINGW:
            vcredist_x86 = os.path.abspath(os.path.join(WORK_DIR, "vcredist_x86.exe"))
            vcredist_x64 = os.path.abspath(os.path.join(WORK_DIR, "vcredist_x64.exe"))
            if on_x86():
                if not os.path.isfile(vcredist_x86):
                    download("http://download.microsoft.com/download/d/d/9/dd9a82d0-52ef-40db-8dab-795376989c03/vcredist_x86.exe")
                options.append('-DVCREDIST_EXE:FILEPATH=' + vcredist_x86)
            if on_x64():
                if not os.path.isfile(vcredist_x64):
                    download("http://download.microsoft.com/download/2/d/6/2d61c766-107b-409d-8fba-c39e61ca08e8/vcredist_x64.exe")
                options.append('-DVCREDIST_EXE:FILEPATH=' + vcredist_x64)
    options = ' '.join(options)

    build_cmake_project(VMTK_BUILD_DIR, source_dir, options)


def generate_package(generator=None):
    generator="TGZ"
    os.chdir(VMTK_BUILD_DIR)
    cmd = "cpack"
    if generator is not None:
        cmd += " -G %s" % generator
    failure, output = getstatusoutput(cmd)
    log(output)
    if failure:
        msg = "Unable to run '%s'\nError message: %s" % (cmd, output)
        raise Exception(msg)
    os.chdir(WORK_DIR)


def build_all():
    build_vtk()
    build_itk()
    build_vmtk()


if __name__ == '__main__':
    build_all()
    generate_package()
