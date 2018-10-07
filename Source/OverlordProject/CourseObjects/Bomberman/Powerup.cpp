#include "stdafx.h"
#include "Powerup.h"
#include "Components/ColliderComponent.h"
#include "Physx/PhysxManager.h"
#include "Components/RigidBodyComponent.h"
#include "BomberMan.h"
#include "Scenegraph/GameScene.h"
#include "Components/TransformComponent.h"
#include "Map/Map.h"


Powerup::Powerup(BombermanGame::Powerups type):
m_Type(type),
m_Active(false),
m_Used(false)
{

}




void Powerup::Initialize(const GameContext& gameContext)
{

	auto physX = PhysxManager::GetInstance()->GetPhysics();

	auto mat = physX->createMaterial(0.f, 0.f, 1.f);

	auto pRigidbody = new RigidBodyComponent(true);
	AddComponent(pRigidbody);

	shared_ptr<PxGeometry> geom(new PxBoxGeometry(3.f, 5.f,3.f));
	auto collider = new ColliderComponent(geom, *mat);
	collider->EnableTrigger(true);
	AddComponent(collider);
	
	GameObject::PhysicsCallback callback = [this](GameObject*triggerObj, GameObject* otherObj, GameObject::TriggerAction action)
	{
		UNREFERENCED_PARAMETER(triggerObj);
		if (action == TriggerAction::ENTER)
		{
			auto player = dynamic_cast<BomberMan*>(otherObj);

			if (player&&!m_Used)
			{
				if (!m_ToDestroy)
				{
					player->GetPowerup(m_Type);
					m_pCurrTile->Remove(this);
					m_pIcon->m_ToDestroy = true;
					m_ToDestroy = true;

					m_Used = true;
				}
				
			}

		}
	
	};
	SetOnTriggerCallBack(callback);
	m_pCurrTile->AddOnTile(this);
	m_StartTime = gameContext.pGameTime->GetTotal();


#pragma region groundplane
	auto pIcon = new GameObject();
	auto pGroundModel = new ModelComponent(L"./Resources/Meshes/UnitPlane.ovm");


	switch (m_Type)
	{
	case BombermanGame::Powerups::BOMBUP:
		pGroundModel->SetMaterial(BombermanGame::BOMBUPMAT);
		break;
	case BombermanGame::Powerups::FIREUP:
		pGroundModel->SetMaterial(BombermanGame::FIREUPMAT);
		break;
	case BombermanGame::Powerups::SPEEDUP:
			pGroundModel->SetMaterial(BombermanGame::SPEEDUPMAT);
			break;
	}


	pIcon->AddComponent(pGroundModel);
	pIcon->GetTransform()->Translate(0.f, 5.f, 0.f);
	pIcon->GetTransform()->Rotate(0.f, 180.f, 0.f);


	AddChild(pIcon);
	m_pIcon = pIcon;
#pragma endregion 


}

void Powerup::Update(const GameContext& gameContext)
{
	if(!m_Active && gameContext.pGameTime->GetTotal()- m_StartTime >=1.f)
	{
		m_Active=true;
	}
	if(m_pCurrTile->GetMap()->GetLoading())
	{
		m_pCurrTile->Remove(this);

		GetScene()->RemoveChild(this);
	}
}

void Powerup::Explode()
{
	if (m_Active)
	{
		m_pCurrTile->Remove(this);
		GetTransform()->Translate(0, -1000, 0);

		/*GetScene()->RemoveChild(this);*/
		RemoveChild(m_pIcon);
		delete m_pIcon;
		m_ToDestroy = true;

	}
}
