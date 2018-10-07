#pragma once
#include "..\Week 2/Character.h"
#include "AnimatedMesh.h"
#include "Helpers.h"
#include <list>
#include "Bomb.h"
#include "Graphics/SpriteFont.h"
class Tile;

class BomberMan:public Character
{
enum  AnimState : unsigned int
{
	IDLE,
	WALKING,
	DEAD
};

	enum BomberManActions
	{
		BOMB =5
	};
public:
	BomberMan();
	~BomberMan();

	void SetPlayerNumber(GamepadIndex player);

	void Initialize(const GameContext& gameContext) override;
	void PostInitialize(const GameContext& gameContext) override;
	void Update(const GameContext& gameContext) override;
	void Draw(const GameContext& gameContext) override;
	void Die();
	void SetTile(Tile* tile);
	Tile* GetTile() { return m_pCurrTile; }
	void GetPowerup(BombermanGame::Powerups powerup);
	void RemoveBomb(Bomb* bomb);
	void SetHealth(int health);

private:

	void UpdateMovement(const GameContext& gameContext)override;
	void UpdateAnimation();
	void InputActions(const GameContext& gameContext);
	void SpawnBomb();
	void IncreaseSpeed();
	void ResetUpgradesAndAnim();
	void DrawInfoText();
	AnimatedMesh* m_pMesh;
	AnimState m_Animation;
	Tile* m_pCurrTile;
	unsigned int m_BombRad,m_SpeedUp,m_Lives,m_MaxBombs;
	unsigned int m_Bombs;
	float m_OrignalRunAcc,m_OriginalMaxSpeed;
	list<Bomb*> m_pBombs;
	float m_DeadAnimTime,m_DeadStart,m_DeadAfterTime;
	SpriteFont* m_pFont;
	XMFLOAT2 m_HealthPos,m_FirePos,m_BombPos,m_SpeedPos;

	FMOD::Sound* m_pDeathSound;
	FMOD::Channel* m_pChannel;
private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	BomberMan(const BomberMan& t);
	BomberMan& operator=(const BomberMan& t);
};

