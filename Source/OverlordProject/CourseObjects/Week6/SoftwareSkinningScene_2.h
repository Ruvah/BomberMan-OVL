#pragma once
#include <Scenegraph/GameScene.h>

struct SkinnedVertex;
class MeshDrawComponent;
class BoneObject;
class SoftwareSkinningScene_2: public GameScene
{
public:
	SoftwareSkinningScene_2();
	~SoftwareSkinningScene_2();

	void Initialize(const GameContext& gameContext) override;
	void Update(const GameContext& gameContext) override;
	void Draw(const GameContext& gameContext) override;
	void SceneActivated() override;
	void SceneDeactivated() override;

private:

	BoneObject * m_pBone0, *m_pBone1;
	float m_BoneRotation;
	int m_RotationSign;
	MeshDrawComponent* m_pMeshDrawer;
	vector<SkinnedVertex> m_SkinnedVertices;
	void CreateMesh(float length);
private:
	SoftwareSkinningScene_2(const SoftwareSkinningScene_2 &obj) = delete;
	SoftwareSkinningScene_2& operator=(const SoftwareSkinningScene_2& obj) = delete;
};

