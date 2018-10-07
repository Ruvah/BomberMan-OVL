#pragma once
#include "Scenegraph/GameScene.h"

class EndScreen:public GameScene
{
public:
	EndScreen();
	~EndScreen();

	void Initialize(const GameContext& gameContext) override;
	void Update(const GameContext& gameContext) override;
	void Draw(const GameContext& gameContext) override;

	static bool m_Lost;

private:
	GameObject * m_BgLost, *m_BgWin;
};

