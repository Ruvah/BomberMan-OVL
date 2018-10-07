#include "stdafx.h"
#include "BoneObject.h"
#include "../Components/ModelComponent.h"
#include "../Components/TransformComponent.h"

BoneObject::BoneObject(int boneId, int materialId, float length)
    : m_Length{length}  
    , m_BoneId{boneId}
    , m_MaterialId{materialId}
{
}

BoneObject::~BoneObject()
{
}

void BoneObject::AddBone(BoneObject* pBone)
{
    pBone->GetTransform()->Translate(0, 0, -m_Length);
    AddChild(pBone);
}

XMFLOAT4X4 BoneObject::GetBindPose() const
{
    return m_BindPose;
}

void BoneObject::CalculateBindPose()
{
    XMFLOAT4X4 world = GetTransform()->GetWorld();
    XMMATRIX inverseWorld = XMLoadFloat4x4(&world);
    inverseWorld = XMMatrixInverse(nullptr, inverseWorld);
    XMStoreFloat4x4(&m_BindPose, inverseWorld);

    for (auto child : GetChildren<BoneObject>())
    {
        child->CalculateBindPose();
    }
}

void BoneObject::Initialize(const GameContext& /*gameContext*/)
{
    ModelComponent* model = new ModelComponent{ L"Resources/Meshes/Bone.ovm" };
    model->SetMaterial(m_MaterialId);

    GameObject* go = new GameObject{};
    go->AddComponent(model);
    go->GetTransform()->Scale(m_Length, m_Length, m_Length);
    AddChild(go);
}
