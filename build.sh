#!/bin/bash

# Roblox Physics & 3D Viewer Build Script

echo "=== Roblox 2008 Build System ==="
echo ""

# Check for dependencies
echo "Checking dependencies..."

# Check if g++ is available
if ! command -v g++ &> /dev/null; then
    echo "ERROR: g++ not found. Install build-essential"
    exit 1
fi

# Create build directory
mkdir -p build
cd build

# Compile physics engine
echo "Building physics engine..."
g++ -std=c++11 ../physics.cpp -o physics -lm
if [ $? -eq 0 ]; then
    echo "✓ Physics engine compiled"
else
    echo "✗ Physics engine compilation failed"
fi

# Compile shape builder (no OpenGL needed)
echo "Building shape builder..."
g++ -std=c++11 ../shape_builder.cpp -o shape_builder -lm
if [ $? -eq 0 ]; then
    echo "✓ Shape builder compiled"
else
    echo "✗ Shape builder compilation failed"
fi

# Try to compile 3D renderer (requires OpenGL + GLFW)
echo "Checking for OpenGL dependencies..."
if pkg-config --exists glfw3 glm; then
    echo "Building 3D renderer..."
    g++ -std=c++11 ../renderer.cpp -o renderer \
        $(pkg-config --cflags glfw3 glm) \
        $(pkg-config --libs glfw3) -lGL -lm
    if [ $? -eq 0 ]; then
        echo "✓ 3D renderer compiled"
    else
        echo "✗ 3D renderer compilation failed (install: sudo apt-get install libglfw3-dev)"
    fi
else
    echo "⚠ OpenGL/GLFW not found - skipping 3D renderer"
    echo "  To install: sudo apt-get install libglfw3-dev"
fi

cd ..

echo ""
echo "=== Build Complete ==="
echo ""
echo "Available programs in ./build/:"
echo "  ./physics        - Physics simulation (text mode)"
echo "  ./shape_builder  - Create and embed shapes"
if [ -f "build/renderer" ]; then
    echo "  ./renderer       - 3D interactive viewer"
fi
echo ""
echo "Usage:"
echo "  ./build/physics          - Run physics simulation"
echo "  ./build/shape_builder    - Interactive shape creation"
if [ -f "build/renderer" ]; then
    echo "  ./build/renderer         - View 3D scenes (if OpenGL available)"
fi
