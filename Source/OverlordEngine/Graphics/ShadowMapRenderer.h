#pragma once

class ShadowMapMaterial_Skinned;
class ShadowMapMaterial;
class MeshFilter;
class RenderTarget;

class ShadowMapRenderer
{
public:
    ShadowMapRenderer();
    ~ShadowMapRenderer();

    void Initialize(const GameContext& gameContext);
    void Begin(const GameContext& gameContext);
    void Draw(const GameContext& gameContext, MeshFilter* pMeshFilter);
    void End(const GameContext& gameContext);
    Material* GetMaterial(bool isSkinned);
    ID3D11ShaderResourceView* GetShadowMap();
    void SetLight(const XMFLOAT3& position, const XMFLOAT3& direction);
    XMFLOAT3 GetLightDirection();
    XMFLOAT4X4 GetLightVP();
    
private:
    RenderTarget * m_pShadowRT;
    ShadowMapMaterial* m_pShadowMaterial;
    ShadowMapMaterial_Skinned* m_pShadowSkinnedMaterial;
    XMFLOAT4X4 m_LightVP;
    XMFLOAT3 m_LightDirection;
    XMFLOAT3 m_LightPosition;
};

