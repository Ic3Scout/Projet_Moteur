#include "App.h"
#include <Window.h>
#include <Renderer.h>
#include <iostream>
#include "Engine/Engine.h"
#include "Core/InputsMethods.h"
#include "Engine/Scene/SceneManager.h"
#include "Engine/ECS/Components/CameraComponent.h"
#include "Engine/ECS/Systems/CameraSystem.h"
#include "Engine/ResourceManager.h"
#include "Engine/ECS/Components/MeshRendererComponent.h"
#include "Render/UploadContext.h" 
#include "Mesh.h"
#include "Engine/ECS/Systems/RenderSystem.h"
#include "ProjetMoteur/GameScene.h"
#include "ProjetMoteur/MenuScene.h"
#include "SandboxScene.h"
#include "Engine/ECS/Systems/StateMachineSystem.h"
#include "Engine/ECS/Systems/PhysicSystem.h"
#include "Engine/ECS/Systems/ColliderSystem.h"
#include "Engine/ECS/Systems/PlayerSystem.h"

using namespace core;

App::App(Engine& engine) : m_engine(engine)
{
	engine.SetInitCallback(std::bind(&App::Initialize, this));
	engine.SetUpdateCallback(std::bind(&App::Update, this));
	engine.SetShutdownCallback(std::bind(&App::Shutdown, this));

	m_resourceManager = &ResourceManager::Instance();
	m_renderer = nullptr;
	m_window = nullptr;
	m_sceneManager = nullptr;
	m_cam = nullptr;
	m_ecs = nullptr;
}

void App::Initialize()
{
	// Initialize your application here
	m_window = new Window(1920, 1080, L"LAB ENGINE GAMEPLAY");
	m_ecs = &m_engine.GetECS();
	CreateCamera();
	m_renderer = new Renderer();
	m_sceneManager = new SceneManager();
	UIFrame* frame = new UIFrame();

	m_renderer->Initialize(m_window, m_cam->GetComponent<CameraComponent>());

	m_ecs->AddSystem<RenderSystem>()->SetRenderer(m_renderer);

	m_sceneManager->CreateScene<MenuScene>("Menu")->Initialize(m_ecs, m_renderer, &m_engine, m_cam, m_sceneManager, frame);
	m_sceneManager->CreateScene<GameScene>("Game")->Initialize(m_ecs, m_renderer, &m_engine, m_cam, m_sceneManager,frame);
	m_sceneManager->CreateScene<SandboxScene>("Sandbox")->Initialize(m_ecs, m_renderer, &m_engine, m_cam, m_sceneManager,frame);

	m_sceneManager->ChangeScene("Menu");

	m_ecs->AddSystem<StateMachineSystem>();
	m_ecs->AddSystem<PhysicSystem>();
	m_ecs->AddSystem<ColliderSystem>();
	m_ecs->AddSystem<PlayerSystem>();
}

void App::Update()
{
	//update your application here

	UpdateWindow();
	m_sceneManager->Update(m_engine.GetDeltaTime());
	HandleInput();
}

void App::Shutdown()
{
	//Clean everything and Exit App
	std::cout << " /!\\ -[ SHUTDOWN ]- /!\\" << std::endl;
	m_engine.Shutdown();
}

void App::UpdateWindow()
{
	m_window->ProcessMessages();
	m_renderer->Update();

	if (m_window->IsOpen() == false)
		m_engine.Shutdown();
}

void App::HandleInput()
{
	if (Input::GetKey(Keyboard::NUMPAD1) || Input::GetKey(Keyboard::_1)) { m_sceneManager->ChangeScene("Menu"); }
	if (Input::GetKey(Keyboard::NUMPAD2) || Input::GetKey(Keyboard::_2)) { m_sceneManager->ChangeScene("Game"); }
	if (Input::GetKey(Keyboard::NUMPAD3) || Input::GetKey(Keyboard::_3)) { m_sceneManager->ChangeScene("Sandbox"); }

	if (Input::GetKeyDown(Keyboard::BACKSPACE)) {
		App::Shutdown();
	}

	if (Input::GetKeyDown(Keyboard::F)) 
	{
		std::cout << "FullScreen" << std::endl;
	}

	Input::Update();
}

void App::CreateCamera()
{
	m_cam = m_ecs->CreateEntity<Entity>();
	m_cam->AddComponent<CameraComponent>();
	m_ecs->AddSystem<CameraSystem>();

	m_cam->SetPosition(0.0f, 0.0f, 0.0f);

	m_cam->GetComponent<CameraComponent>()->SetFOV(45.0f);

	m_cam->GetComponent<CameraComponent>()->SetAll(
		0.1f,
		100.0f,
		static_cast<float>(m_window->GetHeight()),
		static_cast<float>(m_window->GetWidth()),
		true
	);
	XMFLOAT4X4 identityMatrix;
	XMStoreFloat4x4(&identityMatrix, XMMatrixIdentity());
	m_cam->GetComponent<CameraComponent>()->SetViewMatrix(identityMatrix);
}


