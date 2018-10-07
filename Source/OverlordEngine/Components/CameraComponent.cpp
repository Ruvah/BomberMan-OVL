//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "CameraComponent.h"
#include "../Base/OverlordGame.h"
#include "../Components/TransformComponent.h"
#include "../Diagnostics/Logger.h"
#include "../Physx/PhysxManager.h"
#include "../Physx/PhysxProxy.h"
#include "../Scenegraph/GameObject.h"
#include "../Scenegraph/GameScene.h"
#include "../Base/GeneralStructs.h"
#include "../Components/RigidBodyComponent.h"
#include "../Physx/PhysxProxy.h"

CameraComponent::CameraComponent(void):
	m_FOV(XM_PIDIV4),
	m_NearPlane(0.1f),
	m_FarPlane(2500.0f),
	m_Size(25.0f),
	m_PerspectiveProjection(true),
	m_IsActive(true)
{
	XMStoreFloat4x4(&m_Projection, XMMatrixIdentity());
	XMStoreFloat4x4(&m_View, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ViewInverse, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ViewProjection, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ViewProjectionInverse, XMMatrixIdentity());
}


CameraComponent::~CameraComponent(void)
{
}

void CameraComponent::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
}

void CameraComponent::Update(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	auto windowSettings = OverlordGame::GetGameSettings().Window;
	XMMATRIX projection, view, viewInv, viewProjectionInv;

	if(m_PerspectiveProjection)
	{
		projection = XMMatrixPerspectiveFovLH(m_FOV, windowSettings.AspectRatio ,m_NearPlane, m_FarPlane);
	}
	else
	{
		float viewWidth = (m_Size>0) ? m_Size * windowSettings.AspectRatio : windowSettings.Width;
		float viewHeight = (m_Size>0) ? m_Size : windowSettings.Height;
		projection = XMMatrixOrthographicLH(viewWidth, viewHeight, m_NearPlane, m_FarPlane);
	}

	XMVECTOR worldPosition = XMLoadFloat3(&GetTransform()->GetWorldPosition());
	XMVECTOR lookAt = XMLoadFloat3(&GetTransform()->GetForward());
	XMVECTOR upVec = XMLoadFloat3(&GetTransform()->GetUp());

	view = XMMatrixLookAtLH(worldPosition, worldPosition + lookAt, upVec);
	viewInv = XMMatrixInverse(nullptr, view);
	viewProjectionInv = XMMatrixInverse(nullptr, view * projection);

	XMStoreFloat4x4(&m_Projection, projection);
	XMStoreFloat4x4(&m_View, view);
	XMStoreFloat4x4(&m_ViewInverse, viewInv);
	XMStoreFloat4x4(&m_ViewProjection, view * projection);
	XMStoreFloat4x4(&m_ViewProjectionInverse, viewProjectionInv);
}

void CameraComponent::Draw(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
}

void CameraComponent::SetActive()
{
	auto gameObject = GetGameObject();
	if(gameObject == nullptr)
	{
		Logger::LogError(L"[CameraComponent] Failed to set active camera. Parent game object is null");
		return;
	}

	auto gameScene = gameObject->GetScene();
	if(gameScene == nullptr)
	{
		Logger::LogError(L"[CameraComponent] Failed to set active camera. Parent game scene is null");
		return;
	}

	gameScene->SetActiveCamera(this);
}

GameObject* CameraComponent::Pick(const GameContext& gameContext, CollisionGroupFlag ignoreGroups) const
{
	//TODO
	POINT mousePos = gameContext.pInput->GetMousePosition();
	//Getviewport
	float halfWidth = OverlordGame::GetGameSettings().Window.Width/2.f;
	float halfHeight = OverlordGame::GetGameSettings().Window.Height/2.f;

	XMFLOAT2 NDCCoords ={ ((mousePos.x - halfWidth) / halfWidth),((halfHeight - mousePos.y) / halfHeight) };
	
	
	XMVECTOR nearPoint = XMVECTOR{ NDCCoords.x,NDCCoords.y,0.f };
	nearPoint = XMVector3Transform(nearPoint, XMLoadFloat4x4(&m_ViewInverse));

	
	XMVECTOR farPoint = XMVECTOR{ NDCCoords.x,NDCCoords.y,1.f };
	farPoint = XMVector3Transform(farPoint, XMLoadFloat4x4(&m_ViewInverse));

	XMFLOAT3 start;
	XMFLOAT3 farFloat3;

	XMStoreFloat3(&start, nearPoint);
	XMStoreFloat3(&farFloat3, farPoint);

	PxVec3 rayStart = {start.x,start.y,start.z };
	PxVec3 rayFar = { farFloat3.x,farFloat3.y,farFloat3.z };
	PxVec3 rayDir = { rayFar - rayStart };
	rayDir.normalize();

	PxQueryFilterData filterData;
	filterData.data.word0 = ~ignoreGroups;

	PxRaycastBuffer hit;
	auto proxy = GetGameObject()->GetScene()->GetPhysxProxy();
	if (proxy->Raycast(rayStart, rayDir, PX_MAX_F32, hit, PxHitFlag::eDEFAULT, filterData))
	{
		if (hit.hasBlock)
		{
			auto gameobj = static_cast<RigidBodyComponent*>(hit.block.actor->userData)->GetGameObject();
			if (gameobj&&static_cast<RigidBodyComponent*>(hit.block.actor->userData))
			{
				return gameobj;
			}
			
			//return static_cast<GameObject*>(hit.block.actor->userData);
		}
	}
	return nullptr;




	//GameObject* gameObj{ nullptr };

	////**Step0: Get Mouse coordinates
	//POINT mousePos{ gameContext.pInput->GetMousePosition() };

	////**Step1: To NDC
	//GameSettings::WindowSettings wndSettings{ OverlordGame::GetGameSettings().Window };
	//float sHalfWidth{ wndSettings.Width / 2.0f };
	//float sHalfHeight{ wndSettings.Height / 2.0f };

	//XMFLOAT2 ndcCoord{};
	//ndcCoord.x = (mousePos.x - sHalfWidth) / sHalfWidth;
	//ndcCoord.y = (sHalfHeight - mousePos.y) / sHalfHeight;

	////**Step2: Calc near and far clipping point

	////Vectormath, to allow allignment on cpu
	////XMVector3TransformCoord and not XMVector3Transform
	////Remove W component, because intrinsic

	//XMVECTOR nearVector{ ndcCoord.x, ndcCoord.y, 0.0f };
	//XMVECTOR farVector{ ndcCoord.x, ndcCoord.y, 1.0f };
	//XMMATRIX viewInverse{ XMLoadFloat4x4(&(gameContext.pCamera->GetViewProjectionInverse())) };
	//nearVector = XMVector3TransformCoord(nearVector, viewInverse);
	//farVector = XMVector3TransformCoord(farVector, viewInverse);

	////To float4
	//XMFLOAT3 nearFloat3, farFloat3{};
	//XMStoreFloat3(&nearFloat3, nearVector);
	//XMStoreFloat3(&farFloat3, farVector);

	////To PxVec4
	//PxVec3 nearPoint = PxVec3(nearFloat3.x, nearFloat3.y, nearFloat3.z);
	//PxVec3 farPoint = PxVec3(farFloat3.x, farFloat3.y, farFloat3.z);

	////**Step3: PhysX raycast
	//PxVec3 dirVector{ farPoint - nearPoint };
	//dirVector.normalize();

	////Filter out unwanted collisiongroups
	//PxQueryFilterData filterData;
	//filterData.data.word0 = ~ignoreGroups; //~ = bitwise negation

	//									   //Register raycast
	//PxRaycastBuffer hit{};
	//PhysxProxy* physXProxy{ m_pGameObject->GetScene()->GetPhysxProxy() };

	//if (physXProxy->Raycast(nearPoint, dirVector, PX_MAX_F32, hit, PxHitFlag::eDEFAULT, filterData))
	//{
	//	RigidBodyComponent* rigidBodyComp = static_cast<RigidBodyComponent*>(hit.block.actor->userData);
	//	if (rigidBodyComp)
	//	{
	//		gameObj = rigidBodyComp->GetGameObject();
	//		if (gameObj)
	//		{
	//			std::cout << "Not a nullptr" << '\n';
	//		}
	//	}
	//}

	//return gameObj;
}