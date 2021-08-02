appveyor PushArtifact pytest_report.html
mv /home/appveyor/miniconda/conda-bld/*/vmtk*.tar.bz2 .
appveyor PushArtifact vmtk*.tar.bz2
exit 0