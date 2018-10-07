//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "FontTestScene.h"

#include "Scenegraph\GameObject.h"
#include "Diagnostics\Logger.h"
#include "Diagnostics\DebugRenderer.h"
#include "Graphics\SpriteRenderer.h"

#include "Content\ContentManager.h"
#include "Graphics\TextureData.h"
#include "Graphics\TextRenderer.h"
#include "Components\TransformComponent.h"
#include "Graphics\MeshFilter.h"
#include "Content\SpriteFontLoader.h"
FontTestScene::FontTestScene():
	GameScene(L"FontTestScene")
{
}


FontTestScene::~FontTestScene()
{
}

void FontTestScene::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	
}

void FontTestScene::Update(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
}

void FontTestScene::Draw(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	//TextRenderer::GetInstance()->DrawText(ContentManager::Load<SpriteFont>(L"./Resources/SpriteFonts/Consolas_32.fnt"), L"Hello World", XMFLOAT2(0, 0), XMFLOAT4(Colors::Red));
	SpriteFont* pFont{ ContentManager::Load<SpriteFont>(L"Resources/SpriteFonts/Consolas_32.fnt") };
	TextRenderer::GetInstance()->DrawText(pFont, L"Hello World", XMFLOAT2(0.0f, 0.0f), XMFLOAT4(Colors::Red));
}

void FontTestScene::SceneActivated()
{
}

void FontTestScene::SceneDeactivated()
{
}
