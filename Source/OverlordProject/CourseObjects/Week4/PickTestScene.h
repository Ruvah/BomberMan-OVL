#pragma once
#include "Scenegraph/GameScene.h"
#include "Helpers\EffectHelper.h"

class Material;
enum input
{
	LBUTTON
};
class PickTestScene : public GameScene
{
public:
	PickTestScene(void);
	virtual ~PickTestScene(void);

protected:

	virtual void Initialize(const GameContext& gameContext);
	virtual void Update(const GameContext& gameContext);
	virtual void Draw(const GameContext& gameContext);

private:

	float m_FpsInterval;

private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	PickTestScene(const PickTestScene &obj);
	PickTestScene& operator=(const PickTestScene& obj);
};

