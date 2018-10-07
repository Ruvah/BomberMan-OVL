#pragma once
#include "Enemy.h"
#include "Map/Tile.h"
#include "AnimatedMesh.h"
class SimpleEnemy :public Enemy
{
	enum class Anim
	{
		WALKING,
		IDLE,
		DIE
	};
public:
	SimpleEnemy();


	void Initialize(const GameContext& gameContext) override;
	void PostInitialize(const GameContext& gameContext) override;
	void Update(const GameContext& gameContext) override;
	void UpdateMovement(const GameContext& gameContext) override;

	void Die(const GameContext& gameContext);
private:
	void DecideNewDirection();
	bool m_Wait,m_Dead;
	float m_WaitTime, m_WaitStart,m_DeadStart, m_DeadTime;
	AnimatedMesh* m_pMesh;
	GameObject* m_pCollider;
	FMOD::Sound* m_pDeathSound;
	FMOD::Channel* m_pChannel;

};

