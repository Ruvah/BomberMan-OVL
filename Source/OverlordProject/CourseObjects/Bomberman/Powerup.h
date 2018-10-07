#pragma once
#include "Scenegraph/GameObject.h"
#include "Helpers.h"
#include "Map/Tile.h"

class Powerup:public GameObject
{
public:
	Powerup(BombermanGame::Powerups type);
	

	void Initialize(const GameContext& gameContext) override;
	void Update(const GameContext& gameContext) override;

	void SetTile(Tile* pTile) { m_pCurrTile = pTile; };
	void Explode();
private:
	BombermanGame::Powerups m_Type;
	bool m_Active, m_Used;
	float m_StartTime;
	Tile* m_pCurrTile;
	GameObject* m_pIcon;
};

