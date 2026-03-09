#include "ProjetMoteur/GameScene.h"
#include "Engine/ECS/Entity.h"
#include "Engine/ResourceManager.h" 
#include "Engine/ECS/Components/MeshRendererComponent.h"
#include "Render/Renderer.h"
#include "Engine/ECS/ECS.h"
#include "Engine/ECS/Components/PlayerComponent.h"
#include "Engine/Engine.h"
#include "Core/InputsMethods.h"
#include "Engine/ECS/Components/StateMachineComponent.h"
#include "Engine/ECS/Components/RigidBodyComponent.h"
#include "Engine/Projectile.h"
#include "Engine/ECS/Components/ColliderComponent.h"
#include "ProjetMoteur/Tiles/TilesManager.h"
#include "ProjetMoteur/Tiles/Tiles.h"
#include "Window.h"
#include "Engine/Scene/SceneManager.h"

#include <iostream>

using namespace core;

void GameScene::Initialize(ECS* ecs, Renderer* renderer, Engine* engine, Entity* camera, SceneManager* sceneManager, UIFrame* frame)
{
	m_ecs = ecs;
	m_renderer = renderer;
	m_engine = engine;
	m_cam = camera;
	m_sceneManager = sceneManager;
	m_frame = frame;
	m_body = CreateBody();

	m_projectile = new Projectile();
	m_projectile->Initialize(0.08f, Shape::SPHERE, 15, 10.0f, m_engine, m_ecs, m_renderer);

	m_tilesManager = new TilesManager();
	m_tilesManager->Initialize(m_ecs, m_renderer);

	m_angleOrbit = XMConvertToRadians(90.0f);

	for (Entity* entity : m_entities)
	{
		entity->SetActive(false);
	}
}

GameScene::~GameScene(){

}

void GameScene::OnEnter()
{
	Window* window = Window::GetInstance();
	window->LockCursor(true);

	m_tilesManager->OnEnter();
	for (Entity* entity : m_entities)
	{
		entity->SetActive(true);
	}

	m_frame->showSplash = false;
	m_frame->showCrosshair = true;
	m_frame->showScore = true;
	m_tilesManager->SetScoreValue(0);
	m_pivot = 0.0f;
}
void GameScene::OnExit()
{
	// Clean up entities 
	m_tilesManager->OnExit();
	for (Entity* entity : m_entities)
	{
		entity->SetActive(false);
	}
	m_tilesManager->SetScoreValue(0);
}
void GameScene::Update(float dt)
{
	m_deltaTime = dt;
	
	HandleCursor();
	if (!m_locked) { return; }
	
	LookAround();
	if (m_body->IsActive() == false)
	{
		m_sceneManager->ChangeScene("Menu");
	}

	m_projectile->Update(m_deltaTime);
	m_tilesManager->Update(dt);
	m_frame->score = m_tilesManager->GetScoreValue();

	FollowRail();
	MovePlayer();
	Shooting();
	m_renderer->SetUiFrame(*m_frame);
}

// -[CREATING BASE ENTITY]- // 
Entity* GameScene::CreateBody()
{
	Entity* body = m_ecs->CreateEntity<Entity>();
	body->AddComponent<MeshRendererComponent>()->SetMesh(ResourceManager::Instance().GetMeshPreset(MeshPreset::Cylinder), m_renderer);
	body->AddComponent<PlayerComponent>();
	body->AddComponent<ColliderComponent>()->SetType(ColliderComponent::Type::Box);
	body->SetPosition(0, 1, 0);
	body->SetScale({ .01f });
	m_entities.push_back(body);
	return body;
}

// -[PLAYER MOVEMENT]- //
void GameScene::MovePlayer() {

	float offset = .5f;
	int dir = (Input::GetKey(Keyboard::LEFT) - Input::GetKey(Keyboard::RIGHT));

	m_angleOrbit += XMConvertToRadians(dir * 90.0f * m_deltaTime);

	m_body->OrbitAround({ m_pivot, 0.0f, 0.0f }, { 0,0,1 }, m_angleOrbit, offset);
	m_body->RotateLocalZ(XMConvertToRadians(dir * 90.0f * m_deltaTime));

	FollowRail();
}
void GameScene::FollowRail()
{
	std::vector<Tiles*> activeTiles = m_tilesManager->GetActiveTiles();
	float smoothSpeed = m_tilesManager->GetSpeed();
	float playerZ = m_body->GetPosition().z;

	if (activeTiles.empty())
		return;

	Entity* closestRail = nullptr;
	float closestDistance = 100.0f; // big value just in case -> first rail to be checked

	for (Tiles* tile : activeTiles)
	{
		auto& rails = tile->GetRails();

		for (Entity* rail : rails)
		{
			float railZ = rail->GetPosition().z;

			if (railZ >= playerZ)
			{
				float dist = railZ - playerZ;

				if (dist < closestDistance)
				{
					closestDistance = dist;
					closestRail = rail;
				}
			}
		}
	}

	if (closestRail == nullptr)
		return;

	float targetPivot = closestRail->GetPosition().x;
	m_pivot = std::lerp(m_pivot, targetPivot, smoothSpeed * m_deltaTime);
}

// -[SHOOTING]- //
void GameScene::Shooting() {
	if (Input::GetMouseButtonDown(Mouse::LEFT))
	{
		Entity* temp = m_projectile->GetAvaibleProjectile();
		if (temp == nullptr) return; // No projectile available in the pool

		// hadle speed and direction of bullet
		XMFLOAT3 bulletSpeed = m_cam->GetForward();
		float speedMult = 10;
		bulletSpeed.x *= speedMult; bulletSpeed.y *= speedMult; bulletSpeed.z *= speedMult;

		// shoot from player
		temp->SetPosition(m_cam->GetPosition());
		temp->GetComponent<RigidBodyComponent>()->SetVelocity(bulletSpeed);

	}
}

// -[LOCK & HIDE CURSOR]- //
void GameScene::HandleCursor()
{
	Window* window = Window::GetInstance();

	if (Input::GetKeyDown(Keyboard::ESC))
	{
		m_locked = !m_locked;
		window->LockCursor(m_locked);
	}

	if (!m_locked) return;
		
	window->UpdateCursorCenter();
}

// -[CAMERA ROTATION]- //
void GameScene::LookAround()
{
	Window* window = Window::GetInstance();
	POINT mouse;

	GetCursorPos(&mouse);

	float dx = float(mouse.x - window->GetCursorCenter().x);
	float dy = float(mouse.y - window->GetCursorCenter().y);

	float sensitivity = 0.0025f;

	m_yaw += dx * sensitivity;
	m_pitch += dy * sensitivity;

	m_yaw = std::clamp(m_yaw, -1.0f, 1.0f);
	m_pitch = std::clamp(m_pitch, -1.0f, 1.0f);

	SetCursorPos(
		window->GetCursorCenter().x,
		window->GetCursorCenter().y
	);

	UpdateCameraTransform();
}
void GameScene::UpdateCameraTransform()
{
	// Position = body
	m_cam->SetPosition(m_body->GetPosition());

	// Load body rotation 
	XMVECTOR bodyQ = XMLoadFloat4(&m_body->m_quaternion);

	// Up local body
	XMVECTOR bodyUp = XMVector3Rotate(
		XMVectorSet(0, 1, 0, 0),
		bodyQ
	);

	// YAW around up local
	XMVECTOR yawQ = XMQuaternionRotationAxis(bodyUp, m_yaw);

	// Rotation apres yaw
	XMVECTOR yawedBody = XMQuaternionMultiply(bodyQ, yawQ);

	// Right local apres yaw
	XMVECTOR bodyRight = XMVector3Rotate(
		XMVectorSet(1, 0, 0, 0),
		yawedBody
	);

	// Pitch around right
	XMVECTOR pitchQ = XMQuaternionRotationAxis(bodyRight, m_pitch);

	// Rotation
	XMVECTOR finalQ = XMQuaternionMultiply(yawedBody, pitchQ);
	finalQ = XMQuaternionNormalize(finalQ);

	XMFLOAT4 qFloat;
	XMStoreFloat4(&qFloat, finalQ);

	m_cam->SetRotation(qFloat);
}

