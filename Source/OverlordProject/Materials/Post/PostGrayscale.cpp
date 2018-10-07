//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"
#include "PostGrayscale.h"
#include "Graphics/RenderTarget.h"

PostGrayscale::PostGrayscale()
	: PostProcessingMaterial(L"./Resources/Effects/Post/Grayscale.fx"),
	m_pTextureMapVariabele(nullptr)
{
}

PostGrayscale::~PostGrayscale(void)
{
}

void PostGrayscale::LoadEffectVariables()
{
	//Bind the 'gTexture' variable with 'm_pTextureMapVariable'
	//Check if valid!
	m_pTextureMapVariabele=  m_pEffect->GetVariableByName("gTexture")->AsShaderResource();
	if (!m_pTextureMapVariabele->IsValid())
	{
		Logger::LogWarning(L"postgrayscale-> loadeffectvariables, gtexture is not a valid variable \n");
	}
}

void PostGrayscale::UpdateEffectVariables(RenderTarget* rendertarget)
{
	//Update the TextureMapVariable with the Color ShaderResourceView of the given RenderTarget
	m_pTextureMapVariabele->SetResource(rendertarget->GetShaderResourceView());
}