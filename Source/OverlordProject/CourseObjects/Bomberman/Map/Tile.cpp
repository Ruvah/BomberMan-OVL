#include "stdafx.h"
#include "Tile.h"
#include "Components/ModelComponent.h"
#include "Physx/PhysxManager.h"
#include "Components/RigidBodyComponent.h"
#include "Components/ColliderComponent.h"
#include <foundation/PxVec3.h>
#include <foundation/PxTransform.h>
#include "Components/TransformComponent.h"
#include "CourseObjects/Bomberman/BomberMan.h"
#include <common/PxPhysXCommonConfig.h>
#include "Content/ContentManager.h"
#include "CourseObjects/Bomberman/Bomb.h"
#include "../Box.h"
#include "CourseObjects/Bomberman/Enemy.h"


Tile::Tile()
{
}


Tile::~Tile()
{
}

void Tile::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);

	auto physX = PhysxManager::GetInstance()->GetPhysics();

	auto mat = physX->createMaterial(0.f, 0.f, 1.f);

	auto pRigidbody = new RigidBodyComponent();
	AddComponent(pRigidbody);

	shared_ptr<PxGeometry> geom(new PxBoxGeometry(2.f,5.f,2.f));
	auto collider = new ColliderComponent(geom, *mat);
	collider->EnableTrigger(true);
	AddComponent(collider);
	m_pCollider = collider;

	GameObject::PhysicsCallback callback = [this](GameObject*triggerObj, GameObject* otherObj, GameObject::TriggerAction action)
	{
		UNREFERENCED_PARAMETER(triggerObj);
		if (action == TriggerAction::ENTER)
		{
			auto player = dynamic_cast<BomberMan*>(otherObj);
			auto bomb = dynamic_cast<Bomb*>(otherObj->GetParent());
			auto box = dynamic_cast<Box*>(otherObj);
			auto enemy = dynamic_cast<Enemy*>(otherObj);


			if (player)
			{
				m_pOnTile.push_back(otherObj);
				player->SetTile(this);
			}
			else if(bomb)
			{
				m_pOnTile.push_back(otherObj->GetParent());
				bomb->SetTile(this);
				
			}
			else if(box)
			{
				m_pOnTile.push_back(otherObj);
				box->SetTile(this);
			}
			else if (enemy)
			{
				m_pOnTile.push_back(otherObj);
				enemy->SetTile(this);
			}
		}
		else if (action == TriggerAction::LEAVE)
		{
			m_pOnTile.remove(otherObj);
			/*auto player = dynamic_cast<BomberMan*>(otherObj);
			if(player)
			{
				if(player->GetTile()== this)
				{
					player->SetTile(nullptr);
				}
			}*/

		}
	};
	SetOnTriggerCallBack(callback);
#pragma region groundplane
	auto pGroundObj = new GameObject();
	auto pGroundModel = new ModelComponent(L"./Resources/Meshes/UnitPlane.ovm");

	pGroundModel->SetMaterial(1);
	pGroundObj->AddComponent(pGroundModel);
	pGroundObj->GetTransform()->Translate(0.f, -5.f, 0.f);


	AddChild(pGroundObj);
#pragma endregion 





}

void Tile::Update(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	m_pCollider->GetTransform()->Translate(GetTransform()->GetPosition());
}

void Tile::PostInitialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
}

void Tile::AddOnTile(GameObject* obj)
{
	m_pOnTile.push_back(obj);
}

bool Tile::IsBombOnTile()
{
	for(auto obj:m_pOnTile)
	{
		auto bomb = dynamic_cast<Bomb*>(obj);

		if (bomb)return true;
	}

	return false;
}

bool Tile::IsStopTile()
{
	for (auto obj : m_pOnTile)
	{
		auto box = dynamic_cast<Box*>(obj);
		auto bomb = dynamic_cast<Bomb*>(obj);

		if(box)
		{
			return true;
		}
		else if (bomb)
		{
			return true;
		}
	}
	return false;
}


