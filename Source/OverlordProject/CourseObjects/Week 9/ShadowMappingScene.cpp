//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "ShadowMappingScene.h"
#include "Scenegraph\GameObject.h"

#include "Content\ContentManager.h"
#include "Components\Components.h"
#include <Materials/Shadow/DiffuseMaterial_Shadow.h>
#include "Graphics/ShadowMapRenderer.h"
#include "Materials/Shadow/SkinnedDiffuseMaterial_Shadow.h"
#include "Graphics/ModelAnimator.h"

#define ANIM_AMOUNT 5

ShadowMappingScene::ShadowMappingScene(void) :
	GameScene(L"ShadowMappingScene")
    , m_AnimationIndex{}
{
}


ShadowMappingScene::~ShadowMappingScene(void)
{
}

void ShadowMappingScene::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	gameContext.pShadowMapRenderer->SetLight({ -95.6139526f,66.1346436f,-41.1850471f }, { 0.740129888f, -0.597205281f, 0.309117377f });

	//Materials
	//*********
	auto diffMat = new SkinnedDiffuseMaterial_Shadow();
	diffMat->SetDiffuseTexture(L"./Resources/Textures/Knight.jpg");

	gameContext.pMaterialManager->AddMaterial(diffMat, 0);
	

	auto diffMat2 = new DiffuseMaterial_Shadow();
	diffMat2->SetDiffuseTexture(L"./Resources/Textures/GroundBrick.jpg");

	gameContext.pMaterialManager->AddMaterial(diffMat2, 1);

	//Ground Mesh
	//***********

	auto pGroundObj = new GameObject();
	auto pGroundModel = new ModelComponent(L"./Resources/Meshes/UnitPlane.ovm");
	pGroundModel->SetMaterial(1);

	pGroundObj->AddComponent(pGroundModel);
	pGroundObj->GetTransform()->Scale(10.0f, 10.0f, 10.0f);

	AddChild(pGroundObj);

	//Character Mesh
	//**************

	auto pCharacterObj = new GameObject();

	//Level Model Component
	m_pKnightModel = new ModelComponent(L"./Resources/Meshes/Knight.ovm");
	m_pKnightModel->SetMaterial(0);
	pCharacterObj->AddComponent(m_pKnightModel);

	AddChild(pCharacterObj);
	pCharacterObj->GetTransform()->Scale(0.1f, 0.1f, 0.1f);

    m_pKnightModel->GetAnimator()->SetAnimation(0);

	//Input
	//*****
	gameContext.pInput->AddInputAction(InputAction(0, Pressed, VK_SPACE));

    gameContext.pInput->AddInputAction(InputAction(0, Released, 'R'));
    gameContext.pInput->AddInputAction(InputAction(1, Released, 'P'));
    gameContext.pInput->AddInputAction(InputAction(2, Released, 'O'));

    gameContext.pInput->AddInputAction(InputAction(3, Down, VK_UP));
    gameContext.pInput->AddInputAction(InputAction(4, Down, VK_DOWN));

    gameContext.pInput->AddInputAction(InputAction(5, Pressed, VK_LEFT));
    gameContext.pInput->AddInputAction(InputAction(6, Pressed, VK_RIGHT));
}

void ShadowMappingScene::Update(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);

	if (gameContext.pInput->IsActionTriggered(0))
	{
		gameContext.pShadowMapRenderer->SetLight(gameContext.pCamera->GetTransform()->GetPosition(), gameContext.pCamera->GetTransform()->GetForward());
	}

    auto animator = m_pKnightModel->GetAnimator();

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
        }
        else
        {
            animator->Play();
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
        --m_AnimationIndex;
        if (m_AnimationIndex < 0)
            m_AnimationIndex = ANIM_AMOUNT - 1;

        animator->SetAnimation(m_AnimationIndex);
    }

    if (gameContext.pInput->IsActionTriggered(6))
    {
        ++m_AnimationIndex;
        if (m_AnimationIndex >= ANIM_AMOUNT)
            m_AnimationIndex = 0;

        animator->SetAnimation(m_AnimationIndex);
    }
}

void ShadowMappingScene::Draw(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
}
