TOOLS_LIST="cmake mingw-w64-gcc"
PACKAGE_LIST="vulkan glfw3 glm"

install_tools_linux() {
    sudo apt-get update
    sudo apt-get install -y $TOOLS_LIST
}

install_tools_macos() {
    brew install $TOOLS_LIST
}

add_to_path() {
    for TOOL in $TOOLS_LIST; do
        TOOL_PATH=$(which $TOOL)
        if [ -n "$TOOL_PATH" ]; then
            echo "Adding $TOOL to PATH: $TOOL_PATH"
            export PATH="$PATH:$(dirname $TOOL_PATH)"
        else
            echo "Warning: $TOOL not found!"
        fi
    done
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

echo "Setting up environment variables..."
vcpkg integrate bash

echo "Installing required packages..."
for package in $PACKAGE_LIST; do
    vcpkg install $package
done

echo "Compiling tools..."
mkdir tools
cd tools

echo "Compiling glsl..."
git clone https://github.com/KhronosGroup/glslang.git
cd glslang
cmake -B ./build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="$(pwd)/install"
cmake --build .
make -j4 install
cd ..
sudo rm -rf glslang
echo "Done"