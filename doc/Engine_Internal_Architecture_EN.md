# 3D Graphics Engine - Internal Architecture and Flow

Detailed description of internal systems, manager structure, and rendering pipeline

---

## 1. Architecture Overview

### Manager Hierarchy

```
CGIDX (Engine Core)
    │
    ├─── ObjectManager        (Manages Meshes, Cameras, Materials, Shaders, Surfaces)
    ├─── LightManager         (Manages Lights)
    ├─── CameraManager        (Manages active Camera)
    ├─── TextureManager       (Manages Textures)
    ├─── ShaderManager        (Manages Shader compilation)
    ├─── BufferManager        (Manages GPU buffers)
    └─── RenderManager        (Coordinates rendering)
         └─── Requires: ObjectManager & LightManager (friend class)
```

### Class Hierarchy

```
Entity (Base class)
├─── Mesh    (Renderable geometry)
├─── Camera  (Viewpoint)
└─── Light   (Lighting)

Other important classes:
├─── Surface  (Geometry container)
├─── Material (Surface properties + texture)
└─── Shader   (HLSL wrapper)
```

---

## 2. Entity System in Detail

### Entity (Base Class)

**Files:** `Entity.h`, `Entity.cpp`

**Responsibilities:**
- Base for all 3D objects in space
- Transformation (Position, Rotation, Scale)
- Matrix generation
- Constant buffer for GPU

**Important Members:**
```cpp
class Entity {
public:
    Transform transform;              // Position, Rotation, Scale (Quaternion)
    MatrixSet matrixSet;             // World, View, Projection matrices
    ID3D11Buffer* constantBuffer;    // GPU Constant Buffer
    D3D11_VIEWPORT viewport;         // Viewport definition
    
protected:
    bool isActive;                   // Active/inactive status
};
```

**Methods:**
- `Update()` - Updates Transform → Matrix
- `GenerateViewMatrix()` - Creates view matrix
- `GenerateProjectionMatrix()` - Creates projection matrix
- `GenerateViewport()` - Configures viewport

**Memory Alignment:**
```cpp
void* operator new(size_t size) {
    return _aligned_malloc(size, 16);  // 16-byte alignment for SIMD
}
```
DirectX requires 16-byte alignment for XMMATRIX and XMVECTOR.

---

### Mesh (Renderable Geometry)

**Files:** `Mesh.h`, `Mesh.cpp`

**Inherits from:** `Entity`

**Responsibilities:**
- Container for surface geometry
- Reference to material and shader
- Collision detection (OBB)
- Transform update for rendering

**Important Members:**
```cpp
class Mesh : public Entity {
public:
    std::vector<Surface*> surfaces;  // All surfaces of the mesh
    void* pShader;                   // Pointer to shader
    void* pMaterial;                 // Pointer to material
    BoundingOrientedBox obb;         // Collision bounding box
};
```

**Update Variants:**
```cpp
// 1. Simple update (for non-rendering updates)
void Update(const gdx::CDevice* device) override;

// 2. Rendering update with external MatrixSet (from Camera)
void Update(const gdx::CDevice* device, MatrixSet* matrixSet);
```

**Relationships:**
- Mesh → N:1 → Material (One mesh, one material)
- Mesh → 1:N → Surface (One mesh, many surfaces)

---

### Camera (Viewpoint)

**Files:** `Camera.h`, `Camera.cpp`

**Inherits from:** `Entity`

**Responsibilities:**
- Defines viewpoint and view direction
- Generates view and projection matrices
- Controls viewport

**Projection Modes:**
```cpp
enum PROJ_MODE {
    ORTHO_LH,           // Orthographic projection
    PERSPECTIVE_LH,     // Perspective projection (default)
};
```

**Update Method:**
```cpp
void UpdateCamera(XMVECTOR position, XMVECTOR direction, XMVECTOR up);
```
This is called in `CGIDX::UpdateWorld()` with transform data.

---

### Light (Lighting)

**Files:** `Light.h`, `Light.cpp`

**Inherits from:** `Entity`

**Responsibilities:**
- Light source management
- Light constant buffer for shaders
- Supports different light types

**Light Buffer Structure:**
```cpp
__declspec(align(16))
struct LightBufferData {
    XMFLOAT4 lightPosition;      // Position in space
    XMFLOAT4 lightDirection;     // Direction (for Directional/Spot)
    XMFLOAT4 lightDiffuseColor;  // Main color
    XMFLOAT4 lightAmbientColor;  // Ambient light
};
```

**Light Types:**
- `D3DLIGHT_DIRECTIONAL` - Directional light (sun)
- `D3DLIGHT_POINT` - Point light (light bulb)
- `D3DLIGHT_SPOT` - Spotlight (searchlight)

**Update Method:**
```cpp
void UpdateLight(const gdx::CDevice* device, XMVECTOR position, XMVECTOR lookAt);
```

---

## 3. Geometry and Material System

### Surface (Geometry Container)

**Files:** `Surface.h`, `Surface.cpp`

**Responsibilities:**
- Stores vertex data (Position, Normal, Color, UV)
- Stores index data (Triangles)
- Creates and manages GPU buffers
- Executes draw calls

**Data Structure:**
```cpp
class Surface {
public:
    // CPU data (before GPU upload)
    std::vector<XMFLOAT3> position;    // Vertex positions
    std::vector<XMFLOAT3> normal;      // Normals for lighting
    std::vector<XMFLOAT4> color;       // Vertex colors
    std::vector<XMFLOAT2> uv1;         // Texture coordinates 1
    std::vector<XMFLOAT2> uv2;         // Texture coordinates 2
    std::vector<unsigned int> indices; // Triangle indices
    
    // GPU buffers
    ID3D11Buffer* positionBuffer;
    ID3D11Buffer* normalBuffer;
    ID3D11Buffer* colorBuffer;
    ID3D11Buffer* uv1Buffer;
    ID3D11Buffer* uv2Buffer;
    ID3D11Buffer* indexBuffer;
};
```

**Workflow:**
1. Fill CPU-side data with `AddVertex()`, `VertexNormal()`, etc.
2. Define indices with `AddIndex()`
3. `FillBuffer()` copies data to GPU buffers
4. `Draw()` executes render call

**Draw Method:**
```cpp
void Draw(const gdx::CDevice* device, const DWORD flags);
```
- Binds buffers to Input Assembler
- Uses `flags` to determine which buffers are active
- Executes `DrawIndexed()`

---

### Material (Surface Properties)

**Files:** `Material.h`, `Material.cpp`

**Responsibilities:**
- Defines surface properties (color, shininess, transparency)
- Manages texture reference
- Material constant buffer for shaders
- Connects meshes with shaders

**Material Properties:**
```cpp
struct MaterialData {
    XMFLOAT4 diffuseColor;     // Main color (RGBA)
    XMFLOAT4 specularColor;    // Specular highlight color
    float shininess;           // Shininess intensity
    float transparency;        // Transparency (0-1)
    float padding[2];          // GPU alignment
};
```

**Texture System:**
```cpp
ID3D11Texture2D* m_texture;              // Texture resource
ID3D11ShaderResourceView* m_textureView; // Shader resource view
ID3D11SamplerState* m_imageSamplerState; // Sampler (filter settings)
ID3D11Buffer* materialBuffer;            // Constant buffer
```

**Relationships:**
```cpp
std::vector<Mesh*> meshes;    // All meshes with this material
void* pRenderShader;          // Shader for this material
```

---

### Shader (HLSL Wrapper)

**Files:** `Shader.h`, `Shader.cpp`

**Responsibilities:**
- Loads and compiles HLSL shaders
- Manages input layout
- Coordinates materials
- Defines which vertex attributes are used

**Important Members:**
```cpp
class Shader {
public:
    ID3D11VertexShader* vertexShader;
    ID3D11PixelShader* pixelShader;
    ID3D11InputLayout* inputLayout;
    
    std::vector<Material*> materials;  // All materials with this shader
    DWORD flagsVertex;                 // Which vertex data is active
};
```

**Vertex Flags:**
```cpp
#define D3DVERTEX_POSITION   0x0001  // Position enabled
#define D3DVERTEX_NORMAL     0x0002  // Normals enabled
#define D3DVERTEX_COLOR      0x0004  // Color enabled
#define D3DVERTEX_TEXCOORD1  0x0008  // UV1 enabled
#define D3DVERTEX_TEXCOORD2  0x0010  // UV2 enabled
```

---

## 4. Manager System in Detail

### ObjectManager

**Files:** `ObjectManager.h`, `ObjectManager.cpp`

**Central Management:**
- All entities (Meshes, Cameras)
- All surfaces
- All materials
- All shaders

**Managed Vectors:**
```cpp
std::vector<Entity*> m_entities;    // Mesh + Camera
std::vector<Surface*> m_surfaces;
std::vector<Mesh*> m_meshes;
std::vector<Camera*> m_cameras;
std::vector<Material*> m_materials;
std::vector<Shader*> m_shaders;
```

**Responsibilities:**
1. **CREATE** - Create and register objects
2. **ADD** - Build relationships (Surface→Mesh, Mesh→Material, Material→Shader)
3. **DELETE** - Delete objects and remove from vectors
4. **REMOVE** - Dissolve relationships
5. **GET** - Query objects

**Hierarchy Building:**
```cpp
// Add surface to mesh
void addSurfaceToMesh(Mesh* mesh, Surface* surface);

// Assign mesh to material
void addMeshToMaterial(Material* material, Mesh* mesh);

// Assign material to shader
void addMaterialToShader(Shader* shader, Material* material);
```

**Friend Class:**
```cpp
friend class RenderManager;
```
RenderManager has direct access to private members for efficient rendering.

---

### LightManager

**Files:** `LightManager.h`, `LightManager.cpp`

**Responsibilities:**
- Creates and manages all lights
- Updates light constant buffers
- Independent of ObjectManager

**Managed Data:**
```cpp
std::vector<Light*> m_lights;
```

**Update Method:**
```cpp
void Update(const gdx::CDevice* device);
```
Called in `CGIDX::UpdateWorld()`, updates all lights.

---

### CameraManager

**Files:** `CameraManager.h`, `CameraManager.cpp`

**Responsibilities:**
- Manages active camera
- Provides current camera for rendering

**Most Important Method:**
```cpp
Camera* GetCurrentCam() const;
```

---

### RenderManager

**Files:** `RenderManager.h`, `RenderManager.cpp`

**Responsibilities:**
- Coordinates the entire rendering process
- Uses ObjectManager and LightManager
- Sets camera and light for rendering

**Constructor:**
```cpp
RenderManager(ObjectManager& objectManager, LightManager& lightManager);
```
**IMPORTANT:** RenderManager requires references to both managers!

**Main Methods:**
```cpp
void SetCamera(LPENTITY camera);              // Sets active camera
void SetDirectionalLight(LPENTITY dirLight);  // Sets main light
void RenderScene();                           // Main rendering
```

---

## 5. Rendering Pipeline (Detailed)

### Overview: Frame Rendering

```
Engine::UpdateWorld()
    │
    ├─── Update camera transform
    ├─── Call Camera::UpdateCamera()
    └─── Call LightManager::Update()
    
Engine::RenderWorld()
    │
    ├─── Update timer
    ├─── Check device context
    ├─── Set render targets
    ├─── Clear depth/stencil buffer
    ├─── Set viewport
    └─── Call RenderManager::RenderScene()
```

---

### UpdateWorld() - Transform Update

**File:** `gdxengine.cpp`

```cpp
void CGIDX::UpdateWorld()
{
    // 1. Get active camera
    auto* cam = m_cameraManager.GetCurrentCam();
    if (cam == nullptr) {
        Debug::Log("ERROR: UpdateWorld - No camera set");
        return;
    }
    
    // 2. Extract transform data
    XMVECTOR position = cam->transform.getPosition();
    XMVECTOR forward = XMVector3Normalize(cam->transform.getLookAt());
    XMVECTOR up = XMVector3Normalize(cam->transform.getUp());
    
    // 3. Update camera view/projection matrices
    cam->UpdateCamera(position, forward, up);
    
    // 4. Update all lights
    m_lightManager.Update(&m_device);
}
```

**What happens:**
1. Camera transform (quaternion-based) → Position, LookAt, Up
2. Calculate view matrix from camera transform
3. Update projection matrix
4. Send light constant buffer to GPU

---

### RenderWorld() - Main Loop

**File:** `gdxengine.cpp`

```cpp
HRESULT CGIDX::RenderWorld()
{
    // 1. Update timer (Delta Time, FPS)
    Timer::GetInstance()->Update();
    
    // 2. Get device context
    auto* pContext = m_device.GetDeviceContext();
    if (!pContext) return E_FAIL;
    
    // 3. Check render targets and depth buffer
    if (!m_device.m_depthStencilView || !m_device.m_pRenderTargetView)
        return E_POINTER;
    
    // 4. Check camera
    auto* pCamera = m_cameraManager.GetCurrentCam();
    if (!pCamera) return E_FAIL;
    
    // 5. Prepare pipeline
    pContext->ClearDepthStencilView(m_device.m_depthStencilView, 
                                    D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 
                                    1.0f, 0);
    pContext->OMSetRenderTargets(1, &m_device.m_pRenderTargetView, 
                                  m_device.m_depthStencilView);
    pContext->RSSetState(m_device.m_pRasterizerState);
    pContext->RSSetViewports(1, &pCamera->viewport);
    
    // 6. RENDERING
    m_renderManager.RenderScene();
    
    return S_OK;
}
```

---

### RenderScene() - Object Processing

**File:** `RenderManager.cpp`

**Hierarchical Loop: Shader → Material → Mesh → Surface**

```cpp
void RenderManager::RenderScene()
{
    // Safety check
    if (!m_currentCam) {
        Debug::Log("WARNING: RenderManager::RenderScene - Camera not set");
        return;
    }
    
    // LEVEL 1: SHADER
    for (const auto& shader : m_objectManager.m_shaders)
    {
        // Skip empty shaders
        if (shader->materials.empty()) continue;
        
        // Activate shader (VS, PS, Input Layout)
        shader->UpdateShader(m_objectManager.m_device);
        
        // LEVEL 2: MATERIAL (within shader)
        for (const auto& material : shader->materials)
        {
            // Bind texture
            material->SetTexture(m_objectManager.m_device);
            
            // Material constant buffer to GPU
            material->UpdateConstantBuffer(m_objectManager.m_device->GetDeviceContext());
            
            // Skip empty materials
            if (material->meshes.empty()) continue;
            
            // LEVEL 3: MESH (within material)
            for (const auto& mesh : material->meshes)
            {
                // Build MatrixSet
                MatrixSet ms = m_currentCam->matrixSet;  // View + Projection
                ms.worldMatrix = mesh->transform.GetLocalTransformationMatrix(); // World
                
                // Update mesh (constant buffer to GPU)
                mesh->Update(m_objectManager.m_device, &ms);
                
                // LEVEL 4: SURFACE (within mesh)
                if (!mesh->surfaces.empty())
                {
                    for (const auto& surface : mesh->surfaces)
                    {
                        // Execute draw call
                        surface->Draw(m_objectManager.m_device, shader->flagsVertex);
                    }
                }
            }
        }
    }
}
```

---

### Rendering Hierarchy Visualized

```
┌─────────────────────────────────────────────────────────────┐
│ RenderScene()                                               │
│                                                             │
│  FOR EACH SHADER:                                           │
│  ├─ UpdateShader() → Vertex/Pixel Shader + Input Layout   │
│  │                                                          │
│  └─ FOR EACH MATERIAL:                                      │
│     ├─ SetTexture() → Bind texture                        │
│     ├─ UpdateConstantBuffer() → Material data to GPU      │
│     │                                                       │
│     └─ FOR EACH MESH:                                       │
│        ├─ Assemble MatrixSet (World, View, Proj)          │
│        ├─ mesh->Update() → Matrices to GPU                │
│        │                                                    │
│        └─ FOR EACH SURFACE:                                 │
│           └─ surface->Draw() → Render call                │
│              ├─ Bind vertex buffers                        │
│              ├─ Bind index buffer                          │
│              └─ DrawIndexed()                              │
└─────────────────────────────────────────────────────────────┘
```

---

## 6. Object Connections and Data Flow

### Hierarchy Building (Bottom-Up)

```
1. Create surface
   ↓
2. Add surface to mesh
   ↓
3. Assign mesh to material
   ↓
4. Assign material to shader
```

**Code Example:**
```cpp
// 1. Create surface
LPSURFACE surface = nullptr;
Engine::CreateSurface(&surface, mesh);

// Define geometry...
Engine::AddVertex(surface, ...);
Engine::AddTriangle(surface, ...);
Engine::FillBuffer(surface);

// 2. Automatic: Surface was already registered in mesh

// 3. Mesh to material
objectManager.addMeshToMaterial(material, mesh);

// 4. Material to shader
objectManager.addMaterialToShader(shader, material);
```

---

### Data Flow During Rendering

```
CPU SIDE:
┌────────────┐
│   SHADER   │  (ID3D11VertexShader, ID3D11PixelShader)
│            │
│  Materials │
└──────┬─────┘
       │
       ├─ Material 1
       │  ├─ Texture: ID3D11ShaderResourceView
       │  ├─ Material Buffer: diffuse, specular, shininess...
       │  │
       │  └─ Meshes
       │     ├─ Mesh A
       │     │  ├─ Transform → World Matrix
       │     │  └─ Surfaces → Vertex/Index Buffer
       │     │
       │     └─ Mesh B
       │        └─ ...
       │
       └─ Material 2
          └─ ...

GPU SIDE:
┌────────────────────────────────────────┐
│ Vertex Shader                          │
│  Input: Position, Normal, Color, UV    │
│  Const: World, View, Projection Matrix │
│  Output: Transformed Vertices          │
└──────────────┬─────────────────────────┘
               │
┌──────────────▼─────────────────────────┐
│ Pixel Shader                           │
│  Input: Interpolated Vertex Data       │
│  Const: Material (diffuse, specular)   │
│  Const: Light (position, color)        │
│  Texture: Sampled via UV               │
│  Output: Final Pixel Color             │
└────────────────────────────────────────┘
```

---

### Pointer Relationships

```cpp
// SHADER ↔ MATERIAL
class Shader {
    std::vector<Material*> materials;  // Shader knows all materials
};
class Material {
    void* pRenderShader;               // Material knows its shader
};

// MATERIAL ↔ MESH
class Material {
    std::vector<Mesh*> meshes;         // Material knows all meshes
};
class Mesh {
    void* pMaterial;                   // Mesh knows its material
    void* pShader;                     // Mesh also knows shader (redundancy)
};

// MESH ↔ SURFACE
class Mesh {
    std::vector<Surface*> surfaces;    // Mesh knows all surfaces
};
class Surface {
    void* pShader;                     // Surface knows shader (optional)
};
```

**Why void* pointers?**
- Avoids circular includes
- Flexibility for different implementations
- Cast at runtime

---

## 7. Constant Buffer System

### Matrix Constant Buffer (Mesh)

**In Entity/Mesh:**
```cpp
ID3D11Buffer* constantBuffer;  // Per entity
```

**Data Structure:**
```cpp
struct MatrixSet {
    XMMATRIX worldMatrix;      // Object → World
    XMMATRIX viewMatrix;       // World → Camera
    XMMATRIX projectionMatrix; // Camera → Screen
};
```

**Update Flow:**
```cpp
mesh->Update(device, &matrixSet);
    → Copies MatrixSet to constant buffer
    → Buffer to GPU (UpdateSubresource)
    → Vertex shader can use matrices
```

---

### Material Constant Buffer

**In Material:**
```cpp
ID3D11Buffer* materialBuffer;
```

**Data Structure:**
```cpp
struct MaterialData {
    XMFLOAT4 diffuseColor;
    XMFLOAT4 specularColor;
    float shininess;
    float transparency;
    float padding[2];  // 16-byte alignment
};
```

**Update Flow:**
```cpp
material->UpdateConstantBuffer(context);
    → Copies MaterialData to buffer
    → Buffer to GPU
    → Pixel shader can use material properties
```

---

### Light Constant Buffer

**In Light:**
```cpp
ID3D11Buffer* lightBuffer;
```

**Data Structure:**
```cpp
struct LightBufferData {
    XMFLOAT4 lightPosition;
    XMFLOAT4 lightDirection;
    XMFLOAT4 lightDiffuseColor;
    XMFLOAT4 lightAmbientColor;
};
```

**Update Flow:**
```cpp
lightManager.Update(device);
    → For each light:
        → Updates position/direction
        → Copies LightBufferData to buffer
        → Buffer to GPU
```

---

## 8. Memory Management

### Aligned Memory Allocation

DirectX requires 16-byte alignment for SIMD operations:

```cpp
void* operator new(size_t size) {
    return _aligned_malloc(size, 16);
}
void operator delete(void* p) noexcept {
    _aligned_free(p);
}
```

**Where used:**
- Entity (and all derived classes)
- Material
- All classes with XMMATRIX/XMVECTOR

---

### Resource Management

**GPU Resources:**
- All ID3D11* pointers must be released
- Destructors handle cleanup
- COM objects use reference counting

**Object Management:**
- Manager classes own pointers
- Vectors store raw pointers
- Manual memory management (no shared/unique pointers)

---

## 9. Performance Aspects

### Rendering Optimizations

**Batch-like Rendering:**
```
1 Shader change
    → N Material changes
        → M Mesh updates
            → K Surface draws
```

**Advantages:**
- Minimizes shader changes (expensive)
- Groups material changes (medium cost)
- Mesh updates are cheap (only matrix update)
- Surface draws are pure draw calls

---

### Minimize State Changes

**Most Expensive Operations (avoid):**
1. Shader changes
2. Input layout changes
3. Render target changes

**Medium Cost:**
4. Texture changes
5. Material changes

**Cheap Operations:**
6. Constant buffer updates
7. Draw calls

**Engine Strategy:**
- Outer loop: Shader (few changes)
- Middle loop: Material/Texture (more changes ok)
- Inner loop: Mesh/Draw (many calls ok)

---

### GPU vs CPU Bound

**Current Characteristics (based on tests):**
- With 10,000+ objects: **GPU-bound**
- CPU load remains low thanks to efficient managers
- Bottleneck: Fill rate, vertex processing

**Possible Optimizations:**
- Instancing (same meshes more efficient)
- Frustum culling (skip non-visible objects)
- LOD system (Level of Detail)
- Occlusion culling

---

## 10. Summary: Complete Frame Flow

### Step-by-Step

```
1. MAIN LOOP START
   while (Windows::MainLoop())

2. CLEAR
   Engine::Cls(r, g, b)
   → Clear color buffer
   → Clear depth/stencil buffer

3. UPDATE WORLD
   Engine::UpdateWorld()
   → Camera transform → View/Projection matrix
   → Update light positions
   → Light constant buffers to GPU

4. RENDER WORLD
   Engine::RenderWorld()
   → Device context setup
   → Bind render targets
   → Set viewport
   → RenderManager::RenderScene()
       │
       └─ FOR EACH SHADER:
          ├─ Activate shader (VS, PS, Input Layout)
          │
          └─ FOR EACH MATERIAL:
             ├─ Bind texture
             ├─ Material buffer to GPU
             │
             └─ FOR EACH MESH:
                ├─ Calculate world matrix
                ├─ Matrix buffer to GPU
                │
                └─ FOR EACH SURFACE:
                   ├─ Bind vertex buffers
                   ├─ Bind index buffer
                   └─ DrawIndexed()

5. PRESENT
   Engine::Flip()
   → SwapChain->Present()
   → Back buffer → Front buffer

6. REPEAT
```

---

## 11. Important Design Decisions

### Why This Architecture?

**1. Manager Separation:**
- ObjectManager for geometry
- LightManager for lights
- **Advantage:** Clear responsibilities, easy extension

**2. Hierarchical Rendering:**
- Shader → Material → Mesh → Surface
- **Advantage:** Minimizes state changes, natural grouping

**3. Entity Base for All 3D Objects:**
- Mesh, Camera, Light inherit from Entity
- **Advantage:** Unified transform API, code reuse

**4. Friend Class RenderManager:**
- Direct access to manager vectors
- **Advantage:** Efficient rendering without getter overhead

**5. Pointer-based Relationships:**
- Materials → Meshes, Shader → Materials
- **Advantage:** Fast traversal, flexible relationships

---

## Glossary

**Entity** - Base class for all 3D objects in space  
**Mesh** - Renderable 3D object with geometry  
**Surface** - Container for vertex/index data  
**Material** - Surface properties + texture  
**Shader** - HLSL Vertex/Pixel shader wrapper  
**Camera** - Viewpoint with view/projection matrix  
**Light** - Light source (Directional/Point/Spot)  
**Constant Buffer** - GPU buffer for shader parameters  
**MatrixSet** - World/View/Projection matrices  
**Transform** - Position/Rotation/Scale (quaternion-based)  
**State Change** - Change of GPU state (expensive)  
**Draw Call** - GPU command to render geometry
