//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "ColliderComponent.h"
#include "RigidBodyComponent.h"
#include "../Base/GeneralStructs.h"
#include "../Diagnostics/Logger.h"
#include "../Scenegraph/GameObject.h"

ColliderComponent::ColliderComponent(std::shared_ptr<PxGeometry> & geometry, const PxMaterial& material, const PxTransform& localPose)
	:
	m_Geometry(geometry),
	m_Material(material),
	m_LocalPose(localPose),
	m_pShape(nullptr),
	m_isTrigger(false),
m_IsTriggerRuntime(false)
{

}


ColliderComponent::~ColliderComponent(void)
{
}

void ColliderComponent::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);

	auto rigidBody = GetGameObject()->GetComponent<RigidBodyComponent>();
		
	if (rigidBody == nullptr)
	{
		Logger::LogError(L"[ColliderComponent] Cannot add a Collider to an object that does not have a rigid body");
		return;
	}

	rigidBody->AddCollider(this);
}

void ColliderComponent::Update(const GameContext& gameContext)
{
	if(m_IsTriggerRuntime!= m_isTrigger)
	{
		EnableTrigger(m_IsTriggerRuntime);
	}
	UNREFERENCED_PARAMETER(gameContext);
}

void ColliderComponent::Draw(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
}

void ColliderComponent::EnableTrigger(bool isTrigger) 
{ 
	m_isTrigger = isTrigger;
	m_IsTriggerRuntime = isTrigger;
	if (m_pShape != nullptr)
	{
		if (m_isTrigger)
		{

			m_pShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
			m_pShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
		}
		else
		{
			m_pShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, false);
			m_pShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
		}m_isTrigger = isTrigger;
		if (m_pShape != nullptr)
		{
			if (m_isTrigger)
			{

				m_pShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
				m_pShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
			}
			else
			{
				m_pShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, false);
				m_pShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
			}
		} 
	}
}

void ColliderComponent::SetTriggerRuntime(bool enable)
{
	m_IsTriggerRuntime = enable;
}

void ColliderComponent::SetShape(PxShape* shape) 
{ 
	m_pShape = shape; 
	if(m_isTrigger)
	{
		m_pShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, !m_isTrigger);
		m_pShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, m_isTrigger);
	}
}

