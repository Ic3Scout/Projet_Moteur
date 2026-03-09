#ifndef TEXTURE2D_H_INCLUDED
#define TEXTURE2D_H_INCLUDED

#include <cstdint>
#include <vector>
#include <d3d12.h>
#include "Render/DescriptorHeapManager.h"

class UploadContext;

class Texture2D
{
public:
    Texture2D() = default;
    ~Texture2D() { Release(); }

    bool LoadFromFileWIC(ID3D12Device* device, UploadContext* uploader, DescriptorHeapManager* srvHeap, const wchar_t* filename, bool srgb);

    void Release();

    const DescriptorHandle& GetSrv() const { return m_srv; }
    ID3D12Resource* GetResource() const { return m_texture; }
    uint32_t Width() const { return m_width; }
    uint32_t Height() const { return m_height; }

private:
    static bool LoadWIC_RGBA8(const wchar_t* filename, std::vector<uint8_t>& outRGBA, uint32_t& outW, uint32_t& outH);

private:
    ID3D12Resource* m_texture = nullptr;
    ID3D12Resource* m_upload = nullptr; // temp upload (freed after upload)

    DescriptorHandle m_srv{};

    uint32_t m_width = 0;
    uint32_t m_height = 0;
};

#endif // !TEXTURE2D_H_INCLUDED