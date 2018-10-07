#pragma once
#include <Scenegraph/GameScene.h>

class FontTestScene: public GameScene
{
public:
	FontTestScene();
	~FontTestScene();

	void Initialize(const GameContext& gameContext) override;
	void Update(const GameContext& gameContext) override;
	void Draw(const GameContext& gameContext) override;
	void SceneActivated() override;
	void SceneDeactivated() override;

private:


private:
	FontTestScene(const FontTestScene &obj) = delete;
	FontTestScene& operator=(const FontTestScene& obj) = delete;
};

