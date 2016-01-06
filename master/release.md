# Release guide

* Update the package sources in libspider.
* Update Makefile.am if needed
* Update configure script with "autoreconf -vfi"
* Test configure script with "./configure" & "make"
* Clean repository with "make maintainer-clean"
* Update the changelog file in libspider/debian folder:
  * Update the release number (major).(minor)-(build)
  * Give a release description
  * Sign with your name and date
* update the build.sh script and upload.sh script with correct name.
* execute "sh build.sh"
* check installed files with "dpkg -c /var/cache/pbuilder/result/libspider-dev_\*.deb"
* check \*.deb quality with "lintian -Iv libspider_0.\*-\*.dsc"
* execute "sh upload.sh" if no error in build
