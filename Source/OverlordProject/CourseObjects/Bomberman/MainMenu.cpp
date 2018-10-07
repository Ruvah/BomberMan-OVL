#include "stdafx.h"

#include "MainMenu.h"
#include "Components/SpriteComponent.h"
#include "Scenegraph/GameObject.h"
#include "Components/TransformComponent.h"
#include "BomberMan.h"
#include "Scenegraph/SceneManager.h"


MainMenu::MainMenu():GameScene(L"MainMenu")
{
}


MainMenu::~MainMenu()
{
}

void MainMenu::Initialize(const GameContext& gameContext)
{
	m_CurrSelect = Options::START;
	m_OptionsPos.push_back({ 450,275,.9f });

	m_OptionsPos.push_back({ 430,400,.9f });
	m_OptionsPos.push_back({ 450,510,.9f });


	m_pBackgroundTex = new GameObject();
	m_pBackgroundTex->AddComponent(new SpriteComponent(L"./Resources/Bomberman/MainMenu.png", XMFLOAT2(0.5f, 0.5f), XMFLOAT4(1, 1, 1, 1.f)));
	AddChild(m_pBackgroundTex);

	m_pBackgroundTex->GetTransform()->Translate(640.f, 360.f, .9f);


	m_pArrow = new GameObject();
	m_pArrow->AddComponent(new SpriteComponent(L"./Resources/Bomberman/MenuIndicator.png", XMFLOAT2(0.5f, 0.5f), XMFLOAT4(1, 1, 1, 1.f)));
	AddChild(m_pArrow);

	m_pArrow->GetTransform()->Translate(m_OptionsPos[0]);
	m_pArrow->GetTransform()->Scale(0.25f, 0.25f, 1);


	m_pControls = new GameObject();
	m_pControls->AddComponent(new SpriteComponent(L"./Resources/Bomberman/Controls.png", XMFLOAT2(0.5f, 0.5f), XMFLOAT4(1, 1, 1, 0.f)));
	AddChild(m_pControls);

	m_pControls->GetTransform()->Translate(640.f, 360.f, .9f);

	gameContext.pInput->AddInputAction(InputAction(BombermanGame::MenuOptions::DOWN, Pressed, VK_DOWN, -1, XINPUT_GAMEPAD_DPAD_DOWN, GamepadIndex::PlayerOne));
	gameContext.pInput->AddInputAction(InputAction(BombermanGame::MenuOptions::UP, Pressed, VK_UP, -1, XINPUT_GAMEPAD_DPAD_UP, GamepadIndex::PlayerOne));
	gameContext.pInput->AddInputAction(InputAction(BombermanGame::MenuOptions::OK, InputTriggerState::Down, VK_RETURN, -1, XINPUT_GAMEPAD_A, GamepadIndex::PlayerOne));

	
}

void MainMenu::Update(const GameContext& gameContext)
{
	m_pControls->GetComponent<SpriteComponent>()->SetColor({ 1.f, 1.f, 1.f, 0.f });

	if(gameContext.pInput->IsActionTriggered(BombermanGame::DOWN))
	{
		m_CurrSelect = Options(UINT (m_CurrSelect)+1);
		if(m_CurrSelect > Options::QUIT)
		{
			m_CurrSelect = Options::START;
		}
	}
	if (gameContext.pInput->IsActionTriggered(BombermanGame::UP))
	{
		m_CurrSelect = Options(UINT(m_CurrSelect) - 1);
		if (m_CurrSelect > Options::QUIT)
		{
			m_CurrSelect = Options::QUIT;
		}
	}
	if (gameContext.pInput->IsActionTriggered(BombermanGame::OK))
	{
		switch (m_CurrSelect)
		{
		case Options::START:
			SceneManager::GetInstance()->SetActiveGameScene(L"BLevel1");
			break;
		case Options::CONTROLS:
			m_pControls->GetComponent<SpriteComponent>()->SetColor({ 1.f, 1.f, 1.f, 1.f });
			break;
		case Options::QUIT:
			PostQuitMessage(WM_QUIT);
			break;
		}
	}
	m_pArrow->GetTransform()->Translate(m_OptionsPos[m_CurrSelect]);
}

void MainMenu::Draw(const GameContext& gameContext)
{
}
