#pragma once
#include "Scenegraph/GameScene.h"

class ModelComponent;

class HardwareSkinningScene : public GameScene
{
public:
	HardwareSkinningScene(void);
	virtual ~HardwareSkinningScene(void);

protected:

	virtual void Initialize(const GameContext& gameContext);
	virtual void Update(const GameContext& gameContext);
	virtual void Draw(const GameContext& gameContext);

private:

	ModelComponent* m_pModel;
	int m_AnimationIndex = 0;

private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	HardwareSkinningScene(const HardwareSkinningScene &obj);
	HardwareSkinningScene& operator=(const HardwareSkinningScene& obj);
};

