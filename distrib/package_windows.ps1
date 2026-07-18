$arch = $args[0]

$VERSION = ((git describe --tags --always) | Out-String).Trim()
if ($VERSION -match '^v') {
    $VERSION = $VERSION.Substring(1)
}
$VERSION = $VERSION -replace "`n|`r"
Write-Host "Version: ${VERSION}"

$projectRoot = $PWD
if ($arch -eq 'x64') {
    Write-Host "Packaging for 64-bit Windows"
    $PACKAGENAME = "fmit_${VERSION}_win64"
    $INNOSCRIPT = "FMIT_MSVC2012_Win64bit.iss"
    $BIN_DIR = "release"
} else {
    Write-Host "Packaging for 32-bit Windows"
    $PACKAGENAME = "fmit_${VERSION}_win32"
    $INNOSCRIPT = "FMIT_MSVC2012_Win32bit.iss"
    $BIN_DIR = "debug"
}

Write-Host "Packaging ${PACKAGENAME}"

Set-Location distrib

New-Item -ItemType Directory -Name ${PACKAGENAME} -Force | Out-Null

Copy-Item "$projectRoot\$BIN_DIR\fmit.exe" ${PACKAGENAME}

if (Test-Path "C:\vcpkg\installed\x64-windows\bin\libfftw3-3.dll") {
    Copy-Item "C:\vcpkg\installed\x64-windows\bin\libfftw3-3.dll" ${PACKAGENAME}
}

Set-Location ${PACKAGENAME}

$qtBin = "$env:QTDIR\bin"
$env:Path += ";$qtBin"

& "$qtBin\windeployqt.exe" fmit.exe

& "$qtBin\lrelease.exe" "$projectRoot\fmit.pro"
Copy-Item "$projectRoot\tr\fmit_*.qm" .

New-Item -ItemType Directory -Name scales -Force | Out-Null
Copy-Item "$projectRoot\scales\*.scl" scales

Get-ChildItem .
Set-Location ..

$isccPath = "C:\Program Files (x86)\Inno Setup 6\ISCC.exe"
if (-not (Test-Path $isccPath)) {
    $isccPath = "C:\Program Files (x86)\Inno Setup 5\ISCC.exe"
}

& $isccPath /o. /dMyAppVersion=${VERSION} "$projectRoot\distrib\${INNOSCRIPT}"

Set-Location ..
