#!/bin/bash

# Build WebAssembly version of Boblox Physics Engine

echo "=== Building Boblox WebAssembly ==="

# Check if emsdk is available
if [ ! -d "/workspaces/emsdk" ]; then
    echo "Emscripten SDK not found. Please install emsdk first:"
    echo "git clone https://github.com/emscripten-core/emsdk.git"
    echo "cd emsdk && ./emsdk install latest && ./emsdk activate latest"
    exit 1
fi

# Source emsdk
source /workspaces/emsdk/emsdk_env.sh

# Create build directory
mkdir -p web

# Compile to WebAssembly
echo "Compiling physics_wasm.cpp to WebAssembly..."
emcc physics_wasm.cpp \
    -o web/physics.js \
    -s WASM=1 \
    -s EXPORTED_FUNCTIONS="['_initPhysics','_addPhysicsBody','_simulatePhysics','_clearPhysicsWorld','_getBodyCount','_getBodyData','_embedScene']" \
    -s EXPORTED_RUNTIME_METHODS="['ccall','cwrap']" \
    -s ALLOW_MEMORY_GROWTH=1 \
    -O3 \
    --bind

if [ $? -eq 0 ]; then
    echo "✅ WebAssembly build successful!"
    echo "Files created:"
    echo "  web/physics.js"
    echo "  web/physics.wasm"
    echo ""
    echo "To run the web version:"
    echo "  python3 -m http.server 8000"
    echo "  Open http://localhost:8000/index.html"
else
    echo "❌ WebAssembly build failed"
fi
