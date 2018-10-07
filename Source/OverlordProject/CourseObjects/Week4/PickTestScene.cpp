//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "PickTestScene.h"
#include "Scenegraph\GameObject.h"
#include "Diagnostics\Logger.h"
#include "Diagnostics\DebugRenderer.h"

#include "Prefabs\Prefabs.h"
#include "Components\Components.h"
#include "Physx\PhysxProxy.h"
#include "Physx\PhysxManager.h"
#include "Content\ContentManager.h"

#define FPS_COUNTER 1

PickTestScene::PickTestScene(void) :
	GameScene(L"PickTestScene"),
	m_FpsInterval(FPS_COUNTER)
{
}


PickTestScene::~PickTestScene(void)
{
}

void PickTestScene::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);

	GetPhysxProxy()->EnablePhysxDebugRendering(true);
	//GetPhysxProxy()->EnablePhysXFrameStepping(true);
	gameContext.pGameTime->ForceElapsedUpperbound(true);

	//PhysX
	auto physX = PhysxManager::GetInstance()->GetPhysics();
	auto bouncyMaterial = physX->createMaterial(0.5, 0.5, 1.0f);

	//GROUND 
	//****************
	auto ground = new GameObject();
	ground->AddComponent(new RigidBodyComponent(true));

	std::shared_ptr<PxGeometry> geom(new PxPlaneGeometry());
	ground->AddComponent(new ColliderComponent(geom, *bouncyMaterial, PxTransform(PxQuat(XM_PIDIV2, PxVec3(0, 0, 1)))));
	AddChild(ground);

	//SPHERE 1 (Group0) + (Ignore Group1 & Group2)
	//*********
	auto pSphere = new SpherePrefab();
	pSphere->GetTransform()->Translate(0, 2, 0);

	auto pRigidBody = new RigidBodyComponent();

	pSphere->AddComponent(pRigidBody);

	std::shared_ptr<PxGeometry> spheregeom(new PxSphereGeometry(1));
	pSphere->AddComponent(new ColliderComponent(spheregeom, *bouncyMaterial, PxTransform(PxQuat(XM_PIDIV2, PxVec3(0, 0, 1)))));
	AddChild(pSphere);



	gameContext.pInput->AddInputAction(InputAction(LBUTTON, InputTriggerState::Released, -1,VK_LBUTTON));

}

void PickTestScene::Update(const GameContext& gameContext)
{

	m_FpsInterval += gameContext.pGameTime->GetElapsed();
	if (m_FpsInterval >= FPS_COUNTER)
	{
		m_FpsInterval -= FPS_COUNTER;
		Logger::LogFormat(LogLevel::Info, L"FPS: %i", gameContext.pGameTime->GetFPS());
	}

	if (gameContext.pInput->IsActionTriggered(LBUTTON))
	{
		auto obj = gameContext.pCamera->Pick(gameContext);
		if (obj!= nullptr )
		{
			if (!obj->GetComponent<RigidBodyComponent>()->IsKinematic() && !obj->GetComponent<RigidBodyComponent>()->IsStatic())
			{
				obj->GetComponent<RigidBodyComponent>()->AddForce(PxVec3(200.f, 10.f, 0.f));
				
			}
		}
	}
}

void PickTestScene::Draw(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
}
