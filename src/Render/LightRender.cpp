#ifndef LIGHTRENDER_CPP_INCLUDED
#define LIGHTRENDER_CPP_INCLUDED

#include "LightRender.h"

bool LightRender::Initialize(ID3D12Device* device, DescriptorHeapManager* heap, uint32_t backBufferCount, uint32_t maxLights)
{
	Shutdown();

    m_device = device;
    m_heap = heap;
    m_backBufferCount = backBufferCount;
    m_maxLights = maxLights;

    // --------------------------
    // Lights Upload Buffer
    // --------------------------
    D3D12_HEAP_PROPERTIES heapProps{};
    heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

    uint64_t totalLightBytes = uint64_t(sizeof(LightGPU)) * uint64_t(m_maxLights) * uint64_t(m_backBufferCount);

    D3D12_RESOURCE_DESC lightDesc{};
    lightDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    lightDesc.Width = totalLightBytes;
    lightDesc.Height = 1;
    lightDesc.DepthOrArraySize = 1;
    lightDesc.MipLevels = 1;
    lightDesc.SampleDesc.Count = 1;
    lightDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    HRESULT hr = device->CreateCommittedResource(
        &heapProps, D3D12_HEAP_FLAG_NONE, &lightDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
        IID_PPV_ARGS(&m_lightsUpload));

    if (FAILED(hr)) return false;

    m_lightsUpload->Map(0, nullptr, reinterpret_cast<void**>(&m_lightsMapped));

    // 1 SRV par backbuffer
    m_lightsSrv.resize(backBufferCount);

    for (uint32_t bb = 0; bb < backBufferCount; ++bb)
    {
        DescriptorHandle h = m_heap->Allocate();
        m_lightsSrv[bb] = h;

        D3D12_SHADER_RESOURCE_VIEW_DESC srv{};
        srv.Format = DXGI_FORMAT_UNKNOWN;
        srv.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
        srv.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srv.Buffer.FirstElement = uint64_t(bb) * uint64_t(maxLights);
        srv.Buffer.NumElements = maxLights;
        srv.Buffer.StructureByteStride = sizeof(LightGPU);
        srv.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

        device->CreateShaderResourceView(m_lightsUpload, &srv, h.cpu);
    }

    // --------------------------
    // FrameCB Upload Buffer (b1)
    // --------------------------
    m_frameStride = Align256((uint32_t)sizeof(FrameCB));
    uint64_t totalFrameBytes = uint64_t(m_frameStride) * uint64_t(m_backBufferCount);

    D3D12_RESOURCE_DESC frameDesc{};
    frameDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    frameDesc.Width = totalFrameBytes;
    frameDesc.Height = 1;
    frameDesc.DepthOrArraySize = 1;
    frameDesc.MipLevels = 1;
    frameDesc.SampleDesc.Count = 1;
    frameDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    hr = device->CreateCommittedResource(
        &heapProps, D3D12_HEAP_FLAG_NONE, &frameDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
        IID_PPV_ARGS(&m_frameUpload));

    if (FAILED(hr)) return false;

    m_frameUpload->Map(0, nullptr, reinterpret_cast<void**>(&m_frameMapped));

    return true;
}

void LightRender::Shutdown()
{
    if (m_lightsUpload)
    {
        m_lightsUpload->Unmap(0, nullptr);
        m_lightsMapped = nullptr;
    }
    if (m_frameUpload)
    {
        m_frameUpload->Unmap(0, nullptr);
        m_frameMapped = nullptr;
    }

    SafeRelease(m_lightsUpload);
    SafeRelease(m_frameUpload);

    for (DescriptorHandle& h : m_lightsSrv)
        if (h.IsValid()) m_heap->Free(h.index);

    m_lightsSrv.clear();
    m_device = nullptr;
    m_heap = nullptr;

    m_lightCount = 0;
}

void LightRender::Update(uint32_t bb, const std::vector<LightGPU>& lights)
{
    if (!m_lightsMapped) return;

    const uint32_t count = (uint32_t)std::min<size_t>(lights.size(), m_maxLights);
    m_lightCount = count;

    const uint64_t segmentBytes = uint64_t(sizeof(LightGPU)) * m_maxLights;
    uint8_t* dst = m_lightsMapped + segmentBytes * bb;

    if (count > 0)
        std::memcpy(dst, lights.data(), sizeof(LightGPU) * count);
}

void LightRender::UpdateFrame(uint32_t bb, const FrameCB& frame)
{
    if (!m_frameMapped) return;
    std::memcpy(m_frameMapped + uint64_t(bb) * m_frameStride, &frame, sizeof(FrameCB));
}

D3D12_GPU_VIRTUAL_ADDRESS LightRender::GetFrameCbAddress(uint32_t bb) const
{
    return m_frameUpload->GetGPUVirtualAddress() + uint64_t(bb) * m_frameStride;
}

void LightRender::Bind(ID3D12GraphicsCommandList* cmd, uint32_t bb, UINT rootFrameParam, UINT rootLightsParam) const
{
    auto address = GetFrameCbAddress(bb);
    assert((address & 255ull) == 0);
    cmd->SetGraphicsRootConstantBufferView(rootFrameParam, address);
   // cmd->SetGraphicsRootDescriptorTable(rootLightsParam, m_lightsSrv[bb].gpu);
}

D3D12_GPU_DESCRIPTOR_HANDLE LightRender::GetLightsSrv(uint32_t bb) const
{
    return m_lightsSrv[bb].gpu;
}

#endif // !LIGHTS_CPP_INCLUDED