#pragma once
#include "Scenegraph/GameObject.h"
#include "Components/ModelComponent.h"

class AnimatedMesh final :public GameObject
{
public:
	AnimatedMesh(const wstring& path);


	void Initialize(const GameContext& gameContext) override;
	void NextAnim();
	void PreviousAnim();
	void Play();
	void Pause();
	void SetAnim(unsigned int anim);
	void SetAnimationSpeed(float speedPercentage);

private:
	wstring m_FilePath;
	ModelComponent* m_pModel;

	UINT m_CurrentClip;
};

