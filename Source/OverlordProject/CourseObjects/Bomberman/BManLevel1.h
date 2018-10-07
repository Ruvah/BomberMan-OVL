#pragma once
#include "Scenegraph/GameScene.h"
#include "Map/Map.h"

class BManLevel1 : public GameScene
{
	enum Options :UINT
	{
		START,
		CONTROLS,
		RESET,
		QUIT
	};
public:
	BManLevel1();
	~BManLevel1();

	void Initialize(const GameContext& gameContext) override;
	void Update(const GameContext& gameContext) override;
	void Draw(const GameContext& gameContext) override;
	void SceneActivated() override;
	void SceneDeactivated() override;
	bool GetPaused() { return m_Paused; }

private:
	BManLevel1(const BManLevel1 &obj) = delete; 
	BManLevel1& operator=(const BManLevel1& obj) = delete;

	FMOD::Sound* m_pBackgroundMusic;
	FMOD::Channel*m_pChannel;
	Map* m_Map;


	bool m_Paused;
	GameObject * m_pGameBackground;

	GameObject * m_pOptionsBackground;
	GameObject * m_pControls;

	GameObject* m_pArrow;
	vector<XMFLOAT3>m_OptionsPos;
	Options m_CurrSelect;
};

