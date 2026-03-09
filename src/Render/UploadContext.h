#ifndef UPLOAD_CONTEXT_H_INCLUDED
#define UPLOAD_CONTEXT_H_INCLUDED

#include "Helpers/d3dUtil.h"

class UploadContext
{
public:
    UploadContext() = default;
    ~UploadContext() { Shutdown(); }

    // Utilise la queue DIRECT (simple) et la fence du DxContext (ou ta propre fence)
    bool Initialize(ID3D12Device* device, ID3D12CommandQueue* queue);

    void Shutdown();

    // Début/fin d'un batch d'uploads
    void Begin();
    void EndAndWait();     // exécute + wait GPU

    // Helpers upload buffer:
    // - Crée un default buffer GPU + un upload buffer intermédiaire
    // - CopyBufferRegion
    // - Barrières vers finalState
    //
    // NOTE: outUpload doit rester vivant au moins jusqu'à EndAndWait()
    bool UploadBuffer(
        const void* srcData,
        uint64_t numBytes,
        ID3D12Resource*& outDefaultBuffer,
        ID3D12Resource*& outUploadBuffer,
        D3D12_RESOURCE_STATES finalState = D3D12_RESOURCE_STATE_GENERIC_READ);

    // Barrière utilitaire
    void Transition(ID3D12Resource* res,
        D3D12_RESOURCE_STATES before,
        D3D12_RESOURCE_STATES after);

    // Accès à la cmd list si tu veux faire des copies custom
    ID3D12GraphicsCommandList* GetCommandList() const { return m_cmdList; }

private:
    ID3D12Device* m_device = nullptr;
    ID3D12CommandQueue* m_queue = nullptr;

    ID3D12CommandAllocator* m_alloc = nullptr;
    ID3D12GraphicsCommandList* m_cmdList = nullptr;

    ID3D12Fence* m_fence = nullptr;
    uint64_t m_fenceValue = 0;
    HANDLE m_fenceEvent = nullptr;

    bool m_recording = false;
};


#endif // !UPLOAD_CONTEXT_H_INCLUDED