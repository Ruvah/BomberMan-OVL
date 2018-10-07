#pragma once
#include "Scenegraph/GameScene.h"

class ModelComponent;

class ShadowMappingScene : public GameScene
{
public:
	ShadowMappingScene(void);
	virtual ~ShadowMappingScene(void);

protected:

	void Initialize(const GameContext& gameContext) override;
	void Update(const GameContext& gameContext) override;
	void Draw(const GameContext& gameContext) override;

private:

	ModelComponent* m_pKnightModel = nullptr;
    int m_AnimationIndex;


	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	ShadowMappingScene(const ShadowMappingScene &obj) = delete;
	ShadowMappingScene& operator=(const ShadowMappingScene& obj) = delete;
};

