#pragma once
#include "Scenegraph/GameObject.h"
#include "Components/ParticleEmitterComponent.h"
#include "Map/Tile.h"

class BomberMan;
class Bomb:public GameObject
{
public:
	Bomb(BomberMan& player, int radius);
	~Bomb();
	void Initialize(const GameContext& gameContext) override;
	void Update(const GameContext& gameContext) override;
	void SetTile(Tile* tile) { m_pTile = tile; }
	void Explode(const GameContext& gameContext);

private:

	void AffectTiles(const GameContext& gameContext);

	vector<Tile*> m_ExplTiles;
	unsigned int m_Radius;
	float m_LifeDuration,m_SpawnTime,m_ExplTime;
	float m_ExplDuration;
	bool m_Exploded;
	bool m_IsPower;
	bool m_HitlistMadeUp;
	GameObject* m_pFuseFire, *m_pCollision;
	Tile* m_pTile;
	BomberMan* m_pPlayer;
	FMOD::Sound * m_pExplodeSound;
	FMOD::Channel * m_pChannel;

};

