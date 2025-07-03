# This is a temporary script to just automate the CMake building
rm zarc
echo "Cleaned UP"
cd build
cmake ..
make
mv zarc ../
echo "Build Done"