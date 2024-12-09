Ethereal Engine

Description:
Multi-purpose, lightweight graphics engine for writing simulations, machine learining and CV Models training

## Prebuild
Download CMake, vcpkg, MinGW for further convinient crossplatform build and working with engine

# Windows

# Download CMake
winget cmake

# Download and setup vcpkg
git clone https://github.com/microsoft/vcpkg.git "C:\vcpkg"
cd "C:\vcpkg"
.\bootstrap-vcpkg.bat
mklink C:\Program Files\vcpkg C:\vcpkg\vcpkg.exe

# Download MinGW
winget install MSYS2
pacman -Syu
pacman -S mingw-w64-x86_64-gcc
x86_64-w64-mingw32-gcc --version

# Linux
# Download CMake
sudo apt update
sudo apt install -y cmake build-essential curl git

# Download and setup vcpkg
sudo git clone https://github.com/microsoft/vcpkg.git /usr/local/vcpkg
cd /usr/local/vcpkg
sudo ./bootstrap-vcpkg.sh
sudo ln -s /usr/local/vcpkg/vcpkg /usr/local/bin/vcpkg

## MacOS
# Download Homebrew if not installed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Download CMake
brew install cmake

# Download and setup vcpkg
sudo git clone https://github.com/microsoft/vcpkg.git /usr/local/vcpkg
cd /usr/local/vcpkg
sudo ./bootstrap-vcpkg.sh
sudo ln -s /usr/local/vcpkg/vcpkg /usr/local/bin/vcpkg