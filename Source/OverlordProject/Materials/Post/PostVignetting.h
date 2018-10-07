#pragma once
#include "Graphics/PostProcessingMaterial.h"


class PostVignetting:public PostProcessingMaterial
{
public:
	PostVignetting();
	~PostVignetting()=default;

	void SetVignette(float amount);
protected:
	void LoadEffectVariables() override;
	void UpdateEffectVariables(RenderTarget* renderTarget)override;
	static ID3DX11EffectShaderResourceVariable * m_pTextureMapVariabele;
	float m_VignetteAmount;
	static ID3DX11EffectScalarVariable* m_pVignetteAmountVar;

};

