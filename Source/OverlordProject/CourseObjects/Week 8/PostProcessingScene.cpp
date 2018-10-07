//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "PostProcessingScene.h"
#include "Scenegraph\GameObject.h"

#include "Content\ContentManager.h"
#include "Components\TransformComponent.h"
#include "Components\ModelComponent.h"
#include "Graphics\ModelAnimator.h"

#include "..\..\Materials\SkinnedDiffuseMaterial.h"
#include <Materials/Post/PostGrayscale.h>
#include <Materials/Post/PostBlur.h>
#include <Graphics/TextRenderer.h>

PostProcessingScene::PostProcessingScene(void) :
	GameScene(L"PostProcessingScene"),
	m_pModel(nullptr)
{
}


PostProcessingScene::~PostProcessingScene(void)
{
}

void PostProcessingScene::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);

	auto skinnedDiffuseMaterial = new SkinnedDiffuseMaterial();
	skinnedDiffuseMaterial->SetDiffuseTexture(L"./Resources/Textures/Knight.jpg");
	gameContext.pMaterialManager->AddMaterial(skinnedDiffuseMaterial, 0);

	
	m_pModel = new ModelComponent(L"./Resources/Meshes/Knight.ovm");
	m_pModel->SetMaterial(0);
	auto obj = new GameObject();

	obj->AddComponent(m_pModel);
	AddChild(obj);

	m_pModel->GetAnimator()->SetAnimation(2);
	m_pModel->GetAnimator()->Play();

	obj->GetTransform()->Scale(0.1f, 0.1f, 0.1f);

	//Input
	gameContext.pInput->AddInputAction(InputAction(0, Released, 'R'));
	gameContext.pInput->AddInputAction(InputAction(1, Released, 'P'));
	gameContext.pInput->AddInputAction(InputAction(2, Released, 'O'));

	gameContext.pInput->AddInputAction(InputAction(3, Down, VK_UP));
	gameContext.pInput->AddInputAction(InputAction(4, Down, VK_DOWN));

	gameContext.pInput->AddInputAction(InputAction(5, Pressed, VK_LEFT));
	gameContext.pInput->AddInputAction(InputAction(6, Pressed, VK_RIGHT));

	//SpriteFont
	m_pFont = ContentManager::Load<SpriteFont>(L"./Resources/SpriteFonts/Consolas_32.fnt");

	//PP
	gameContext.pMaterialManager->AddMaterial_PP(new PostGrayscale(), 0);
	gameContext.pMaterialManager->AddMaterial_PP(new PostBlur(), 1);

	AddPostProcessingMaterial(0);
	AddPostProcessingMaterial(1);
}

void PostProcessingScene::Update(const GameContext& gameContext)
{
	auto animator = m_pModel->GetAnimator();

	UNREFERENCED_PARAMETER(gameContext);
	if (gameContext.pInput->IsActionTriggered(0))
	{
		animator->Reset();
	}

	if (gameContext.pInput->IsActionTriggered(1))
	{
		if (animator->IsPlaying())
		{
			animator->Pause();
			RemovePostProcessingMaterial(1);
		}
		else
		{
			animator->Play();
			AddPostProcessingMaterial(1);
		}
	}

	if (gameContext.pInput->IsActionTriggered(2))
	{
		animator->SetPlayReversed(!animator->IsReversed());
	}

	if (gameContext.pInput->IsActionTriggered(3))
	{
		auto speed = animator->GetAnimationSpeed();
		speed += 0.5f * gameContext.pGameTime->GetElapsed();
		Clamp<float>(speed, 5, 0);

		animator->SetAnimationSpeed(speed);
	}

	if (gameContext.pInput->IsActionTriggered(4))
	{
		auto speed = animator->GetAnimationSpeed();
		speed -= 0.5f * gameContext.pGameTime->GetElapsed();
		Clamp<float>(speed, 5, 0);

		animator->SetAnimationSpeed(speed);
	}

	if (gameContext.pInput->IsActionTriggered(5))
	{
		animator->SetAnimation(0);
	}

	if (gameContext.pInput->IsActionTriggered(6))
	{
		animator->SetAnimation(1);
	}
}

void PostProcessingScene::Draw(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);

	wstringstream sText;
	sText << L"PP EFFECT: BLUR";
	if (m_pModel->GetAnimator()->IsPlaying())
		sText << L" + BLUR";

	TextRenderer::GetInstance()->DrawText(m_pFont, sText.str() , XMFLOAT2(10, 10), XMFLOAT4(Colors::Red));
}
