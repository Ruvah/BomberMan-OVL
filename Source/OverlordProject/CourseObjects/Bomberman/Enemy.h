#pragma once
#include "CourseObjects/Week 2/Character.h"
#include "Map/Tile.h"

class Enemy: public Character
{

public:
	Enemy();
	~Enemy();
	void SetTile(Tile* pTile) { m_pCurrTile = pTile; };
	static int m_AmountEnemies;

protected:

	enum class Direction
	{
		UP,
		RIGHT,
		DOWN,
		LEFT,
	};

	Tile * m_pCurrTile;
	Direction m_Direction;
};

