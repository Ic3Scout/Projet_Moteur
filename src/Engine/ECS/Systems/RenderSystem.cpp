#include <d3d12.h> 
#include "RenderSystem.h"
#include "Engine/ECS/ECS.h"
#include "Engine/ECS/Entity.h"
#include "Engine/ECS/Components/MeshRendererComponent.h"
#include "Renderer.h"
#include "Mesh.h"

void RenderSystem::Update(ECS& ecs, float deltaTime)
{
	m_vObj.clear();
	for (Entity* entity : ecs.GetEntities()) 
	{ 
		if (entity == nullptr ) continue;
		if (entity->IsActive() == false) continue;

		MeshRendererComponent* meshRenderer = entity->GetComponent<MeshRendererComponent>();
		if (meshRenderer != nullptr) 
		{
			XMFLOAT4X4 world = entity->GetMatrix();
			XMMATRIX worldMatrix = XMLoadFloat4x4(&world);

			XMFLOAT4X4 worldViewProjT = m_renderer->BuildWorldViewProjMatrix(worldMatrix); // Matrice Transposée
			XMFLOAT4X4 worldT;
			worldMatrix = XMMatrixTranspose(worldMatrix);
			XMStoreFloat4x4(&worldT, worldMatrix); // Matrice Transposée

			meshRenderer->UpdateConstants(worldT, worldViewProjT);

			m_vObj.push_back(meshRenderer);
		} 
	} 
	if (m_renderer != nullptr)
	{ 
		m_renderer->Render(m_vObj);
	}
}