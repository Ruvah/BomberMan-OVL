#pragma once
#include "Graphics\Material.h"

class TextureData;

class SkinnedDiffuseMaterial_Shadow : public Material
{
public:
    SkinnedDiffuseMaterial_Shadow();
	~SkinnedDiffuseMaterial_Shadow();

	void SetDiffuseTexture(const wstring& assetFile);

protected:
	virtual void LoadEffectVariables();
	virtual void UpdateEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent);

private:

	TextureData* m_pDiffuseTexture;
	static ID3DX11EffectShaderResourceVariable* m_pDiffuseSRVvariable;
	static ID3DX11EffectMatrixVariable* m_pBoneTransformsVariable;
    static ID3DX11EffectShaderResourceVariable* m_pShadowMapVariable;
    static ID3DX11EffectMatrixVariable* m_pLightVPVar;
    static ID3DX11EffectVectorVariable* m_pLightDirection;
};

