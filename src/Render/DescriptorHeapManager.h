#ifndef DESCRIPTOR_HEAP_MANAGER_H_INCLUDED
#define DESCRIPTOR_HEAP_MANAGER_H_INCLUDED

#include <cstdint>
#include <vector>
#include <d3d12.h>

struct DescriptorHandle
{
    D3D12_CPU_DESCRIPTOR_HANDLE cpu{ 0 };
    D3D12_GPU_DESCRIPTOR_HANDLE gpu{ 0 };   // gpu.ptr == 0 si heap non shader-visible
    uint32_t index = UINT32_MAX;

    bool IsValid() const { return cpu.ptr != 0 && index != UINT32_MAX; }
};

class DescriptorHeapManager
{
public:
    DescriptorHeapManager() = default;
    ~DescriptorHeapManager() { Shutdown(); }

    bool Initialize(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, uint32_t numDescriptors, bool shaderVisible);

    void Shutdown();

    // Allocation/libération d’un descripteur
    DescriptorHandle Allocate();
    void Free(uint32_t index);

    ID3D12DescriptorHeap* GetHeap() const { return m_heap; }
    D3D12_DESCRIPTOR_HEAP_TYPE GetType() const { return m_type; }
    uint32_t Capacity() const { return m_capacity; }
    uint32_t DescriptorSize() const { return m_descriptorSize; }
    bool IsShaderVisible() const { return m_shaderVisible; }

    DescriptorHandle GetHandle(uint32_t index) const;

    void ResetAll();

private:
    ID3D12Device* m_device = nullptr;
    ID3D12DescriptorHeap* m_heap = nullptr;

    D3D12_DESCRIPTOR_HEAP_TYPE m_type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    bool m_shaderVisible = true;

    uint32_t m_capacity = 0;
    uint32_t m_descriptorSize = 0;

    D3D12_CPU_DESCRIPTOR_HANDLE m_cpuStart{ 0 };
    D3D12_GPU_DESCRIPTOR_HANDLE m_gpuStart{ 0 };

    std::vector<uint32_t> m_freeList;
    uint32_t m_nextLinear = 0;
};

#endif // !DESCRIPTOR_HEAP_MANAGER_H_INCLUDED