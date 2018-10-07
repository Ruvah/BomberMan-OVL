//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"
#include "PostBlur.h"
#include "Graphics/RenderTarget.h"

PostBlur::PostBlur()
	: PostProcessingMaterial(L"./Resources/Effects/Post/Blur.fx"),
	m_pTextureMapVariabele(nullptr)
{
}

PostBlur::~PostBlur(void)
{
}

void PostBlur::LoadEffectVariables()
{
	//Bind the 'gTexture' variable with 'm_pTextureMapVariable'
	//Check if valid!
	m_pTextureMapVariabele = m_pEffect->GetVariableByName("gTexture")->AsShaderResource();
	if (!m_pTextureMapVariabele->IsValid())
	{
		Logger::LogWarning(L"postblur-> loadeffectvariables, gtexture is not a valid variable \n");
	}
}

void PostBlur::UpdateEffectVariables(RenderTarget* rendertarget)
{
	//Update the TextureMapVariable with the Color ShaderResourceView of the given RenderTarget
	m_pTextureMapVariabele->SetResource(rendertarget->GetShaderResourceView());

}