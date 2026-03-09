#ifndef RESOURCE_MANAGER_CPP_INCLUDED
#define RESOURCE_MANAGER_CPP_INCLUDED

#include "ResourceManager.h"

static ResourceManager* s_instance = nullptr;

ResourceManager& ResourceManager::Instance()
{
    if (s_instance == nullptr)
        s_instance = new ResourceManager();
    return *s_instance;
}

MeshHandle ResourceManager::GetMeshPreset(MeshPreset mesh)
{
    return MeshHandle{ static_cast<uint32_t>(mesh) };
}

TextureHandle ResourceManager::LoadTexture(const std::wstring& path, bool srgb)
{
    auto it = m_textureByPath.find(path);
    if (it != m_textureByPath.end())
        return it->second;

    TextureHandle h{ m_nextTextureId++ };
    m_textureByPath.emplace(path, h);
    m_textureDescs.emplace(h.id, TextureDesc{ path, srgb });
    return h;
}

const TextureDesc* ResourceManager::GetTextureDesc(TextureHandle h) const
{
    auto it = m_textureDescs.find(h.id);
    if (it == m_textureDescs.end())
        return nullptr;
    return &it->second;
}

#endif // !RESOURCE_MANAGER_CPP_INCLUDED