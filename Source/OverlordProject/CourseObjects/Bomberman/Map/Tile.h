#pragma once
#include "Scenegraph/GameObject.h"
#include <list>
#include "Components/ColliderComponent.h"

class Tile : public GameObject
{
	friend class Map;
public:
	Tile();
	~Tile();


	void Initialize(const GameContext& gameContext) override;
	void Update(const GameContext& gameContext) override;
	void PostInitialize(const GameContext& gameContext) override;

	Map* GetMap() { return m_Map; }
	pair<int, int> GetRowAndCol() { return pair<int, int>{ m_Row, m_Col }; }
	list<GameObject*> GetOnTile() { return m_pOnTile; };
	void AddOnTile(GameObject* obj);
	void Remove( GameObject* obj) { m_pOnTile.remove(obj); };
	bool IsBombOnTile();
	bool IsStopTile();

private:
	int m_Row, m_Col;
	ColliderComponent* m_pCollider;
	list<GameObject*> m_pOnTile;
	Map* m_Map;
};

