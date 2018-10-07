#include "stdafx.h"
#include "BoneObject.h"
#include "Components\Components.h"
BoneObject::BoneObject(int boneId, int materialId, float length) :
	m_Length{ length },
	m_BoneId{ boneId },
	m_MaterialId{materialId}
{
}

BoneObject::~BoneObject(void)
{
}

void BoneObject::AddBone(BoneObject * pBone)
{
	pBone->GetTransform()->Translate(0.f, 0.f, -m_Length);
	AddChild(pBone);
}

void BoneObject::CalculateBindPose()
{
	XMMATRIX bindpose = (XMLoadFloat4x4(&GetTransform()->GetWorld()));
	bindpose = XMMatrixInverse(&XMMatrixDeterminant(bindpose), bindpose);
	XMStoreFloat4x4(&m_BindPose,bindpose);
	for (auto* pBone : GetChildren<BoneObject>())
	{
		pBone->CalculateBindPose();
	}
}

void BoneObject::Initialize(const GameContext & gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	auto pBone = new GameObject();
	auto pModel = (new ModelComponent(L"Resources/Meshes/Bone.ovm"));

	pModel->SetMaterial(m_MaterialId);
	pBone->AddComponent(pModel);
	AddChild(pBone);
	pBone->GetTransform()->Scale(m_Length, m_Length, m_Length);
}
