#ifndef TEXTURE2D_CPP_INCLUDED
#define TEXTURE2D_CPP_INCLUDED

#include "Render/Texture2D.h"
#include "Render/UploadContext.h"

#include <wincodec.h>
#include <cstring>

#pragma comment(lib, "windowscodecs.lib")

bool Texture2D::LoadWIC_RGBA8(const wchar_t* filename, std::vector<uint8_t>& outRGBA, uint32_t& outW, uint32_t& outH)
{
    outRGBA.clear(); outW = outH = 0;

    IWICImagingFactory* factory = nullptr;
    IWICBitmapDecoder* decoder = nullptr;
    IWICBitmapFrameDecode* frame = nullptr;
    IWICFormatConverter* converter = nullptr;

    HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    (void)hr; // ok if already initialized

    hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&factory));
    if (FAILED(hr)) return false;

    hr = factory->CreateDecoderFromFilename(filename, nullptr, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &decoder);
    if (FAILED(hr))
    { 
        factory->Release();
        return false; 
    }

    hr = decoder->GetFrame(0, &frame);
    if (FAILED(hr))
    { 
        decoder->Release();
        factory->Release(); 
        return false;
    }

    UINT w = 0, h = 0;
    frame->GetSize(&w, &h);
    outW = (uint32_t)w;
    outH = (uint32_t)h;

    hr = factory->CreateFormatConverter(&converter);
    if (FAILED(hr))
    { 
        frame->Release(); 
        decoder->Release();
        factory->Release(); 
        return false; 
    }

    hr = converter->Initialize(frame, GUID_WICPixelFormat32bppRGBA,
        WICBitmapDitherTypeNone, nullptr, 0.0, WICBitmapPaletteTypeCustom);

    if (FAILED(hr))
    { 
        converter->Release();
        frame->Release(); 
        decoder->Release(); 
        factory->Release(); 
        return false;
    }

    outRGBA.resize((size_t)outW * (size_t)outH * 4);
    hr = converter->CopyPixels(nullptr, outW * 4, (UINT)outRGBA.size(), outRGBA.data());

    converter->Release();
    frame->Release();
    decoder->Release();
    factory->Release();

    return SUCCEEDED(hr);
}

bool Texture2D::LoadFromFileWIC(ID3D12Device* device, UploadContext* uploader, DescriptorHeapManager* srvHeap, const wchar_t* filename, bool srgb)
{
    if (device == nullptr) return false;

    std::vector<uint8_t> rgba;
    uint32_t w = 0, h = 0;
    if (LoadWIC_RGBA8(filename, rgba, w, h) == false)
        return false;

    Release();
    m_width = w;
    m_height = h;

	DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
    if (srgb)
        format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    else
		format = DXGI_FORMAT_R8G8B8A8_UNORM;

    D3D12_RESOURCE_DESC texDesc{};
    texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    texDesc.Width = w;
    texDesc.Height = h;
    texDesc.DepthOrArraySize = 1;
    texDesc.MipLevels = 1;
    texDesc.Format = format;
    texDesc.SampleDesc.Count = 1;
    texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

    D3D12_HEAP_PROPERTIES defaultHeap{};
    defaultHeap.Type = D3D12_HEAP_TYPE_DEFAULT;

    HRESULT hr = device->CreateCommittedResource(
        &defaultHeap, D3D12_HEAP_FLAG_NONE, &texDesc,
        D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_texture));

    if (FAILED(hr)) 
        return false;

    UINT64 uploadSize = 0;
    D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint{};
    UINT numRows = 0;
    UINT64 rowSizeInBytes = 0;
    device->GetCopyableFootprints(&texDesc, 0, 1, 0, &footprint, &numRows, &rowSizeInBytes, &uploadSize);

    D3D12_RESOURCE_DESC bufDesc{};
    bufDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    bufDesc.Width = uploadSize;
    bufDesc.Height = 1;
    bufDesc.DepthOrArraySize = 1;
    bufDesc.MipLevels = 1;
    bufDesc.SampleDesc.Count = 1;
    bufDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    D3D12_HEAP_PROPERTIES uploadHeap{};
    uploadHeap.Type = D3D12_HEAP_TYPE_UPLOAD;

    hr = device->CreateCommittedResource(
        &uploadHeap, D3D12_HEAP_FLAG_NONE, &bufDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_upload));

    if (FAILED(hr))
        return false;

    uint8_t* mapped = nullptr;
    D3D12_RANGE r{ 0,0 };
    hr = m_upload->Map(0, &r, (void**)&mapped);

    if (FAILED(hr))
        return false;

    const uint32_t srcRowPitch = w * 4;
    uint8_t* dst = mapped + footprint.Offset;
    for (uint32_t y = 0; y < h; ++y)
        std::memcpy(dst + y * footprint.Footprint.RowPitch, rgba.data() + y * srcRowPitch, srcRowPitch);

    m_upload->Unmap(0, nullptr);

    uploader->Begin();
    ID3D12GraphicsCommandList* cmd = uploader->GetCommandList();

    D3D12_TEXTURE_COPY_LOCATION dstLoc{};
    dstLoc.pResource = m_texture;
    dstLoc.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    dstLoc.SubresourceIndex = 0;

    D3D12_TEXTURE_COPY_LOCATION srcLoc{};
    srcLoc.pResource = m_upload;
    srcLoc.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
    srcLoc.PlacedFootprint = footprint;

    cmd->CopyTextureRegion(&dstLoc, 0, 0, 0, &srcLoc, nullptr);
    uploader->Transition(m_texture, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    uploader->EndAndWait();

    m_srv = srvHeap->Allocate();
    if (m_srv.cpu.ptr == 0 || m_srv.gpu.ptr == 0)
        return false;

    D3D12_SHADER_RESOURCE_VIEW_DESC srv{};
    srv.Format = format;
    srv.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srv.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srv.Texture2D.MipLevels = 1;
    device->CreateShaderResourceView(m_texture, &srv, m_srv.cpu);

    SafeRelease(m_upload);

    return true;
}

void Texture2D::Release()
{
    SafeRelease(m_texture);
    SafeRelease(m_upload);
    m_srv = {};
    m_width = 0;
    m_height = 0;
}

#endif // !TEXTURE2D_CPP_INCLUDED