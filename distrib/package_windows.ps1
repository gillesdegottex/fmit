$ErrorActionPreference = 'Stop'
$arch = $args[0]

Write-Host "========================================"
Write-Host "FMIT Windows Packaging Script"
Write-Host "========================================"

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

# Use QT_ROOT_DIR (GitHub Actions) or QTDIR (local)
if (-not $env:QTDIR -and $env:QT_ROOT_DIR) {
    $env:QTDIR = $env:QT_ROOT_DIR
}

Write-Host "Packaging ${PACKAGENAME}"
Write-Host "Project root: ${projectRoot}"
Write-Host "QTDIR: ${env:QTDIR}"

if (-not $env:QTDIR) {
    Write-Host "ERROR: QTDIR not set!"
    exit 1
}

# Verify fmit.exe exists before copying
$fmitExe = "$projectRoot\$BIN_DIR\fmit.exe"
if (-not (Test-Path $fmitExe)) {
    Write-Host "ERROR: fmit.exe not found at ${fmitExe}"
    Write-Host "Contents of ${projectRoot}:"
    Get-ChildItem $projectRoot -Recurse -Filter "*.exe" | ForEach-Object { Write-Host "  Found: $($_.FullName)" }
    exit 1
}
Write-Host "Found fmit.exe: $((Get-Item $fmitExe).Length) bytes"

Set-Location distrib

New-Item -ItemType Directory -Name ${PACKAGENAME} -Force | Out-Null
Write-Host "Created package directory: ${PACKAGENAME}"

Copy-Item "$projectRoot\$BIN_DIR\fmit.exe" ${PACKAGENAME}
Write-Host "Copied fmit.exe to package directory"

# Copy FFTW3 DLL
Write-Host "Checking for FFTW3 DLL..."
$fftwDllPath = "C:\vcpkg\installed\x64-windows\bin\fftw3.dll"
if (Test-Path $fftwDllPath) {
    Copy-Item $fftwDllPath ${PACKAGENAME}
    Write-Host "Copied fftw3.dll: $((Get-Item $fftwDllPath).Length) bytes"
} else {
    Write-Host "WARNING: fftw3.dll not found at ${fftwDllPath}"
    Write-Host "Contents of C:\vcpkg\installed\x64-windows\bin:\"
    Get-ChildItem "C:\vcpkg\installed\x64-windows\bin\" -Filter "*fftw*" | ForEach-Object { Write-Host "  $($_.Name)" }
}

# Download VC++ 2022 Redistributable
Write-Host "Downloading VC++ 2022 Redistributable..."
$vcredistUrl = "https://aka.ms/vs/17/release/vc_redist.x64.exe"
$vcredistDest = "${PACKAGENAME}\vcredist_x64.exe"
try {
    Invoke-WebRequest -Uri $vcredistUrl -OutFile $vcredistDest -UseBasicParsing
    Write-Host "Downloaded vcredist_x64.exe: $((Get-Item $vcredistDest).Length) bytes"
} catch {
    Write-Host "WARNING: Failed to download vcredist_x64.exe: $_"
}

Set-Location ${PACKAGENAME}

$qtBin = "$env:QTDIR\bin"
$env:Path += ";$qtBin"

Write-Host "Running windeployqt..."
Write-Host "  windeployqt: $qtBin\windeployqt.exe"
Write-Host "  fmit.exe: $(Get-Location)\fmit.exe"
Write-Host "  QTDIR: $env:QTDIR"

& "$qtBin\windeployqt.exe" fmit.exe
if ($LASTEXITCODE -ne 0) {
    Write-Host "ERROR: windeployqt failed with exit code $LASTEXITCODE"
    exit 1
}
Write-Host "windeployqt completed successfully"

Write-Host "Package directory contents after windeployqt:"
Get-ChildItem . -Recurse | ForEach-Object { Write-Host "  $($_.FullName)" }

Write-Host "Running lrelease..."
& "$qtBin\lrelease.exe" "$projectRoot\fmit.pro"
Copy-Item "$projectRoot\tr\fmit_*.qm" .
Write-Host "Copied translation files"

New-Item -ItemType Directory -Name scales -Force | Out-Null
Copy-Item "$projectRoot\scales\*.scl" scales
Write-Host "Copied scale files"

Write-Host "Package directory contents:"
Get-ChildItem . | ForEach-Object { Write-Host "  $($_.Name) ($($_.Length) bytes)" }

Set-Location ..

Write-Host "Compiling Inno Setup installer..."
$isccPath = "C:\Program Files (x86)\Inno Setup 6\ISCC.exe"
if (-not (Test-Path $isccPath)) {
    $isccPath = "C:\Program Files (x86)\Inno Setup 5\ISCC.exe"
}
Write-Host "Using Inno Setup: ${isccPath}"

& $isccPath /o. /dMyAppVersion=${VERSION} "$projectRoot\distrib\${INNOSCRIPT}"

Set-Location ..

Write-Host "========================================"
Write-Host "Packaging complete!"
Write-Host "========================================"
