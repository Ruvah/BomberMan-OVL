#include "stdafx.h"

#include "SimpleEnemy.h"
#include "Map/Map.h"
#include "Components/TransformComponent.h"
#include "Components/ControllerComponent.h"
#include "AnimatedMesh.h"
#include "BomberMan.h"
#include "Physx/PhysxManager.h"
#include "Components/RigidBodyComponent.h"
#include "Base/SoundManager.h"
#include "BManLevel1.h"


SimpleEnemy::SimpleEnemy() :
	m_Wait(false),m_Dead(false)
{
	m_MaxRunVelocity = 10.f;
	m_Direction = Direction::UP;
	m_pCurrTile = nullptr;
	m_Radius = 3.f;
	m_Height = 11.f;
	m_WaitTime = 1.f;
	m_DeadTime = 2.65f;
}



void SimpleEnemy::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	auto physX = PhysxManager::GetInstance()->GetPhysics();

	m_pController = new ControllerComponent(physX->createMaterial(1.f, 1.f, 0.f), m_Radius, m_Height);
	AddComponent(m_pController);



	m_pMesh = new AnimatedMesh(L"./Resources/Bomberman/Enemy/Enemy1.ovm");
	AddChild(m_pMesh);

	m_pMesh->GetTransform()->Scale(0.1f, 0.1f, 0.1f);
	m_pMesh->GetTransform()->Translate(0, -m_Height*0.8f, 0);

	m_pMesh->GetTransform()->Rotate(0, 180, 0);



	auto mat = physX->createMaterial(1.f, 1.f, 0.f);

	auto pRigidbody = new RigidBodyComponent();

	shared_ptr<PxGeometry> geom(new PxBoxGeometry(5.f, 6.f, 5.f));
	auto collider = new ColliderComponent(geom, *mat);
	auto collision = new GameObject();
	auto collPos = GetTransform()->GetPosition();
	collision->GetTransform()->Translate(collPos);
	collision->AddComponent(pRigidbody);
	collision->AddComponent(collider);
	collider->EnableTrigger(true);
	m_pCollider = collision;
	GameObject::PhysicsCallback callback = [this](GameObject*triggerObj, GameObject* otherObj, GameObject::TriggerAction action)
	{
		UNREFERENCED_PARAMETER(triggerObj);
		auto player = dynamic_cast<BomberMan*>(otherObj);
		if (player && action == TriggerAction::ENTER)
		{
			
			player->Die();
			
		}
	};
	collision->SetOnTriggerCallBack(callback);
	AddChild(m_pCollider);

	m_pCollider->GetComponent<RigidBodyComponent>()->SetCollisionIgnoreGroups(Group8);

#pragma region sounds
	FMOD_RESULT fmodResult;
	auto fmodSystem = SoundManager::GetInstance()->GetSystem();
	fmodResult = fmodSystem->createStream("./Resources/Bomberman/Sounds/EnemyDeath2.wav", FMOD_LOOP_OFF | FMOD_2D, 0, &m_pDeathSound);
	SoundManager::ErrorCheck(fmodResult);
	//m_pChannel->setPaused(false);
#pragma endregion 

}

void SimpleEnemy::PostInitialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	m_pMesh->GetComponent<ModelComponent>()->SetMaterial(BombermanGame::ENEMY);
	m_pMesh->SetAnim(1);
	m_pMesh->Play();
}

void SimpleEnemy::Update(const GameContext& gameContext)
{
	auto scene = dynamic_cast<BManLevel1*>(GetScene());
	if (scene && !scene->GetPaused())
	{
		m_pMesh->SetAnimationSpeed(1);

		if (m_Dead&&gameContext.pGameTime->GetTotal() - m_DeadStart >= m_DeadTime)
		{

			m_ToDestroy = true;
		}
		else if (!m_Dead)
		{
			m_pCollider->GetTransform()->Translate(GetTransform()->GetPosition());
			if (!m_Wait)
			{
				UpdateMovement(gameContext);
			}
			else if (m_Wait && gameContext.pGameTime->GetTotal() - m_WaitStart >= m_WaitTime)
			{
				m_Wait = false;
				DecideNewDirection();
			}
		}
	
	}
	else
	{
		m_pCollider->GetTransform()->Translate(GetTransform()->GetPosition());

		m_pMesh->SetAnimationSpeed(0);
	}
}

void SimpleEnemy::UpdateMovement(const GameContext& gameContext)
{
	if (m_pCurrTile)
	{
		Tile* m_NextTile = nullptr;
		auto currRowCol = m_pCurrTile->GetRowAndCol();
		int row = currRowCol.first;
		int col = currRowCol.second;
		auto map = m_pCurrTile->GetMap();
		switch (m_Direction)
		{
		case Direction::DOWN:
			m_NextTile = map->GetTile(row + 1, col);
			if (row > 17) cout << row << endl;
			break;
		case Direction::LEFT:
			m_NextTile = map->GetTile(row, col - 1);
			break;
		case Direction::UP:
			m_NextTile = map->GetTile(row - 1, col);
			break;
		case Direction::RIGHT:
			m_NextTile = map->GetTile(row, col + 1);
			break;
		}
		if (m_NextTile->IsStopTile())
		{
			m_Wait = true;
			m_WaitStart = gameContext.pGameTime->GetTotal();
			m_Velocity.x = 0;
			m_Velocity.z = 0;
			m_Velocity.y = m_JumpVelocity;
			m_pMesh->SetAnim(0);
			m_pMesh->Play();

		}
		else
		{
			XMVECTOR forward = XMLoadFloat3(&GetTransform()->GetForward());
			XMVECTOR direction = XMVector3Normalize((forward));

			m_RunVelocity += m_RunAcceleration * gameContext.pGameTime->GetElapsed();
			Clamp(m_RunVelocity, m_MaxRunVelocity, 0.f);
			//auto component = m_Velocity.y;
			auto newVel = direction * m_RunVelocity;
			XMStoreFloat3(&m_Velocity, newVel);
			m_Velocity.y = m_JumpVelocity;


			if ((GetComponent<ControllerComponent>()->GetCollisionFlags()) != PxControllerCollisionFlag::eCOLLISION_DOWN)
			{
				m_JumpVelocity -= m_JumpAcceleration * gameContext.pGameTime->GetElapsed();


			}
			else
			{
				m_Velocity.y = 0.f;
			}

			XMFLOAT3 displacement;
			XMStoreFloat3(&displacement, XMLoadFloat3(&m_Velocity)*gameContext.pGameTime->GetElapsed());

			GetComponent<ControllerComponent>()->Move(displacement);
			GetTransform()->Rotate(0, m_TotalYaw, 0);

		}
	}
}

void SimpleEnemy::Die(const GameContext& gameContext)
{
	if (!m_Dead)
	{
		m_Dead = true;
		m_DeadStart = gameContext.pGameTime->GetTotal();
		m_pMesh->SetAnim(2);
		m_pMesh->Play();
		m_pController->SetCollisionGroup(CollisionGroupFlag::Group9);
		FMOD_RESULT fmodResult;
		auto fmodSystem = SoundManager::GetInstance()->GetSystem();

		fmodResult = fmodSystem->playSound(m_pDeathSound, 0, false, &m_pChannel);
		m_pCurrTile->Remove(this);
	}
}

void SimpleEnemy::DecideNewDirection()
{
	UINT times{};
	Tile* m_NextTile = nullptr;
	auto currRowCol = m_pCurrTile->GetRowAndCol();
	int row = currRowCol.first;
	int col = currRowCol.second;
	auto map = m_pCurrTile->GetMap();
	while((!m_NextTile || m_NextTile->IsStopTile())&&times<4)
	{
		switch (m_Direction)
		{

		case Direction::DOWN:
			m_Direction = Direction::LEFT;
			m_TotalYaw = -90;

			m_NextTile = map->GetTile(row, col - 1);
			break;
		case Direction::LEFT:
			m_Direction = Direction::UP;
			m_TotalYaw = 0;


			m_NextTile = map->GetTile(row - 1, col);
			break;
		case Direction::UP:
			m_Direction = Direction::RIGHT;
			m_TotalYaw = 90;


			m_NextTile = map->GetTile(row, col + 1);
			break;
		case Direction::RIGHT:
			m_Direction = Direction::DOWN;
			m_TotalYaw = 180;

			m_NextTile = map->GetTile(row + 1, col);
			break;

		}
		times++;
	}

	m_pMesh->SetAnim(1);
	m_pMesh->Play();
}
