#ifndef UPLOAD_CONTEXT_CPP_INCLUDED
#define UPLOAD_CONTEXT_CPP_INCLUDED

#include "UploadContext.h"

bool UploadContext::Initialize(ID3D12Device* device, ID3D12CommandQueue* queue)
{
    if (device == nullptr || queue == nullptr)
        return false;

    Shutdown();

    m_device = device;
    m_queue = queue;

    // Allocator + cmd list
    HRESULT hr = m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_alloc));
    if (FAILED(hr)) return false;

    hr = m_device->CreateCommandList(
        0,
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        m_alloc,
        nullptr,
        IID_PPV_ARGS(&m_cmdList));
    if (FAILED(hr)) return false;

    // on la veut "closed" au repos
    m_cmdList->Close();

    // Fence + event
    hr = m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));
    if (FAILED(hr)) 
        return false;

    m_fenceValue = 0;
    m_fenceEvent = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);
    if (m_fenceEvent == nullptr) return false;

    m_recording = false;
    return true;
}

void UploadContext::Shutdown()
{
    if (m_fenceEvent)
    {
        CloseHandle(m_fenceEvent);
        m_fenceEvent = nullptr;
    }

    SafeRelease(m_fence);
    SafeRelease(m_cmdList);
    SafeRelease(m_alloc);

    m_device = nullptr;
    m_queue = nullptr;

    m_fenceValue = 0;
    m_recording = false;
}

void UploadContext::Begin()
{
    if (m_device == nullptr || m_queue == nullptr)
        throw std::runtime_error("UploadContext::Begin: not initialized");

    if (m_recording)
        throw std::runtime_error("UploadContext::Begin: already recording");

	HRESULT hr = m_alloc->Reset();

    if (FAILED(hr))
		throw std::runtime_error("UploadContext: allocator reset failed");

    hr = m_cmdList->Reset(m_alloc, nullptr);

    if (FAILED(hr))
        throw std::runtime_error("UploadContext: cmd list reset failed");

    m_recording = true;
}

void UploadContext::EndAndWait()
{
    if (m_recording == false)
        throw std::runtime_error("UploadContext::EndAndWait: Begin() not called");

	HRESULT hr = m_cmdList->Close();

	if (FAILED(hr))
        throw std::runtime_error("UploadContext: Close failed");

    ID3D12CommandList* lists[] = { m_cmdList };
    m_queue->ExecuteCommandLists(1, lists);

    // Signal and wait
    ++m_fenceValue;
	hr = m_queue->Signal(m_fence, m_fenceValue);

    if (FAILED(hr))
		throw std::runtime_error("UploadContext: Signal failed");

    if (m_fence->GetCompletedValue() < m_fenceValue)
    {
        hr = m_fence->SetEventOnCompletion(m_fenceValue, m_fenceEvent);

        if (FAILED(hr))
			throw std::runtime_error("UploadContext: SetEventOnCompletion failed");

        WaitForSingleObject(m_fenceEvent, INFINITE);
    }

    m_recording = false;
}

void UploadContext::Transition(ID3D12Resource* res,
    D3D12_RESOURCE_STATES before,
    D3D12_RESOURCE_STATES after)
{
    if (m_recording == false)
        throw std::runtime_error("UploadContext::Transition: Begin() not called");

    D3D12_RESOURCE_BARRIER b = {};
    b.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    b.Transition.pResource = res;
    b.Transition.StateBefore = before;
    b.Transition.StateAfter = after;
    b.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

    m_cmdList->ResourceBarrier(1, &b);
}

bool UploadContext::UploadBuffer(
    const void* srcData,
    uint64_t numBytes,
    ID3D12Resource*& outDefaultBuffer,
    ID3D12Resource*& outUploadBuffer,
    D3D12_RESOURCE_STATES finalState)
{
    if (m_recording == false)
        throw std::runtime_error("UploadContext::UploadBuffer: Begin() not called");

    if (srcData == nullptr || numBytes == 0)
        return false;

    if (outDefaultBuffer) { SafeRelease(outDefaultBuffer); }
    if (outUploadBuffer) { SafeRelease(outUploadBuffer); }

    // Default heap (GPU)
    D3D12_HEAP_PROPERTIES defaultHeap = {};
    defaultHeap.Type = D3D12_HEAP_TYPE_DEFAULT;
    defaultHeap.CreationNodeMask = 1;
    defaultHeap.VisibleNodeMask = 1;

    D3D12_RESOURCE_DESC bufDesc = {};
    bufDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    bufDesc.Width = numBytes;
    bufDesc.Height = 1;
    bufDesc.DepthOrArraySize = 1;
    bufDesc.MipLevels = 1;
    bufDesc.Format = DXGI_FORMAT_UNKNOWN;
    bufDesc.SampleDesc.Count = 1;
    bufDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    HRESULT hr = m_device->CreateCommittedResource(
        &defaultHeap,
        D3D12_HEAP_FLAG_NONE,
        &bufDesc,
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
		IID_PPV_ARGS(&outDefaultBuffer));

	if (FAILED(hr))
		throw std::runtime_error("UploadContext: CreateCommittedResource DEFAULT failed");

    // Upload heap (CPU visible)
    D3D12_HEAP_PROPERTIES uploadHeap = {};
    uploadHeap.Type = D3D12_HEAP_TYPE_UPLOAD;
    uploadHeap.CreationNodeMask = 1;
    uploadHeap.VisibleNodeMask = 1;

    hr = m_device->CreateCommittedResource(
        &uploadHeap,
        D3D12_HEAP_FLAG_NONE,
        &bufDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&outUploadBuffer));

    if (FAILED(hr))
        throw std::runtime_error("UploadContext: CreateCommittedResource UPLOAD failed");

    // Copy data into upload buffer
    void* mapped = nullptr;
    D3D12_RANGE range = { 0, 0 }; // no CPU reads
    hr = outUploadBuffer->Map(0, &range, &mapped);

    if (FAILED(hr))
        throw std::runtime_error("UploadContext: Map upload failed");

    std::memcpy(mapped, srcData, static_cast<size_t>(numBytes));
    outUploadBuffer->Unmap(0, nullptr);

    // GPU copy upload -> default
    m_cmdList->CopyBufferRegion(outDefaultBuffer, 0, outUploadBuffer, 0, numBytes);

    // Transition to final state
    Transition(outDefaultBuffer, D3D12_RESOURCE_STATE_COPY_DEST, finalState);

    return true;
}

#endif // !UPLOAD_CONTEXT_CPP_INCLUDED