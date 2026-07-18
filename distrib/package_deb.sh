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

echo "Detecting dependencies..."
mkdir -p $PKGNAME/DEBIAN
dpkg-shlibdeps -d$PKGNAME/usr/bin $PKGNAME/usr/bin/fmit > $PKGNAME/DEBIAN/shlibs.deps 2>/dev/null
DEPENDS=`dpkg-shlibdeps $PKGNAME/usr/bin/fmit 2>/dev/null | grep -oP 'Depends: \K.*' | sed 's/,/ /g' | tr ' ' '\n' | sort -u | tr '\n' ',' | sed 's/,$//' | sed 's/,/, /g'`
if [ -z "$DEPENDS" ]; then
    echo "Warning: dpkg-shlibdeps failed, using minimal dependencies"
    DEPENDS="libc6, libfftw3-bin"
fi
echo "Detected dependencies: $DEPENDS"


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

# Any legal and info txt
mkdir -p $PKGNAME/usr/share/doc/fmit
cp ../README.txt $PKGNAME/usr/share/doc/fmit/
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
