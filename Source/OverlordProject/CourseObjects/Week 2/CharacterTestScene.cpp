//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "CharacterTestScene.h"
#include "Scenegraph/GameObject.h"
#include "Components/Components.h"
#include "Content/ContentManager.h"
#include "Physx/PhysxManager.h"
#include "Materials\ColorMaterial.h"
#include "Materials\DiffuseMaterial.h"
#include "Prefabs\SpherePrefab.h"
#include "Components\ControllerComponent.h"
#include "Physx\PhysxProxy.h"
#include "Character.h"
CharacterTestScene::CharacterTestScene(void) :
	GameScene(L"CharacterTestScene")
	
{
}


CharacterTestScene::~CharacterTestScene(void)
{
}

void CharacterTestScene::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);

	GetPhysxProxy()->EnablePhysxDebugRendering(true);

	//GROUND PLANE
	//************
	auto physX = PhysxManager::GetInstance()->GetPhysics();

	auto bouncyMaterial = physX->createMaterial(0, 0, 1);
	auto ground = new GameObject();
	ground->AddComponent(new RigidBodyComponent(true));

	std::shared_ptr<PxGeometry> geom(new PxPlaneGeometry());
	ground->AddComponent(new ColliderComponent(geom, *bouncyMaterial, PxTransform(PxQuat(XM_PIDIV2, PxVec3(0, 0, 1)))));
	AddChild(ground);
	//sphere
	auto pSphere = new SpherePrefab();
	pSphere->GetTransform()->Translate(0, 20, 0);

	std::shared_ptr<PxGeometry> spheregeom(new PxSphereGeometry(1));
	auto pRigidBody = new RigidBodyComponent();
	pSphere->AddComponent(pRigidBody);

	pSphere->AddComponent(new ColliderComponent(spheregeom, *bouncyMaterial, PxTransform(PxQuat(XM_PIDIV2, PxVec3(0, 0, 1)))));
	AddChild(pSphere);

	//SPHERE 2
	pSphere = new SpherePrefab();
	pSphere->GetTransform()->Translate(0, 10, 0);

	pRigidBody = new RigidBodyComponent();
	pSphere->AddComponent(pRigidBody);

	pSphere->AddComponent(new ColliderComponent(spheregeom, *bouncyMaterial, PxTransform(PxQuat(XM_PIDIV2, PxVec3(0, 0, 1)))));
	AddChild(pSphere);


	//levelmesh
	auto defaultMaterial = physX->createMaterial(0.5f, 0.5f, 0.f);

	auto m_pLevel = new GameObject();
	m_pLevel->AddComponent(new ModelComponent(L"Resources/Meshes/SimpleLevel.ovm"));

	auto lvlBody = new RigidBodyComponent(true);
	auto colorMat = new ColorMaterial();

	gameContext.pMaterialManager->AddMaterial(colorMat, 0);
	m_pLevel->GetComponent<ModelComponent>()->SetMaterial(0);

	m_pLevel->AddComponent(lvlBody);
	PxTriangleMesh* pLevelTriangleMesh = ContentManager::Load <PxTriangleMesh>(L"Resources/Meshes/SimpleLevel.ovpt");

	std::shared_ptr<PxGeometry> meshGeometry = std::make_shared<PxTriangleMeshGeometry>(pLevelTriangleMesh);
	ColliderComponent* pCollider{ new ColliderComponent(meshGeometry, *defaultMaterial) };
	m_pLevel->AddComponent(pCollider);

	AddChild(m_pLevel);

	//controller
	

	auto character = new Character();
	
	character->GetTransform()->Translate(0.f, 10.f, 0.f);
	
	
	AddChild(character);
	
}

void CharacterTestScene::Update(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);

}

void CharacterTestScene::Draw(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
}
