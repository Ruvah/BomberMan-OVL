#pragma once
#include "Graphics\Material.h"
class TextureData;

class SkyBoxMaterial: public Material
{
public:
	SkyBoxMaterial(bool enableTransparency = false);
	~SkyBoxMaterial();

	void SetDiffuseTexture(const wstring& assetFile);
protected:
	void LoadEffectVariables() override;
	virtual void UpdateEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent) override;

private:
	TextureData * m_pSkyBoxTexture;
	static ID3DX11EffectShaderResourceVariable* m_pSkyBoxSRVvariable;

	SkyBoxMaterial(const SkyBoxMaterial & obj) = delete;
	SkyBoxMaterial& operator=(const SkyBoxMaterial& obj) = delete;
};

