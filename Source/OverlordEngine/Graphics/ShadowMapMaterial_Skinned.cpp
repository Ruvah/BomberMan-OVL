#include "stdafx.h"
#include "ShadowMapMaterial_Skinned.h"
#include "../Components/ModelComponent.h"
#include "ModelAnimator.h"

ID3DX11EffectMatrixVariable* ShadowMapMaterial_Skinned::m_pLightVPVar{ nullptr };
ID3DX11EffectMatrixVariable* ShadowMapMaterial_Skinned::m_pBoneTransformsVariable{ nullptr };

ShadowMapMaterial_Skinned::ShadowMapMaterial_Skinned()
    : Material{L"Resources/Effects/Shadow/ShadowMap_Skinned.fx"} 
{
}

ShadowMapMaterial_Skinned::~ShadowMapMaterial_Skinned()
{
}

void ShadowMapMaterial_Skinned::SetLightVP(const XMFLOAT4X4& lightVP)
{
    m_LightVP = lightVP;
}

void ShadowMapMaterial_Skinned::LoadEffectVariables()
{
    m_pLightVPVar = m_pEffect->GetVariableByName("gLightVP")->AsMatrix();
    m_pBoneTransformsVariable = m_pEffect->GetVariableByName("gBones")->AsMatrix();
}

void ShadowMapMaterial_Skinned::UpdateEffectVariables(const GameContext& /*gameContext*/, ModelComponent* pModelComponent)
{
    m_pLightVPVar->SetMatrix(reinterpret_cast<float*>(&m_LightVP));
    m_pBoneTransformsVariable->SetMatrixArray( &pModelComponent->GetAnimator()->GetBoneTransforms().data()->_11, 0, pModelComponent->GetAnimator()->GetBoneTransforms().size());
}
