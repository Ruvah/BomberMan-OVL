//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "SoftwareSkinningScene_1.h"
#include <Physx/PhysxProxy.h>
#include "Materials\ColorMaterial.h"
#include "BoneObject.h"
#include "./Components/TransformComponent.h"
SoftwareSkinningScene_1::SoftwareSkinningScene_1():
	GameScene(L"SoftwareSkinningScene_1"),
	m_pBone0{nullptr},
	m_pBone1{nullptr},
	m_BoneRotation{0},
	m_RotationSign{1}
{
}


SoftwareSkinningScene_1::~SoftwareSkinningScene_1()
{
}

void SoftwareSkinningScene_1::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	auto pColormat = new ColorMaterial;
	gameContext.pMaterialManager->AddMaterial(pColormat, 0);
	auto root = new GameObject();
	m_pBone0 = new BoneObject(0, 0, 15.f);
	m_pBone1 = new BoneObject(1, 0, 15.f);

	root->AddChild(m_pBone0);
	m_pBone0->AddBone(m_pBone1);
	root->GetTransform()->Rotate(0.f,-90.f,0.f);
	AddChild(root);
}

void SoftwareSkinningScene_1::Update(const GameContext& gameContext)
{
	m_BoneRotation += m_RotationSign * 45 * gameContext.pGameTime->GetElapsed();
	if (m_RotationSign < 0 && m_BoneRotation <= -45)m_RotationSign = 1;
	if (m_RotationSign > 0 && m_BoneRotation >= 45)m_RotationSign = -1;

	m_pBone0->GetTransform()->Rotate (m_BoneRotation, 0.f,0.f);
	m_pBone1->GetTransform()->Rotate(-m_BoneRotation*2, 0.f, 0.f);


}

void SoftwareSkinningScene_1::Draw(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
}

void SoftwareSkinningScene_1::SceneActivated()
{
}

void SoftwareSkinningScene_1::SceneDeactivated()
{
}
