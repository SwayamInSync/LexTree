rm -rf build
mkdir build
cd build

CC=clang CXX=clang++ cmake ..
make