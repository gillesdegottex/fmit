$arch=$args[0]

#http://doc.qt.io/qt-5/windows-deployment.html
$VERSION = ((git describe --tags --always) | Out-String).Substring(1)
$VERSION = $VERSION -replace "`n|`r"
echo "Version: $VERSION"

If ($arch -eq 'x64') {
    echo "Packaging for 64bits Windows"
    $PACKAGENAME = "fmit_$VERSION`_win64bit"
    $QTPATH = "\Qt\5.4\msvc2013_64_opengl"
}
Else {
    echo "Packaging for 32bits Windows"
    $PACKAGENAME = "fmit_$VERSION`_win32bit"
    $QTPATH = "\Qt\5.4\msvc2013_opengl"
}

echo "Packaging $PACKAGENAME"
echo " "

cd distrib

# Create a directory for the files to package
New-Item -ItemType directory -Name $PACKAGENAME | Out-Null

# Add the executable
Copy-Item c:\projects\fmit\release\fmit.exe $PACKAGENAME

# Add external libraries
Copy-Item c:\projects\fmit\lib\libfft\libfftw3-3.dll $PACKAGENAME

# Add the Qt related libs, qt translations and installer of MSVC redist.
cd $PACKAGENAME
#C:/Qt/5.4/msvc2013_opengl/bin/qtenv2.bat
#export PATH=\C$QTPATH\bin:$PATH
#echo $PATH
$env:Path += ";C:\$QTPATH\bin"
& c:$QTPATH\bin\windeployqt.exe fmit.exe

# Add the translations
& c:$QTPATH/bin/lrelease.exe ../../fmit.pro
Copy-Item c:\projects\fmit\tr\fmit_*.qm .

# Add the scala files
New-Item -ItemType directory -Name scales | Out-Null
Copy-Item c:\projects\fmit\scales\*.scl scales

Get-ChildItem .
cd ..

# Run Inno setup to create the installer
If ($arch -eq 'x64') { $INNOSCRIPT = "FMIT_MSVC2012_Win64bit.iss" }
Else { $INNOSCRIPT = "FMIT_MSVC2012_Win32bit.iss" }
& "c:\Program Files (x86)\Inno Setup 5\ISCC.exe" /o. /dMyAppVersion=$VERSION c:\projects\fmit\distrib\$INNOSCRIPT

# Get out of distrib
cd ..
