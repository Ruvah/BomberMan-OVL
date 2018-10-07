#pragma once
#include "Graphics/Material.h"

class DiffuseMaterial_Shadow : public Material
{
public:
    DiffuseMaterial_Shadow();
    ~DiffuseMaterial_Shadow();

    void SetDiffuseTexture(const std::wstring& assetFile);

protected:
    virtual void LoadEffectVariables() override;
    virtual void UpdateEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent) override;

private:
    class TextureData* m_pDiffuseTexture;
    static ID3DX11EffectShaderResourceVariable* m_pDiffuseSRVVariable;
    static ID3DX11EffectShaderResourceVariable* m_pShadowMapVariable;
    static ID3DX11EffectMatrixVariable* m_pLightVPVar;
    static ID3DX11EffectVectorVariable* m_pLightDirection;
};

