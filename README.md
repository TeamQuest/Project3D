# Project3D Game

### Compilation with CMake
Go to the project directory and run script ./cmake\_generic to generate CMake files.

After successful configuration go to build directory and then compile & run (make and ./bin/project3D)

#### Example for LLVM clang/clang++:

```bash
./cmake_generic.sh $(pwd)/build/ -DURHO3D_HOME=$(pwd)/libs/Urho3D/build -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang -DURHO3D_C++11=1 -Wno-deprecated
cd build
make && ./bin/project3D
```
