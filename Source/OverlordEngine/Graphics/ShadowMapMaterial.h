#pragma once
#include "Material.h"

class ShadowMapMaterial : public Material
{
public:
    ShadowMapMaterial();
    ~ShadowMapMaterial();

    void SetLightVP(const XMFLOAT4X4& lightVP);

protected:
    virtual void LoadEffectVariables() override;
    virtual void UpdateEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent) override;

private:
    static ID3DX11EffectMatrixVariable* m_pLightVPVar;

    XMFLOAT4X4 m_LightVP;
};

