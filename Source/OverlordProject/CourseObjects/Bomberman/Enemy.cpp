#include "stdafx.h"
#include "Enemy.h"

int Enemy::m_AmountEnemies = 0;

Enemy::Enemy()
{
	m_AmountEnemies++;
}


Enemy::~Enemy()
{
	Character::~Character();
	m_AmountEnemies--;
}
