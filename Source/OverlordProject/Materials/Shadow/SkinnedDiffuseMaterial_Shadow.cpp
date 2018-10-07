//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "SkinnedDiffuseMaterial_Shadow.h"
#include "Base/GeneralStructs.h"
#include "Diagnostics/Logger.h"
#include "Content/ContentManager.h"
#include "Graphics/TextureData.h"
#include "Components/ModelComponent.h"
#include "Graphics/ModelAnimator.h"
#include "Graphics/ShadowMapRenderer.h"

ID3DX11EffectShaderResourceVariable* SkinnedDiffuseMaterial_Shadow::m_pDiffuseSRVvariable = nullptr;
ID3DX11EffectMatrixVariable* SkinnedDiffuseMaterial_Shadow::m_pBoneTransformsVariable = nullptr;
ID3DX11EffectShaderResourceVariable* SkinnedDiffuseMaterial_Shadow::m_pShadowMapVariable{};
ID3DX11EffectMatrixVariable* SkinnedDiffuseMaterial_Shadow::m_pLightVPVar{};
ID3DX11EffectVectorVariable* SkinnedDiffuseMaterial_Shadow::m_pLightDirection{};

SkinnedDiffuseMaterial_Shadow::SkinnedDiffuseMaterial_Shadow() : Material(L"./Resources/Effects/Shadow/PosNormTex3D_Skinned_Shadow.fx"),
	m_pDiffuseTexture(nullptr)
{
}


SkinnedDiffuseMaterial_Shadow::~SkinnedDiffuseMaterial_Shadow()
{

}

void SkinnedDiffuseMaterial_Shadow::SetDiffuseTexture(const wstring& assetFile)
{
	m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile);
}

void SkinnedDiffuseMaterial_Shadow::LoadEffectVariables()
{
	if (!m_pDiffuseSRVvariable)
	{
		m_pDiffuseSRVvariable = m_pEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();
		if (!m_pDiffuseSRVvariable->IsValid())
		{
			Logger::LogWarning(L"SkinnedDiffuseMaterial::LoadEffectVariables() > \'gDiffuseMap\' variable not found!");
			m_pDiffuseSRVvariable = nullptr;
		}
	}

	//Create a link to the gBones variable
    if (!m_pBoneTransformsVariable)
    {
        m_pBoneTransformsVariable = m_pEffect->GetVariableByName("gBones")->AsMatrix();
        if (!m_pBoneTransformsVariable->IsValid())
        {
            Logger::LogWarning(L"SkinnedDiffuseMaterial::LoadEffectVariables() > \'gBones\' variable not found!");
            m_pBoneTransformsVariable = nullptr;
        }
    }

    m_pShadowMapVariable = m_pEffect->GetVariableByName("gShadowMap")->AsShaderResource();
    m_pLightVPVar = m_pEffect->GetVariableByName("gLightVP")->AsMatrix();
    m_pLightDirection = m_pEffect->GetVariableByName("gLightDirection")->AsVector();
}

void SkinnedDiffuseMaterial_Shadow::UpdateEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent)
{
	UNREFERENCED_PARAMETER(gameContext);
	UNREFERENCED_PARAMETER(pModelComponent);

	if (m_pDiffuseTexture && m_pDiffuseSRVvariable)
	{
		m_pDiffuseSRVvariable->SetResource(m_pDiffuseTexture->GetShaderResourceView());
	}

	//Set the matrix array (BoneTransforms of the ModelAnimator)
    if (m_pBoneTransformsVariable)
    {
        m_pBoneTransformsVariable->SetMatrixArray(
            &pModelComponent->GetAnimator()->GetBoneTransforms().data()->_11, 
            0,
            pModelComponent->GetAnimator()->GetBoneTransforms().size()
        );
    }

    m_pShadowMapVariable->SetResource(gameContext.pShadowMapRenderer->GetShadowMap());
    XMFLOAT4X4 lvp = gameContext.pShadowMapRenderer->GetLightVP();
    m_pLightVPVar->SetMatrix(reinterpret_cast<const float*>(&lvp));
    XMFLOAT3 lightDir = gameContext.pShadowMapRenderer->GetLightDirection();
    m_pLightDirection->SetFloatVector(reinterpret_cast<const float*>(&lightDir));
}
