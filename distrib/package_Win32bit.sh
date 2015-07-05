#http://doc.qt.io/qt-5/windows-deployment.html
VERSION=$(git describe --tags --always)
echo Version: $VERSION

PACKAGENAME=FMIT-$VERSION-Win32bit
QTPATH=/Qt/5.2.1/msvc2012

echo Packaging $PACKAGENAME
echo " "

rm -fr FMIT-*-Win32bit
mkdir $PACKAGENAME
mkdir $PACKAGENAME/tr

# Add the executable
cp ../../build-fmit-Desktop_Qt_5_2_1_MSVC2012_32bit-Release/release/fmit.exe $PACKAGENAME/

# Add libraries
cp ../../lib/fftw-3.3.4-dll32/libfftw3-3.dll $PACKAGENAME/
cp ../../lib/freeglut-MSVC-3.0.0-1.mp/freeglut/bin/freeglut.dll $PACKAGENAME/

# Add the Qt related libs
cd $PACKAGENAME/
#C:/Qt/5.2.1/msvc2012_64_opengl/bin/qtenv2.bat
export PATH=/C$QTPATH/bin:$PATH
echo $PATH
C:$QTPATH/bin/windeployqt.exe --no-translations fmit.exe
cd ..

# Add the translations
cp -r ../tr/fmit_*.ts $PACKAGENAME/tr/
C:$QTPATH/bin/lrelease.exe $PACKAGENAME/tr/*
rm -f $PACKAGENAME/tr/*.ts

# Add the MSVC redistribution installer
cp C:/Qt/vcredist/vcredist_x86.exe $PACKAGENAME/


#"C:/Program Files (x86)/Inno Setup 5/ISCC"

"C:/Program Files (x86)/Inno Setup 5/ISCC.exe" //o. //dMyAppVersion=$VERSION "FMIT_MSVC2012_OpenGL_Win32bit.iss"
