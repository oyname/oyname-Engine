# 3D Graphics Engine - Function Overview

Based on analysis of `EntityTest.cpp`

## 0. Basic Code Structure

### Required Header
```cpp
#include "gidx.h"
```
The main header file **must** be included at the beginning of every application. It contains:
- All engine functions and classes
- Entity types (LPENTITY, LPSURFACE, LPTEXTURE, LPMATERIAL)
- Windows system integration
- DirectX 11 wrapper

### Main Function
```cpp
int main()
{
    // Engine initialization
    Engine::Graphics(1200, 650);
    
    // Setup code (Camera, Lights, Meshes)
    // ...
    
    // Main Loop
    while (Windows::MainLoop() && !(GetAsyncKeyState(VK_ESCAPE) & 0x8000))
    {
        Engine::Cls(0, 64, 128);
        Engine::UpdateWorld();
        Engine::RenderWorld();
        Engine::Flip();
    }
    
    // Shutdown
    return Windows::ShutDown();
}
```

**Important:**
- The application requires an `int main()` function
- This is started by the engine framework as a separate thread
- The framework handles WinMain() and the Windows message loop
- main() is the entry point for custom application logic


---

## 1. Engine Initialization

### Graphics System
```cpp
Engine::Graphics(1200, 650)                    // Windowed mode
Engine::Graphics(1980, 1080, false)            // Fullscreen mode
```
- Initializes the DirectX 11 rendering system
- Creates window or fullscreen application
- Sets up Device, Context, and Swap Chain

### Windows System
```cpp
Windows::MainLoop()                            // Message processing
Windows::ShutDown()                            // Resource cleanup
```

---

## 2. Asset Management

### Texture Management
```cpp
Engine::LoadTexture(&texture, L"..\\media\\face.bmp")
```
- Loads image files as GPU textures
- Supports common image formats
- Creates `LPTEXTURE` pointer

### Material System
```cpp
Engine::CreateMaterial(&material)              // Create material
Engine::MaterialTexture(material, texture)     // Assign texture
```
- Encapsulates shader properties and textures
- Connects textures with materials
- Defines surface properties

---

## 3. Entity System (Unified Architecture)

The engine uses a unified entity system with type-safe wrappers:

### Light (Lighting)
```cpp
Engine::CreateLight(&light, D3DLIGHT_DIRECTIONAL)
Engine::TurnEntity(light, -45, 0, 0)
```
**Light Types:**
- `D3DLIGHT_DIRECTIONAL` - Directional light (e.g., sun)
- `D3DLIGHT_POINT` - Point light
- `D3DLIGHT_SPOT` - Spotlight

### Camera
```cpp
Engine::CreateCamera(&camera)
Engine::PositionEntity(camera, 0, 0, -5)
```
- Defines viewpoint for the scene
- Calculates View and Projection matrices
- Uses quaternion-based transformation

### Mesh (3D Objects)
```cpp
Engine::CreateMesh(&cube)
Engine::CreateMesh(&mesh, material)            // With material
```
- Renderable 3D geometry
- Can be assigned materials
- Supports multiple surfaces

---

## 4. Geometry Creation

### Surface System
```cpp
Engine::CreateSurface(&surface, mesh)
```
Surfaces are sub-units of a mesh and contain the actual geometry.

### Vertex Definition
```cpp
Engine::AddVertex(surface, x, y, z)            // Position
Engine::VertexNormal(surface, nx, ny, nz)      // Normal for lighting
Engine::VertexColor(surface, r, g, b)          // Vertex color
Engine::VertexTexCoord(surface, u, v)          // Texture coordinates
```

**Workflow:**
1. Define position
2. Set normal for lighting
3. Optional: Color and/or texture coordinates

### Triangle Definition
```cpp
Engine::AddTriangle(surface, v0, v1, v2)       // Vertex indices
```
- Defines triangles through vertex indices
- Winding order determines back/front face
- Typically counter-clockwise for front face

### Buffer Finalization
```cpp
Engine::FillBuffer(surface)
```
- Transfers geometry data to GPU buffers
- Creates vertex and index buffers
- Optimized for GPU rendering

**Example: Cube Face**
```cpp
// Front-Face (4 Vertices)
Engine::AddVertex(wuerfel, -1.0f, -1.0f, -1.0f);
Engine::VertexNormal(wuerfel, 0.0f, 0.0f, -1.0f);
Engine::VertexColor(wuerfel, 224, 224, 224);
Engine::VertexTexCoord(wuerfel, 0.0f, 1.0f);
// ... more vertices

// Triangles
Engine::AddTriangle(wuerfel, 0, 1, 2);
Engine::AddTriangle(wuerfel, 3, 2, 1);
```

---

## 5. Transform Operations

All entity types support the same transformations:

### Position
```cpp
Engine::PositionEntity(entity, x, y, z)
```
- Absolute position in world coordinate system
- Works for Mesh, Camera, and Light

### Rotation
```cpp
Engine::RotateEntity(entity, pitch, yaw, roll)    // Absolute rotation
Engine::TurnEntity(entity, pitch, yaw, roll)      // Incremental rotation
```
- **RotateEntity**: Sets absolute rotation
- **TurnEntity**: Adds rotation relative to current
- Internally uses quaternions for correct rotation
- Angles in degrees

### Scale
```cpp
Engine::ScaleEntity(entity, sx, sy, sz)
```
- Scaling along three axes
- Values < 1.0 shrink, > 1.0 enlarge
- 1.0 = original size

---

## 6. Render Pipeline

### Main Loop
```cpp
while (Windows::MainLoop() && !(GetAsyncKeyState(VK_ESCAPE) & 0x8000))
{
    Engine::Cls(0, 64, 128);           // Clear with color (R,G,B)
    
    Engine::UpdateWorld();             // Update transform hierarchy
    Engine::RenderWorld();             // Render scene
    Engine::Flip();                    // Swap back/front buffer
}
```

### Render Functions

#### Clear Screen
```cpp
Engine::Cls(r, g, b)
```
- Clears color and depth buffers
- RGB values 0-255
- Prepares frame for new rendering

#### Update World
```cpp
Engine::UpdateWorld()
```
- Updates all entity transformations
- Calculates world matrices
- Processes transform hierarchies
- Prepares constant buffers

#### Render World
```cpp
Engine::RenderWorld()
```
- Iterates through all meshes
- Sets shader and material
- Binds buffers and textures
- Executes draw calls
- Applies lighting

#### Flip
```cpp
Engine::Flip()
```
- Present(): Displays rendered frame
- Swap chain buffer swap
- VSync synchronization

---

## 7. Architecture Highlights

### Unified Entity System
All objects (Mesh, Camera, Light) are entities with:
- Unified transform API
- Type-safe wrapper functions
- Quaternion-based rotation
- Consistent position/rotation/scale

### Manager System
- **ObjectManager**: Manages meshes and cameras
- **LightManager**: Manages lights
- **TextureManager**: Manages textures
- **MaterialManager**: Manages materials
- **ShaderManager**: Manages shaders
- **BufferManager**: Manages GPU buffers

### Rendering Flow
```
Initialization
    ↓
Asset Loading (Textures, Materials)
    ↓
Entity Creation (Camera, Light, Meshes)
    ↓
Geometry Definition (Vertices, Triangles)
    ↓
Main Loop:
    ├─ Clear Screen
    ├─ Update Transforms
    ├─ Render All Entities
    └─ Present Frame
```

---

## 8. Usage Patterns

### Minimal Rendering Setup
```cpp
#include "gidx.h"

int main()
{
    // 1. Initialize graphics
    Engine::Graphics(1200, 650);

    // 2. Create camera
    LPENTITY camera = nullptr;
    Engine::CreateCamera(&camera);
    Engine::PositionEntity(camera, 0, 0, -5);

    // 3. Create light
    LPENTITY light = nullptr;
    Engine::CreateLight(&light, D3DLIGHT_DIRECTIONAL);

    // 4. Create mesh
    LPENTITY mesh = nullptr;
    Engine::CreateMesh(&mesh);
    // ... define geometry

    // 5. Render loop
    while (Windows::MainLoop() && !(GetAsyncKeyState(VK_ESCAPE) & 0x8000)) {
        Engine::Cls(0, 64, 128);
        Engine::UpdateWorld();
        Engine::RenderWorld();
        Engine::Flip();
    }
    
    // 6. Shutdown
    return Windows::ShutDown();
}
```

### Geometry with Texture
```cpp
#include "gidx.h"

int main()
{
    Engine::Graphics(1200, 650);
    
    // Material with texture
    LPTEXTURE texture = nullptr;
    Engine::LoadTexture(&texture, L"texture.bmp");
    LPMATERIAL material = nullptr;
    Engine::CreateMaterial(&material);
    Engine::MaterialTexture(material, texture);

    // Mesh with material
    LPENTITY mesh = nullptr;
    Engine::CreateMesh(&mesh, material);
    // ... define geometry with VertexTexCoord
    
    // Camera and light
    LPENTITY camera = nullptr;
    Engine::CreateCamera(&camera);
    Engine::PositionEntity(camera, 0, 0, -5);
    
    LPENTITY light = nullptr;
    Engine::CreateLight(&light, D3DLIGHT_DIRECTIONAL);
    
    // Render loop
    while (Windows::MainLoop() && !(GetAsyncKeyState(VK_ESCAPE) & 0x8000)) {
        Engine::Cls(0, 64, 128);
        Engine::UpdateWorld();
        Engine::RenderWorld();
        Engine::Flip();
    }
    
    return Windows::ShutDown();
}
```

---

## 9. Performance Characteristics

Based on engine testing:
- **GPU-bound**: With many objects (10,000+ cubes)
- **Low CPU load**: Efficient transform calculations
- **DirectX 11**: Modern graphics pipeline
- **Possible optimizations**: Instancing, Frustum Culling, GPU Compute

---

## Summary

The engine provides:
✓ Unified entity system for all 3D objects
✓ Flexible geometry creation with vertices and triangles
✓ Material and texture system
✓ Lighting system (Directional, Point, Spot)
✓ Quaternion-based transformations
✓ Simple and intuitive API
✓ DirectX 11 backend
✓ Manager-based resource management
