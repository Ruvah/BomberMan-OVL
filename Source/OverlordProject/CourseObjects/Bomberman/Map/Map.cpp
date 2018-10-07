#include "stdafx.h"

#include "Map.h"
#include "Components/TransformComponent.h"
#include "../Box.h"
#include "Scenegraph/GameScene.h"
#include "CourseObjects/Bomberman/SimpleEnemy.h"


Map::Map(unsigned int rows , unsigned int cols):
m_Rows(rows),
m_Cols(cols),
m_DoLoad(false),
m_LoadAll(true)
{

}


Map::Map(const wstring& file) :
m_FileName(file),
m_DoLoad(false),
m_LoadAll(true)

{

}

Map::~Map()
{
}

void Map::Initialize(const GameContext& gameContext)
{

	LoadMap(gameContext);

}

void Map::Update(const GameContext& gameContext)
{
	if(m_DoLoad)
	{
		LoadMap(gameContext);

		m_DoLoad = false;
	}
}


void Map::LoadMap(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	//load csv
	ifstream stream(m_FileName);
	string nextInp{};
	int playerSpawned{};
	if (stream.good())
	{
		getline(stream, nextInp, '\n');
		m_Rows = stoi(nextInp);
		getline(stream, nextInp, '\n');
		m_Cols = stoi(nextInp);

		if (m_LoadAll)
		{
			CreateTiles();
		}
		for (size_t row{}; row < m_Rows; row++)
		{
			for (size_t col{}; col < m_Cols; col++)
			{
				getline(stream, nextInp, ',');
				SpawnPoint type = SpawnPoint(stoi(nextInp));
				GameObject* obj = nullptr;
				auto pos = GetTile(row, col)->GetTransform()->GetPosition();
				pos.y += 5.f;
				switch (type)
				{
				case SpawnPoint::DESTR:
					obj = new Box(true);
					obj->GetTransform()->Translate(pos);
					GetScene()->AddChild(obj);
					m_ObstaclesAndEnems.push_back(obj);

					break;
				case SpawnPoint::NONDESTR:
					obj = new Box(false);

					obj->GetTransform()->Translate(pos);
					GetScene()->AddChild(obj);
					m_ObstaclesAndEnems.push_back(obj);

					break;
				case SpawnPoint::ENEMY1:
					obj = new SimpleEnemy();
					obj->GetTransform()->Translate(pos);
					GetScene()->AddChild(obj);
					m_ObstaclesAndEnems.push_back(obj);
					break;
				case SpawnPoint::PLAYERSPAWN:
					if (m_LoadAll)
					{
						obj = new BomberMan();
						dynamic_cast<BomberMan*>(obj)->SetPlayerNumber(GamepadIndex(m_pPlayers.size()));
						obj->GetTransform()->Translate(pos);
						m_pPlayers.push_back(obj);
						GetScene()->AddChild(obj);
					}
					else
					{

						m_pPlayers[playerSpawned]->GetTransform()->Translate(pos);

						playerSpawned++;

					}
					break;
				default:
					break;
				}
			}
		}
	}
	stream.close();

}


void Map::ResetPlayerDie()
{
	m_LoadAll = false;
	for(auto obj:m_ObstaclesAndEnems)
	{

		GetScene()->RemoveChild(obj);
		//SafeDelete(obj);
		//SafeRelease(obj);
	}
	m_ObstaclesAndEnems.clear();
	for (auto tile : m_pTiles)
	{
		tile->m_pOnTile.clear();
	}
	m_DoLoad = true;


}

void Map::ResetMap()
{
	ResetPlayerDie();
	for(auto player:m_pPlayers)
	{
		dynamic_cast<BomberMan*>(player)->SetHealth(3);
	}

}

bool Map::GetLoading()
{
	return m_DoLoad;
}

void Map::CreateTiles()
{
	const float tileSize(10.f);
	XMFLOAT3 startpos, currpos;
	startpos.x = GetTransform()->GetPosition().x - m_Cols * tileSize*0.5f;
	startpos.y = GetTransform()->GetPosition().y;
	startpos.z = GetTransform()->GetPosition().z + m_Rows * tileSize*0.5f;
	currpos = startpos;
	for (unsigned int i{}; i< m_Rows; i++)
	{
		for (unsigned int j{}; j < m_Rows; j++)
		{
			auto tile = new Tile();
			tile->m_Row = i;
			tile->m_Col = j;
			tile->m_Map = this;
			tile->GetTransform()->Translate(currpos);
			m_pTiles.push_back(tile);
			AddChild(tile);
			currpos.x += tileSize;
		}
		currpos.x = startpos.x;
		currpos.z -= tileSize;
	}
}
