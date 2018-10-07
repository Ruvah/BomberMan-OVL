//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "ParticleEmitterComponent.h"
#include "../Helpers/EffectHelper.h"
#include "../Content/ContentManager.h"
#include "../Content/TextureDataLoader.h"
#include "../Graphics/Particle.h"
#include "../Components/TransformComponent.h"
#include "../Diagnostics/Logger.h"
#include <numeric>

ParticleEmitterComponent::ParticleEmitterComponent(const wstring& assetFile, int particleCount) :
	m_ParticleCount(particleCount),
	m_AssetFile(assetFile)
    , m_pWvpVariable{}
{
    for (int i = 0; i < m_ParticleCount; ++i)
    {
        m_Particles.emplace_back(new Particle{ m_Settings });
    }
}


ParticleEmitterComponent::~ParticleEmitterComponent(void)
{
    for (Particle* particle : m_Particles)
        delete particle;

    SafeRelease(m_pVertexBuffer);
    SafeRelease(m_pInputLayout);
}

void ParticleEmitterComponent::Initialize(const GameContext& gameContext)
{
    LoadEffect(gameContext);

    CreateVertexBuffer(gameContext);

    m_pParticleTexture = ContentManager::Load<TextureData>(m_AssetFile);
}

void ParticleEmitterComponent::LoadEffect(const GameContext& gameContext)
{
    m_pEffect = ContentManager::Load<ID3DX11Effect>(L"./Resources/Effects/ParticleRenderer.fx");
    m_pDefaultTechnique = m_pEffect->GetTechniqueByIndex(0);

    if (!m_pWvpVariable)
        m_pWvpVariable = m_pEffect->GetVariableBySemantic("WorldViewProjection")->AsMatrix();

    if (!m_pViewInverseVariable)
        m_pViewInverseVariable = m_pEffect->GetVariableBySemantic("ViewInverse")->AsMatrix();

    if (!m_pParticleTexture)
        m_pTextureVariable = m_pEffect->GetVariableByName("gParticleTexture")->AsShaderResource();


    vector<ILDescription> descs{};
    UINT id{};
    EffectHelper::BuildInputLayout(gameContext.pDevice, m_pDefaultTechnique, &m_pInputLayout, descs, m_pInputLayoutSize, id);
}

void ParticleEmitterComponent::CreateVertexBuffer(const GameContext& gameContext)
{
    SafeRelease(m_pVertexBuffer);

    D3D11_BUFFER_DESC buffer = {};
    buffer.Usage = D3D11_USAGE_DYNAMIC;
    buffer.ByteWidth = m_ParticleCount * sizeof(ParticleVertex);
    buffer.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    buffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    buffer.MiscFlags = 0;

    HRESULT hr = gameContext.pDevice->CreateBuffer(&buffer, nullptr, &m_pVertexBuffer);

    Logger::LogHResult(hr, L"ParticleEmitterComponent::CreateVertexBuffer: Failed to Create VertexBuffer");
}

void ParticleEmitterComponent::Update(const GameContext& gameContext)
{
    float particleInterval = (m_Settings.MaxEnergy - m_Settings.MinEnergy) / m_ParticleCount;
    m_LastParticleInit += gameContext.pGameTime->GetElapsed();
    m_ActiveParticles = 0;

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    gameContext.pDeviceContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    ParticleVertex* pBuffer = static_cast<ParticleVertex*>(mappedResource.pData);

    for (Particle* particle : m_Particles)
    {
        particle->Update(gameContext);
        if(particle->IsActive())
        {
            pBuffer[m_ActiveParticles++] = particle->GetVertexInfo();
        }
        else if(m_LastParticleInit >= particleInterval)
        {
            particle->Init(GetTransform()->GetPosition());
            pBuffer[m_ActiveParticles++] = particle->GetVertexInfo();
            m_LastParticleInit = 0.0f;
        }
    }
    gameContext.pDeviceContext->Unmap(m_pVertexBuffer, 0);
}

void ParticleEmitterComponent::Draw(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
}

void ParticleEmitterComponent::PostDraw(const GameContext& gameContext)
{
    XMMATRIX viewProjection = XMLoadFloat4x4(&gameContext.pCamera->GetViewProjection());
    m_pWvpVariable->SetMatrix(reinterpret_cast<float*>(&viewProjection));

    XMMATRIX viewInverse = XMLoadFloat4x4(&gameContext.pCamera->GetViewInverse());
    m_pViewInverseVariable->SetMatrix(reinterpret_cast<float*>(&viewInverse));

    m_pTextureVariable->SetResource(m_pParticleTexture->GetShaderResourceView());

    gameContext.pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

    gameContext.pDeviceContext->IASetInputLayout(m_pInputLayout);

    UINT offset = 0;
    UINT stride = sizeof(ParticleVertex);
    gameContext.pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

    D3DX11_TECHNIQUE_DESC techDesc{};
    m_pDefaultTechnique->GetDesc(&techDesc);
    for (UINT p = 0; p < techDesc.Passes; ++p)
    {
        m_pDefaultTechnique->GetPassByIndex(p)->Apply(0, gameContext.pDeviceContext);
        gameContext.pDeviceContext->Draw(m_ActiveParticles * 3, 0);
    }
}
