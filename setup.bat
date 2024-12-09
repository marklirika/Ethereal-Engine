
@echo off
setlocal

set TOOLS_LIST=cmake mingw
set PACKAGE_LIST=vulkan glfw3 glm

@"%SystemRoot%\System32\WindowsPowerShell\v1.0\powershell.exe" -NoProfile -InputFormat None -ExecutionPolicy Bypass -Command "[System.Net.ServicePointManager]::SecurityProtocol = 3072; iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))" && SET "PATH=%PATH%;%ALLUSERSPROFILE%\chocolatey\bin"

echo "Installing required tools..."
choco install -y %TOOLS_LIST%

echo "Cloning vcpkg..."
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg || exit
./bootstrap-vcpkg.bat
cd ..

echo "Setting up environment variables..."
.\vcpkg.exe integrate install

echo "Installing required packages..."
.\vcpkg.exe install %PACKAGE_LIST%

endlocal