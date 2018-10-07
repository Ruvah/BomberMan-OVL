#pragma once
#include <bemapiset.h>

namespace BombermanGame
{
	enum Materials :UINT
	{
		BOMBERMAN1 = 0,
		FLOOR = 1,
		BOMB = 2,
		BOXDESTR = 3,
		BOXNONDESTR = 4,
		FIREUPMAT = 5,
		BOMBUPMAT = 6,
		SPEEDUPMAT = 7,
		ENEMY = 8,
		BACKGROUND=9
	};

	enum Powerups:UINT
	{
		FIREUP=0,
		BOMBUP=1,
		SPEEDUP=2
	};

	enum MenuOptions :UINT
	{
		DOWN = 125,
		UP = 126,
		OK = 127,
		PAUSE = 128
	};
}