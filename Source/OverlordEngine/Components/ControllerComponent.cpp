//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "ControllerComponent.h"
#include "TransformComponent.h"
#include "../Physx/PhysxProxy.h"
#include "../Scenegraph/GameObject.h"
#include "../Scenegraph/GameScene.h"
#include "../Diagnostics/Logger.h"
#include "../Physx/PhysxManager.h"




ControllerComponent::ControllerComponent(PxMaterial* material, float radius, float height, wstring name, PxCapsuleClimbingMode::Enum climbingMode)
	: m_pMaterial(material),
	m_Radius(radius),
	m_Height(height),
	m_Name(name),
	m_ClimbingMode(climbingMode),
	m_pController(nullptr),
	m_CollisionFlag(PxControllerCollisionFlags()),
	m_CollisionGroups(PxFilterData(CollisionGroupFlag::Group0, 0, 0, 0))
{
}


ControllerComponent::~ControllerComponent()
{
	m_pController->release();
}

void ControllerComponent::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	if (m_pController != nullptr)
		Logger::LogError(L"[ControllerComponent] Cannot initialize a controller twice");
	//auto physX = PhysxManager::GetInstance()->GetPhysics();
	//1. Retrieve the ControllerManager from the PhysX Proxy (PhysxProxy::GetControllerManager();)
	
	auto controllerManager = BaseComponent::m_pGameObject->GetScene()->GetPhysxProxy()->GetControllerManager();
	
	//2. Create a PxCapsuleControllerDesc (Struct)
	auto capsuleControllerDesc =  PxCapsuleControllerDesc();

	//  > Call the "setToDefault()" method of the PxCapsuleControllerDesc
	capsuleControllerDesc.setToDefault();
	//	> Fill in all the required fields
	//  > Radius, Height, ClimbingMode, UpDirection (PxVec3(0,1,0)), ContactOffset (0.1f), Material [See Initializer List]
	//capsuleControllerDesc
	//  > Position -> Use the position of the parent GameObject
	//  > UserData -> This component
	capsuleControllerDesc.height = m_Height;
	capsuleControllerDesc.radius = m_Radius;
	capsuleControllerDesc.climbingMode = m_ClimbingMode;
	capsuleControllerDesc.upDirection = PxVec3(0, 1, 0);
	capsuleControllerDesc.contactOffset = 0.1f;
	capsuleControllerDesc.material = m_pMaterial;
	auto pos = m_pGameObject->GetTransform()->GetPosition();
	capsuleControllerDesc.position = { pos.x,pos.y,pos.z };
	capsuleControllerDesc.userData = this;
	//3. Create the controller object (m_pController), use the ControllerManager to do that (CHECK IF VALID!!)
	
	m_pController = controllerManager->createController(capsuleControllerDesc);
	if (m_pController == nullptr)
	{
		Logger::LogError(L"[ControllerComponent] Failed to create controller");
		return;
	}

	//4. Set the controller's name (use the value of m_Name) [PxController::setName]
	//   > Converting 'wstring' to 'string' > Use one of the constructor's of the string class	
	//	 > Converting 'string' to 'char *' > Use one of the string's methods ;)
	string name = string(m_Name.begin(), m_Name.end());
	m_pController->getActor()->setName(name.c_str());

	//5. Set the controller's actor's userdata > This Component

	m_pController->setUserData(this);

	SetCollisionGroup((CollisionGroupFlag)m_CollisionGroups.word0);
	SetCollisionIgnoreGroups((CollisionGroupFlag)m_CollisionGroups.word1);

	m_pController->getActor()->userData = this;
	
}

void ControllerComponent::Update(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
}

void ControllerComponent::Draw(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
}
void ControllerComponent::Translate(XMFLOAT3 position)
{
	if(m_pController == nullptr)
		Logger::LogError(L"[ControllerComponent] Cannot Translate. No controller present");
	else
		m_pController->setPosition(ToPxExtendedVec3(position));
}

void ControllerComponent::Translate(float x, float y, float z)
{
	Translate(XMFLOAT3(x,y,z));
}

void ControllerComponent::Move(XMFLOAT3 displacement, float minDist)
{
	if(m_pController == nullptr)
		Logger::LogError(L"[ControllerComponent] Cannot Move. No controller present");
	else
		m_CollisionFlag = m_pController->move(ToPxVec3(displacement), minDist, 0, PxControllerFilters{&m_CollisionGroups}, 0);
}

XMFLOAT3 ControllerComponent::GetPosition()
{
	if(m_pController == nullptr)
		Logger::LogError(L"[ControllerComponent] Cannot get position. No controller present");
	else
		return ToXMFLOAT3(m_pController->getPosition());

	return XMFLOAT3();
}

XMFLOAT3 ControllerComponent::GetFootPosition()
{
	if(m_pController == nullptr)
		Logger::LogError(L"[ControllerComponent] Cannot get footposition. No controller present");
	else
		return ToXMFLOAT3(m_pController->getFootPosition());

	return XMFLOAT3();
}

void ControllerComponent::SetCollisionIgnoreGroups(CollisionGroupFlag ignoreGroups)//EDIT (Controller-Trigger-Bug)
{
	m_CollisionGroups.word1 = ignoreGroups;

	if (!m_pController)
		return;

	auto nbShapes = m_pController->getActor()->getNbShapes();

	PxShape* buffer;
	m_pController->getActor()->getShapes(&buffer, nbShapes * sizeof(PxShape));

	for (UINT i = 0; i < nbShapes; ++i)
	{
		buffer[i].setSimulationFilterData(m_CollisionGroups);
	}
}

void ControllerComponent::SetCollisionGroup(CollisionGroupFlag group)//EDIT (Controller-Trigger-Bug)
{
	m_CollisionGroups.word0 = group;

	if (!m_pController)
		return;

	auto nbShapes = m_pController->getActor()->getNbShapes();

	PxShape* buffer;
	m_pController->getActor()->getShapes(&buffer, nbShapes * sizeof(PxShape));

	for (UINT i = 0; i < nbShapes; ++i)
	{
		buffer[i].setSimulationFilterData(m_CollisionGroups);
		buffer[i].setQueryFilterData(m_CollisionGroups);
	}
}

