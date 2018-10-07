//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "SoftwareSkinningScene_3.h"
#include <Physx/PhysxProxy.h>
#include "Materials\ColorMaterial.h"
#include "BoneObject.h"
#include "./Components/TransformComponent.h"
#include "SoftwareSkinningScene_3.h"
#include "Components\MeshDrawComponent.h"
SoftwareSkinningScene_3::SoftwareSkinningScene_3():
	GameScene(L"SoftwareSkinningScene_3"),
	m_pBone0{nullptr},
	m_pBone1{nullptr},
	m_BoneRotation{0},
	m_RotationSign{1}
{
}


SoftwareSkinningScene_3::~SoftwareSkinningScene_3()
{
}

void SoftwareSkinningScene_3::Initialize(const GameContext& gameContext)
{
	CreateMesh(15);

	auto pColormat = new ColorMaterial;
	gameContext.pMaterialManager->AddMaterial(pColormat, 0);
	//root
	auto root = new GameObject();
	root->GetTransform()->Rotate(0.f, -90.f, 0.f);
	//create bones
	m_pBone0 = new BoneObject(0, 0, 15.f);
	m_pBone1 = new BoneObject(1, 0, 15.f);

	root->AddChild(m_pBone0);
	m_pBone0->AddBone(m_pBone1);
	AddChild(root);

	//meshdrawcomponent
	auto pMeshDrawer = new GameObject;
	m_pMeshDrawer = new MeshDrawComponent(24);
	pMeshDrawer->AddComponent(m_pMeshDrawer);
	AddChild(pMeshDrawer);

	//bindposes
	m_pBone0->CalculateBindPose();

}

void SoftwareSkinningScene_3::Update(const GameContext& gameContext)
{
	XMMATRIX boneTransform0 = XMMatrixMultiply(XMLoadFloat4x4(&m_pBone0->GetBindPose()), XMLoadFloat4x4(&m_pBone0->GetTransform()->GetWorld()));
	XMMATRIX boneTransform1 = XMMatrixMultiply(XMLoadFloat4x4(&m_pBone1->GetBindPose()), XMLoadFloat4x4(&m_pBone1->GetTransform()->GetWorld()));


	m_BoneRotation += m_RotationSign * 45 * gameContext.pGameTime->GetElapsed();
	if (m_RotationSign < 0 && m_BoneRotation <= -45)m_RotationSign = 1;
	if (m_RotationSign > 0 && m_BoneRotation >= 45)m_RotationSign = -1;

	m_pBone0->GetTransform()->Rotate (m_BoneRotation, 0.f,0.f);
	m_pBone1->GetTransform()->Rotate(-m_BoneRotation*2, 0.f, 0.f);

	for (size_t i = 0; i < m_SkinnedVertices.size(); ++i)
	{
		XMMATRIX transform= ( boneTransform0 * m_SkinnedVertices[i].weight0 + boneTransform1 * m_SkinnedVertices[i].weight0 );

		XMFLOAT3 pos = m_SkinnedVertices[i].OriginalVertex.Position;
		XMVECTOR vec = XMVector3Transform(XMLoadFloat3(&pos), transform);
		XMStoreFloat3(&m_SkinnedVertices[i].TransformedVertex.Position, vec);
	}


}

void SoftwareSkinningScene_3::Draw(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);

	m_pMeshDrawer->RemoveTriangles();
	for (size_t i{};i<m_SkinnedVertices.size();i+=4)
	{
		
		QuadPosNormCol quad{ m_SkinnedVertices[i].TransformedVertex,
			m_SkinnedVertices[i + 1].TransformedVertex,
			m_SkinnedVertices[i + 2].TransformedVertex,
			m_SkinnedVertices[i + 3].TransformedVertex };
		m_pMeshDrawer->AddQuad(quad);
	}
	m_pMeshDrawer->UpdateBuffer();
}

void SoftwareSkinningScene_3::SceneActivated()
{
}

void SoftwareSkinningScene_3::SceneDeactivated()
{
}

void SoftwareSkinningScene_3::CreateMesh(float length)
{
	auto pos = XMFLOAT3(length / 2, 0, 0);
	auto offset = XMFLOAT3(length / 2, 2.5f, 2.5f);
	auto col = XMFLOAT4(1, 0, 0, 0.5f);
	//***** //BOX1* //*****
	//FRONT
	auto norm = XMFLOAT3(0, 0, -1);
	m_SkinnedVertices.push_back(SkinnedVertex(XMFLOAT3(-offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z), norm, col));
	m_SkinnedVertices.push_back(SkinnedVertex(XMFLOAT3(offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z), norm, col, 0.5f, 0.5f));
	m_SkinnedVertices.push_back(SkinnedVertex(XMFLOAT3(offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z), norm, col, 0.5f, 0.5f));
	m_SkinnedVertices.push_back(SkinnedVertex(XMFLOAT3(-offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z), norm, col));
	//BACK
	norm = XMFLOAT3(0, 0, 1);
	m_SkinnedVertices.push_back(SkinnedVertex(XMFLOAT3(offset.x + pos.x, offset.y + pos.y, offset.z + pos.z), norm, col, 0.5f, 0.5f));
	m_SkinnedVertices.push_back(SkinnedVertex(XMFLOAT3(-offset.x + pos.x, offset.y + pos.y, offset.z + pos.z), norm, col));
	m_SkinnedVertices.push_back(SkinnedVertex(XMFLOAT3(-offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z), norm, col));
	m_SkinnedVertices.push_back(SkinnedVertex(XMFLOAT3(offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z), norm, col, 0.5f, 0.5f));
	//TOP
	norm = XMFLOAT3(0, 1, 0);
	m_SkinnedVertices.push_back(SkinnedVertex(XMFLOAT3(-offset.x + pos.x, offset.y + pos.y, offset.z + pos.z), norm, col));
	m_SkinnedVertices.push_back(SkinnedVertex(XMFLOAT3(offset.x + pos.x, offset.y + pos.y, offset.z + pos.z), norm, col, 0.5f, 0.5f));
	m_SkinnedVertices.push_back(SkinnedVertex(XMFLOAT3(offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z), norm, col, 0.5f, 0.5f));
	m_SkinnedVertices.push_back(SkinnedVertex(XMFLOAT3(-offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z), norm, col));
	//BOTTOM
	norm = XMFLOAT3(0, -1, 0);
	m_SkinnedVertices.push_back(SkinnedVertex(XMFLOAT3(-offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z), norm, col));
	m_SkinnedVertices.push_back(SkinnedVertex(XMFLOAT3(offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z), norm, col, 0.5f, 0.5f));
	m_SkinnedVertices.push_back(SkinnedVertex(XMFLOAT3(offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z), norm, col, 0.5f, 0.5f));
	m_SkinnedVertices.push_back(SkinnedVertex(XMFLOAT3(-offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z), norm, col));
	//LEFT
	norm = XMFLOAT3(-1, 0, 0);
	m_SkinnedVertices.push_back(SkinnedVertex(XMFLOAT3(-offset.x + pos.x, offset.y + pos.y, offset.z + pos.z), norm, col));
	m_SkinnedVertices.push_back(SkinnedVertex(XMFLOAT3(-offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z), norm, col));
	m_SkinnedVertices.push_back(SkinnedVertex(XMFLOAT3(-offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z), norm, col));
	m_SkinnedVertices.push_back(SkinnedVertex(XMFLOAT3(-offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z), norm, col));
	//RIGHT
	norm = XMFLOAT3(1, 0, 0);
	m_SkinnedVertices.push_back(SkinnedVertex(XMFLOAT3(offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z), norm, col, 0.5f, 0.5f));
	m_SkinnedVertices.push_back(SkinnedVertex(XMFLOAT3(offset.x + pos.x, offset.y + pos.y, offset.z + pos.z), norm, col, 0.5f, 0.5f));
	m_SkinnedVertices.push_back(SkinnedVertex(XMFLOAT3(offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z), norm, col, 0.5f, 0.5f));
	m_SkinnedVertices.push_back(SkinnedVertex(XMFLOAT3(offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z), norm, col, 0.5f, 0.5f));
	//***** //BOX2* //*****
	col = XMFLOAT4(0, 1, 0, 0.5f);
	pos = XMFLOAT3(22.5f, 0, 0);
	//FRONT
	norm = XMFLOAT3(0, 0, -1);
	m_SkinnedVertices.push_back(SkinnedVertex(XMFLOAT3(-offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z), norm, col, 0.5f, 0.5f));
	m_SkinnedVertices.push_back(SkinnedVertex(XMFLOAT3(offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z), norm, col,0,1));
	m_SkinnedVertices.push_back(SkinnedVertex(XMFLOAT3(offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z), norm, col,0,1));
	m_SkinnedVertices.push_back(SkinnedVertex(XMFLOAT3(-offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z), norm, col, 0.5f, 0.5f));
	//BACK
	norm = XMFLOAT3(0, 0, 1);
	m_SkinnedVertices.push_back(SkinnedVertex(XMFLOAT3(offset.x + pos.x, offset.y + pos.y, offset.z + pos.z), norm, col,0,1));
	m_SkinnedVertices.push_back(SkinnedVertex(XMFLOAT3(-offset.x + pos.x, offset.y + pos.y, offset.z + pos.z), norm, col, 0.5f, 0.5f));
	m_SkinnedVertices.push_back(SkinnedVertex(XMFLOAT3(-offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z), norm, col, 0.5f, 0.5f));
	m_SkinnedVertices.push_back(SkinnedVertex(XMFLOAT3(offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z), norm, col,0,1));
	//TOP
	norm = XMFLOAT3(0, 1, 0);
	m_SkinnedVertices.push_back(SkinnedVertex(XMFLOAT3(-offset.x + pos.x, offset.y + pos.y, offset.z + pos.z), norm, col, 0.5f, 0.5f));
	m_SkinnedVertices.push_back(SkinnedVertex(XMFLOAT3(offset.x + pos.x, offset.y + pos.y, offset.z + pos.z), norm, col,0,1));
	m_SkinnedVertices.push_back(SkinnedVertex(XMFLOAT3(offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z), norm, col,0,1));
	m_SkinnedVertices.push_back(SkinnedVertex(XMFLOAT3(-offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z), norm, col, 0.5f, 0.5f));
	//BOTTOM
	norm = XMFLOAT3(0, -1, 0);
	m_SkinnedVertices.push_back(SkinnedVertex(XMFLOAT3(-offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z), norm, col, 0.5f, 0.5f));
	m_SkinnedVertices.push_back(SkinnedVertex(XMFLOAT3(offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z), norm, col,0,1));
	m_SkinnedVertices.push_back(SkinnedVertex(XMFLOAT3(offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z), norm, col,0,1));
	m_SkinnedVertices.push_back(SkinnedVertex(XMFLOAT3(-offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z), norm, col, 0.5f, 0.5f));
	//LEFT
	norm = XMFLOAT3(-1, 0, 0);
	m_SkinnedVertices.push_back(SkinnedVertex(XMFLOAT3(-offset.x + pos.x, offset.y + pos.y, offset.z + pos.z), norm, col, 0.5f, 0.5f));
	m_SkinnedVertices.push_back(SkinnedVertex(XMFLOAT3(-offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z), norm, col, 0.5f, 0.5f));
	m_SkinnedVertices.push_back(SkinnedVertex(XMFLOAT3(-offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z), norm, col, 0.5f, 0.5f));
	m_SkinnedVertices.push_back(SkinnedVertex(XMFLOAT3(-offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z), norm, col, 0.5f, 0.5f));
	//RIGHT
	norm = XMFLOAT3(1, 0, 0);
	m_SkinnedVertices.push_back(SkinnedVertex(XMFLOAT3(offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z), norm, col,0,1));
	m_SkinnedVertices.push_back(SkinnedVertex(XMFLOAT3(offset.x + pos.x, offset.y + pos.y, offset.z + pos.z), norm, col,0,1));
	m_SkinnedVertices.push_back(SkinnedVertex(XMFLOAT3(offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z), norm, col,0,1));
	m_SkinnedVertices.push_back(SkinnedVertex(XMFLOAT3(offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z), norm, col,0,1));
}
