#pragma once
#include "GameObject.h"

class BoneObject : public GameObject
{
public:
    BoneObject(int boneId, int materialId, float length = 5.0f);
    ~BoneObject();

    void AddBone(BoneObject* pBone);
    XMFLOAT4X4 GetBindPose() const;
    void CalculateBindPose();

protected:
    virtual void Initialize(const GameContext& gameContext) override;

private:
    float m_Length;
    int m_BoneId;
    int m_MaterialId;
    XMFLOAT4X4 m_BindPose;
};

