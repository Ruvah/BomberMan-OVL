#pragma once
#include "Scenegraph/GameScene.h"

class MainMenu:public GameScene
{
	enum Options:UINT
	{
		START,
		CONTROLS,
		QUIT
	};
public:
	MainMenu();
	~MainMenu();

	void Initialize(const GameContext& gameContext) override;
	void Update(const GameContext& gameContext) override;
	void Draw(const GameContext& gameContext) override;

private:
	GameObject * m_pBackgroundTex;
	GameObject * m_pControls;

	GameObject* m_pArrow;
	vector<XMFLOAT3>m_OptionsPos;
	Options m_CurrSelect;
};

