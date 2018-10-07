#include "stdafx.h"
#include "DiffuseMaterial_Shadow.h"
#include "Graphics/TextureData.h"
#include "Content/ContentManager.h"
#include "Graphics/ShadowMapRenderer.h"

ID3DX11EffectShaderResourceVariable* DiffuseMaterial_Shadow::m_pDiffuseSRVVariable{};
ID3DX11EffectShaderResourceVariable* DiffuseMaterial_Shadow::m_pShadowMapVariable{};
ID3DX11EffectMatrixVariable* DiffuseMaterial_Shadow::m_pLightVPVar{};
ID3DX11EffectVectorVariable* DiffuseMaterial_Shadow::m_pLightDirection{};

DiffuseMaterial_Shadow::DiffuseMaterial_Shadow()
    : Material{L"Resources/Effects/Shadow/PosNormTex3D_Shadow.fx"}
{
}

DiffuseMaterial_Shadow::~DiffuseMaterial_Shadow()
{
}

void DiffuseMaterial_Shadow::SetDiffuseTexture(const std::wstring& assetFile)
{
    m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile);
}

void DiffuseMaterial_Shadow::LoadEffectVariables()
{
    m_pDiffuseSRVVariable = m_pEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();
    m_pShadowMapVariable = m_pEffect->GetVariableByName("gShadowMap")->AsShaderResource();
    m_pLightVPVar = m_pEffect->GetVariableByName("gLightVP")->AsMatrix();
    m_pLightDirection = m_pEffect->GetVariableByName("gLightDirection")->AsVector();
}

void DiffuseMaterial_Shadow::UpdateEffectVariables(const GameContext& gameContext, ModelComponent* /*pModelComponent*/)
{
    m_pDiffuseSRVVariable->SetResource(m_pDiffuseTexture->GetShaderResourceView());
    m_pShadowMapVariable->SetResource(gameContext.pShadowMapRenderer->GetShadowMap());
    XMFLOAT4X4 lvp = gameContext.pShadowMapRenderer->GetLightVP();
    m_pLightVPVar->SetMatrix(reinterpret_cast<const float*>(&lvp));
    XMFLOAT3 lightDir = gameContext.pShadowMapRenderer->GetLightDirection();
    m_pLightDirection->SetFloatVector(reinterpret_cast<const float*>(&lightDir));
}
