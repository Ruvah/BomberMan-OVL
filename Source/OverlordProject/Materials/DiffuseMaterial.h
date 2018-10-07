#pragma once
#include "Graphics\Material.h"
class TextureData;

class DiffuseMaterial: public Material
{
public:
	DiffuseMaterial(bool enableTransparency = false);
	~DiffuseMaterial();

	void SetDiffuseTexture(const wstring& assetFile);
protected:
	void LoadEffectVariables() override;
	virtual void UpdateEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent) override;

private:
	TextureData * m_pDiffuseTexture;
	static ID3DX11EffectShaderResourceVariable* m_pDiffuseSRVvariable;

	DiffuseMaterial(const DiffuseMaterial & obj) = delete;
	DiffuseMaterial& operator=(const DiffuseMaterial& obj) = delete;
};

