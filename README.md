# boost-spirit-wavefront-obj

[![Build Status](https://travis-ci.org/jamesmcgill/boost-spirit-wavefront-obj.svg?branch=master)](https://travis-ci.org/jamesmcgill/boost-spirit-wavefront-obj)  [![Build status](https://ci.appveyor.com/api/projects/status/5nnfjq0yk9i42hav?svg=true)](https://ci.appveyor.com/project/jamesmcgill/boost-spirit-wavefront-obj)  [![Coverage Status](https://coveralls.io/repos/github/jamesmcgill/boost-spirit-wavefront-obj/badge.svg?branch=master)](https://coveralls.io/github/jamesmcgill/boost-spirit-wavefront-obj?branch=master)  

CMake project that uses Boost Spirit to parse Wavefront Obj model files  
Includes GTest powered unit tests  

## Building:
+ Install Boost on your system  
e.g. `vcpkg install boost`  
or  
`sudo apt-get install libboost-all-dev`  

+ On Windows run the provided `build.bat` file   
   Note: if you are using vcpkg, set the environment variable VCPKG_PATH to the root location of your vcpkg installation (see build-generate.bat).    
   
+ On gnu tools run `build.sh`  
   
+ Or type the following commands:  
   `mkdir build && cd build`  
   
   `cmake ..`  
     or to use your specific C++17 compiler:  
   `cmake .. -DCMAKE_CXX_COMPILER=g++-7.1`  
   
   then finally:  
   `cmake --build .`  

## Requirements:
+ A compiler with some C++ 17 support (at least <string_view>, <optional>)  
    (Tested on VS 2017 15.5 and GCC 7.1) 

+ CMake 3.9.2 or higher

+ Boost  
    (v1.55 tested on GCC. v1.66 tested on MSVC) 
