echo Running cmake...
mkdir bin
cd bin
cmake -G "Visual Studio 15 2017" -DBUILD_FORECASTER=ON -DBUILD_OPTIMIZER=ON -DBUILD_VIEWER=ON -DUSE_GUI=ON -DCMAKE_PREFIX_PATH="C:\projects\libs" -DwxWidgets_CONFIGURATION=mswu ..
cmake --build . --config release