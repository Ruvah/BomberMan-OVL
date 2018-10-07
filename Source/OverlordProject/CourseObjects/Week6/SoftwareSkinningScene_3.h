#pragma once
#include <Scenegraph/GameScene.h>

struct SkinnedVertex;
class MeshDrawComponent;
class BoneObject;
class SoftwareSkinningScene_3: public GameScene
{
public:
	SoftwareSkinningScene_3();
	~SoftwareSkinningScene_3();

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
	SoftwareSkinningScene_3(const SoftwareSkinningScene_3 &obj) = delete;
	SoftwareSkinningScene_3& operator=(const SoftwareSkinningScene_3& obj) = delete;
};

