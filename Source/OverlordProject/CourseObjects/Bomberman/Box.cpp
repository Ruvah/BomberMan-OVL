#include "stdafx.h"
#include "Box.h"
#include "Physx/PhysxManager.h"
#include "Components/RigidBodyComponent.h"
#include "Components/ColliderComponent.h"
#include "Components/TransformComponent.h"
#include "Components/ModelComponent.h"
#include "CourseObjects/Bomberman/Helpers.h"
#include "Scenegraph/GameScene.h"
#include "Powerup.h"
#include <ctime>


Box::Box(bool isDestr):
m_Destructable(isDestr)
{
}


Box::~Box()
{



}

void Box::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
#pragma region mesh
	if (m_Destructable)
	{
		auto modelComp = new ModelComponent(L"./Resources/Bomberman/Box/BoxDestr.ovm");
		modelComp->SetMaterial(BombermanGame::Materials::BOXDESTR);
		GetTransform()->Scale(5.f, 5.f, 5.f);

		AddComponent(modelComp);
	}
	else
	{
		auto modelComp = new ModelComponent(L"./Resources/Bomberman/Box/BoxNonDestr.ovm");
		modelComp->SetMaterial(BombermanGame::Materials::BOXNONDESTR);
		GetTransform()->Scale(5.f, 5.f, 5.f);

		AddComponent(modelComp);
	}
#pragma endregion 
#pragma region collision
	auto physX = PhysxManager::GetInstance()->GetPhysics();

	auto mat = physX->createMaterial(1.f, 1.f, 1.f);

	auto pRigidbody = new RigidBodyComponent(true);

	shared_ptr<PxGeometry> geom(new PxBoxGeometry(4.5f, 4.5f, 4.5f));
	auto collider = new ColliderComponent(geom, *mat);

	AddComponent(pRigidbody);
	AddComponent(collider);

#pragma endregion

}

void Box::SetTile(Tile* tile)
{
	m_pCurrTile = tile;
}

void Box::Break()
{
	int seed = int(time(nullptr)) + ((m_pCurrTile->GetRowAndCol().first*2)*(m_pCurrTile->GetRowAndCol().second-1));
	srand(seed);

	const int noSpawn{ 75 };
	const int roll{ rand() % 101 };
	cout << roll << '\n';

	if (roll > noSpawn)
	{
		SpawnPowerup();
	}
	GetTransform()->Translate(0, -100000, 0);
	m_pCurrTile->Remove(this);

	GetScene()->RemoveChild(this);

}

void Box::SpawnPowerup()
{

	UINT max = BombermanGame::Powerups::SPEEDUP + 1;
	int roll = rand() % max;
	cout << roll<<'\n';
	auto powerup = new Powerup(BombermanGame::Powerups(roll));
	powerup->GetTransform()->Translate(GetTransform()->GetPosition());
	powerup->SetTile(m_pCurrTile);
	GetScene()->AddChild(powerup);

}
