appveyor PushArtifact pytest_report.html
# conda-build produces .conda packages by default nowadays; keep matching the
# legacy .tar.bz2 format too in case it is ever re-enabled.
mv /home/appveyor/miniconda/conda-bld/*/vmtk*.conda . 2>/dev/null
mv /home/appveyor/miniconda/conda-bld/*/vmtk*.tar.bz2 . 2>/dev/null
for package in vmtk*.conda vmtk*.tar.bz2; do
    if [ -f "$package" ]; then
        appveyor PushArtifact "$package"
    fi
done
exit 0