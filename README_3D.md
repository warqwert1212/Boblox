# Roblox 2008 Physics Engine & 3D Viewer

A C++ implementation of Roblox physics from 2008 with 3D visualization, shape builder, and scene embedding capabilities.

## Features

### 🎮 Physics Engine (`physics.cpp`)
- Realistic gravity and collision detection
- Anchored parts (static objects)
- Elasticity and friction simulation
- Part-to-part collision response
- Quaternion-based rotation
- Ground collision detection with bouncing

### 🎨 3D Renderer (`renderer.cpp`)
- OpenGL-based real-time visualization
- Phong lighting model
- Interactive camera controls
- **EMBED BUTTON**: Press **E** to save the current scene as a permanent file

### 🛠️ Shape Builder (`shape_builder.cpp`)
- Create custom shapes (cube, sphere, cylinder, wedge)
- Apply materials and textures from library
- Adjust size, position, color
- **EMBED shapes individually** as JSON files that persist forever

### 📦 Material Library
Built-in materials:
- BrickRed - Standard red brick
- BrickBlue - Standard blue brick
- Gold - Metallic gold
- Steel - Metal texture
- Plastic - Plastic material
- Wood - Wooden texture
- Glass - Transparent glass
- Green - Green brick

## Building

### Quick Build (Linux/macOS)
```bash
chmod +x build.sh
./build.sh
```

### Manual Build

**Physics Engine** (no dependencies):
```bash
g++ -std=c++11 -o physics physics.cpp -lm
```

**Shape Builder** (no dependencies):
```bash
g++ -std=c++11 -o shape_builder shape_builder.cpp -lm
```

**3D Renderer** (requires OpenGL and GLFW):
```bash
# Install dependencies (Ubuntu/Debian)
sudo apt-get install libglfw3-dev libglm-dev

# Compile
g++ -std=c++11 -o renderer renderer.cpp $(pkg-config --cflags glfw3 glm) $(pkg-config --libs glfw3) -lGL -lm
```

## Usage

### Physics Simulation
```bash
./physics
```
Shows text-based simulation of bricks falling and bouncing with physics.

**Output includes:**
- Part positions and velocities
- Collision detection
- Settling detection

### Shape Builder - Interactive Creation
```bash
./shape_builder
```

**Commands:**
```
create cube MyBrick          # Create a cube named "MyBrick"
size MyBrick 2 2 2           # Set size to 2x2x2
pos MyBrick 0 5 0            # Position at (0, 5, 0)
material MyBrick gold        # Apply gold material
show MyBrick                  # Display shape info
embed MyBrick                # EMBED the shape forever to JSON file

create sphere MyBall
material MyBall glass
embed MyBall

list                         # Show all created shapes
exit                         # Quit
```

### 3D Viewer with Embed
```bash
./renderer
```

**Controls:**
- **W/A/S/D** - Rotate camera around scene
- **UP/DOWN arrows** - Move camera up/down
- **E key** - **EMBED the current scene** as a `.scene` file

When you press E, the scene is saved to `embedded_scene_[timestamp].scene` and persists forever.

## Files Generated

### After Embedding Shapes
```
embedded_MyBrick.shape      # JSON file with brick data
embedded_MyBall.shape       # JSON file with sphere data
```

### After Embedding Scene
```
embedded_scene_1713607834.scene  # Complete scene snapshot
```

These files contain all geometry, materials, positions, and can be reloaded.

## Example Workflow

1. **Create shapes:**
   ```bash
   ./shape_builder
   > create cube Brick1
   > size Brick1 2 4 1
   > material Brick1 red
   > embed Brick1
   ```

2. **View in 3D:**
   ```bash
   ./renderer
   ```
   Rotate around with WASD, press E to save scene permanently.

3. **Check embedded files:**
   ```bash
   ls embedded_*.scene
   ls embedded_*.shape
   cat embedded_Brick1.shape
   ```

## Physical Properties

### Default Gravity
```
9.81 * 10 = 98.1 m/s²  (Roblox-style, stronger than real)
```

### Collision Parameters
- **Elasticity**: Controls bounce (0.0 = no bounce, 1.0 = infinite bounce)
- **Friction**: Slows moving parts (0.3–0.6 typical)
- **Ground Level**: Y = 0 by default

## Architecture

```
┌─────────────────┐
│   physics.cpp   │  ← Core physics simulation
├─────────────────┤
│  renderer.cpp   │  ← OpenGL 3D visualization + embed
├─────────────────┤
│ shape_builder   │  ← Interactive shape creation + embed
└─────────────────┘
     ↓ embeds to ↓
  .scene files
  .shape files
```

## System Requirements

- **C++11** compatible compiler (g++, clang)
- For 3D renderer: **OpenGL 3.3+** and **GLFW3**
- Linux/macOS/Windows (with GLFW installed)

## Troubleshooting

**3D Renderer won't compile:**
```bash
sudo apt-get install libglfw3-dev libglm-dev
```

**Physics simulation too fast/slow:**
Adjust `dt` in main loop (currently 0.016s = ~60 FPS)

**Can't embed scenes:**
Make sure you have write permissions in the current directory.

## Future Enhancements

- [ ] Sphere and cylinder shapes in renderer
- [ ] Weld/hinge joints
- [ ] Terrain support
- [ ] Script execution (Lua)
- [ ] Network multiplayer
- [ ] Humanoid models and animation

## License

Educational project - Roblox 2008 recreation in C++
