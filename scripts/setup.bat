@echo off
setlocal

set SCRIPT_DIR=%~dp0

set SCRIPT_DIR=%SCRIPT_DIR:~0,-1%

for %%i in ("%SCRIPT_DIR%\..") do set ROOT_DIR=%%~fi

echo ROOT_DIR : %ROOT_DIR%
set BIN_DIR="%ROOT_DIR%\bin"

set TOOLS_LIST="cmake" "mingw"
set PACKAGE_LIST="vulkan" "glfw3" "glm"

@"%SystemRoot%\System32\WindowsPowerShell\v1.0\powershell.exe" -NoProfile -InputFormat None -ExecutionPolicy Bypass -Command "[System.Net.ServicePointManager]::SecurityProtocol = 3072; iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))" && SET "PATH=%PATH%;%ALLUSERSPROFILE%\chocolatey\bin"

echo "Installing required tools..."
choco install -y %TOOLS_LIST%

echo "Installing python 3.x.x."
for /f "delims=" %%i in ('python --version 2^>^&1') do set PYTHON_VERSION=%%i

for /f "tokens=2 delims= " %%a in ("%PYTHON_VERSION%") do set PYTHON_VERSION=%%a

echo %PYTHON_VERSION% | findstr "^3." >nul
if %errorlevel% neq 0 (
    echo "Python 3.x.x! Installing Python 3.12"
    choco install python --version=3.12 -y
) else (
    echo "Python 3.x found! Version: %VERSION%"
)

for %%T in (%TOOLS_LIST%) do (
    set "TOOL_PATH=%ALLUSERSPROFILE%\chocolatey\lib\%%T\tools"
    if exist "!TOOL_PATH!" (
        echo Adding %%T to PATH: !TOOL_PATH!
        set "PATH=!TOOL_PATH!;%PATH%"
    ) else (
        echo Warning: Path for %%T not found!
    )
)

echo "Cloning vcpkg..."
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg

call bootstrap-vcpkg.bat

echo "Setting up environment variables..."
.\vcpkg.exe integrate install

echo "Installing required packages..."
.\vcpkg.exe install %PACKAGE_LIST%

cd ..

echo "Compiling binaries..."
mkdir %BIN_DIR%
cd %BIN_DIR%

if not exist "%BIN_DIR%\glslangValidator.exe" (
    echo "Compiling glslang..."
    git clone https://github.com/KhronosGroup/glslang.git

    cd glslang

    python3 update_glslang_sources.py

    cmake -G "MinGW Makefiles" -B "%BIN_DIR%\glslang\build" -DCMAKE_INSTALL_PREFIX="install"
    cmake --build %BIN_DIR%\glslang\build --config Debug --target install

    echo "Compilation done."
    move %BIN_DIR%\glslang\install\bin\glslangValidator.exe %BIN_DIR%

    rmdir /s /q glslang

    cd ..
) else (
    echo "glslang compiler already exists in bin directory"
)

echo "Done."

endlocal