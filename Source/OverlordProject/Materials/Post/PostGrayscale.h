#pragma once
#include "Graphics/PostProcessingMaterial.h"

class ID3D11EffectShaderResourceVariable;

class PostGrayscale : public PostProcessingMaterial
{
public:
	PostGrayscale();
	~PostGrayscale();

protected:
	virtual void LoadEffectVariables();
	virtual void UpdateEffectVariables(RenderTarget* rendertarget);

	ID3DX11EffectShaderResourceVariable* m_pTextureMapVariabele;

private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	PostGrayscale(const PostGrayscale &obj);
	PostGrayscale& operator=(const PostGrayscale& obj);
};
