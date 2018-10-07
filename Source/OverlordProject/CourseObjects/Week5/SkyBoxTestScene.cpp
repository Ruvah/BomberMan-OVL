//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "SkyBoxTestScene.h"


#include <Physx/PhysxProxy.h>
#include "Prefabs\SkyBoxPrefab.h"
#include "Components/SpriteComponent.h"
#include "Components/TransformComponent.h"

SkyBoxTestScene::SkyBoxTestScene():
	GameScene(L"SkyBoxTestScene")
{
}


SkyBoxTestScene::~SkyBoxTestScene()
{
}

void SkyBoxTestScene::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	auto skybox = new SkyBoxPrefab(wstring(L"Resources/Textures/SkyBox.dds"));
	AddChild(skybox);


	auto Obj = new GameObject();
	Obj->AddComponent(new SpriteComponent(L"./Resources/Textures/Chair_Dark.dds", XMFLOAT2(0.5f, 0.5f), XMFLOAT4(1, 1, 1, 0.5f)));
	AddChild(Obj);

	Obj->GetTransform()->Translate(500.f, 350.f, .9f);
	Obj->GetTransform()->Scale(1.f, 2.f, 1.f);

}

void SkyBoxTestScene::Update(const GameContext& gameContext)
{

}

void SkyBoxTestScene::Draw(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
}

void SkyBoxTestScene::SceneActivated()
{
}

void SkyBoxTestScene::SceneDeactivated()
{
}
