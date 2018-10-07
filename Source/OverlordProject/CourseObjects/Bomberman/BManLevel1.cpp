#include "stdafx.h"
#include "BManLevel1.h"
#include "Graphics/ShadowMapRenderer.h"
#include "Materials/Shadow/SkinnedDiffuseMaterial_Shadow.h"
#include "Components/ModelComponent.h"
#include "Scenegraph/GameObject.h"
#include "Materials/Shadow/DiffuseMaterial_Shadow.h"
#include "Components/TransformComponent.h"
#include "BomberMan.h"

#include "Helpers.h"
#include "Graphics/ModelAnimator.h"
#include "Physx/PhysxProxy.h"
#include "Components/ColliderComponent.h"
#include "Physx/PhysxManager.h"
#include "Components/RigidBodyComponent.h"
#include "Prefabs/FixedCamera.h"
#include "Map/Tile.h"
#include "Map/Map.h"
#include "Box.h"
#include "Diagnostics/DebugRenderer.h"
#include <chrono>
#include "Materials/DiffuseMaterial.h"
#include "SimpleEnemy.h"
#include "Base/SoundManager.h"
#include "EndScreen.h"
#include "Scenegraph/SceneManager.h"
#include "Powerup.h"
#include "Components/SpriteComponent.h"
#include "Materials/Post/PostVignetting.h"
#include "Materials/Post/PostGrayscale.h"


BManLevel1::BManLevel1():
	GameScene(L"BLevel1")
{
}


BManLevel1::~BManLevel1()
{
}

void BManLevel1::Initialize(const GameContext& gameContext)
{

#pragma region gamestuff
	gameContext.pShadowMapRenderer->SetLight({ 117.392822f,60.0746841f,128.006363f }, { -0.591161907f, -0.547116220f, -0.592614055f });
	//GetPhysxProxy()->EnablePhysxDebugRendering(true);
	DebugRenderer::ToggleDebugRenderer();


	//Materials
	//*********

	auto diffMat2 = new DiffuseMaterial_Shadow();
	diffMat2->SetDiffuseTexture(L"./Resources/Textures/GroundBrick.jpg");

	gameContext.pMaterialManager->AddMaterial(diffMat2, 1);

	auto bombMat = new DiffuseMaterial_Shadow();
	bombMat->SetDiffuseTexture(L"./Resources/Bomberman/Bomb/Bomb.png");

	gameContext.pMaterialManager->AddMaterial(bombMat, BombermanGame::BOMB);


	auto boxDestr = new DiffuseMaterial_Shadow();
	boxDestr->SetDiffuseTexture(L"./Resources/Bomberman/Box/BoxDestr.png");

	gameContext.pMaterialManager->AddMaterial(boxDestr, BombermanGame::BOXDESTR);

	auto boxNonDestr = new DiffuseMaterial_Shadow();
	boxNonDestr->SetDiffuseTexture(L"./Resources/Bomberman/Box/boxNonDestr.jpg");

	gameContext.pMaterialManager->AddMaterial(boxNonDestr, BombermanGame::BOXNONDESTR);

	auto speedUpMat = new DiffuseMaterial_Shadow();
	speedUpMat->SetDiffuseTexture(L"./Resources/Bomberman/Powerups/SpeedUp.png");

	gameContext.pMaterialManager->AddMaterial(speedUpMat, BombermanGame::SPEEDUPMAT);

	auto fireUpMat = new DiffuseMaterial();
	fireUpMat->SetDiffuseTexture(L"./Resources/Bomberman/Powerups/FireUp.png");

	gameContext.pMaterialManager->AddMaterial(fireUpMat, BombermanGame::FIREUPMAT);

	auto bombUp = new DiffuseMaterial();
	bombUp->SetDiffuseTexture(L"./Resources/Bomberman/Powerups/BombUp.png");

	gameContext.pMaterialManager->AddMaterial(bombUp, BombermanGame::BOMBUPMAT);


	auto enemyDiff = new SkinnedDiffuseMaterial_Shadow();
	enemyDiff->SetDiffuseTexture(L"./Resources/Bomberman/Enemy/Enemy1.png");
	gameContext.pMaterialManager->AddMaterial(enemyDiff, BombermanGame::Materials::ENEMY);

	auto backgr = new DiffuseMaterial();
	backgr->SetDiffuseTexture(L"./Resources/Bomberman/GameBackground.png");
	gameContext.pMaterialManager->AddMaterial(backgr, BombermanGame::Materials::BACKGROUND);

	//GROUND PLANE
	//************
	auto physX = PhysxManager::GetInstance()->GetPhysics();

	auto bouncyMaterial = physX->createMaterial(0, 0, 1);
	auto ground = new GameObject();
	ground->AddComponent(new RigidBodyComponent(true));

	std::shared_ptr<PxGeometry> geom(new PxPlaneGeometry());
	ground->AddComponent(new ColliderComponent(geom, *bouncyMaterial, PxTransform(PxQuat(XM_PIDIV2, PxVec3(0, 0, 1)))));
	AddChild(ground);	


	//BackgroundMusic
	FMOD_RESULT fmodResult;
	auto fmodSystem = SoundManager::GetInstance()->GetSystem();
	fmodResult = fmodSystem->createStream("./Resources/Bomberman/Sounds/Background.wav", FMOD_LOOP_NORMAL | FMOD_2D, 0, &m_pBackgroundMusic);
	SoundManager::ErrorCheck(fmodResult);
	fmodResult = fmodSystem->playSound(m_pBackgroundMusic, 0, true, &m_pChannel);

	//camera
	auto cam = new FixedCamera();
	AddChild(cam);
	cam->GetTransform()->Translate(0, 240, 5);
	cam->GetTransform()->Rotate(90, 0, 0);
	SetActiveCamera(cam->GetComponent<CameraComponent>());

	m_Map = new Map(L"./Resources/Bomberman/map.txt");
	AddChild(m_Map);


#pragma endregion 

#pragma region OptionsMenu

	m_OptionsPos.push_back({ 450,275,.9f });

	m_OptionsPos.push_back({ 430,400,.9f });
	m_OptionsPos.push_back({ 450,510,.9f });
	m_OptionsPos.push_back({ 450,620,.9f });


	m_Paused = false;

	m_pOptionsBackground = new GameObject();
	m_pOptionsBackground->AddComponent(new SpriteComponent(L"./Resources/Bomberman/PauseMenu.png", XMFLOAT2(0.5f, 0.5f), XMFLOAT4(1, 1, 1, 0.f)));
	AddChild(m_pOptionsBackground);

	m_pOptionsBackground->GetTransform()->Translate(640.f, 360.f, .9f);


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
	gameContext.pInput->AddInputAction(InputAction(BombermanGame::MenuOptions::PAUSE, InputTriggerState::Pressed, VK_SPACE, -1, XINPUT_GAMEPAD_START, GamepadIndex::PlayerOne));

	m_CurrSelect = Options::START;
#pragma endregion 


#pragma region groundplane
	auto pGroundObj = new GameObject();
	auto pGroundModel = new ModelComponent(L"./Resources/Meshes/UnitPlane.ovm");

	pGroundModel->SetMaterial(BombermanGame::Materials::BACKGROUND);
	pGroundObj->AddComponent(pGroundModel);
	pGroundObj->GetTransform()->Scale(50.f, 1.f, 25.f);
	pGroundObj->GetTransform()->Translate(0.f, -10.f, 0.f);


	AddChild(pGroundObj);
#pragma endregion 


#pragma region PP
	auto postvign = new PostVignetting();
	postvign->SetVignette(0.7f);
	gameContext.pMaterialManager->AddMaterial_PP(postvign, 0);

	AddPostProcessingMaterial(0);
#pragma endregion
}

void BManLevel1::Update(const GameContext& gameContext)
{


#pragma region Maploadstuff
	if(Enemy::m_AmountEnemies<=0&&!m_Map->GetLoading())
	{
		EndScreen::m_Lost = false;
		SceneManager::GetInstance()->SetActiveGameScene(L"EndScreen");
	}
	else if(m_Map->GetLoading())
	{
		for (auto child : m_pChildren)
		{
			auto powerup = dynamic_cast<Powerup*>(child);
			if (powerup)
			{

				powerup->Explode();
				
			}
		}
	}
#pragma endregion 

#pragma region Options
	m_pControls->GetComponent<SpriteComponent>()->SetColor({ 1.f, 1.f, 1.f, 0.f });
	m_pOptionsBackground->GetComponent<SpriteComponent>()->SetColor({ 1.f, 1.f, 1.f, 0.f });
	m_pArrow->GetComponent<SpriteComponent>()->SetColor({ 1.f, 1.f, 1.f, 0.f });
	if(gameContext.pInput->IsActionTriggered(BombermanGame::MenuOptions::PAUSE))
	{
		m_CurrSelect = Options::START;

		m_Paused = !m_Paused;
	}
	if (m_Paused)
	{
		m_pOptionsBackground->GetComponent<SpriteComponent>()->SetColor({ 1.f, 1.f, 1.f, 1.f });
		m_pArrow->GetComponent<SpriteComponent>()->SetColor({ 1.f, 1.f, 1.f, 1.f });

		if (gameContext.pInput->IsActionTriggered(BombermanGame::DOWN))
		{
			m_CurrSelect = Options(UINT(m_CurrSelect) + 1);
			if (m_CurrSelect > Options::QUIT)
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
				m_Paused = false;
				break;
			case Options::CONTROLS:
				m_pControls->GetComponent<SpriteComponent>()->SetColor({ 1.f, 1.f, 1.f, 1.f });
				break;
			case Options::RESET:
				m_Map->ResetMap();
				m_Paused = false;
				break;
			case Options::QUIT:
				PostQuitMessage(WM_QUIT);
				break;
			}
		}
		m_pArrow->GetTransform()->Translate(m_OptionsPos[m_CurrSelect]);
	}
#pragma endregion 

}

void BManLevel1::Draw(const GameContext& gameContext)
{
	gameContext;
}

void BManLevel1::SceneActivated()
{
	m_pChannel->setPaused(false);
	m_CurrSelect = Options::START;

}

void BManLevel1::SceneDeactivated()
{
	m_pChannel->setPaused(true);
	m_Map->ResetMap();

}
