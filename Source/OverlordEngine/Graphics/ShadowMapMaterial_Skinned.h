#pragma once
#include "Material.h"

class ShadowMapMaterial_Skinned : public Material
{
public:
    ShadowMapMaterial_Skinned();
    ~ShadowMapMaterial_Skinned();

    void SetLightVP(const XMFLOAT4X4& lightVP);

protected:
    virtual void LoadEffectVariables() override;
    virtual void UpdateEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent) override;

private:
    static ID3DX11EffectMatrixVariable* m_pLightVPVar;
    static ID3DX11EffectMatrixVariable* m_pBoneTransformsVariable;

    XMFLOAT4X4 m_LightVP;
};

