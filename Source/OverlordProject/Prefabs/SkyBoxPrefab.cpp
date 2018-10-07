#include "stdafx.h"
#include "SkyBoxPrefab.h"
#include "Components/Components.h"
#include "Materials/SkyBoxMaterial.h"

SkyBoxPrefab::SkyBoxPrefab(wstring& cubeMap)
	:m_CubeMap{ cubeMap }
{
}


SkyBoxPrefab::~SkyBoxPrefab()
{
}

void SkyBoxPrefab::Initialize(const GameContext & gameContext)
{
	AddComponent(new ModelComponent(L"Resources/Meshes/Box.ovm"));
	auto skyboxMat = new SkyBoxMaterial();
	skyboxMat->SetDiffuseTexture(m_CubeMap);
	gameContext.pMaterialManager->AddMaterial(skyboxMat, 999);
	GetComponent<ModelComponent>()->SetMaterial(999);
}
