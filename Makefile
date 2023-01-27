all: release

# to compile and install paperCrop to ~/bin/
# make install_dependencies_ubuntu11.10; make; make console; make install
.PHONY: release debug clean console
release:
	mkdir -p build_linux;cd build_linux;cmake ..;make;cp paperCrop ..

debug:
	mkdir -p build_linux_debug;cd build_linux_debug;cmake -D "CMAKE_BUILD_TYPE=Debug" ..; make; cp paperCrop ..

console: 
	mkdir -p build_linux_console;cd build_linux_console;cmake -D "CONSOLE=True" ..;make;cp paperCrop ../paperCrop_console

clean:
	rm -rf build_linux
	rm -rf build_linux_debug
	rm -rf build_linux_console

lldb:
	make debug
	lldb ./build_linux_debug/paperCrop
cgdb:
	make debug
	cgdb ./build_linux_debug/paperCrop

install_dependencies_mac:
	brew install devil fltk eigen lua@5.1 poppler

install_dependencies_ubuntu:
	sudo apt-get install libpoppler-dev libpoppler-private-dev libdevil-dev libfontconfig-dev libfltk1.1-dev libdevil-dev libfreeimage-dev liblua5.1-dev libpng-dev lua5.1 cmake g++ libxml2-dev libgsl0-dev openjdk-8-jre

install_dependencies_ubuntu11.10:
	sudo apt-get install libpoppler-dev libdevil-dev libfontconfig-dev libfltk1.1-dev libdevil-dev libfreeimage-dev liblua5.1-dev libpng12-dev lua5.1 cmake g++ libxml2-dev libgsl0-dev openjdk-7-jre

install_dependencies_ubuntu12.04:
	make install_dependencies_ubuntu11.10

install_dependencies_ubuntu12.10:
	make install_dependencies_ubuntu11.10

install_dependencies_fedora:
	sudo yum install DevIL-devel fltk-devel fltk-static fontconfig-devel freeimage-devel lua-devel libpng-devel libpng10-devel boost-devel libxml2-devel ogre-devel gsl-devel luabind-devel fltk-fluid fltk lcms-devel lcms-libs poppler-devel
	sudo yum install libjpeg-turbo-devel

install: 
	if test -d ~/bin/papercrop_bin; then echo "Test passed2"; else mkdir -p ~/bin/papercrop_bin; fi
	cp -rf . ~/bin/papercrop_bin/
	cp paperCrop ~/bin/papercrop_bin/paperCrop
	cp paperCrop.py ~/bin/paperCrop
	cp paperCrop_auto.py ~/bin/paperCropAuto
	chmod 700 ~/bin/paperCrop
	chmod 700 ~/bin/paperCropAuto
	if test -f paperCrop_console; then cp paperCrop_console ~/bin/papercrop_bin/paperCrop_console; fi
