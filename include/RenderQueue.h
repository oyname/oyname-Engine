#pragma once
#include <vector>
#include <unordered_map>

class Shader;
class Material;
class Mesh;
class Surface;

struct DrawEntry { Mesh* mesh; Surface* surface; };

struct MaterialBatch {
    Material* material = nullptr;
    std::vector<DrawEntry> draws;
};

struct ShaderBatch {
    Shader* shader = nullptr;
    int flagsVertex = 0;
    std::vector<MaterialBatch> materials;
    std::unordered_map<Material*, size_t> matIndex;
};

struct RenderQueue {
    std::vector<ShaderBatch> shaders;
    std::unordered_map<Shader*, size_t> shIndex;
};

