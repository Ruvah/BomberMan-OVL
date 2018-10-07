#include "stdafx.h"
#include "ShadowMapMaterial.h"
#include "../Components/ModelComponent.h"
#include "ModelAnimator.h"

ID3DX11EffectMatrixVariable* ShadowMapMaterial::m_pLightVPVar{ nullptr };

ShadowMapMaterial::ShadowMapMaterial()
    : Material{L"Resources/Effects/Shadow/ShadowMap.fx"} 
{
}

ShadowMapMaterial::~ShadowMapMaterial()
{
}

void ShadowMapMaterial::SetLightVP(const XMFLOAT4X4& lightVP)
{
    m_LightVP = lightVP;
}

void ShadowMapMaterial::LoadEffectVariables()
{
    m_pLightVPVar = m_pEffect->GetVariableByName("gLightVP")->AsMatrix();
}

void ShadowMapMaterial::UpdateEffectVariables(const GameContext& /*gameContext*/, ModelComponent* /*pModelComponent*/)
{
    m_pLightVPVar->SetMatrix(reinterpret_cast<float*>(&m_LightVP));
}
