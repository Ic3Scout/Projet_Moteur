#include "ProjetMoteur/MenuScene.h"
#include "Engine/ECS/Entity.h"
#include "Engine/ResourceManager.h" 
#include "Engine/ECS/Components/MeshRendererComponent.h"
#include "Render/Renderer.h"
#include "Engine/ECS/ECS.h"
#include "InputSystem.h"
#include "InputsMethods.h"
#include "Window.h"

void MenuScene::Initialize(ECS* ecs, Renderer* renderer, Engine* engine, Entity* camera, SceneManager* sceneManager, UIFrame* frame)
{
	m_engine = engine;
	m_ecs = ecs;
	m_renderer = renderer;
	m_cam = camera;
	m_sceneManager = sceneManager;
	m_frame = frame;

 	Entity* menuEntity = m_ecs->CreateEntity<Entity>();
	MeshRendererComponent* mrc = menuEntity->AddComponent<MeshRendererComponent>();
	mrc->SetMesh(ResourceManager::Instance().GetMeshPreset(MeshPreset::Cube), m_renderer);
	TextureHandle test = ResourceManager::Instance().LoadTexture(L"../../res/testTexture.png");
	mrc->SetTexture(test);
	
	menuEntity->SetPosition(0.0f, 0.0f, 5.0f);
	m_entities.push_back(menuEntity);
}

void MenuScene::OnEnter()
{
	m_cam->SetPosition(0.0f, 0.0f, 0.0f);
	m_cam->SetRotation({ 0.0f, 0.0f, 0.0f, 1.0f });

	for (Entity* entity : m_entities)
	{
		entity->SetActive(true);
	}
	
	Window* window = Window::GetInstance();
	window->LockCursor(false);
}

void MenuScene::OnExit()
{
	// Clean up entities 
	for (Entity* entity : m_entities) 
	{
		entity->SetActive(false);
	}
}

void MenuScene::Update(float dt)
{

}
