rm zarc
echo "Cleaned UP"
cd build
cmake ..
make
mv zarc ../
echo "Build Done"
