#ifndef LIGHTS_H_INCLUDED
#define LIGHTS_H_INCLUDED

#include "Helpers/d3dUtil.h"

using namespace DirectX;

class Entity;
class LightSystem;

#include "DescriptorHeapManager.h"

struct PointLightCB
{
    XMFLOAT3 pos;
    float range;
    XMFLOAT3 color;
    float intensity;
};

struct FrameCB
{
    XMFLOAT4X4 viewProj;
    XMFLOAT3 cameraPos;
    uint32_t pad0;

    PointLightCB light0;
    PointLightCB light1;
};

struct LightGPU
{
    XMFLOAT3 position;
    XMFLOAT3 color;    
    XMFLOAT3 direction; 
    float range;
    float intensity;
    float spotCosAngle;
    uint32_t type;
    uint32_t pad0, pad1, pad2;
};

class LightRender
{
public:
    bool Initialize(ID3D12Device* device, DescriptorHeapManager* heap, uint32_t backBufferCount, uint32_t maxLights);
    void Shutdown();

    void Update(uint32_t backBufferIndex, const std::vector<LightGPU>& lights);

    void UpdateFrame(uint32_t backBufferIndex, const FrameCB& frame);
    D3D12_GPU_VIRTUAL_ADDRESS GetFrameCbAddress(uint32_t backBufferIndex) const;

    void Bind(ID3D12GraphicsCommandList* cmd, uint32_t backBufferIndex, UINT rootFrameParam /*CBV b1*/, UINT rootLightsParam /*t1*/) const;

    uint32_t GetLightCount() const { return m_lightCount; }
    D3D12_GPU_DESCRIPTOR_HANDLE GetLightsSrv(uint32_t backBufferIndex) const;

private:
    static inline uint32_t Align256(uint32_t x) { return (x + 255u) & ~255u; }

    ID3D12Device* m_device = nullptr;
    DescriptorHeapManager* m_heap = nullptr;

    uint32_t m_backBufferCount = 0;
    uint32_t m_maxLights = 0;
    uint32_t m_lightCount = 0;

    ID3D12Resource* m_lightsUpload = nullptr;
    uint8_t* m_lightsMapped = nullptr;

    std::vector<DescriptorHandle> m_lightsSrv;

    ID3D12Resource* m_frameUpload = nullptr;
    uint8_t* m_frameMapped = nullptr;
    uint32_t m_frameStride = 0;
};

#endif // !LIGHTS_H_INCLUDED