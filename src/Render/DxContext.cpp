#ifndef DXCONTEXT_CPP_INCLUDED
#define DXCONTEXT_CPP_INCLUDED

#include "DxContext.h"

DxContext::DxContext()
{
}

DxContext::~DxContext()
{
}

bool DxContext::CreateDevice()
{
    HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(&m_dxgiFactory));

    // Try hardware device first.
    hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_d3dDevice));
    if (FAILED(hr))
    {
        // Fallback to WARP.
        IDXGIAdapter* warpAdapter = nullptr;
        if (FAILED(m_dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter))))
            return false;

        hr = D3D12CreateDevice(warpAdapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_d3dDevice));
        warpAdapter->Release();
        if (FAILED(hr))
            return false;
    }

    // Fence.
    hr = m_d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));
    if (FAILED(hr))
        throw std::runtime_error("Fence creation failed.");

    m_rtvDescriptorSize = m_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    m_dsvDescriptorSize = m_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    m_cbvSrvUavDescriptorSize = m_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    return true;
}

bool DxContext::CreateCommandObjects()
{
    assert(m_d3dDevice);

    // Command queue.
    D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};
    cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

    HRESULT hr = m_d3dDevice->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&m_commandQueue));
    if (FAILED(hr))
        throw std::runtime_error("Command queue creation failed.");

    // Command allocator.
    hr = m_d3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_directCmdListAlloc));
    if (FAILED(hr))
        throw std::runtime_error("Command allocator creation failed.");

    // Command list.
    hr = m_d3dDevice->CreateCommandList(
        0,
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        m_directCmdListAlloc,
        nullptr,
        IID_PPV_ARGS(&m_commandList));
    if (FAILED(hr))
        throw std::runtime_error("Command list creation failed.");

    // Start closed.
    m_commandList->Close();

    return true;
}

void DxContext::WaitForGpu()
{
    if (m_commandQueue == nullptr || m_fence == nullptr)
        return;

    ++m_currentFence;
    HRESULT hr = m_commandQueue->Signal(m_fence, m_currentFence);
    if (FAILED(hr))
        throw std::runtime_error("Command queue signal failed.");

    if (m_fence->GetCompletedValue() < m_currentFence)
    {
        HANDLE eventHandle = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);
        if (!eventHandle)
            throw std::runtime_error("CreateEventEx failed.");

        HRESULT hr = m_fence->SetEventOnCompletion(m_currentFence, eventHandle);
        if (FAILED(hr))
        {
            CloseHandle(eventHandle);
            throw std::runtime_error("SetEventOnCompletion failed.");
        }

        WaitForSingleObject(eventHandle, INFINITE);
        CloseHandle(eventHandle);
    }
}

void DxContext::FlushCommandQueue()
{
    m_currentFence++;
    HRESULT hr = m_commandQueue->Signal(m_fence, m_currentFence);
    if (FAILED(hr))
        throw std::runtime_error("Command queue signal failed.");

    // Wait until the GPU has completed commands up to this fence point.
    if (m_fence->GetCompletedValue() < m_currentFence)
    {
        HANDLE eventHandle = CreateEventEx(nullptr, nullptr, false, EVENT_ALL_ACCESS);

        if (eventHandle == nullptr)
        {
            // Gérer l'erreur de création d'événement (par exemple, lancer une exception ou retourner)
            throw std::runtime_error("Échec de la création de l'événement pour la synchronisation GPU.");
        }

        // Fire event when GPU hits current fence.  
        hr = m_fence->SetEventOnCompletion(m_currentFence, eventHandle);
        if (FAILED(hr))
        {
            CloseHandle(eventHandle);
            throw std::runtime_error("Échec de SetEventOnCompletion pour la synchronisation GPU.");
        }

        // Wait until the GPU hits current fence event is fired.
        WaitForSingleObject(eventHandle, INFINITE);
        CloseHandle(eventHandle);
    }
}

void DxContext::Shutdown()
{
    SafeRelease(m_commandList);
    SafeRelease(m_directCmdListAlloc);
    SafeRelease(m_commandQueue);
    SafeRelease(m_fence);
    SafeRelease(m_d3dDevice);
	SafeRelease(m_dxgiFactory);
}

#endif // !DXCONTEXT_CPP_INCLUDED