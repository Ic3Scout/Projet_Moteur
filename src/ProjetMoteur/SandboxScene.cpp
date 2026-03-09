#include "SandboxScene.h"
#include "Engine/ECS/Entity.h"
#include "Engine/ResourceManager.h" 
#include "Engine/ECS/Components/MeshRendererComponent.h"
#include "Render/Renderer.h"
#include "Engine/ECS/ECS.h"
#include "Engine/ECS/Components/PlayerComponent.h"
#include "Engine/Engine.h"
#include "Core/InputsMethods.h"
#include "Engine/ECS/Components/StateMachineComponent.h"
#include "ProjetMoteur/EnemyIdleState.h"
#include "ProjetMoteur/EnemyChaseState.h"
#include "Engine/ECS/Components/RigidBodyComponent.h"
#include "Engine/Projectile.h"
#include "Engine/ECS/Components/ColliderComponent.h"
#include "ProjetMoteur/Tiles/Tile6.h"
#include "Window.h"
#include <iostream>

using namespace core;

void SandboxScene::Initialize(ECS* ecs, Renderer* renderer, Engine* engine, Entity* camera, SceneManager* sceneManager, UIFrame* frame)
{
	m_ecs = ecs;
	m_renderer = renderer;
	m_engine = engine;
	m_cam = camera;
	m_sceneManager = sceneManager;
	m_frame = frame;

	


	// -[TEST OBJECTS]- //
	m_cylinder = CreateCylinder();
	m_cube = CreateCube();
	m_sphere = CreateSphere();
	m_moon = CreateMoon();
	m_moonMoon = CreateMoonMoon();

	Tile6* tile = new Tile6();
	tile->Initialize(m_ecs, m_renderer);
	tile->SetPosition({ 0.0f, 0.0f, 0.0f });
	tile->SetActive(false);

	// Ugly but needed until fix
	for (Entity* entity : m_entities)
	{
		entity->SetActive(false);
	}
}

void SandboxScene::OnEnter()
{
	for (Entity* entity : m_entities)
	{
		entity->SetActive(true);
	}

	m_cam->SetPosition(0.0f, 0.0f, 0.0f);
	m_cam->SetRotation({ 1.0f, 0.0f, 0.0f, 1.0f });
}

void SandboxScene::OnExit()
{
	// Clean up entities 
	for (Entity* entity : m_entities)
	{
		entity->SetActive(false);
	}
}

void SandboxScene::Update(float dt)
{
	m_deltaTime = dt;

	MoveCamera();
	HandleCursor();
	Debug();

	//Test on entities
	m_cube->RotateX(XMConvertToRadians(1.f * m_deltaTime));
	m_cube->RotateLocalY(XMConvertToRadians(5.f * m_deltaTime));
	m_cube->RotateZ(XMConvertToRadians(5.f * m_deltaTime));

	m_cylinder->RotateX(XMConvertToRadians(.05f));

	m_angleOrbit += XMConvertToRadians(90.0F * m_deltaTime);

	m_moon->OrbitAround(m_cylinder->GetPosition(), { 0.0f, 1.0f, 0.0f }, m_angleOrbit, 1.0f);
	m_moonMoon->OrbitAround(m_moon->GetPosition(), {  0.0f, 0.0f, 1.0f}, m_angleOrbit * 2.0f, .25f);
}
// -[TEST ENTITIES]- //
Entity* SandboxScene::CreateSphere()
{
	Entity* sphere = m_ecs->CreateEntity<Entity>();
	sphere->AddComponent<MeshRendererComponent>()->SetMesh(ResourceManager::Instance().GetMeshPreset(MeshPreset::Sphere), m_renderer);
	sphere->SetPosition(-2.0f, 0.0f, 5.0f);
	sphere->AddComponent<PlayerComponent>();
	sphere->AddComponent<ColliderComponent>()->SetType(ColliderComponent::Type::Sphere);

	m_entities.push_back(sphere);
	return sphere;
}

Entity* SandboxScene::CreateCylinder()
{
	Entity* cylinder = m_ecs->CreateEntity<Entity>();
	cylinder->AddComponent<MeshRendererComponent>()->SetMesh(ResourceManager::Instance().GetMeshPreset(MeshPreset::Cylinder), m_renderer);
	cylinder->SetPosition(2.0f, 0.0f, 5.0f);
	cylinder->SetScale(.3f);
	cylinder->GetComponent<MeshRendererComponent>()->SetTexture(ResourceManager::Instance().LoadTexture(L"../../res/RailTex.png"));
	m_entities.push_back(cylinder);
	return cylinder;
}

Entity* SandboxScene::CreateCube() {
	Entity* cube = m_ecs->CreateEntity<Entity>();
	cube->AddComponent<MeshRendererComponent>()->SetMesh(ResourceManager::Instance().GetMeshPreset(MeshPreset::Cube), m_renderer);
	cube->SetPosition(0.0f, 0.0f, 5.0f);
	cube->SetScale(.5f);
	cube->GetComponent<MeshRendererComponent>()->SetTexture(ResourceManager::Instance().LoadTexture(L"../../res/TargetTex.png"));

	m_entities.push_back(cube);
	return cube;
}

Entity* SandboxScene::CreateMoon() {
	Entity* moon = m_ecs->CreateEntity<Entity>();
	moon->AddComponent<MeshRendererComponent>()->SetMesh(ResourceManager::Instance().GetMeshPreset(MeshPreset::Sphere), m_renderer);
	moon->SetPosition(m_cylinder->GetPosition().x, m_cylinder->GetPosition().y, m_cylinder->GetPosition().z);
	moon->SetScale(.1f);
	moon->GetComponent<MeshRendererComponent>()->SetTexture(ResourceManager::Instance().LoadTexture(L"../../res/briqueTex.png"));
	m_entities.push_back(moon);
	return moon;
}

Entity* SandboxScene::CreateMoonMoon() {
	Entity* moonMoon = m_ecs->CreateEntity<Entity>();
	moonMoon->AddComponent<MeshRendererComponent>()->SetMesh(ResourceManager::Instance().GetMeshPreset(MeshPreset::Sphere), m_renderer);
	moonMoon->SetPosition(m_moon->GetPosition().x, m_moon->GetPosition().y, m_moon->GetPosition().z);
	moonMoon->SetScale(.05f);
	moonMoon->GetComponent<MeshRendererComponent>()->SetTexture(ResourceManager::Instance().LoadTexture(L"../../res/briqueTex.png"));
	m_entities.push_back(moonMoon);
	return moonMoon;
}


// -[LOCK & HIDE CURSOR + Calculate DeltaMouse]- //
void SandboxScene::HandleCursor()
{
	Window* window = Window::GetInstance();

	if (Input::GetKeyDown(Keyboard::ESC))
	{
		m_locked = !m_locked;
		window->LockCursor(m_locked);
	}

	if (!m_locked)
	{
		return;
	}

	window->UpdateCursorCenter();

	POINT mouse;
	GetCursorPos(&mouse);

	float dx = float(mouse.x - window->GetCursorCenter().x);
	float dy = float(mouse.y - window->GetCursorCenter().y);

	float sensitivity = 0.0025f;

	m_yaw += dx * sensitivity;
	m_pitch += dy * sensitivity;

	m_pitch = std::clamp(m_pitch, -1.0f, 1.0f);

	SetCursorPos(
		window->GetCursorCenter().x,
		window->GetCursorCenter().y
	);

	UpdateCameraTransform();
}


// -[CAMERA]- //
void SandboxScene::MoveCamera()
{
	// WASD / ZQSD movement
	float directionForward = 0;
	float directionRight = 0;

	if (Input::GetKey(Keyboard::Z) || Input::GetKey(Keyboard::W))
	{
		directionForward += 1;
	}
	if (Input::GetKey(Keyboard::S))
	{
		directionForward -= 1;
	};

	if (Input::GetKey(Keyboard::A) || Input::GetKey(Keyboard::Q))
	{
		directionRight -= 1;
	}
	if (Input::GetKey(Keyboard::D))
	{
		directionRight += 1;
	}

	m_cam->MoveRight((directionRight * m_speedPlayer * m_deltaTime));
	m_cam->MoveForward((directionForward * m_speedPlayer * m_deltaTime));
}


void SandboxScene::UpdateCameraTransform()
{
	// Quaternion Yaw (axe Y monde)
	XMVECTOR yawQ = XMQuaternionRotationAxis(
		XMVectorSet(0, 1, 0, 0),
		m_yaw
	);

	// Right vector après yaw
	XMVECTOR right = XMVector3Rotate(
		XMVectorSet(1, 0, 0, 0),
		yawQ
	);

	// Quaternion pitch
	XMVECTOR pitchQ = XMQuaternionRotationAxis(
		right,
		m_pitch
	);

	// Rotation finale
	XMVECTOR finalQ = XMQuaternionMultiply(yawQ, pitchQ);
	finalQ = XMQuaternionNormalize(finalQ);

	XMFLOAT4 q;
	XMStoreFloat4(&q, finalQ);

	m_cam->SetRotation(q);
}

// -[DEBUG]- //
void SandboxScene::Debug() {

}
