BINFILE=$1

VERSION=`git describe --tags --always |cut -c2-`-github
DISTRIB=`lsb_release -d |sed 's/Description:\s//g' |sed 's/\s/_/g'`
echo "Distribution: "$DISTRIB
if [ "${DISTRIB/Ubuntu}" != "$DISTRIB" ] ; then
    DISTRIB=`echo $DISTRIB |sed 's/\(Ubuntu_[0-9]\+\.[0-9]\+\).*/\1/g'`
fi

echo "Packaging FMIT "$VERSION
echo "Distribution: "$DISTRIB
BINARCH=`objdump -p $BINFILE |grep 'file format' |sed 's/^.*file format\s//g'`
if [ "${BINARCH/elf64}" = "${BINARCH}" ] ; then
    ARCH=i386
else
    ARCH=amd64
fi
echo "Architecture: "$ARCH

PKGNAME=fmit_$VERSION\_$ARCH
echo "Package name "$PKGNAME

rm -fr $PKGNAME
mkdir -p $PKGNAME

echo "List dependencies:"
DEPS=`objdump -p $BINFILE |grep NEEDED |awk '{ print $2 }'`
for dep in $DEPS; do
    dpkg -S $dep |grep -v chefdk
    deplist=`dpkg -S $dep |grep -v chefdk |sed 's/:.*$//g'`
    depdpkg=`echo "$deplist" |sort |uniq`
    depcurver=`dpkg -s $depdpkg |grep 'Version' |awk '{ print $2 }' |sed 's/:.*$//g' |sed 's/-.*$//g' |sed 's/+.*$//g'`
    echo "Dependency "$dep"    in package:"$depdpkg"    version:"$depcurver
    echo $depdpkg >> Depends_$PKGNAME
done
DEPENDS=`cat Depends_$PKGNAME |sort |uniq`
rm -f Depends_$PKGNAME
DEPENDS=`echo $DEPENDS |sed 's/ /,\ /g'`
echo "Automatic detection of Depends: "$DEPENDS
# This list if too dependent on external repo used for Travis CI ...
# ... so let's overwrite it with a predefined list of packages
# (http://packages.ubuntu.com/precise/allpackages can help to build it)
DEPENDS=`cat package_deb.Depends_$DISTRIB`
# If cannot list the dependencies properly, skip them ...
# DEPENDS=""
echo "Replaced by predefined list Depends: "$DEPENDS


# Build the file tree

# Copy files
mkdir -p $PKGNAME/DEBIAN
cp package_deb.control $PKGNAME/DEBIAN/control
sed -i "s/^Version:.*$/Version: $VERSION/g" $PKGNAME/DEBIAN/control
sed -i "s/^Architecture:.*$/Architecture: $ARCH/g" $PKGNAME/DEBIAN/control
sed -i "s/^Depends:.*$/Depends: $DEPENDS/g" $PKGNAME/DEBIAN/control

# The binary
mkdir -p $PKGNAME/usr/bin
cp $BINFILE $PKGNAME/usr/bin/

# Any legal txt
mkdir -p $PKGNAME/usr/share/doc/fmit
cp ../COPYING_*.txt $PKGNAME/usr/share/doc/fmit/

# The menu related files
mkdir -p $PKGNAME/usr/share/appdata
cp fmit.appdata.xml $PKGNAME/usr/share/appdata/
mkdir -p $PKGNAME/usr/share/applications
cp fmit.desktop $PKGNAME/usr/share/applications/
mkdir -p $PKGNAME/usr/share/menu
cp fmit.menu $PKGNAME/usr/share/menu/fmit

# The icon
mkdir -p $PKGNAME/usr/share/icons/hicolor/scalable/apps
cp ../ui/images/fmit.svg $PKGNAME/usr/share/icons/hicolor/scalable/apps/
mkdir -p $PKGNAME/usr/share/icons/hicolor/128x128/apps
cp ../ui/images/fmit.png $PKGNAME/usr/share/icons/hicolor/128x128/apps/

# Translations *.qm in share/fmit/tr/
mkdir -p $PKGNAME/usr/share/fmit/tr
cp ../tr/*.qm $PKGNAME/usr/share/fmit/tr/

# Scales
mkdir -p $PKGNAME/usr/share/fmit/scales
cp ../scales/* $PKGNAME/usr/share/fmit/scales/

dpkg-deb --build $PKGNAME

ls $PKGNAME.deb
