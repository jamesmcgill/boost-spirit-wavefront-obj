:: Generates CMake Cache
:: Deletes any previous cache and build files

@echo off
set BASE_PATH=%~dp0
set BUILD_PATH=%BASE_PATH%build\
set VCPKG_PATH="S:\tools\vcpkg\\"

IF NOT EXIST %BUILD_PATH% GOTO MissingBuildDir
echo "Deleting Build Path..."
rmdir /S /Q %BUILD_PATH%
:MissingBuildDir
IF NOT EXIST %BUILD_PATH% mkdir %BUILD_PATH%

pushd %BUILD_PATH%
cmake -G "Visual Studio 15 2017" .. "-DCMAKE_TOOLCHAIN_FILE=%VCPKG_PATH%scripts\buildsystems\vcpkg.cmake"
popd

echo[
echo CMake Cache Generation Done!
echo ----------------------------

