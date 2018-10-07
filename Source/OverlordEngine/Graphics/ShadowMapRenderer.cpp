#include "stdafx.h"
#include "ShadowMapRenderer.h"
#include "RenderTarget.h"
#include "OverlordGame.h"
#include "MeshFilter.h"
#include "ShadowMapMaterial.h"
#include "../Scenegraph/SceneManager.h"
#include "ShadowMapMaterial_Skinned.h"

ShadowMapRenderer::ShadowMapRenderer()
{
}


ShadowMapRenderer::~ShadowMapRenderer()
{
    SafeDelete(m_pShadowRT);
    SafeDelete(m_pShadowMaterial);
    SafeDelete(m_pShadowSkinnedMaterial);
}

void ShadowMapRenderer::Initialize(const GameContext& gameContext)
{
    m_pShadowRT = new RenderTarget{ gameContext.pDevice };
    RENDERTARGET_DESC desc{};
    desc.EnableDepthBuffer = true;
    desc.EnableDepthSRV = true;
    desc.EnableColorBuffer = false;
    desc.EnableColorSRV = false;
    desc.Width = OverlordGame::GetGameSettings().Window.Width;
    desc.Height = OverlordGame::GetGameSettings().Window.Height;
    m_pShadowRT->Create(desc);

    m_pShadowMaterial = new ShadowMapMaterial{};
    m_pShadowMaterial->Initialize(gameContext);

    m_pShadowSkinnedMaterial = new ShadowMapMaterial_Skinned{};
    m_pShadowSkinnedMaterial->Initialize(gameContext);
}

void ShadowMapRenderer::Begin(const GameContext& gameContext)
{
    gameContext.pDeviceContext->OMSetRenderTargets(0, 0, m_pShadowRT->GetDepthStencilView());
    gameContext.pDeviceContext->ClearDepthStencilView(m_pShadowRT->GetDepthStencilView(), D3D11_CLEAR_DEPTH, 1.0f, 0);
} 

void ShadowMapRenderer::Draw(const GameContext& gameContext, MeshFilter* pMeshFilter)
{
    Material* activeMaterial{};

    if (pMeshFilter->m_AnimationClips.empty())
        activeMaterial = m_pShadowMaterial;
    else
        activeMaterial = m_pShadowSkinnedMaterial;

    m_pShadowMaterial->SetLightVP(m_LightVP);
    m_pShadowSkinnedMaterial->SetLightVP(m_LightVP);

    pMeshFilter->BuildVertexBuffer(gameContext, activeMaterial);

    //Set Inputlayout
    gameContext.pDeviceContext->IASetInputLayout(activeMaterial->GetInputLayout());
   
    //Set Vertex Buffer
    UINT offset = 0;
    auto vertexBufferData = pMeshFilter->GetVertexBufferData(gameContext, activeMaterial);
    gameContext.pDeviceContext->IASetVertexBuffers(0, 1, &vertexBufferData.pVertexBuffer, &vertexBufferData.VertexStride, &offset);

    //Set Index Buffer
    gameContext.pDeviceContext->IASetIndexBuffer(pMeshFilter->m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

    //Set Primitive Topology
    gameContext.pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    //DRAW
    auto tech = activeMaterial->GetTechnique();
    D3DX11_TECHNIQUE_DESC techDesc;
    tech->GetDesc(&techDesc);
    for (UINT p = 0; p < techDesc.Passes; ++p)
    {
        tech->GetPassByIndex(p)->Apply(0, gameContext.pDeviceContext);
        gameContext.pDeviceContext->DrawIndexed(pMeshFilter->m_IndexCount, 0, 0);
    }
}

void ShadowMapRenderer::End(const GameContext& /*gameContext*/)
{
    RenderTarget* pInitRt = SceneManager::GetInstance()->GetGame()->GetRenderTarget();
    SceneManager::GetInstance()->GetGame()->SetRenderTarget(pInitRt);
}

Material *ShadowMapRenderer::GetMaterial(bool isSkinned)
{
    if (isSkinned) return m_pShadowSkinnedMaterial;
    else return m_pShadowMaterial;
}

ID3D11ShaderResourceView* ShadowMapRenderer::GetShadowMap()
{
    return m_pShadowRT->GetDepthShaderResourceView();
}

void ShadowMapRenderer::SetLight(const XMFLOAT3& position, const XMFLOAT3& direction)
{
    m_LightPosition = position;
    m_LightDirection = direction;

    XMMATRIX p = XMMatrixPerspectiveFovLH(0.78f, 16.0f / 9.0f, 0.1f, 2500.0f);
    //XMMATRIX p = XMMatrixOrthographicLH(25.0f * 16.0f/9.0f, 25.0f, 0.1f, 2500.0f);

    XMVECTOR worldPosition = XMLoadFloat3(&m_LightPosition);
    XMVECTOR dir = XMLoadFloat3(&m_LightDirection);
    XMMATRIX v = XMMatrixLookAtLH(worldPosition, worldPosition + dir, { 0,1,0 });

    XMStoreFloat4x4(&m_LightVP, v * p);
}

XMFLOAT3 ShadowMapRenderer::GetLightDirection()
{
    return m_LightDirection;
}

XMFLOAT4X4 ShadowMapRenderer::GetLightVP()
{
    return m_LightVP;
}
