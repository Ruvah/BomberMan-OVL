#pragma once
#include "Scenegraph/GameObject.h"
#include "CourseObjects/Bomberman/Map/Tile.h"

class Box:public GameObject
{
public:
	Box(bool isDestr);
	~Box();

	void Initialize(const GameContext& gameContext) override;

	void SetTile(Tile* tile);
	Tile* GetTile() { return m_pCurrTile; }
	void Break();
	bool GetDestr() { return m_Destructable; };
private:

	void SpawnPowerup();
	bool m_Destructable;
	Tile* m_pCurrTile;
};

