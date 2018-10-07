//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "ModelTestScene.h"
#include "Scenegraph/GameObject.h"
#include "Components/Components.h"
#include "Content/ContentManager.h"
#include "Physx/PhysxManager.h"
#include "Materials\ColorMaterial.h"
#include "Materials\DiffuseMaterial.h"
#include "Physx\PhysxProxy.h"
ModelTestScene::ModelTestScene(void) :
	GameScene(L"ModelTestScene"),
	m_pChair(nullptr)
{
}


ModelTestScene::~ModelTestScene(void)
{
}

void ModelTestScene::Initialize(const GameContext& gameContext)
{
	

	//GROUND PLANE
	//************
	auto physX = PhysxManager::GetInstance()->GetPhysics();
	GetPhysxProxy()->EnablePhysxDebugRendering(true);

	auto bouncyMaterial = physX->createMaterial(0, 0, 1);
	auto ground = new GameObject();
	ground->AddComponent(new RigidBodyComponent(true));

	std::shared_ptr<PxGeometry> geom(new PxPlaneGeometry());
	ground->AddComponent(new ColliderComponent(geom, *bouncyMaterial, PxTransform(PxQuat(XM_PIDIV2, PxVec3(0, 0, 1)))));
	AddChild(ground);

	//CHAIR OBJECT
	//************
	m_pChair = new GameObject();
	
	//1. Attach a modelcomponent (chair.ovm)
	m_pChair->AddComponent(new ModelComponent( L"Resources/Meshes/chair.ovm" ));
	//2. Create a ColorMaterial and add it to the material manager
	auto diffuseMat = new DiffuseMaterial();
	diffuseMat->SetDiffuseTexture(L"Resources/Textures/Chair_Dark.dds");
	gameContext.pMaterialManager->AddMaterial(diffuseMat, 0);

	//3. Assign the material to the previous created modelcomponent
	m_pChair->GetComponent<ModelComponent>()->SetMaterial(0);
	//AddChild(m_pChair);
	// Build and Run

	//4. Create a DiffuseMaterial (using PosNormTex3D.fx)
	//		Make sure you are able to set a texture (DiffuseMaterial::SetDiffuseTexture(const wstring& assetFile))
	//		Load the correct shadervariable and set it during the material variable update
	//diffuseMat->SetEffectVariables(gameContext, m_pChair->GetComponent<ModelComponent>());
	//5. Assign the material to the modelcomponent
	// Build and Run

	//6. Attach a rigidbody component (pure-dynamic)
	auto chairBody = new RigidBodyComponent();
	m_pChair->AddComponent(chairBody);

	//7. Attach a collider component (Use a PxConvexMeshGeometry [chair.ovpc])
	auto pChairConvexMesh =(ContentManager::Load<PxConvexMesh>(L"Resources/Meshes/chair.ovpc"));
	
	std::shared_ptr<PxGeometry> meshGeometry = std::make_shared<PxConvexMeshGeometry>(pChairConvexMesh);
	ColliderComponent* pCollider{ new ColliderComponent(meshGeometry, *bouncyMaterial) };
	m_pChair->AddComponent(pCollider);

	m_pChair->GetTransform()->Translate(0.0f, 1.0f, 0.0f);
	AddChild(m_pChair);
	// Build and Run

}

void ModelTestScene::Update(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);

}

void ModelTestScene::Draw(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
}
