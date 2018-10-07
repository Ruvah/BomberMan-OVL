#include "stdafx.h"
#include "SkyBoxMaterial.h"
#include "Base/GeneralStructs.h"
#include "Graphics\TextureData.h"
#include "Content\ContentManager.h"
#include "Helpers\EffectHelper.h"

ID3DX11EffectShaderResourceVariable* SkyBoxMaterial::m_pSkyBoxSRVvariable = nullptr;

SkyBoxMaterial::SkyBoxMaterial(bool enableTransparency )
:Material(L"./Resources/Effects/SkyBox.fx", enableTransparency ? L"TransparencyTech" : L"")
{
}


SkyBoxMaterial::~SkyBoxMaterial()
{
}

void SkyBoxMaterial::SetDiffuseTexture(const wstring & assetFile)
{
	m_pSkyBoxTexture = ContentManager::Load<TextureData>(assetFile);
}

void SkyBoxMaterial::LoadEffectVariables()
{
	m_pSkyBoxSRVvariable = m_pEffect->GetVariableByName("m_CubeMap")->AsShaderResource();
}

void SkyBoxMaterial::UpdateEffectVariables(const GameContext & gameContext, ModelComponent * pModelComponent)
{
	UNREFERENCED_PARAMETER(gameContext);

	UNREFERENCED_PARAMETER(pModelComponent);

	m_pSkyBoxSRVvariable->SetResource(m_pSkyBoxTexture->GetShaderResourceView());
}
