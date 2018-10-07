#pragma once
#include <Scenegraph/GameScene.h>

class BoneObject;
class SoftwareSkinningScene_1: public GameScene
{
public:
	SoftwareSkinningScene_1();
	~SoftwareSkinningScene_1();

	void Initialize(const GameContext& gameContext) override;
	void Update(const GameContext& gameContext) override;
	void Draw(const GameContext& gameContext) override;
	void SceneActivated() override;
	void SceneDeactivated() override;

private:

	BoneObject * m_pBone0, *m_pBone1;
	float m_BoneRotation;
	int m_RotationSign;
private:
	SoftwareSkinningScene_1(const SoftwareSkinningScene_1 &obj) = delete;
	SoftwareSkinningScene_1& operator=(const SoftwareSkinningScene_1& obj) = delete;
};

