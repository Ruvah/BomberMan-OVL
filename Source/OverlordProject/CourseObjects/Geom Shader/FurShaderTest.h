#pragma once
#include "Scenegraph/GameScene.h"
#include "Helpers\EffectHelper.h"

class Material;
class FurMaterial;
class GameObject;

class FurShaderTest : public GameScene
{
	enum class Input
	{
		LENGTHU,
		LENGTHD,
		GRAVITYU,
		GRAVITYD,
		FORCEU,
		FORCED,
		LAYERU,
		LAYERD,
		FTSU,
		FTSD,
		UVU,
		UVD,
		THICKU,
		THICKD,
		HAIRPU,
		HAIRPD,
		RESET
	};
public:
	FurShaderTest(void);
	virtual ~FurShaderTest(void);

protected:

	virtual void Initialize(const GameContext& gameContext);
	virtual void Update(const GameContext& gameContext);
	virtual void Draw(const GameContext& gameContext);

private:

	float m_FpsInterval;
	FurMaterial* m_FurMat;
	GameObject* m_Sphere;


	float m_FurLength{ 1.f}, m_GravityStrength{ .2f},
		m_ForceStrength{ 1.5f }, m_Layers{ 30.f }, m_FinToShell{ 1.2f }, m_UVScale{1.f}
	, m_Thickness{ 1.f }, m_MinHairPercent{ 0.8f };

	

private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	FurShaderTest(const FurShaderTest  &obj);
	FurShaderTest & operator=(const FurShaderTest & obj);
};