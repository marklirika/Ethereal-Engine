TOOLS_LIST="cmake mingw-w64-gcc"
PACKAGE_LIST="vulkan glfw3 glm"

install_tools_linux() {
    sudo apt-get update
    sudo apt-get install -y $TOOLS_LIST
}

install_tools_macos() {
    brew install $TOOLS_LIST
}

echo "Installing tools for $OSTYPE..."

if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    echo "Linux"
    install_tools_linux
elif [[ "$OSTYPE" == "darwin"* ]]; then
    echo "MacOS"
else
    echo "Unsupported OS"
    exit 1
fi

echo "Cloning vcpkg..."
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg || exit
./bootstrap-vcpkg.sh
cd ..

echo "Setting up environment variables..."
vcpkg integrate bash

echo "Installing required packages..."
vcpkg install $PACKAGE_LIST

