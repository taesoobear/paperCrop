sudo yum install subversion
echo 'svn checking out....(wait)'
mkdir -p ~/bin
mkdir -p ~/bin/papercrop
cd ~/bin/papercrop
svn checkout http://papercrop.googlecode.com/svn/trunk/ paperCrop_source
cd paperCrop_source
echo 'svn update .'
svn update .
sudo yum install DevIL-devel fltk-devel fltk-static fontconfig-devel freeimage-devel lua-devel libpng-devel libpng10-devel boost-devel libxml2-devel ogre-devel gsl-devel luabind-devel fltk-fluid fltk lcms-devel lcms-libs poppler-devel
echo 'start building paperCrop'
sh clean_build_fedora.sh
cd ..
echo "cd paperCrop_source" > paperCropLauncher
echo "./paperCrop" >> paperCropLauncher
chmod 700 paperCropLauncher
./paperCropLauncher
