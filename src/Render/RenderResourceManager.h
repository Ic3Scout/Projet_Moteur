#ifndef RENDERRESOURCEMANAGER_H
#define RENDERRESOURCEMANAGER_H

class Mesh;
class UploadContext;
class DxContext;
class DescriptorHeapManager;
class Texture2D;

#include "Engine/ResourceManager.h"

class RenderResourceManager
{
public:
	static RenderResourceManager& Instance();

	void Initialize(DxContext* dx, UploadContext* uploader, DescriptorHeapManager* srvHeap);
	void Shutdown();

	Mesh* ResolveMesh(MeshHandle h);
	Texture2D* ResolveTexture(TextureHandle h);

	void FinalizeUpload();
	void ReleaseAll();

private:
	RenderResourceManager() = default;

	Mesh* CreateMeshPreset(MeshPreset mesh);
	Texture2D* CreateTextureFromDesc(const TextureDesc& desc);
	void CreateWhiteTexture1x1();

	DxContext* m_dx = nullptr;
	UploadContext* m_uploader = nullptr;
	DescriptorHeapManager* m_srvHeap = nullptr;

	std::unordered_map<uint32_t, std::unique_ptr<Mesh>> m_meshes;
	std::unordered_map<uint32_t, std::unique_ptr<Texture2D>> m_textures;

	Texture2D* m_defaultTexture = nullptr;
};

#endif // !RENDERRESSOURCEMANAGER_H