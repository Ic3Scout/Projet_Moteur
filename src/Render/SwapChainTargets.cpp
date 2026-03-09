#ifndef SWAP_CHAIN_TARGETS_CPP_INCLUDED
#define SWAP_CHAIN_TARGETS_CPP_INCLUDED

#include "SwapChainTargets.h"

#include "Renderer.h"
#include "DxContext.h" 
#include "Window.h"

SwapChainTargets::SwapChainTargets(Renderer* pRenderer, uint32_t width, uint32_t height)
{
	m_pRenderer = pRenderer;

    m_screenViewport.TopLeftX = 0.0f;
    m_screenViewport.TopLeftY = 0.0f;
    m_screenViewport.Width = static_cast<float>(width);
    m_screenViewport.Height = static_cast<float>(height);
    m_screenViewport.MinDepth = 0.0f;
    m_screenViewport.MaxDepth = 1.0f;

    m_scissorRect.left = 0;
    m_scissorRect.top = 0;
    m_scissorRect.right = static_cast<LONG>(width);
    m_scissorRect.bottom = static_cast<LONG>(height);

    for (uint8_t i = 0; i < SwapChainBufferCount; ++i)
    {
        m_swapChainBuffer[i] = nullptr;
    }
}

SwapChainTargets::~SwapChainTargets()
{
}

bool SwapChainTargets::CreateSwapChain(HWND hwnd, uint32_t width, uint32_t height)
{
	DxContext* pDxContext = m_pRenderer->GetDxContext();
	IDXGIFactory4* pFactory = pDxContext->GetFactory();
    ID3D12CommandQueue* pCommandQueue = pDxContext->GetCommandQueue();

    assert(pFactory);
    assert(pCommandQueue);

    // Release previous swap chain if any.
    SafeRelease(m_swapChain);

    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
    swapChainDesc.BufferDesc.Width = width;
    swapChainDesc.BufferDesc.Height = height;
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDesc.BufferDesc.Format = m_backBufferFormat;
    swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = SwapChainBufferCount;
    swapChainDesc.OutputWindow = hwnd;
    swapChainDesc.Windowed = TRUE;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    HRESULT hr = pFactory->CreateSwapChain(
        pCommandQueue,
        &swapChainDesc,
        &m_swapChain);
    if (FAILED(hr))
        throw std::runtime_error("Swap chain creation failed.");

    return true;
}

bool SwapChainTargets::CreateRtvDsvHeaps()
{
	ID3D12Device* pDevice = m_pRenderer->GetDxContext()->GetDevice();
    assert(pDevice);

    // RTV heap.
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
    rtvHeapDesc.NumDescriptors = SwapChainBufferCount;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

    HRESULT hr = pDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap));
	if (FAILED(hr))
		throw std::runtime_error("RTV heap creation failed.");

    // DSV heap.
    D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
    dsvHeapDesc.NumDescriptors = 1;
    dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

    hr = pDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_dsvHeap));
    if (FAILED(hr))
		throw std::runtime_error("DSV heap creation failed.");

    return true;
}

void SwapChainTargets::CreateRenderTargets()
{
	ID3D12Device* pDevice = m_pRenderer->GetDxContext()->GetDevice();
    assert(m_swapChain);
    assert(pDevice);
    assert(m_rtvHeap);

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());
    for (UINT i = 0; i < SwapChainBufferCount; i++)
    {
        SafeRelease(m_swapChainBuffer[i]);
        HRESULT hr = m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_swapChainBuffer[i]));
        if (FAILED(hr))
            throw std::runtime_error("Swap chain buffer retrieval failed.");

        pDevice->CreateRenderTargetView(m_swapChainBuffer[i], nullptr, rtvHeapHandle);
        rtvHeapHandle.Offset(1, m_pRenderer->GetDxContext()->GetRtvDescriptorSize());
    }
}

void SwapChainTargets::CreateDepthStencil(uint32_t width, uint32_t height)
{
    ID3D12Device* pDevice = m_pRenderer->GetDxContext()->GetDevice();
	ID3D12CommandAllocator* pDirectCmdListAlloc = m_pRenderer->GetDxContext()->GetCommandAllocator();
	ID3D12GraphicsCommandList* pCommandList = m_pRenderer->GetDxContext()->GetCommandList();
    ID3D12CommandQueue* pCommandQueue = m_pRenderer->GetDxContext()->GetCommandQueue();

    assert(pDevice);
    assert(m_dsvHeap);

    SafeRelease(m_depthStencilBuffer);

    D3D12_RESOURCE_DESC depthDesc = {};
    depthDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    depthDesc.Width = width;
    depthDesc.Height = height;
    depthDesc.DepthOrArraySize = 1;
    depthDesc.MipLevels = 1;
    depthDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
    depthDesc.SampleDesc.Count = 1;
    depthDesc.SampleDesc.Quality = 0;
    depthDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    depthDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_CLEAR_VALUE optClear = {};
    optClear.Format = m_depthStencilFormat;
    optClear.DepthStencil.Depth = 1.f;
    optClear.DepthStencil.Stencil = 0;

    D3D12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);

    HRESULT hr = pDevice->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &depthDesc,
        D3D12_RESOURCE_STATE_COMMON,
        &optClear,
        IID_PPV_ARGS(&m_depthStencilBuffer));
    if (FAILED(hr))
        throw std::runtime_error("Depth stencil buffer creation failed.");

    // Create DSV.
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Format = m_depthStencilFormat;
    dsvDesc.Texture2D.MipSlice = 0;

    pDevice->CreateDepthStencilView(
        m_depthStencilBuffer,
        &dsvDesc,
        m_dsvHeap->GetCPUDescriptorHandleForHeapStart());

    hr = pDirectCmdListAlloc->Reset();
    if (FAILED(hr))
        throw std::runtime_error("Command allocator reset failed.");

    hr = pCommandList->Reset(pDirectCmdListAlloc, nullptr);
    if (FAILED(hr))
        throw std::runtime_error("Command list reset failed.");

    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource = m_depthStencilBuffer;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_DEPTH_WRITE;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    pCommandList->ResourceBarrier(1, &barrier);

    pCommandList->Close();
    ID3D12CommandList* lists[] = { pCommandList };
    pCommandQueue->ExecuteCommandLists(1, lists);
    m_pRenderer->GetDxContext()->WaitForGpu();
}

void SwapChainTargets::Resize(uint32_t width, uint32_t height)
{
	ID3D12Device* pDevice = m_pRenderer->GetDxContext()->GetDevice();
	ID3D12CommandAllocator* pDirectCmdListAlloc = m_pRenderer->GetDxContext()->GetCommandAllocator();
	ID3D12GraphicsCommandList* pCommandList = m_pRenderer->GetDxContext()->GetCommandList();
	Window* m_pWindow = m_pRenderer->GetWindow();

    if (pDevice == nullptr || m_swapChain == nullptr || pDirectCmdListAlloc == nullptr)
        return;

    // Si minimizé, ne pas redimensionner.
    if (m_pWindow->GetWidth() == 0 || m_pWindow->GetHeight() == 0)
        return;

    // Flush before changing any resources.
    m_pRenderer->GetDxContext()->FlushCommandQueue();

    width = m_pWindow->GetWidth();
    height = m_pWindow->GetHeight();

	m_pRenderer->GetDxContext()->WaitForGpu();

    // Release old buffers.
    for (int i = 0; i < SwapChainBufferCount; ++i)
    {
        SafeRelease(m_swapChainBuffer[i]);
    }

    SafeRelease(m_depthStencilBuffer);

    // Resize swap chain.
    HRESULT hr = m_swapChain->ResizeBuffers(
        SwapChainBufferCount,
        width,
        height,
        m_backBufferFormat,
        DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
    if (FAILED(hr))
        throw std::runtime_error("Swap chain resize failed.");

    m_currBackBuffer = 0;

    CreateRenderTargets();
    CreateDepthStencil(width, height);

    m_screenViewport.TopLeftX = 0.0f;
    m_screenViewport.TopLeftY = 0.0f;
    m_screenViewport.Width = static_cast<float>(width);
    m_screenViewport.Height = static_cast<float>(height);
    m_screenViewport.MinDepth = 0.0f;
    m_screenViewport.MaxDepth = 1.0f;

    m_scissorRect.left = 0;
    m_scissorRect.top = 0;
    m_scissorRect.right = static_cast<LONG>(width);
    m_scissorRect.bottom = static_cast<LONG>(height);

    m_pWindow->SetResizing(false);
}

void SwapChainTargets::UpdateCurrentBackBuffer()
{
    m_currBackBuffer = (m_currBackBuffer + 1) % SwapChainBufferCount;
}

void SwapChainTargets::Shutdown()
{
    SafeRelease(m_depthStencilBuffer);
    for (int i = 0; i < SwapChainBufferCount; ++i)
        SafeRelease(m_swapChainBuffer[i]);

    SafeRelease(m_rtvHeap);
    SafeRelease(m_dsvHeap);
    SafeRelease(m_swapChain);
}
#endif // !SWAP_CHAIN_TARGETS_CPP_INCLUDED