#ifndef DESCRIPTOR_HEAP_MANAGER_CPP_INCLUDED
#define DESCRIPTOR_HEAP_MANAGER_CPP_INCLUDED

#include "DescriptorHeapManager.h"
#include "Helpers/d3dUtil.h"

#include <stdexcept>

static D3D12_CPU_DESCRIPTOR_HANDLE CpuOffset(D3D12_CPU_DESCRIPTOR_HANDLE h, uint32_t index, uint32_t size)
{
    h.ptr += static_cast<SIZE_T>(index) * size;
    return h;
}

static D3D12_GPU_DESCRIPTOR_HANDLE GpuOffset(D3D12_GPU_DESCRIPTOR_HANDLE h, uint32_t index, uint32_t size)
{
    h.ptr += static_cast<UINT64>(index) * size;
    return h;
}

bool DescriptorHeapManager::Initialize(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, uint32_t numDescriptors, bool shaderVisible)
{
    if (device == nullptr || numDescriptors == 0) return false;

    Shutdown();

    m_device = device;
    m_type = heapType;
    m_capacity = numDescriptors;
    m_shaderVisible = shaderVisible;

    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc.NumDescriptors = numDescriptors;
    desc.Type = heapType;
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

    if (shaderVisible && (heapType == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV || heapType == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER))
    {
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    }

    HRESULT hr = m_device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_heap));

    if (FAILED(hr)) return false;

    m_descriptorSize = m_device->GetDescriptorHandleIncrementSize(heapType);
    m_cpuStart = m_heap->GetCPUDescriptorHandleForHeapStart();

    if (shaderVisible)
        m_gpuStart = m_heap->GetGPUDescriptorHandleForHeapStart();
    else
        m_gpuStart = D3D12_GPU_DESCRIPTOR_HANDLE{ 0 };

    m_freeList.clear();
    m_freeList.reserve(m_capacity);

    for (uint32_t i = 0; i < m_capacity; ++i)
        m_freeList.push_back(m_capacity - 1 - i);

    m_nextLinear = 0;

    return true;
}

void DescriptorHeapManager::Shutdown()
{
    if (m_heap)
    {
        m_heap->Release();
        m_heap = nullptr;
    }

    m_capacity = 0;
    m_descriptorSize = 0;
    m_cpuStart = { 0 };
    m_gpuStart = { 0 };
    m_freeList.clear();
	m_nextLinear = 0;
}

DescriptorHandle DescriptorHeapManager::Allocate()
{
    if (m_heap == nullptr)
        throw std::runtime_error("DescriptorHeapManager::Allocate: heap not initialized.");

    uint32_t index = UINT32_MAX;

    if (m_freeList.empty() == false)
    {
        index = m_freeList.back();
        m_freeList.pop_back();
    }
    else
    {
        // fallback linear (si tu n’utilises jamais Free())
        if (m_nextLinear >= m_capacity)
            throw std::runtime_error("DescriptorHeapManager::Allocate: out of descriptors.");
        index = m_nextLinear++;
    }

    return GetHandle(index);
}

void DescriptorHeapManager::Free(uint32_t index)
{
    if (m_heap == nullptr)
        return;

    if (index >= m_capacity)
        return;

    m_freeList.push_back(index);
}

DescriptorHandle DescriptorHeapManager::GetHandle(uint32_t index) const
{
    if (m_heap == nullptr || index >= m_capacity)
        return {};

    DescriptorHandle h;
    h.index = index;
    h.cpu = CpuOffset(m_cpuStart, index, m_descriptorSize);

    if (m_shaderVisible)
        h.gpu = GpuOffset(m_gpuStart, index, m_descriptorSize);
    else
        h.gpu = D3D12_GPU_DESCRIPTOR_HANDLE{ 0 };

    return h;
}

void DescriptorHeapManager::ResetAll()
{
    if (m_heap == nullptr)
        return;

    m_freeList.clear();
    m_freeList.reserve(m_capacity);

    for (uint32_t i = 0; i < m_capacity; ++i)
        m_freeList.push_back(m_capacity - 1 - i);

    m_nextLinear = 0;
}

#endif // !DESCRIPTOR_HEAP_MANAGER_CPP_INCLUDED