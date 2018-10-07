#include "stdafx.h"
#include "FurMaterial.h"
#include "Graphics\TextureData.h"
#include "Content\ContentManager.h"
#include "Base/GeneralStructs.h"
#include "Graphics\TextureData.h"
#include "Helpers\EffectHelper.h"

ID3DX11EffectVectorVariable* FurMaterial::m_pLightDirVariable{nullptr};
ID3DX11EffectShaderResourceVariable* FurMaterial::m_pDiffuseSRVvariable{nullptr};
ID3DX11EffectShaderResourceVariable* FurMaterial::m_pShellSRVvariable{ nullptr };
ID3DX11EffectShaderResourceVariable* FurMaterial::m_pFinSRVvariable{ nullptr };

ID3DX11EffectVectorVariable* FurMaterial::m_pDiffuseColorVariable{ nullptr };
ID3DX11EffectScalarVariable* FurMaterial::m_pHairLengthVariable{nullptr};
ID3DX11EffectVectorVariable* FurMaterial::m_pGravityVariable{nullptr};
ID3DX11EffectScalarVariable* FurMaterial::m_pGravStrVar{ nullptr };
ID3DX11EffectVectorVariable* FurMaterial::m_pForceVariable{ nullptr };
ID3DX11EffectScalarVariable* FurMaterial::m_pForceStrVar{ nullptr };
ID3DX11EffectScalarVariable* FurMaterial::m_pLayerVar{ nullptr };
ID3DX11EffectScalarVariable* FurMaterial::m_pUVVar{ nullptr };
ID3DX11EffectScalarVariable* FurMaterial::m_pFinToShellVar{ nullptr };
ID3DX11EffectScalarVariable* FurMaterial::m_pFThickVar{ nullptr };
ID3DX11EffectScalarVariable* FurMaterial::m_pMinLengthVar{nullptr};



FurMaterial::FurMaterial()
	:Material(L"./Resources/Effects/Fur.fx")
{
}


FurMaterial::~FurMaterial()
{
}

void FurMaterial::SetLightDir(XMFLOAT3 direction)
{
	m_LightDir = direction;
}

void FurMaterial::SetDiffuseTexture(const wstring & assetFile)
{
	m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile);
}

void FurMaterial::SetDiffuseColor(XMFLOAT4 color)
{
	m_ColorDiffuse = color;
}

void FurMaterial::SetUVScale(float UVScale)
{
	m_UVScale = UVScale;
}

void FurMaterial::SetFurLength(float length)
{
	m_Length = length;
}

void FurMaterial::SetGravity(XMFLOAT3 gravity)
{
	m_Gravity = gravity;
}

void FurMaterial::SetGravityStrength(float strength)
{
	m_GravStrength = strength;
}

void FurMaterial::SetForceDirection(XMFLOAT3 Direction)
{
	m_Force = Direction;
}

void FurMaterial::SetForceStrength(float strength)
{
	m_ForceStrength = strength;
}

void FurMaterial::SetMinHairPercent(float min)
{
	m_MinHairPercent = min;
}

void FurMaterial::SetLayer(float layer)
{
	m_Layers = (int)layer;
}

void FurMaterial::SetThickness(float thickness)
{
	m_FurThickness = thickness;
}

void FurMaterial::SetShellTex(const wstring & assetFile)
{
	m_pShellTexture = ContentManager::Load<TextureData>(assetFile);

}

void FurMaterial::SetFinToShellRatio(float ratio)
{
	m_FinToShellRatio = ratio;
}

void FurMaterial::SetFinTexture(const wstring & assetFile)
{
	m_pFinTexture = ContentManager::Load<TextureData>(assetFile);

}

void FurMaterial::LoadEffectVariables()
{
	m_pDiffuseColorVariable = m_pEffect->GetVariableByName("gColorDiffuse")->AsVector();
	m_pDiffuseSRVvariable = m_pEffect->GetVariableByName("gFurTexture")->AsShaderResource();
	m_pUVVar = m_pEffect->GetVariableByName("gUVScale")->AsScalar();
	m_pLightDirVariable = m_pEffect->GetVariableByName("gLightDir")->AsVector();
	m_pHairLengthVariable = m_pEffect->GetVariableByName("gFurLength")->AsScalar();
	m_pGravityVariable = m_pEffect->GetVariableByName("gGravity")->AsVector();
	m_pGravStrVar = m_pEffect->GetVariableByName("gGravityForce")->AsScalar();
	m_pForceVariable = m_pEffect->GetVariableByName("gForceDirection")->AsVector();
	m_pForceStrVar = m_pEffect->GetVariableByName("gForceStrength")->AsScalar();
	m_pLayerVar = m_pEffect->GetVariableByName("gLayers")->AsScalar();
	m_pFThickVar = m_pEffect->GetVariableByName("gHairThickness")->AsScalar();
	m_pShellSRVvariable = m_pEffect->GetVariableByName("gShellTexture")->AsShaderResource();
	m_pFinToShellVar = m_pEffect->GetVariableByName("gFinToShellRatio")->AsScalar();
	m_pFinSRVvariable = m_pEffect->GetVariableByName("gFinTexture")->AsShaderResource();
	m_pMinLengthVar = m_pEffect->GetVariableByName("gLengthMinimumPercentage")->AsScalar();

}

void FurMaterial::UpdateEffectVariables(const GameContext & gameContext, ModelComponent * pModelComponent)
{
	UNREFERENCED_PARAMETER(gameContext);

	UNREFERENCED_PARAMETER(pModelComponent);
	m_pDiffuseColorVariable->SetFloatVector(reinterpret_cast<float*>(&m_ColorDiffuse));
	m_pDiffuseSRVvariable->SetResource(m_pDiffuseTexture->GetShaderResourceView());
	m_pUVVar->SetFloat(m_UVScale);
	m_pLightDirVariable->SetFloatVector(reinterpret_cast<float*>(&m_LightDir));
	m_pHairLengthVariable->SetFloat((m_Length));
	m_pGravityVariable->SetFloatVector(reinterpret_cast<float*>(&m_Gravity));
	m_pGravStrVar->SetFloat(m_GravStrength);
	m_pForceVariable->SetFloatVector(reinterpret_cast<float*>(&m_Force));
	m_pForceStrVar->SetFloat(m_ForceStrength);
	m_pShellSRVvariable->SetResource(m_pShellTexture->GetShaderResourceView());
	m_pFThickVar->SetFloat(m_FurThickness);
	m_pFinSRVvariable->SetResource(m_pFinTexture->GetShaderResourceView());
	m_pFinToShellVar->SetFloat(m_FinToShellRatio);
	m_pLayerVar->SetFloat(float(m_Layers));
	m_pMinLengthVar->SetFloat(m_MinHairPercent);
}
