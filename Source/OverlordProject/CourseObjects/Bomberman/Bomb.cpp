#include "stdafx.h"
#include "Bomb.h"
#include "Materials/Shadow/DiffuseMaterial_Shadow.h"
#include "CourseObjects/Bomberman/Helpers.h"
#include "Components/ModelComponent.h"
#include "Components/TransformComponent.h"
#include "Components/RigidBodyComponent.h"
#include "Physx/PhysxManager.h"
#include "Components/ColliderComponent.h"
#include <foundation/PxTransform.h>
#include "Map/Map.h"
#include "BomberMan.h"
#include "Scenegraph/GameScene.h"
#include "Box.h"
#include "Powerup.h"
#include "SimpleEnemy.h"
#include "Base/SoundManager.h"
#include "BManLevel1.h"


Bomb::Bomb(BomberMan& player, int radius) :
m_Radius(radius),
m_ExplDuration(1.f),
m_IsPower(false),
m_LifeDuration(3.f),
m_Exploded(false),
m_pPlayer(&player),
m_HitlistMadeUp(false)
{

}


Bomb::~Bomb()
{
	

}

void Bomb::Initialize(const GameContext& gameContext)
{



#pragma region Particles
	//fuse
	m_pFuseFire = new GameObject();
	auto fusePart = new ParticleEmitterComponent(L"./Resources/Bomberman/Bomb/Particles/FuseFire2.png",1);
	fusePart->SetVelocity(XMFLOAT3(.0f, 0.f, 0.f));
	fusePart->SetMinSize(2.5f);
	fusePart->SetMaxSize(2.5f);
	fusePart->SetMinEnergy(3.f);
	fusePart->SetMaxEnergy(3.f);
	fusePart->SetMinEmitterRange(0.f);
	fusePart->SetMaxEmitterRange(0.f);
	fusePart->SetMinSizeGrow(00.99f);
	fusePart->SetMaxSizeGrow(00.99f);
	fusePart->SetColor(XMFLOAT4(1.f, 1.f, 1.f, 1.f));
	m_pFuseFire->AddComponent(fusePart);
	auto pos = GetTransform()->GetPosition();

	pos.y += 10.f;

	m_pFuseFire->GetTransform()->Translate(pos);

	AddChild(m_pFuseFire);
#pragma endregion


#pragma region mesh and transf
	auto modelComp = new ModelComponent(L"./Resources/Bomberman/Bomb/Bomb.ovm");
	modelComp->SetMaterial(BombermanGame::Materials::BOMB);
	AddComponent(modelComp);
	GetTransform()->Rotate(90, 0, 0);
	GetTransform()->Scale(0.1f, 0.1f, 0.1f);
#pragma endregion

#pragma region collision
	auto physX = PhysxManager::GetInstance()->GetPhysics();

	auto mat = physX->createMaterial(1.f, 1.f, 0.f);

	auto pRigidbody = new RigidBodyComponent(true);

	shared_ptr<PxGeometry> geom(new PxBoxGeometry(5.f,5.f,5.f));
	auto collider = new ColliderComponent(geom, *mat);
	auto collision = new GameObject();

	auto collPos = GetTransform()->GetPosition();
	collision->GetTransform()->Translate(collPos);
	collision->AddComponent(pRigidbody);
	collision->AddComponent(collider);
	collider->EnableTrigger(true);


	GameObject::PhysicsCallback callback = [this](GameObject*triggerObj, GameObject* otherObj, GameObject::TriggerAction action)
	{
		UNREFERENCED_PARAMETER(triggerObj);
		if(dynamic_cast<BomberMan*>(otherObj)&&action == TriggerAction::LEAVE)
		{

			m_pCollision->GetComponent<ColliderComponent>()->SetTriggerRuntime(false);
		}
	};
	collision->SetOnTriggerCallBack(callback);

	AddChild(collision);
	m_pCollision = collision;

#pragma endregion

#pragma region sounds
	FMOD_RESULT fmodResult;
	auto fmodSystem = SoundManager::GetInstance()->GetSystem();
	fmodResult = fmodSystem->createStream("./Resources/Bomberman/Sounds/Bomb.mp3", FMOD_LOOP_OFF | FMOD_2D, 0, &m_pExplodeSound);
	SoundManager::ErrorCheck(fmodResult);
	//m_pChannel->setPaused(false);
#pragma endregion 


	m_SpawnTime = gameContext.pGameTime->GetTotal();
	m_pTile->AddOnTile(this);
}

void Bomb::Update(const GameContext& gameContext)
{
	auto scene = dynamic_cast<BManLevel1*>(GetScene());
	if (scene && !scene->GetPaused())
	{
		if (gameContext.pGameTime->GetTotal() - m_SpawnTime >= 3.f && !m_Exploded)
		{
			Explode(gameContext);
		}
		if (m_Exploded && gameContext.pGameTime->GetTotal() - m_ExplTime >= m_ExplDuration)
		{
			GetTransform()->Translate(0, -1000.f, 0);
			m_pTile->Remove(this);
			m_pPlayer->RemoveBomb(this);
			
			GetScene()->RemoveChild(this);

		}
		else if (m_Exploded)
		{
			AffectTiles(gameContext);
		}
	}
}

void Bomb::Explode(const GameContext& gameContext)
{
	if (!m_Exploded)
	{

		FMOD_RESULT fmodResult;
		auto fmodSystem = SoundManager::GetInstance()->GetSystem();

		fmodResult = fmodSystem->playSound(m_pExplodeSound, 0, false, &m_pChannel);


		auto bombMesh = GetComponent<ModelComponent>();
		RemoveComponent(bombMesh);
		delete bombMesh;
		m_Exploded = true;
		RemoveChild(m_pFuseFire);
		delete m_pFuseFire;	
		delete m_pCollision;
		RemoveChild(m_pCollision);
		AffectTiles(gameContext);
		m_ExplTime = gameContext.pGameTime->GetTotal();

	}
}

void Bomb::AffectTiles(const GameContext& gameContext)
{
	auto map = m_pTile->GetMap();

	auto rowCol = m_pTile->GetRowAndCol();
	

	bool continueTop{ true }, continueRight{ true }, continueBot{ true }, continueLeft{ true };
	if (!m_HitlistMadeUp)
	{
		for (unsigned int i{ 1 }; i <= m_Radius; ++i)
		{
			auto  top = map->GetTile(rowCol.first - i, rowCol.second),
				bot = map->GetTile(rowCol.first + i, rowCol.second),
				right = map->GetTile(rowCol.first, rowCol.second + i),
				left = map->GetTile(rowCol.first, rowCol.second - i);
			if (top && continueTop)
			{
				if (top->IsStopTile())

					continueTop = false;

				m_ExplTiles.push_back(top);
			}
			if (bot && continueBot)
			{
				if (bot->IsStopTile())
					continueBot = false;

				m_ExplTiles.push_back(bot);
			}
			if (right && continueRight)
			{
				if (right->IsStopTile())
					continueRight = false;

				m_ExplTiles.push_back(right);
			}
			if (left && continueLeft)
			{
				if (left->IsStopTile())
					continueLeft = false;

				m_ExplTiles.push_back(left);
			}

		}
		m_ExplTiles.push_back(m_pTile);
		m_HitlistMadeUp = true;
	}
	for(auto& tile:m_ExplTiles)
	{


		auto objs = tile->GetOnTile();
		bool noStone = true;
		for(auto& obj:objs)
		{
			if (obj)
			{
				auto player = dynamic_cast<BomberMan*>(obj);
				auto bomb = dynamic_cast<Bomb*>(obj);
				auto box = dynamic_cast<Box*>(obj);
				auto powerup = dynamic_cast<Powerup*>(obj);
				auto enemy = dynamic_cast<SimpleEnemy*>(obj);

				if (player)
				{
					player->Die();
				}
				else if (bomb&&bomb != this)
				{
					bomb->Explode(gameContext);
				}
				else if(box)
				{
					if(box->GetDestr())
					{
						box->Break();
					}
					else
					{
						noStone = false;
					}
				}
				else if(powerup)
				{
					powerup->Explode();
				}
				else if (enemy)
				{
					enemy->Die(gameContext);
				}
			}


		}


		if (noStone)
		{
			auto emitter = new GameObject();

			auto particle = new ParticleEmitterComponent(L"./Resources/Bomberman/Bomb/Particles/Explosion2.png", 10);
			particle->SetVelocity({ 0.f, 0.f, 0.f });
			particle->SetMaxSize(2.f);
			particle->SetMinSize(2.f);
			particle->SetMaxEnergy(1.1f);
			particle->SetMinEnergy(1.1f);
			particle->SetMinSizeGrow(10.f);
			particle->SetMaxSizeGrow(10.f);
			particle->SetMinEmitterRange(-5.f);
			particle->SetMaxEmitterRange(5.f);

			emitter->AddComponent(particle);


			
			emitter->GetTransform()->Translate(tile->GetTransform()->GetPosition());

			AddChild(emitter);
		}

	}
	
}
