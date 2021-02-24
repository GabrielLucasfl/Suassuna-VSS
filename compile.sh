# Removing build folder
rm -r build

# Removing all protobuf files
cd include/ 
find . -name "*.pb.h" -type f -delete
find . -name "*.pb.cc" -type f -delete

# Creating new build folder and compiling
cd ..
mkdir build
cd build
qmake ..
make
