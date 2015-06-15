#http://doc.qt.io/qt-5/windows-deployment.html
# TODO Get version with git ?
VERSION=1.1.0

PACKAGENAME=FMIT-$VERSION-Win64bit

echo Packaging $PACKAGENAME
echo " "

rm -fr FMIT-*-Win64bit
mkdir $PACKAGENAME
mkdir $PACKAGENAME/tr

cp ../../build-fmit-Desktop_Qt_5_2_1_MSVC2012_OpenGL_64bit-Release/release/fmit.exe $PACKAGENAME/

cp ../../lib/fftw-3.3.4-dll64/libfftw3-3.dll $PACKAGENAME/
cp ../../lib/freeglut-MSVC-3.0.0-1.mp/freeglut/bin/x64/freeglut.dll $PACKAGENAME/

cp C:/Qt/vcredist/vcredist_sp1_x64.exe $PACKAGENAME/

cd $PACKAGENAME/
C:/Qt/5.2.1/msvc2012_64_opengl/bin/windeployqt fmit.exe
cd ..

cp -fr ../tr/fmit_*.qm $PACKAGENAME/tr/

#"C:/Program Files (x86)/Inno Setup 5/ISCC"

"C:/Program Files (x86)/Inno Setup 5/ISCC.exe" //o. //dMyAppVersion=$VERSION "FMIT_MSVC2012_OpenGL_Win64bit.iss"

