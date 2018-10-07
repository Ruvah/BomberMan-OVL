#pragma once
class ShadowMapRenderer
{
public:

	~ShadowMapRenderer();
	void Begin(const GameContext& gameContext);
	void Draw(const GameContext& gameContexxt, MeshFilter* pMeshFilter);
};

