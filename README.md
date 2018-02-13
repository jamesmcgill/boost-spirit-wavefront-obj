# boost-spirit-wavefront-obj

CMake project that uses Boost Spirit to parse Wavefront Obj model files  
Includes GTest powered unit tests  

## Building:
+ Install Boost on your system  
e.g. `vcpkg install boost`  
or  
`sudo apt-get install libboost-all-dev`  
+ On Windows run the provided `build.bat` file   
   Note: the file `build-generate.bat` currently contains a hardcoded path: `set VCPKG_PATH="S:\tools\vcpkg\\"`  
   Either change the path to point to your vcpkg installation or remove usages of the path if not using vcpkg.  
   
+ Or type the following commands:  
   `mkdir build && cd build`  
   
   `cmake ..`  
     or to use your specific C++17 compiler:  
   `cmake .. -DCMAKE_CXX_COMPILER=g++-7.1`  
   
   then finally:  
   `cmake --build .`  

## Requirements:
+ A compiler with some C++ 17 support (at least <string_view>)  
    Tested on VS 2017 15.5 and GCC 7.1 

+ CMake 3.10 or above

+ Boost  
    Tested GCC with v1.58, MSVC with v1.66 
