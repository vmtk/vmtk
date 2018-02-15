class Vmtk < Formula
  desc "The Vascular Modeling Toolkit"
  homepage "http://www.vmtk.org"
  url "https://github.com/vmtk/vmtk/archive/v1.3.2.tar.gz"
  version "1.3.2"
  sha256 "2632a74341605bd3ddd97971fad70941329e77a112f9363bc5053a1e2ba7c30e"
  head "https://github.com/vmtk/vmtk.git"
  revision 1

  # bottle do
  #   sha256 "" => :el_capitan
  # end

  depends_on "cmake" => :build
  depends_on "vtk"
  depends_on "insightToolkit"
  depends_on :python
  depends_on :python3 => :optional

  def install
    args = std_cmake_args + %W[
      -DBUILD_TESTING=OFF
      -DBUILD_SHARED_LIBS=ON
      -DCMAKE_INSTALL_RPATH:STRING=#{lib}
      -DCMAKE_INSTALL_NAME_DIR:STRING=#{lib}
    ]

    python_executable = `which python`.strip if build.with? "python"
    python_executable = `which python3`.strip if build.with? "python3"

    python_prefix = `#{python_executable} -c 'import sys;print(sys.prefix)'`.chomp
    python_include = `#{python_executable} -c 'from distutils import sysconfig;print(sysconfig.get_python_inc(True))'`.chomp
    python_version = "python" + `#{python_executable} -c 'import sys;print(sys.version[:3])'`.chomp
    py_site_packages = "#{lib}/#{python_version}/site-packages"

    args << ".."
    args << "-DUSE_SYSTEM_ITK=ON"
    args << "-DITK_DIR=#{HOMEBREW_CELLAR}/insighttoolkit/4.9.0"

    args << "-DUSE_SYSTEM_VTK=ON"
    args << "-DVTK_DIR=#{HOMEBREW_CELLAR}/vtk/7.1.0"

    args << "-DVMTK_USE_SUPERBUILD=OFF"
    args << "-DUSE_VTK6_SUPERBUILD=OFF"

    args << "-DVMTK_USE_RENDERING=ON"

    args << "-DVTK_WRAP_PYTHON=ON"
    args << "-DPYTHON_EXECUTABLE='#{python_executable}'"
    args << "-DPYTHON_INCLUDE_DIR='#{python_include}'"
    # CMake picks up the system's python dylib, even if we have a brewed one.
    if File.exist? "#{python_prefix}/Python"
      args << "-DPYTHON_LIBRARY='#{python_prefix}/Python'"
    elsif File.exist? "#{python_prefix}/lib/lib#{python_version}.a"
      args << "-DPYTHON_LIBRARY='#{python_prefix}/lib/lib#{python_version}.a'"
    elsif File.exist? "#{python_prefix}/lib/lib#{python_version}.#{dylib}"
      args << "-DPYTHON_LIBRARY='#{python_prefix}/lib/lib#{python_version}.#{dylib}'"
    elsif File.exist? "#{python_prefix}/lib/x86_64-linux-gnu/lib#{python_version}.#{dylib}"
      args << "-DPYTHON_LIBRARY='#{python_prefix}/lib/x86_64-linux-gnu/lib#{python_version}.so'"
    else
      odie "No libpythonX.Y.{dylib|so|a} file found!"
    end
    # Set the prefix for the python bindings to the Cellar
    args << "-DVTK_INSTALL_PYTHON_MODULE_DIR='#{py_site_packages}/'"

    args << ".."

    mkdir "build" do
      system "cmake", *args
      system "make"
      system "make", "install"
    end
  end

  test do
    # `test do` will create, run in and delete a temporary directory.
    #
    # This test will fail and we won't accept that! It's enough to just replace
    # "false" with the main program this formula installs, but it'd be nice if you
    # were more thorough. Run the test with `brew test vmtk`. Options passed
    # to `brew install` such as `--HEAD` also need to be provided to `brew test`.
    #
    # The installed folder is not in the path, so use the entire path to any
    # executables being tested: `system "#{bin}/program", "do", "something"`.
    system "false"
  end
end
