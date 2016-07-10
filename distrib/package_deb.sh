BINFILE=$1

VERSION=`git describe --tags --always |cut -c2-`-1github1
ARCH=amd64

PKGNAME=fmit-$VERSION

echo "Packaging FMIT "$VERSION" in "$PKGNAME
echo "List dependencies:"
objdump -p $BINFILE |grep 'file format'
# TODO
# DEPS=`objdump -p $BINFILE |grep NEEDED |awk '{ print $2 }'`
# for dep in $DEPS; do
#     deplist=`dpkg -S $dep |sed 's/:.*$//g'`
#     depdpkg=`echo "$deplist" |uniq`
#     depcurver=`dpkg -s $depdpkg |grep 'Version' |awk '{ print $2 }' |sed 's/:.*$//g' |sed 's/-.*$//g' |sed 's/+.*$//g'`
#     echo "Dependency "$dep":"$depdpkg":"$depcurver
# done

# Start to build the files tree

rm -fr package_deb_$PKGNAME
mkdir -p package_deb_$PKGNAME

# Copy files
mkdir -p package_deb_$PKGNAME/$PKGNAME/DEBIAN
cp package_deb.control package_deb_$PKGNAME/$PKGNAME/DEBIAN/control
sed -i "s/^Version:.*$/Version: $VERSION/g" package_deb_$PKGNAME/$PKGNAME/DEBIAN/control
sed -i "s/^Architecture:.*$/Architecture: $ARCH/g" package_deb_$PKGNAME/$PKGNAME/DEBIAN/control

# The binary
mkdir -p package_deb_$PKGNAME/$PKGNAME/usr/bin
cp $BINFILE package_deb_$PKGNAME/$PKGNAME/usr/bin/

# Any legal txt
mkdir -p package_deb_$PKGNAME/$PKGNAME/usr/share/doc/fmit
cp ../COPYING_*.txt package_deb_$PKGNAME/$PKGNAME/usr/share/doc/fmit/

# The menu related files
mkdir -p package_deb_$PKGNAME/$PKGNAME/usr/share/appdata
cp fmit.appdata.xml package_deb_$PKGNAME/$PKGNAME/usr/share/appdata/
mkdir -p package_deb_$PKGNAME/$PKGNAME/usr/share/applications
cp fmit.desktop package_deb_$PKGNAME/$PKGNAME/usr/share/applications/
mkdir -p package_deb_$PKGNAME/$PKGNAME/usr/share/menu
cp fmit.menu package_deb_$PKGNAME/$PKGNAME/usr/share/menu/fmit

# The icon
mkdir -p package_deb_$PKGNAME/$PKGNAME/usr/share/icons/hicolor/scalable/apps
cp ../ui/images/fmit.svg package_deb_$PKGNAME/$PKGNAME/usr/share/icons/hicolor/scalable/apps/
mkdir -p package_deb_$PKGNAME/$PKGNAME/usr/share/icons/hicolor/128x128/apps
cp ../ui/images/fmit.png package_deb_$PKGNAME/$PKGNAME/usr/share/icons/hicolor/128x128/apps/

# Translations *.qm in share/fmit/tr/
mkdir -p package_deb_$PKGNAME/$PKGNAME/usr/share/fmit/tr
cp ../tr/*.qm package_deb_$PKGNAME/$PKGNAME/usr/share/fmit/tr/

# Scales
mkdir -p package_deb_$PKGNAME/$PKGNAME/usr/share/fmit/scales
cp ../scales/* package_deb_$PKGNAME/$PKGNAME/usr/share/fmit/scales/

dpkg-deb --build package_deb_$PKGNAME/$PKGNAME
