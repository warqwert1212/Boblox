#!/bin/bash

# Run Boblox Web Version

echo "=== Boblox Web Physics Engine ==="
echo ""

# Check if WebAssembly files exist
if [ ! -f "web/physics.js" ]; then
    echo "WebAssembly files not found. Building..."
    chmod +x build_wasm.sh
    ./build_wasm.sh
    
    if [ $? -ne 0 ]; then
        echo "Build failed. Running without WebAssembly..."
    fi
fi

echo ""
echo "Starting web server..."
echo "Open your browser to: http://localhost:8000/index.html"
echo ""
echo "Controls:"
echo "  Add Shape - Click to add falling cubes"
echo "  Embed Scene - Save current scene as JSON file"
echo "  Check browser console for physics debug info"
echo ""
echo "Press Ctrl+C to stop server"
echo ""

python3 -m http.server 8000
