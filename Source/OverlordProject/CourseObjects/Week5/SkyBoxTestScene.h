#pragma once
#include <Scenegraph/GameScene.h>

class SkyBoxTestScene: public GameScene
{
public:
	SkyBoxTestScene();
	~SkyBoxTestScene();

	void Initialize(const GameContext& gameContext) override;
	void Update(const GameContext& gameContext) override;
	void Draw(const GameContext& gameContext) override;
	void SceneActivated() override;
	void SceneDeactivated() override;

private:

	GameObject* m_pTorus0 = nullptr;
	GameObject* m_pTorus1 = nullptr;
	GameObject* m_pTorus2 = nullptr;

	float m_TotalRotation = 0.f;

private:
	SkyBoxTestScene(const SkyBoxTestScene &obj) = delete;
	SkyBoxTestScene& operator=(const SkyBoxTestScene& obj) = delete;
};

