#ifndef RESOURCE_MANAGER_H_INCLUDED
#define RESOURCE_MANAGER_H_INCLUDED

#include <unordered_map>
#include <memory>
#include <string>

enum class MeshPreset : uint32_t
{
    Cube = 1,
    Cylinder = 2,
    Sphere = 3,
};

struct MeshHandle
{
    uint32_t id = 0;
    explicit operator bool() const { return id != 0; }
};

struct TextureHandle
{
    uint32_t id = 0;
    explicit operator bool() const { return id != 0; }
};

struct TextureDesc
{
    std::wstring path;
    bool srgb = true; // albedo typically true; normal maps typically false.
};

// Engine resource manager:
// - owns *names/paths/metadata*
// - returns stable handles
// - does NOT create GPU resources
class ResourceManager
{
public:
    static ResourceManager& Instance();

    // Built-in meshes
    MeshHandle GetMeshPreset(MeshPreset mesh);

    // Textures by file path
    TextureHandle LoadTexture(const std::wstring& path, bool srgb = true);
    const TextureDesc* GetTextureDesc(TextureHandle h) const;

private:
    ResourceManager() = default;

    uint32_t m_nextTextureId = 1;
    std::unordered_map<std::wstring, TextureHandle> m_textureByPath;
    std::unordered_map<uint32_t, TextureDesc> m_textureDescs;
};


#endif // !RESOURCE_MANAGER_H_INCLUDED