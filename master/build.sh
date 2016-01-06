
rm libspider_0.5.orig.tar.gz
tar cf libspider_0.5.orig.tar libspider
gzip libspider_0.5.orig.tar
mkdir libspider-0.5
cp libspider/* libspider-0.5 -R
cd libspider-0.5/
debuild -S -sa --lintian-opts -i
cd ..
sudo pbuilder build libspider_0.5-1.dsc
