#!/usr/bin/env python
"""Check that the native libraries of an installed vmtk package are self-contained.

Every shared library and extension module of the package must find its
dependencies inside the installed environment: in the package directory
itself, in the directory the wheel repair tool put the bundled libraries
in, or in the "vtk" package that provides the VTK libraries.

The check exists because the wheels are built and tested on the same
machine: the build trees of the dependencies (ITK in particular) are
still present there, and a library that resolves a dependency through a
run path pointing into such a build tree imports fine on the build
machine and fails everywhere else. Run paths that lead outside the
environment are therefore ignored while resolving.

Mach-O, ELF and PE binaries are parsed here rather than through otool,
readelf or dumpbin, which are not installed in every wheel test
environment. Windows binaries have no run paths: their DLLs must be in
one of the directories that the package __init__ registers with
os.add_dll_directory, which are searched here instead.

Run it against an installed package (no arguments) or against the
package directory of an unpacked wheel (one argument).
"""

import os
import struct
import sys

# Dependencies found in these directories, or whose name starts with one
# of these prefixes, are satisfied by the operating system.
SYSTEM_PREFIXES = ("/usr/lib/", "/System/", "/lib/", "/lib64/", "/usr/lib64/")
SYSTEM_DIRS = ("/lib64", "/usr/lib64", "/lib", "/usr/lib", "/usr/lib/x86_64-linux-gnu",
               "/usr/lib/aarch64-linux-gnu")
SYSTEM_NAMES = ("libc.so", "libm.so", "libdl.so", "libpthread.so", "librt.so",
                "libstdc++.so", "libgcc_s.so", "ld-linux", "libutil.so", "libGL.so",
                "libX11.so", "libXt.so", "libXcursor.so")
WINDOWS_SYSTEM_DIRS = tuple(
    os.path.join(os.environ.get("SystemRoot", r"C:\Windows"), sub)
    for sub in ("System32", "SysWOW64"))

LC_LOAD_DYLIB = 0x0C
LC_LOAD_WEAK_DYLIB = 0x18 | 0x80000000
LC_RPATH = 0x1C | 0x80000000
MACHO_64 = 0xFEEDFACF
FAT_MAGICS = (0xCAFEBABE, 0xCAFEBABF)

DT_NULL, DT_NEEDED, DT_STRTAB, DT_RPATH, DT_RUNPATH = 0, 1, 5, 15, 29


def macho_slices(data):
    """Return the 64-bit Mach-O images in a thin or fat binary."""
    if len(data) < 8:
        return []
    if struct.unpack(">I", data[:4])[0] in FAT_MAGICS:
        count = struct.unpack(">I", data[4:8])[0]
        images = []
        for i in range(count):
            offset, size = struct.unpack(">2I", data[8 + i * 20 + 8:8 + i * 20 + 16])
            images.append(data[offset:offset + size])
        return [image for image in images
                if len(image) > 4 and struct.unpack("<I", image[:4])[0] == MACHO_64]
    if struct.unpack("<I", data[:4])[0] == MACHO_64:
        return [data]
    return []


def macho_dependencies(data):
    """Return the (dependencies, run paths) of one Mach-O image."""
    command_count = struct.unpack("<I", data[16:20])[0]
    offset = 32
    dependencies, run_paths = [], []
    for _ in range(command_count):
        command, size = struct.unpack("<2I", data[offset:offset + 8])
        if command in (LC_LOAD_DYLIB, LC_LOAD_WEAK_DYLIB, LC_RPATH):
            name_offset = struct.unpack("<I", data[offset + 8:offset + 12])[0]
            name = data[offset + name_offset:offset + size].split(b"\0")[0].decode()
            (run_paths if command == LC_RPATH else dependencies).append(name)
        offset += size
    return dependencies, run_paths


def elf_dependencies(data):
    """Return the (dependencies, run paths) of a 64-bit ELF binary, or None."""
    if data[:4] != b"\x7fELF" or data[4] != 2:
        return None
    section_offset = struct.unpack("<Q", data[0x28:0x30])[0]
    section_size, section_count = struct.unpack("<HH", data[0x3A:0x3E])
    dynamic = None
    for i in range(section_count):
        header = section_offset + i * section_size
        if struct.unpack("<I", data[header + 4:header + 8])[0] == 6:  # SHT_DYNAMIC
            dynamic = struct.unpack("<QQ", data[header + 0x18:header + 0x28])
            break
    if dynamic is None:
        return None
    entries = []
    for position in range(dynamic[0], dynamic[0] + dynamic[1], 16):
        tag, value = struct.unpack("<Qq", data[position:position + 16])
        if tag == DT_NULL:
            break
        entries.append((tag, value))

    # The string table is given by its virtual address; map it to a file offset.
    addresses = [value for tag, value in entries if tag == DT_STRTAB]
    if not addresses:
        return None
    strings = None
    segment_offset = struct.unpack("<Q", data[0x20:0x28])[0]
    segment_size, segment_count = struct.unpack("<HH", data[0x36:0x3A])
    for i in range(segment_count):
        header = segment_offset + i * segment_size
        if struct.unpack("<I", data[header:header + 4])[0] != 1:  # PT_LOAD
            continue
        offset, address = struct.unpack("<QQ", data[header + 8:header + 0x18])
        size = struct.unpack("<Q", data[header + 0x20:header + 0x28])[0]
        if address <= addresses[0] < address + size:
            strings = offset + (addresses[0] - address)
            break
    if strings is None:
        return None

    def string(index):
        start = strings + index
        return data[start:data.index(b"\0", start)].decode()

    dependencies = [string(value) for tag, value in entries if tag == DT_NEEDED]
    run_paths = []
    for tag, value in entries:
        if tag in (DT_RPATH, DT_RUNPATH):
            run_paths.extend(string(value).split(":"))
    return dependencies, run_paths


def pe_dependencies(data):
    """Return the (imported DLL names, run paths) of a PE binary, or None.

    Windows has no run paths: the extension modules find their DLLs in the
    directories the package __init__ registers with os.add_dll_directory
    (the package directory and the sibling vtkmodules directory), which the
    caller passes as search directories instead.
    """
    if data[:2] != b"MZ":
        return None
    header = struct.unpack("<I", data[0x3C:0x40])[0]
    if data[header:header + 4] != b"PE\0\0":
        return None
    section_count = struct.unpack("<H", data[header + 6:header + 8])[0]
    optional_size = struct.unpack("<H", data[header + 20:header + 22])[0]
    optional = header + 24
    magic = struct.unpack("<H", data[optional:optional + 2])[0]
    if magic != 0x20B:  # PE32+ (64-bit); no 32-bit wheels are built
        return None
    # Data directories start at offset 112 of a PE32+ optional header; the
    # import directory is the second one.
    imports = struct.unpack("<I", data[optional + 120:optional + 124])[0]
    if not imports:
        return [], []

    sections = []
    for i in range(section_count):
        entry = optional + optional_size + i * 40
        # virtual size, virtual address, size of raw data, pointer to raw data
        virtual_size, address, raw_size, raw_offset = struct.unpack(
            "<4I", data[entry + 8:entry + 24])
        sections.append((address, max(virtual_size, raw_size), raw_offset))

    def offset_of(address):
        for start, size, raw in sections:
            if start <= address < start + size:
                return raw + (address - start)
        return None

    def string_at(address):
        start = offset_of(address)
        if start is None:
            return None
        return data[start:data.index(b"\0", start)].decode("ascii", "replace")

    dependencies = []
    descriptor = offset_of(imports)
    while descriptor is not None:
        entry = data[descriptor:descriptor + 20]
        if len(entry) < 20 or entry == b"\0" * 20:
            break
        name = string_at(struct.unpack("<I", entry[12:16])[0])
        if name:
            dependencies.append(name)
        descriptor += 20
    return dependencies, []


def usable_run_paths(run_paths, binary, environment):
    """Drop the run paths that lead outside the installed environment.

    A dependency found only through such a path (typically a dependency
    build tree left on the build machine) is not available to users.
    """
    usable, ignored = [], []
    for run_path in run_paths:
        expanded = (run_path
                    .replace("@loader_path", os.path.dirname(binary))
                    .replace("$ORIGIN", os.path.dirname(binary))
                    .replace("${ORIGIN}", os.path.dirname(binary)))
        if os.path.isabs(expanded) and not expanded.startswith(environment):
            ignored.append(run_path)
        else:
            usable.append(expanded)
    return usable, ignored


def unresolved_dependencies(binary, dependencies, run_paths, environment, search_dirs=()):
    """Return the dependencies that cannot be found from the installed environment."""
    directory = os.path.dirname(binary)
    usable, ignored = usable_run_paths(run_paths, binary, environment)
    unresolved = []
    for dependency in dependencies:
        if dependency.startswith(SYSTEM_PREFIXES):
            continue
        name = dependency.rsplit("/", 1)[-1]
        if name.startswith(SYSTEM_NAMES):
            continue
        if name.lower().startswith(("api-ms-win-", "python3")):
            continue  # Windows API sets and the interpreter's own DLL
        if dependency.startswith("@loader_path/") or dependency.startswith("@executable_path/"):
            candidates = [os.path.join(directory, dependency.split("/", 1)[1])]
        elif dependency.startswith("@rpath/"):
            candidates = [os.path.join(run_path, dependency[len("@rpath/"):])
                          for run_path in usable]
        elif os.path.isabs(dependency):
            candidates = [dependency]
        else:  # ELF and PE: run paths and search directories, then the system ones
            candidates = [os.path.join(run_path, name) for run_path in usable]
            candidates += [os.path.join(search, name) for search in search_dirs]
            candidates += [os.path.join(system, name)
                           for system in SYSTEM_DIRS + WINDOWS_SYSTEM_DIRS]
        if not any(os.path.exists(candidate) for candidate in candidates):
            unresolved.append((dependency, ignored))
    return unresolved


def check_package(package_directory, environment):
    """Return the dependency problems of the native libraries in the package."""
    # The directories the package __init__ registers on Windows; harmless
    # to search on the other platforms, where the same locations are
    # reached through the relative run paths.
    #
    # A wheel repaired by delvewheel or auditwheel keeps the libraries it
    # bundles in a "<distribution>.libs" directory next to the package, and
    # its package __init__ registers that directory (vtkmodules does so for
    # the VTK DLLs in vtk.libs). Search all of them: they are part of the
    # installed environment, unlike the build trees this check looks for.
    site_packages = os.path.dirname(package_directory)
    search_dirs = [package_directory, os.path.join(package_directory, ".dylibs"),
                   os.path.join(site_packages, "vtkmodules")]
    try:
        search_dirs += [os.path.join(site_packages, entry)
                        for entry in os.listdir(site_packages)
                        if entry.endswith(".libs")
                        and os.path.isdir(os.path.join(site_packages, entry))]
    except OSError:
        pass
    problems = []
    checked = 0
    for entry in sorted(os.listdir(package_directory)):
        if not entry.endswith((".so", ".dylib", ".pyd", ".dll")):
            continue
        binary = os.path.join(package_directory, entry)
        with open(binary, "rb") as library:
            data = library.read()
        parsed = [macho_dependencies(image) for image in macho_slices(data)]
        if not parsed:
            single = elf_dependencies(data) or pe_dependencies(data)
            parsed = [single] if single else []
        if not parsed:
            problems.append("%s: not a binary this check understands" % entry)
            continue
        checked += 1
        for dependencies, run_paths in parsed:
            unresolved = unresolved_dependencies(binary, dependencies, run_paths,
                                                 environment, search_dirs)
            for dependency, ignored in unresolved:
                problems.append(
                    "%s: %s not found in the installed environment (run paths "
                    "leading outside it, ignored: %s)" % (entry, dependency, ignored or "none"))
    if not checked:
        problems.append("no native libraries found in %s" % package_directory)
    return checked, problems


def main():
    if len(sys.argv) > 1:
        # Check the package directory of an unpacked wheel instead of the
        # installed package (useful for checking a wheel built elsewhere):
        # the directory it was unpacked into stands in for the environment
        # the package would be installed into.
        package_directory = os.path.realpath(sys.argv[1])
        environment = os.path.dirname(package_directory)
    else:
        import vmtk
        package_directory = os.path.dirname(os.path.realpath(vmtk.__file__))
        environment = os.path.realpath(sys.prefix)
    checked, problems = check_package(package_directory, environment)
    for problem in problems:
        print("ERROR:", problem)
    if problems:
        print("%d problems found: the package in %s is not self-contained"
              % (len(problems), package_directory))
        return 1
    print("%d native libraries in %s resolve their dependencies inside the environment"
          % (checked, package_directory))
    return 0


if __name__ == "__main__":
    sys.exit(main())