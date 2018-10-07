//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "Particle.h"
#include "../Components/TransformComponent.h"
#include "../Helpers/MathHelper.h"


Particle::Particle(const ParticleEmitterSettings& emitterSettings) :
	m_EmitterSettings(emitterSettings)
{
}


Particle::~Particle(void)
{
}

void Particle::Update(const GameContext& gameContext)
{
	//See Lab10_2
    if (!m_IsActive) return;

    m_CurrentEnergy -= gameContext.pGameTime->GetElapsed();
    if(m_CurrentEnergy <= 0.0f)
    {
        m_IsActive = false;
        return;
    }

    m_VertexInfo.Position = m_VertexInfo.Position + m_EmitterSettings.Velocity * gameContext.pGameTime->GetElapsed();
    
    m_VertexInfo.Color = m_EmitterSettings.Color;
    float particleLifePercent = m_CurrentEnergy / m_TotalEnergy;
    m_VertexInfo.Color.w = particleLifePercent * 2;

    if(m_SizeGrow < 1)
    {
        m_VertexInfo.Size = m_InitSize - (1 - particleLifePercent) * m_SizeGrow;
    }
    else if (m_SizeGrow > 1)
    {
        m_VertexInfo.Size = m_InitSize +  (1 - particleLifePercent) * m_SizeGrow;
    }
}

void Particle::Init(XMFLOAT3 initPosition)
{
    m_IsActive = true;

    // Energy
    m_TotalEnergy = randF(m_EmitterSettings.MinEnergy, m_EmitterSettings.MaxEnergy);
    m_CurrentEnergy = m_TotalEnergy;

    // Position
    XMVECTOR unitVec = { 1,0,0 };
    XMMATRIX randRotation = XMMatrixRotationRollPitchYaw(randF(-XM_PI, XM_PI), randF(-XM_PI, XM_PI), randF(-XM_PI, XM_PI));
    XMVECTOR randRotVec = XMVector3TransformNormal(unitVec, randRotation);

    float distance = randF(m_EmitterSettings.MinEmitterRange, m_EmitterSettings.MaxEmitterRange);

    XMFLOAT3 randRotF;
    XMStoreFloat3(&randRotF, randRotVec);
    m_VertexInfo.Position = initPosition + randRotF * distance;

    // Size
    m_VertexInfo.Size = randF(m_EmitterSettings.MinSize, m_EmitterSettings.MaxSize);
    m_SizeGrow = randF(m_EmitterSettings.MinSizeGrow, m_EmitterSettings.MaxSizeGrow);
    m_InitSize = m_VertexInfo.Size;

    // Rotation
    m_VertexInfo.Rotation = randF(-XM_PI, XM_PI);
}
