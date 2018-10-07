//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "FurShaderTest.h"
#include "Scenegraph\GameObject.h"
#include "Diagnostics\Logger.h"
#include "Diagnostics\DebugRenderer.h"

#include "Prefabs\Prefabs.h"
#include "Components\Components.h"
#include "Physx\PhysxProxy.h"
#include "Physx\PhysxManager.h"
#include "Content\ContentManager.h"
#include "Graphics\MeshFilter.h"
#include "Materials\FurMaterial.h"
#include "Components\ModelComponent.h"
#include "Content\SpriteFontLoader.h"
#include "Graphics\TextRenderer.h"

#define FPS_COUNTER 1

FurShaderTest::FurShaderTest(void) :
	GameScene(L"FurShaderTest"),
	m_FpsInterval(FPS_COUNTER)
{
}


FurShaderTest ::~FurShaderTest(void)
{
}

void FurShaderTest::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);

	//GetPhysxProxy()->EnablePhysxDebugRendering(true);
	//gameContext.pGameTime->ForceElapsedUpperbound(true);

	// Create PhysX ground plane
	auto physX = PhysxManager::GetInstance()->GetPhysics();

	auto bouncyMaterial = physX->createMaterial(0, 0, 1);
	auto ground = new GameObject();
	ground->AddComponent(new RigidBodyComponent(true));

	std::shared_ptr<PxGeometry> geom(new PxPlaneGeometry());
	ground->AddComponent(new ColliderComponent(geom, *bouncyMaterial, PxTransform(PxQuat(XM_PIDIV2, PxVec3(0, 0, 1)))));
	AddChild(ground);


	//Material Test
	m_FurMat = new FurMaterial();
	m_FurMat->SetDiffuseColor({ 1.f,0.f,0.f,1.f });
	m_FurMat->SetDiffuseTexture(L"./Resources/Textures/bigTiger.jpg");
	m_FurMat->SetShellTex(L"./Resources/Textures/furShell.jpg");
	m_FurMat->SetFinTexture(L"./Resources/Textures/FinText2.jpg");
	m_FurMat->SetFurLength(m_FurLength);
	m_FurMat->SetGravity({ 0.f,-1.f,0.f });
	m_FurMat->SetGravityStrength(m_GravityStrength);
	m_FurMat->SetForceDirection({ -1.f,0.f,0.f });
	m_FurMat->SetForceStrength(m_ForceStrength);
	m_FurMat->SetLightDir({ 0.2f,-1.f,0.2f });
	m_FurMat->SetLayer(m_Layers);
	m_FurMat->SetFinToShellRatio(m_FinToShell);
	m_FurMat->SetUVScale(m_UVScale);
	m_FurMat->SetThickness(m_Thickness);
	m_FurMat->SetMinHairPercent(m_MinHairPercent);
	gameContext.pMaterialManager->AddMaterial(m_FurMat, 60);

	auto model = new ModelComponent(L"./Resources/Meshes/Sphere.ovm");
	model->SetMaterial(60);


	m_Sphere = new GameObject();
	m_Sphere->AddComponent(model);
	
	AddChild(m_Sphere);
	m_Sphere->GetTransform()->Translate(0, 10, 0);
	m_Sphere->GetTransform()->Scale(0.2f, 0.2f, 0.2f);
	gameContext.pInput->AddInputAction(InputAction(int(Input::LENGTHU), InputTriggerState::Pressed, '1'));
	gameContext.pInput->AddInputAction(InputAction(int(Input::LENGTHD), InputTriggerState::Pressed, '2'));
	gameContext.pInput->AddInputAction(InputAction(int(Input::GRAVITYU), InputTriggerState::Pressed, '3'));
	gameContext.pInput->AddInputAction(InputAction(int(Input::GRAVITYD), InputTriggerState::Pressed, '4'));
	gameContext.pInput->AddInputAction(InputAction(int(Input::FORCEU), InputTriggerState::Pressed, '5'));
	gameContext.pInput->AddInputAction(InputAction(int(Input::FORCED), InputTriggerState::Pressed, '6'));
	gameContext.pInput->AddInputAction(InputAction(int(Input::LAYERU), InputTriggerState::Pressed, 'E'));
	gameContext.pInput->AddInputAction(InputAction(int(Input::LAYERD), InputTriggerState::Pressed, 'R'));
	gameContext.pInput->AddInputAction(InputAction(int(Input::FTSU), InputTriggerState::Pressed, '7'));
	gameContext.pInput->AddInputAction(InputAction(int(Input::FTSD), InputTriggerState::Pressed, '8'));
	gameContext.pInput->AddInputAction(InputAction(int(Input::UVU), InputTriggerState::Pressed, 'C'));
	gameContext.pInput->AddInputAction(InputAction(int(Input::UVD), InputTriggerState::Pressed, 'V'));
	gameContext.pInput->AddInputAction(InputAction(int(Input::THICKU), InputTriggerState::Pressed, 'T'));
	gameContext.pInput->AddInputAction(InputAction(int(Input::THICKD), InputTriggerState::Pressed, 'Y'));
	gameContext.pInput->AddInputAction(InputAction(int(Input::HAIRPU), InputTriggerState::Pressed, 'G'));
	gameContext.pInput->AddInputAction(InputAction(int(Input::HAIRPD), InputTriggerState::Pressed, 'H'));
	gameContext.pInput->AddInputAction(InputAction(int(Input::RESET), InputTriggerState::Pressed, 'K'));




}

void FurShaderTest::Update(const GameContext& gameContext)
{

	m_FpsInterval += gameContext.pGameTime->GetElapsed();
	if (m_FpsInterval >= FPS_COUNTER)
	{
		m_FpsInterval -= FPS_COUNTER;
		Logger::LogFormat(LogLevel::Info, L"FPS: %i", gameContext.pGameTime->GetFPS());

	}
	float amplitude = m_ForceStrength;
	auto forceStr = sin(gameContext.pGameTime->GetTotal())*amplitude;
	m_FurMat->SetForceStrength(forceStr);


	if (gameContext.pInput->IsActionTriggered(int(Input::FORCED))) m_ForceStrength -= 0.1f;
	if (gameContext.pInput->IsActionTriggered(int(Input::FORCEU))) m_ForceStrength += 0.1f;
	if (gameContext.pInput->IsActionTriggered(int(Input::FTSD))) m_FinToShell -= 0.1f;
	if (gameContext.pInput->IsActionTriggered(int(Input::FTSU))) m_FinToShell += 0.1f;
	if (gameContext.pInput->IsActionTriggered(int(Input::GRAVITYD))) m_GravityStrength -= 0.1f;
	if (gameContext.pInput->IsActionTriggered(int(Input::GRAVITYU))) m_GravityStrength += 0.1f;
	if (gameContext.pInput->IsActionTriggered(int(Input::HAIRPD))) m_MinHairPercent -= 0.05f;
	if (gameContext.pInput->IsActionTriggered(int(Input::HAIRPU))) m_MinHairPercent += 0.05f;
	if (gameContext.pInput->IsActionTriggered(int(Input::LAYERD))) m_Layers -= 1.f;
	if (gameContext.pInput->IsActionTriggered(int(Input::LAYERU))) m_Layers += 1.f;
	if (gameContext.pInput->IsActionTriggered(int(Input::LENGTHD))) m_FurLength -= .5f;
	if (gameContext.pInput->IsActionTriggered(int(Input::UVD))) m_UVScale -= .1f;
	if (gameContext.pInput->IsActionTriggered(int(Input::UVU))) m_UVScale+= .1f;
	if (gameContext.pInput->IsActionTriggered(int(Input::THICKD)))
		m_Thickness -= .1f;
	if (gameContext.pInput->IsActionTriggered(int(Input::THICKU)))
		m_Thickness += .1f;
	if (gameContext.pInput->IsActionTriggered(int(Input::LENGTHU)))
		m_FurLength += .5f;
	if (gameContext.pInput->IsActionTriggered(int(Input::RESET)))
	{
		m_FurLength = { 1.f };
		m_GravityStrength = { .2f };
		m_ForceStrength={  1.5f };
		m_Layers = { 30.f };
		m_FinToShell = { 1.2f };
		m_UVScale = { 1.f };
		m_Thickness={ 1.f };
		m_MinHairPercent={ 0.8f };
	}



	m_FurMat->SetFurLength(m_FurLength);

	m_FurMat->SetGravityStrength(m_GravityStrength);



	m_FurMat->SetLayer(m_Layers);
	m_FurMat->SetFinToShellRatio(m_FinToShell);
	m_FurMat->SetUVScale(m_UVScale);
	m_FurMat->SetThickness(m_Thickness);
	m_FurMat->SetMinHairPercent(m_MinHairPercent);





}

void FurShaderTest::Draw(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);


	SpriteFont* pFont{ ContentManager::Load<SpriteFont>(L"Resources/SpriteFonts/Consolas_32.fnt") };
	TextRenderer::GetInstance()->DrawText(pFont, L"HairLength: 1/2" , XMFLOAT2(0.0f, 0.0f), XMFLOAT4(Colors::White));
	TextRenderer::GetInstance()->DrawText(pFont, L"GravityForce: 3/4", XMFLOAT2(0.0f, 30.0f), XMFLOAT4(Colors::White));
	TextRenderer::GetInstance()->DrawText(pFont, L"MaxWindForce: 5/6", XMFLOAT2(0.0f, 60.0f), XMFLOAT4(Colors::White));
	TextRenderer::GetInstance()->DrawText(pFont, L"FinToShellRatio: 7/8", XMFLOAT2(0.0f, 90.0f), XMFLOAT4(Colors::White));
	TextRenderer::GetInstance()->DrawText(pFont, L"Layers: E/R", XMFLOAT2(0.0f, 120.0f), XMFLOAT4(Colors::White));
	TextRenderer::GetInstance()->DrawText(pFont, L"HairThickness: T/Y", XMFLOAT2(0.0f, 150.0f), XMFLOAT4(Colors::White));
	TextRenderer::GetInstance()->DrawText(pFont, L"UVScale: C/V", XMFLOAT2(0.0f,180.0f), XMFLOAT4(Colors::White));
	TextRenderer::GetInstance()->DrawText(pFont, L"MinimumHairLength: G/H", XMFLOAT2(0.0f, 210.0f), XMFLOAT4(Colors::White));
	TextRenderer::GetInstance()->DrawText(pFont, L"Reset: K", XMFLOAT2(0.0f, 240.0f), XMFLOAT4(Colors::White));



}


