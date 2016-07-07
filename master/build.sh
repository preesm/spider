
rm libspider_0.6.orig.tar.gz
tar cf libspider_0.6.orig.tar libspider
gzip libspider_0.6.orig.tar
mkdir libspider-0.6
cp libspider/* libspider-0.6 -R
cd libspider-0.6/
debuild -S -sa --lintian-opts -i
cd ..
sudo pbuilder build libspider_0.6-1.dsc
