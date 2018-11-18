#!/usr/bin/env sh

brew install proj

# Build libraries
chmod +x ci/travis/build-wxwidgets-osx.sh
ci/travis/build-wxwidgets-osx.sh
chmod +x ci/travis/build-gdal-osx.sh
ci/travis/build-gdal-osx.sh

brew install jasper
brew install netcdf

# Changing permissions of Homebrew libraries
sudo chmod -R 777 /usr/local/Cellar

ls $HOME/.libs/include