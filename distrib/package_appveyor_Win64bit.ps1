#http://doc.qt.io/qt-5/windows-deployment.html
$VERSION = (git describe --tags --always) | Out-String
$VERSION = $VERSION -replace "`n|`r"
echo "Version: $VERSION"

$PACKAGENAME = "FMIT-$VERSION-Win64bit"
$QTPATH = "\Qt\5.4\msvc2013_64_opengl"

echo "Packaging $PACKAGENAME"
echo " "

cd distrib

New-Item -ItemType directory -Name $PACKAGENAME | Out-Null

# Add the executable
Copy-Item c:\projects\fmit\release\fmit.exe $PACKAGENAME

# Add libraries
Copy-Item c:\projects\fmit\lib\libfft\libfftw3-3.dll $PACKAGENAME

# Add the Qt related libs
cd $PACKAGENAME
#C:/Qt/5.2.1/msvc2012_64_opengl/bin/qtenv2.bat
#export PATH=\C$QTPATH\bin:$PATH
#echo $PATH
$env:Path += ";C:\$QTPATH\bin"
& c:$QTPATH\bin\windeployqt.exe fmit.exe
#--no-translations

# Add the translations
& c:$QTPATH/bin/lrelease.exe ../../fmit.pro
Copy-Item c:\projects\fmit\tr\fmit_*.qm .

Get-ChildItem .
cd ..

#"C:/Program Files (x86)/Inno Setup 5/ISCC"

& "c:\Program Files (x86)\Inno Setup 5\ISCC.exe" /o. /dMyAppVersion=$VERSION c:\projects\fmit\distrib\FMIT_MSVC2012_Win64bit.iss

# Get out of distrib
cd ..
