sudo apt-get remove libspider
mkdir m4
aclocal
autoheader 
automake
automake --add-missing
autoreconf 
libtoolize 
./configure 
make
sudo make install
sudo make uninstall
