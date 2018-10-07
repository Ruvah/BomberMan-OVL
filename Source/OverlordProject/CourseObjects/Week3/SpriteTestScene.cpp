//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "Base\stdafx.h"

#include "SpriteTestScene.h"
#include "Scenegraph\GameObject.h"
#include "Diagnostics\Logger.h"
#include "Diagnostics\DebugRenderer.h"
#include "Graphics\SpriteRenderer.h"

#include "Content\ContentManager.h"
#include "Graphics\TextureData.h"
#include "Components\SpriteComponent.h"
#include "Components\TransformComponent.h"
#include "Graphics\MeshFilter.h"

#define FPS_COUNTER 1

SpriteTestScene::SpriteTestScene(void) :
GameScene(L"SpriteTestScene"),
m_FpsInterval(FPS_COUNTER),
m_pBackground(nullptr)
{
}


SpriteTestScene::~SpriteTestScene(void)
{
}

void SpriteTestScene::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);

	m_pBackground = new GameObject();
	m_pBackground->AddComponent(new SpriteComponent(L"./Resources/Textures/Chair_Dark.dds", XMFLOAT2(0.5f,0.5f),XMFLOAT4(1,1,1,0.5f)));
	AddChild(m_pBackground);

	m_pBackground->GetTransform()->Translate(500.f, 350.f, .9f);
	m_pBackground->GetTransform()->Scale(1.f, 2.f, 1.f);
}

void SpriteTestScene::Update(const GameContext& gameContext)
{

	m_FpsInterval += gameContext.pGameTime->GetElapsed();
	if (m_FpsInterval >= FPS_COUNTER)
	{
		m_FpsInterval -= FPS_COUNTER;
		Logger::LogFormat(LogLevel::Info, L"FPS: %i", gameContext.pGameTime->GetFPS());
	}

	m_pBackground->GetTransform()->Rotate(0, 0, XM_PIDIV2 * gameContext.pGameTime->GetTotal(), false);
}

void SpriteTestScene::Draw(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
}
