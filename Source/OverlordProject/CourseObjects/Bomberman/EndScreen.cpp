#include "stdafx.h"

#include "EndScreen.h"
#include "Components/SpriteComponent.h"
#include "Scenegraph/GameObject.h"
#include "Components/TransformComponent.h"
#include "Scenegraph/SceneManager.h"
#include "CourseObjects/Bomberman/BomberMan.h"

bool EndScreen::m_Lost{ false };

EndScreen::EndScreen():GameScene(L"EndScreen")
{
}


EndScreen::~EndScreen()
{
}

void EndScreen::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
		m_BgLost = new GameObject();
		m_BgLost->AddComponent(new SpriteComponent(L"./Resources/Bomberman/Lose.png", XMFLOAT2(0.5f, 0.5f), XMFLOAT4(1, 1, 1, 0.f)));
		AddChild(m_BgLost);

		m_BgLost->GetTransform()->Translate(640.f, 360.f, .9f);

		m_BgWin = new GameObject();
		m_BgWin->AddComponent(new SpriteComponent(L"./Resources/Bomberman/Win.png", XMFLOAT2(0.5f, 0.5f), XMFLOAT4(1, 1, 1, 0.f)));
		AddChild(m_BgWin);

		m_BgWin->GetTransform()->Translate(640.f, 360.f, .9f);
		gameContext.pInput->AddInputAction(InputAction(BombermanGame::MenuOptions::OK, InputTriggerState::Released, VK_RETURN, -1, XINPUT_GAMEPAD_A, GamepadIndex::PlayerOne));

}

void EndScreen::Update(const GameContext& gameContext)
{
	if (gameContext.pInput->IsActionTriggered(BombermanGame::OK))
	{
		SceneManager::GetInstance()->SetActiveGameScene(L"MainMenu");
	}

	if(m_Lost)
	{
		m_BgLost->GetComponent<SpriteComponent>()->SetColor({ 1.f, 1.f, 1.f, 1.f });

		m_BgWin->GetComponent<SpriteComponent>()->SetColor({ 1.f, 1.f, 1.f, 0.f });
	}
	else
	{
		m_BgLost->GetComponent<SpriteComponent>()->SetColor({ 1.f, 1.f, 1.f, 0.f });

		m_BgWin->GetComponent<SpriteComponent>()->SetColor({ 1.f, 1.f, 1.f, 1.f });
	}



}

void EndScreen::Draw(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
}


