#include "stdafx.h"
#include "BomberMan.h"
#include "Physx/PhysxManager.h"
#include "Components/ControllerComponent.h"
#include "Prefabs/FixedCamera.h"
#include "Components/TransformComponent.h"
#include "Helpers.h"
#include "Materials/Shadow/SkinnedDiffuseMaterial_Shadow.h"
#include "Scenegraph/GameScene.h"
#include "Bomb.h"
#include "Map/Tile.h"
#include "Scenegraph/SceneManager.h"
#include "Map/Map.h"
#include "Base/SoundManager.h"
#include "EndScreen.h"
#include "Components/SpriteComponent.h"
#include "Base/OverlordGame.h"
#include "Graphics/TextRenderer.h"
#include "Content/ContentManager.h"
#include "BManLevel1.h"


BomberMan::BomberMan():Character()
,m_BombRad(1)
,m_SpeedUp(0)
,m_Lives(3)
,m_MaxBombs(1)
,m_Bombs(0)
,m_DeadAfterTime(4)
,m_DeadAnimTime(2.5f)
,m_DeadStart()
,m_pDeathSound(nullptr),
m_pMesh(nullptr),
m_Animation(),
m_pCurrTile(),
m_pFont(),
m_pChannel()
{
	m_MaxRunVelocity = 20.f;
	m_Radius = 5.f;
	m_Height = 11.f;
	m_OrignalRunAcc = m_RunAcceleration;
	m_OriginalMaxSpeed = m_MaxRunVelocity;
}


BomberMan::~BomberMan()
{
}

void BomberMan::SetPlayerNumber(GamepadIndex player)
{
	m_PlayerNumber = (player);
}

void BomberMan::Initialize(const GameContext& gameContext)
{
#pragma region CharacterController
	auto physX = PhysxManager::GetInstance()->GetPhysics();

	m_pController =new ControllerComponent( physX->createMaterial(0.5f, 0.5f, 0.f), m_Radius, m_Height );
	AddComponent(m_pController);
	m_pController->SetCollisionIgnoreGroups(CollisionGroupFlag::Group9);
	switch (m_PlayerNumber)
	{
	case GamepadIndex::PlayerOne:
		gameContext.pInput->AddInputAction(InputAction(LEFT, Down, VK_LEFT));
		gameContext.pInput->AddInputAction(InputAction(RIGHT, Down, VK_RIGHT ));
		gameContext.pInput->AddInputAction(InputAction(FORWARD, Down, VK_UP));
		gameContext.pInput->AddInputAction(InputAction(BACKWARD, Down, VK_DOWN));
		

		gameContext.pInput->AddInputAction(InputAction(BomberManActions::BOMB, Pressed, VK_RETURN,-1,XINPUT_GAMEPAD_A,m_PlayerNumber));

	}


#pragma endregion 

#pragma region Mesh&Mat

	auto bombermanManDiff = new SkinnedDiffuseMaterial_Shadow();
	bombermanManDiff->SetDiffuseTexture(L"./Resources/Bomberman/Player/Diffuse.png");
	gameContext.pMaterialManager->AddMaterial(bombermanManDiff, BombermanGame::Materials::BOMBERMAN1);

	m_pMesh = new AnimatedMesh(L"./Resources/Bomberman/Player/Mesh.ovm");
	AddChild(m_pMesh);

	m_pMesh->GetTransform()->Scale(0.1f, 0.1f, 0.1f);
	m_pMesh->GetTransform()->Translate(0, -m_Height*0.8f, 0);
	
	m_pMesh->GetTransform()->Rotate(0, 180, 0);
#pragma endregion 

#pragma region sounds
	FMOD_RESULT fmodResult;
	auto fmodSystem = SoundManager::GetInstance()->GetSystem();
	fmodResult = fmodSystem->createStream("./Resources/Bomberman/Sounds/PlayerDeath.wav", FMOD_LOOP_OFF | FMOD_2D, 0, &m_pDeathSound);
	SoundManager::ErrorCheck(fmodResult);
	m_pChannel->setPaused(false);
#pragma endregion 


#pragma  region UI

	m_pFont= ContentManager::Load<SpriteFont>(L"./Resources/SpriteFonts/Consolas_32.fnt") ;

	auto settings = SceneManager::GetInstance()->GetGame()->GetGameSettings();

	auto health = new GameObject;
	health->AddComponent(new SpriteComponent(L"./Resources/Bomberman/Powerups/HealthUp.png", XMFLOAT2(0.5f, 0.5f), XMFLOAT4(1.f, 1.f, 1.f, .75f)));
	AddChild(health);
	health->GetTransform()->Scale(0.15f, 0.15f, 1.f);
	m_HealthPos = { settings.Window.Width/2.f - 120.f,settings.Window.Height - 40.f };
	health->GetTransform()->Translate(settings.Window.Width / 2.f - 150.f, settings.Window.Height - 25.f, 0.9f);


	auto firePower = new GameObject;
	firePower->AddComponent(new SpriteComponent(L"./Resources/Bomberman/Powerups/FireUp.png", XMFLOAT2(0.5f, 0.5f), XMFLOAT4(1.f, 1.f, 1.f, .75f)));
	AddChild(firePower);
	firePower->GetTransform()->Scale(0.15f, 0.15f, 1.f);
	m_FirePos = { settings.Window.Width / 2.f - 20.f,settings.Window.Height - 40.f };
	firePower->GetTransform()->Translate(settings.Window.Width / 2.f - 50, settings.Window.Height - 25.f, 0.9f);

	auto speed = new GameObject;
	speed->AddComponent(new SpriteComponent(L"./Resources/Bomberman/Powerups/SpeedUp.png", XMFLOAT2(0.5f, 0.5f), XMFLOAT4(1.f, 1.f, 1.f, .75f)));
	AddChild(speed);
	speed->GetTransform()->Scale(0.15f, 0.15f, 1.f);
	m_SpeedPos = { settings.Window.Width / 2.f + 80.f,settings.Window.Height - 40.f };
	speed->GetTransform()->Translate(settings.Window.Width / 2.f +50, settings.Window.Height - 25.f, 0.9f);


	auto bombs = new GameObject;
	bombs->AddComponent(new SpriteComponent(L"./Resources/Bomberman/Powerups/BombUp.png", XMFLOAT2(0.5f, 0.5f), XMFLOAT4(1.f, 1.f, 1.f, .75f)));
	AddChild(bombs);
	bombs->GetTransform()->Scale(0.15f, 0.15f, 1.f);
	m_BombPos = { settings.Window.Width / 2.f + 180.f,settings.Window.Height - 40.f };
	bombs->GetTransform()->Translate(settings.Window.Width / 2.f + 150, settings.Window.Height - 25.f, 0.9f);
#pragma endregion 

	m_Lives = 3;
}

void BomberMan::PostInitialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	m_pMesh->GetComponent<ModelComponent>()->SetMaterial(BombermanGame::Materials::BOMBERMAN1);

	m_pMesh->SetAnim(0);
	m_pMesh->Play();

}

void BomberMan::Update(const GameContext& gameContext)
{
	auto scene = dynamic_cast<BManLevel1*>(GetScene());
	if (scene && !scene->GetPaused())
	{
		m_pMesh->SetAnimationSpeed(1);


		if (m_Animation != AnimState::DEAD)
		{
			UpdateMovement(gameContext);
			InputActions(gameContext);

		}
		UpdateAnimation();

		if (m_Animation == AnimState::DEAD)
		{
			if (m_DeadStart == 0)
				m_DeadStart = gameContext.pGameTime->GetTotal();

			if (gameContext.pGameTime->GetTotal() - m_DeadStart >= m_DeadAfterTime)
			{
				if (m_Lives == 0)
				{
					m_pCurrTile->GetMap()->ResetMap();

					EndScreen::m_Lost = true;
					SceneManager::GetInstance()->SetActiveGameScene(L"EndScreen");

				}
				m_pCurrTile->GetMap()->ResetPlayerDie();

				ResetUpgradesAndAnim();

			}
			else if (gameContext.pGameTime->GetTotal() - m_DeadStart >= m_DeadAnimTime)
			{
				m_pMesh->Pause();
			}
		}
	}
	else
	{
		m_pMesh->SetAnimationSpeed(0);
	}
}

void BomberMan::Draw(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	DrawInfoText();
}

void BomberMan::Die()
{
	if(m_Animation != DEAD)
	{
		FMOD_RESULT fmodResult;
		auto fmodSystem = SoundManager::GetInstance()->GetSystem();

		fmodResult = fmodSystem->playSound(m_pDeathSound, 0, false, &m_pChannel);

		m_Animation = DEAD;
		m_pMesh->SetAnim(m_Animation);
		m_pMesh->Play();
		m_Lives--;

	}
}

void BomberMan::SetTile(Tile* tile)
{
	m_pCurrTile = tile;
}

void BomberMan::GetPowerup(BombermanGame::Powerups powerup)
{
	switch (powerup)
	{
	case BombermanGame::Powerups::BOMBUP:
		m_MaxBombs++;
		break;
	case BombermanGame::Powerups::FIREUP:
		m_BombRad++;
		break;
	case BombermanGame::Powerups::SPEEDUP:
		IncreaseSpeed();
		break;
	}
}

void BomberMan::RemoveBomb(Bomb* bomb)
{
	m_pBombs.remove(bomb);
	--m_Bombs;
}

void BomberMan::SetHealth(int health)
{
	m_Lives = health;
}

void BomberMan::UpdateMovement(const GameContext& gameContext)
{

	//HANDLE INPUT
	XMFLOAT2 move = XMFLOAT2(0, 0);
	move.y = gameContext.pInput->IsActionTriggered(FORWARD) ? 1.0f : 0.0f;
	if (move.y == 0) move.y = -(gameContext.pInput->IsActionTriggered(BACKWARD) ? 1.0f : 0.0f);
	if (move.y == 0) move.y = gameContext.pInput->GetThumbstickPosition(true,m_PlayerNumber).y;
	
	if (move.y == 0)
	{
		move.x = gameContext.pInput->IsActionTriggered(RIGHT) ? 1.0f : 0.0f;
		if (move.x == 0) move.x = -(gameContext.pInput->IsActionTriggered(LEFT) ? 1.0f : 0.0f);
		if (move.x == 0) move.x = gameContext.pInput->GetThumbstickPosition(true, m_PlayerNumber).x;
	}
	//SPRINTING
	/*auto currSpeed = m_MoveSpeed;
	if (gameContext.pInput->IsKeyboardKeyDown(VK_LSHIFT))
		currSpeed *= m_RunAcceleration;*/
	//CALCULATE TRANSFORMS

	XMVECTOR forward = XMLoadFloat3(&GetTransform()->GetForward());

	
	

	XMVECTOR direction = XMVector3Normalize((forward));



	if (move.x > 0.3f || move.x <-0.3f || move.y > 0.5f || move.y <-0.5f)
	{
		m_RunVelocity += m_RunAcceleration * gameContext.pGameTime->GetElapsed();
		if (move.y != 0) direction *-1;
		Clamp(m_RunVelocity, m_MaxRunVelocity, 0.f);
		//auto component = m_Velocity.y;
		auto newVel = direction * m_RunVelocity;
		XMStoreFloat3(&m_Velocity, newVel);
		m_Velocity.y = m_JumpVelocity;

	}
	else
	{
		m_Velocity.x = 0;
		m_Velocity.z = 0;
		m_Velocity.y = m_JumpVelocity;

	}


	if ((GetComponent<ControllerComponent>()->GetCollisionFlags()) != PxControllerCollisionFlag::eCOLLISION_DOWN)
	{
		m_JumpVelocity -= m_JumpAcceleration * gameContext.pGameTime->GetElapsed();
		

	}
	else
	{
		m_Velocity.y = 0.f;
	}

	if(move.y <0)
		m_TotalYaw  =180;
	else if(move.y>0)
		m_TotalYaw = 0;
	if (move.x <0)
		m_TotalYaw = -90;
	else if (move.x >0)
		m_TotalYaw = 90;

	GetTransform()->Rotate(0, m_TotalYaw, 0);


	//gameContext.pCamera->GetTransform()->Rotate(m_TotalPitch, m_TotalYaw, 0);

	XMFLOAT3 displacement;
	XMStoreFloat3(&displacement, XMLoadFloat3(&m_Velocity)*gameContext.pGameTime->GetElapsed());

	GetComponent<ControllerComponent>()->Move(displacement);


}

void BomberMan::UpdateAnimation()
{
	if (m_Animation != AnimState::DEAD)
	{
		XMVECTOR vel = (XMLoadFloat3(&m_Velocity));


		if (XMVectorGetX(vel) != 0 || XMVectorGetZ(vel)!=0)
		{
			if (m_Animation != AnimState::WALKING)
			{
				m_Animation = AnimState::WALKING;
				m_pMesh->SetAnim(m_Animation);
				m_pMesh->Play();
			}

		}
		else
		{
			if (m_Animation != AnimState::IDLE)
			{
				m_Animation = AnimState::IDLE;
				m_pMesh->SetAnim(m_Animation);
				m_pMesh->Play();


			}
		}
	}

}

void BomberMan::InputActions(const GameContext& gameContext)
{
	if(gameContext.pInput->IsActionTriggered(BomberManActions::BOMB))
	{
		auto pos = GetTransform()->GetPosition();
		//pos.x += 15.f;
		//GetTransform()->Translate(pos);
		SpawnBomb();
	}
}

void BomberMan::SpawnBomb()
{
	if (!m_pCurrTile->IsBombOnTile() && m_Bombs < m_MaxBombs)
	{
		BomberMan* me = this;
		auto bomb = new Bomb(*me,m_BombRad);
		auto pos = m_pCurrTile->GetTransform()->GetPosition();
		pos.y += 5.f;
		bomb->GetTransform()->Translate(pos);
		bomb->SetTile(m_pCurrTile);
		m_pBombs.push_back(bomb);
		SceneManager::GetInstance()->GetActiveScene()->AddChild(bomb);
		m_Bombs++;
	}
}

void BomberMan::IncreaseSpeed()
{
	m_SpeedUp++;
	m_RunAcceleration += m_OrignalRunAcc * (m_SpeedUp*0.25f);
	m_MaxRunVelocity += m_OriginalMaxSpeed * (m_SpeedUp*0.25f);

}

void BomberMan::ResetUpgradesAndAnim()
{


	m_MaxBombs = 1;
	m_BombRad = 1;
	m_SpeedUp = 0;
	m_MaxRunVelocity = m_OriginalMaxSpeed;
	m_RunAcceleration = m_OrignalRunAcc;
	m_pMesh->Play();
	m_Animation = AnimState::IDLE;
	m_pMesh->SetAnim(m_Animation);
	m_DeadStart = 0;
}

void BomberMan::DrawInfoText()
{
	TextRenderer::GetInstance()->DrawText(m_pFont, to_wstring(m_Lives),m_HealthPos);
	TextRenderer::GetInstance()->DrawText(m_pFont, to_wstring(m_BombRad), m_FirePos);
	TextRenderer::GetInstance()->DrawText(m_pFont, to_wstring(m_SpeedUp+1), m_SpeedPos);
	TextRenderer::GetInstance()->DrawText(m_pFont, to_wstring(m_MaxBombs), m_BombPos);


}




