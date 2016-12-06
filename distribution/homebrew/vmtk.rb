class Vmtk < Formula
  desc "The Vascular Modeling Toolkit"
  homepage "http://www.vmtk.org"
  url "https://github.com/vmtk/vmtk/archive/v1.3.1.tar.gz"
  version "1.3.1"
  sha256 "c3220f3442036e4c2824700688044e472d173573fc9d5c65cfdbbe628d6ecc05"
  head "https://github.com/vmtk/vmtk.git"
  revision 1

  # bottle do
  #   sha256 "" => :el_capitan
  # end

  depends_on "cmake" => :build
  depends_on "vtk"
  depends_on "insightToolkit"
  depends_on :python
  
  def install
    args = std_cmake_args + %W[
      -DBUILD_TESTING=OFF
      -DBUILD_SHARED_LIBS=ON
      -DCMAKE_INSTALL_RPATH:STRING=#{lib}
      -DCMAKE_INSTALL_NAME_DIR:STRING=#{lib}
    ]

    args << ".."
    args << "-DUSE_SYSTEM_ITK=ON"
    args << "-DITK_DIR=#{HOMEBREW_CELLAR}/insighttoolkit/4.9.0"

    args << "-DUSE_SYSTEM_VTK=ON"
    args << "-DVTK_DIR=#{HOMEBREW_CELLAR}/vtk/7.1.0"

    args << "-DVMTK_USE_SUPERBUILD=OFF"
    args << "-DUSE_VTK6_SUPERBUILD=OFF"

    args << "-DVMTK_USE_RENDERING=ON"

    args << "-DVTK_WRAP_PYTHON=ON"

    args << "-DPYTHON_EXECUTABLE=#{`which python`}"
    args << "-DPYTHON_LIBRARY='#{`python-config --prefix`.chomp}/lib/libpython2.7.dylib'"
    args << "-DPYTHON_INCLUDE_DIR='#{`python-config --prefix`.chomp}/include/python2.7'"

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
