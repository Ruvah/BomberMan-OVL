#pragma once
#include "Scenegraph/GameObject.h"
#include "CourseObjects/Bomberman/Map/Tile.h"
#include "CourseObjects/Bomberman/BomberMan.h"

class Map:public GameObject
{
	enum SpawnPoint:int
	{
		NOTHING,
		NONDESTR,
		DESTR,
		PLAYERSPAWN,
		ENEMY1
	};
public:
	Map(unsigned int rows,unsigned int cols);
	Map(const wstring& file);
	~Map();

	void Initialize(const GameContext& gameContext) override;
	void Update(const GameContext& gameContext) override;
	void LoadMap(const GameContext& gameContext);
	void ResetPlayerDie();
	void ResetMap();
	bool GetLoading();
	Tile* GetTile(unsigned int row = 0, unsigned int col = 0)
	{
		if(!(row<0 || row >= m_Rows || col<0 || col >= m_Cols))
		return m_pTiles[(row*m_Cols) + col];
		return nullptr;
	}
private:
	void CreateTiles();
	vector<GameObject*> m_ObstaclesAndEnems;
	vector<GameObject*> m_pPlayers;
	vector<Tile*> m_pTiles;
	wstring m_FileName;
	unsigned int m_Rows, m_Cols;
	bool m_LoadAll;
	bool m_DoLoad;
};

