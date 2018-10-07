#include "stdafx.h"
#include "PostVignetting.h"
#include "Graphics/RenderTarget.h"


ID3DX11EffectShaderResourceVariable* PostVignetting::m_pTextureMapVariabele = nullptr;
ID3DX11EffectScalarVariable* PostVignetting::m_pVignetteAmountVar=nullptr;
PostVignetting::PostVignetting() : 
PostProcessingMaterial(L"./Resources/Effects/Post/Vignette.fx")
{
}

void PostVignetting::SetVignette(float amount)
{
	m_VignetteAmount = amount;
}

void PostVignetting::LoadEffectVariables()
{
	// Bind the 'gTexture' variable with 'm_pTextureMapVariable'
	//Check if valid!
	m_pTextureMapVariabele = m_pEffect->GetVariableByName("gTexture")->AsShaderResource();
	if (!m_pTextureMapVariabele->IsValid())
	{
		Logger::LogWarning(L"postvignette-> loadeffectvariables, gTexture is not a valid variable \n");
	}
	m_pVignetteAmountVar = m_pEffect->GetVariableByName("gVignetteAmount")->AsScalar();
	if (!m_pVignetteAmountVar->IsValid())
	{
		Logger::LogWarning(L"postvignette-> loadeffectvariables, gVignetteAmount is not a valid variable \n");
	}
}

void PostVignetting::UpdateEffectVariables(RenderTarget* renderTarget)
{
	//Update the TextureMapVariable with the Color ShaderResourceView of the given RenderTarget
	m_pTextureMapVariabele->SetResource(renderTarget->GetShaderResourceView());
	m_pVignetteAmountVar->SetFloat(m_VignetteAmount);
}
