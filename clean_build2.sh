cd libharu-2.0.8
chmod 700 ./configure
./configure
make
cd ..
cd build_linux
rm CMakeCache.txt
cmake ..
make clean
make
cp paperCrop ..
cp ../paperCrop.py ../../../paperCrop
chmod 700 ../../../paperCrop
mkdir -p ~/.referencer
mkdir -p ~/.referencer/plugins
cp ../referencer_plugins/papercrop* ~/.referencer/plugins/
