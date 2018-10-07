#include "stdafx.h"
#include "DiffuseMaterial.h"
#include "Base/GeneralStructs.h"
#include "Graphics\TextureData.h"
#include "Content\ContentManager.h"
#include "Helpers\EffectHelper.h"

ID3DX11EffectShaderResourceVariable* DiffuseMaterial::m_pDiffuseSRVvariable = nullptr;

DiffuseMaterial::DiffuseMaterial(bool enableTransparency )
:Material(L"./Resources/Effects/PosNormTex3D.fx", enableTransparency ? L"TransparencyTech" : L"")
{
}


DiffuseMaterial::~DiffuseMaterial()
{
}

void DiffuseMaterial::SetDiffuseTexture(const wstring & assetFile)
{
	m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile);
}

void DiffuseMaterial::LoadEffectVariables()
{
	m_pDiffuseSRVvariable = m_pEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();
}

void DiffuseMaterial::UpdateEffectVariables(const GameContext & gameContext, ModelComponent * pModelComponent)
{
	UNREFERENCED_PARAMETER(gameContext);

	UNREFERENCED_PARAMETER(pModelComponent);

	m_pDiffuseSRVvariable->SetResource(m_pDiffuseTexture->GetShaderResourceView());
}
