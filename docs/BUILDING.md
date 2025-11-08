# Building libcsvkit

Guide to building, installing, and using libcsvkit.

## Table of Contents

- [Requirements](#requirements)
- [Quick Start](#quick-start)
- [Configuration Options](#configuration-options)
- [Building](#building)
- [Installation](#installation)
- [Uninstallation](#uninstallation)
- [Platform-Specific Notes](#platform-specific-notes)
- [Troubleshooting](#troubleshooting)

## Requirements

### C Library

- **C Compiler**: C99-compliant compiler (gcc, clang, etc.)
- **Make**: GNU Make or compatible
- **Standard C Library**: POSIX-compliant

### C++ Bindings (Optional)

- **C++ Compiler**: C++11 or later (g++, clang++)
- **C Library**: libcsvkit must be built first

### Supported Platforms

- Linux (tested on Arch, Ubuntu, Debian, Fedora)
- macOS
- BSD variants
- Windows (via MinGW, Cygwin, or WSL)

## Quick Start

```bash
# Clone or download the repository
git clone https://github.com/yourusername/libcsvkit.git
cd libcsvkit

# Configure with defaults
./configure

# Build
make

# Install (may require sudo)
sudo make install
```

### With C++ Bindings

```bash
# Configure with C++ support
./configure --enable-cpp

# Build C library
make

# Build C++ bindings
make cpp

# Install both
sudo make install
sudo make install-cpp
```

## Configuration Options

Run `./configure --help` to see all options:

```bash
./configure --help
```

### Common Options

#### Installation Paths

```bash
# Install to /usr instead of /usr/local (default)
./configure --prefix=/usr

# Custom library directory
./configure --libdir=/usr/lib64

# Custom include directory
./configure --includedir=/usr/include
```

#### Compiler Selection

```bash
# Use clang instead of gcc
./configure --cc=clang --cxx=clang++

# Specify compiler path
./configure --cc=/usr/bin/gcc-12 --cxx=/usr/bin/g++-12
```

#### Build Type

```bash
# Debug build with symbols and no optimization
./configure --build-type=Debug

# Release build with optimizations (default)
./configure --build-type=Release

# Release with debug symbols
./configure --build-type=Release --enable-debug
```

#### Features

```bash
# Enable C++ bindings
./configure --enable-cpp

# Disable static library
./configure --disable-static

# Disable shared library
./configure --disable-shared

# Disable examples
./configure --disable-examples

# Enable verbose compilation output
./configure --enable-verbose
```

#### Combined Example

```bash
./configure \
    --prefix=/usr \
    --build-type=Release \
    --enable-cpp \
    --enable-debug \
    --cc=clang \
    --cxx=clang++
```

## Building

### Build Targets

```bash
# Build C library (default target)
make

# Build C++ bindings (requires --enable-cpp)
make cpp

# Build examples
make examples

# Build everything
make all
make cpp
make examples
```

### Build Output

After building, you'll find:

```
lib/
├── libcsvkit.a              # Static library
├── libcsvkit.so             # Shared library symlink
├── libcsvkit.so.0           # Shared library versioned symlink
├── libcsvkit.so.0.1.0       # Shared library
├── libcsvkit++.so           # C++ library symlink (if enabled)
├── libcsvkit++.so.0         # C++ library versioned symlink
└── libcsvkit++.so.0.1.0     # C++ shared library

build/
├── parser.o                 # Object files
├── writer.o
└── cpp_csvkit.o             # C++ object file (if enabled)
```

### Parallel Builds

Speed up compilation with parallel jobs:

```bash
# Use 4 parallel jobs
make -j4

# Use all CPU cores
make -j$(nproc)
```

## Installation

### Standard Installation

```bash
# Install C library
sudo make install

# Install C++ bindings (if built)
sudo make install-cpp
```

### Installation Locations

With default `--prefix=/usr/local`:

```
/usr/local/lib/
├── libcsvkit.a
├── libcsvkit.so -> libcsvkit.so.0.1.0
├── libcsvkit.so.0 -> libcsvkit.so.0.1.0
├── libcsvkit.so.0.1.0
├── libcsvkit++.so -> libcsvkit++.so.0.1.0
├── libcsvkit++.so.0 -> libcsvkit++.so.0.1.0
└── libcsvkit++.so.0.1.0

/usr/local/include/
├── csvkit.h
└── csvkit.hpp
```

### User Installation (No sudo)

```bash
# Install to home directory
./configure --prefix=$HOME/.local
make
make install
make cpp
make install-cpp

# Add to PATH and LD_LIBRARY_PATH
echo 'export PATH=$HOME/.local/bin:$PATH' >> ~/.bashrc
echo 'export LD_LIBRARY_PATH=$HOME/.local/lib:$LD_LIBRARY_PATH' >> ~/.bashrc
source ~/.bashrc
```

### Verify Installation

```bash
# Check library location
ldconfig -p | grep csvkit

# Check headers
ls /usr/local/include/csvkit*

# Try compiling a test program
cat > test.c << 'EOF'
#include <csvkit.h>
#include <stdio.h>
int main() {
    printf("libcsvkit version: %d.%d.%d\n",
           CSVKIT_VERSION_MAJOR,
           CSVKIT_VERSION_MINOR,
           CSVKIT_VERSION_PATCH);
    return 0;
}
EOF

gcc test.c -o test -lcsvkit
./test
```

## Uninstallation

### Remove C Library

```bash
sudo make uninstall
```

### Remove C++ Bindings

```bash
sudo make uninstall-cpp
```

### Complete Removal

```bash
sudo make uninstall
sudo make uninstall-cpp
```

## Platform-Specific Notes

### Linux

#### Ubuntu/Debian

```bash
# Install build tools
sudo apt-get update
sudo apt-get install build-essential

# For C++ bindings
sudo apt-get install g++

# Build and install
./configure --enable-cpp
make -j$(nproc)
sudo make install
sudo make install-cpp
sudo ldconfig
```

#### Fedora/RHEL/CentOS

```bash
# Install build tools
sudo dnf groupinstall "Development Tools"
sudo dnf install gcc-c++

# Build and install
./configure --prefix=/usr --enable-cpp
make -j$(nproc)
sudo make install
sudo make install-cpp
sudo ldconfig
```

#### Arch Linux

```bash
# Install build tools
sudo pacman -S base-devel

# Build and install
./configure --enable-cpp
make -j$(nproc)
sudo make install
sudo make install-cpp
```

### macOS

```bash
# Install Xcode Command Line Tools
xcode-select --install

# Build and install
./configure --enable-cpp
make -j$(sysctl -n hw.ncpu)
sudo make install
sudo make install-cpp

# Update dyld cache
sudo update_dyld_shared_cache
```

### Windows

#### MinGW/MSYS2

```bash
# Install MinGW toolchain
pacman -S mingw-w64-x86_64-gcc

# Build
./configure --enable-cpp
make

# Install to MinGW prefix
make install prefix=/mingw64
```

#### WSL (Windows Subsystem for Linux)

```bash
# Use Ubuntu/Debian instructions
sudo apt-get install build-essential
./configure --enable-cpp
make
sudo make install
```

## Troubleshooting

### `configure: command not found`

Make configure executable:

```bash
chmod +x configure
./configure
```

### Compiler Not Found

Specify compiler path:

```bash
./configure --cc=/usr/bin/gcc --cxx=/usr/bin/g++
```

Or install compiler:

```bash
# Ubuntu/Debian
sudo apt-get install gcc g++

# Fedora
sudo dnf install gcc gcc-c++

# macOS
xcode-select --install
```

### Library Not Found at Runtime

Add library path to `LD_LIBRARY_PATH`:

```bash
# Temporary
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH

# Permanent (add to ~/.bashrc)
echo 'export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH' >> ~/.bashrc
source ~/.bashrc

# Or run ldconfig (requires sudo)
sudo ldconfig
```

### Permission Denied During Install

Use sudo:

```bash
sudo make install
```

Or install to home directory:

```bash
./configure --prefix=$HOME/.local
make
make install
```

### C++11 Features Not Available

Update compiler or specify C++11:

```bash
# Check compiler version
g++ --version

# Update compiler (Ubuntu/Debian)
sudo apt-get install g++-9

# Specify newer compiler
./configure --cxx=g++-9
```

### `make: *** No rule to make target 'cpp'`

Configure with `--enable-cpp`:

```bash
./configure --enable-cpp
make
make cpp
```

### Linking Errors

Ensure both libraries are linked:

```bash
# C++ programs need both libraries
g++ myapp.cpp -o myapp -lcsvkit++ -lcsvkit

# Or in correct order
g++ myapp.cpp -o myapp -lcsvkit++ -lcsvkit
```

### Build Artifacts Remain

Clean build:

```bash
make clean

# Or complete clean including Makefile
make distclean
./configure
make
```

## Advanced Build Configuration

### Cross-Compilation

```bash
./configure \
    --cc=arm-linux-gnueabihf-gcc \
    --cxx=arm-linux-gnueabihf-g++ \
    --ar=arm-linux-gnueabihf-ar \
    --prefix=/usr/arm-linux-gnueabihf
```

### Static-Only Build

```bash
./configure --disable-shared
make
```

### Shared-Only Build

```bash
./configure --disable-static
make
```

### Custom Flags

Edit `configure` or modify generated `Makefile`:

```bash
# Add custom CFLAGS
./configure
# Edit Makefile and add to CFLAGS line
make
```

## Build System Information

View configuration:

```bash
make config
```

Output shows:
- Platform and architecture
- Compiler paths and versions
- Build flags
- Installation directories
- Enabled features

## Next Steps

After installation:

1. Read the [C API documentation](API.md)
2. Read the [C++ API documentation](CPP_API.md)
3. Check out [examples/](../examples/) directory
4. Read the [User Guide](GUIDE.md)
