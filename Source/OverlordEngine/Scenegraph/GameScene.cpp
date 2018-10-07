//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "GameScene.h"
#include "GameObject.h"
#include "SceneManager.h"
#include "../Diagnostics/Logger.h"
#include "../Prefabs/Prefabs.h"
#include "../Components/Components.h"
#include "../Diagnostics/DebugRenderer.h"
#include "../Physx/PhysxProxy.h"
#include <SoundManager.h>
#include "../Graphics/SpriteRenderer.h"
#include "../Graphics/TextRenderer.h"
#include "../Graphics/PostProcessingMaterial.h"
#include "../Base/OverlordGame.h"
#include "../Graphics/RenderTarget.h"
#include "../Graphics/ShadowMapRenderer.h"

GameScene::GameScene(wstring sceneName):
	m_pChildren(vector<GameObject*>()),
	m_GameContext(GameContext()),
	m_IsInitialized(false),
	m_SceneName(sceneName),
	m_pDefaultCamera(nullptr),
	m_pActiveCamera(nullptr),
	m_pPhysxProxy(nullptr),
	m_PostProcessingMaterialIds(vector<UINT>())
{
}


GameScene::~GameScene(void)
{
	SafeDelete(m_GameContext.pGameTime);
	SafeDelete(m_GameContext.pInput);
	SafeDelete(m_GameContext.pMaterialManager);

	for(GameObject* pChild:m_pChildren)
	{
		SafeDelete(pChild);
	}

	SafeDelete(m_pPhysxProxy);
    SafeDelete(m_GameContext.pShadowMapRenderer);
}

void GameScene::AddChild(GameObject* obj)
{
#if _DEBUG
	if(obj->m_pParentScene)
	{
		if(obj->m_pParentScene == this)
			Logger::LogWarning(L"GameScene::AddChild > GameObject is already attached to this GameScene");
		else
			Logger::LogWarning(L"GameScene::AddChild > GameObject is already attached to another GameScene. Detach it from it's current scene before attaching it to another one.");

		return;
	}

	if(obj->m_pParentObject)
	{
		Logger::LogWarning(L"GameScene::AddChild > GameObject is currently attached to a GameObject. Detach it from it's current parent before attaching it to another one.");
		return;
	}
#endif

	obj->m_pParentScene = this;
	obj->RootInitialize(m_GameContext);
	m_pChildren.push_back(obj);
}

void GameScene::RemoveChild(GameObject* obj, bool deleteObject)
{
	auto it = find(m_pChildren.begin(), m_pChildren.end(), obj);

#if _DEBUG
	if(it == m_pChildren.end())
	{
		Logger::LogWarning(L"GameScene::RemoveChild > GameObject to remove is not attached to this GameScene!");
		return;
	}
#endif
	if (it == m_pChildren.end())
	{
		return;
	}
	m_pChildren.erase(it);
	if(deleteObject)
	{
		delete obj;
		obj = nullptr;
	}
	else
		obj->m_pParentScene = nullptr;
}

void GameScene::RootInitialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	if(m_IsInitialized)
		return;

	//Create DefaultCamera
	auto freeCam = new FreeCamera();
	freeCam->SetRotation(30, 0);
	freeCam->GetTransform()->Translate(0,50,-80);
	AddChild(freeCam);
	m_pDefaultCamera = freeCam->GetComponent<CameraComponent>();
	m_pActiveCamera = m_pDefaultCamera;
	m_GameContext.pCamera = m_pDefaultCamera;

	//Create GameContext
	m_GameContext.pGameTime = new GameTime();
	m_GameContext.pGameTime->Reset();
	m_GameContext.pGameTime->Stop();

	m_GameContext.pInput = new InputManager();
	m_GameContext.pInput->Initialize();

	m_GameContext.pMaterialManager = new MaterialManager();

	m_GameContext.pDevice = pDevice;
	m_GameContext.pDeviceContext = pDeviceContext;

    m_GameContext.pShadowMapRenderer = new ShadowMapRenderer{};
    m_GameContext.pShadowMapRenderer->Initialize(m_GameContext);

	// Initialize Physx
	m_pPhysxProxy = new PhysxProxy();
	m_pPhysxProxy->Initialize(this);

	//User-Scene Initialize
	Initialize(m_GameContext);

	//Root-Scene Initialize
	for(GameObject* pChild:m_pChildren)
	{
		pChild->RootInitialize(m_GameContext);
	}

	//Initialize Materials
	m_GameContext.pMaterialManager->Initialize(m_GameContext);

	m_IsInitialized = true;
}

void GameScene::RootUpdate()
{
	m_GameContext.pGameTime->Update();
	m_GameContext.pInput->Update();
	m_GameContext.pCamera = m_pActiveCamera;

	SoundManager::GetInstance()->GetSystem()->update();

	//User-Scene Update
	Update(m_GameContext);

	//Root-Scene Update
	vector<GameObject*> toDelete;
	int size = m_pChildren.size();
	for(int i=0; i<size;++i)
	{
		if(i<size && unsigned int(i)<m_pChildren.size())
		{
			if(!m_pChildren[i]->m_ToDestroy)
			m_pChildren.at(i)->RootUpdate(m_GameContext);
			else
			{
				toDelete.push_back(m_pChildren[i]);
			}
		}
	}
	for(auto obj:toDelete)
	{
		RemoveChild(obj);
	}

	//for(GameObject* pChild:m_pChildren)
	//{
	//	
	//	pChild->RootUpdate(m_GameContext);
	//}

	m_pPhysxProxy->Update(m_GameContext);
}

void GameScene::RootDraw()
{
    // Object Shadows
    m_GameContext.pShadowMapRenderer->Begin(m_GameContext);
    for (GameObject* pChild : m_pChildren)
    {
        pChild->RootDraw_ShadowMap(m_GameContext);
    }
    m_GameContext.pShadowMapRenderer->End(m_GameContext);

	//User-Scene Draw
	Draw(m_GameContext);

	//Object-Scene Draw
	for(GameObject* pChild:m_pChildren)
	{
		pChild->RootDraw(m_GameContext);
	}

	//Object-Scene Post-Draw
	for (GameObject* pChild : m_pChildren)
	{
		pChild->RootPostDraw(m_GameContext);
	}

	//Draw PhysX
	m_pPhysxProxy->Draw(m_GameContext);

	//Draw Debug Stuff
	DebugRenderer::Draw(m_GameContext);
	SpriteRenderer::GetInstance()->Draw(m_GameContext);
	TextRenderer::GetInstance()->Draw(m_GameContext);

	//Post Processing Implementation START
	//************************************

	//1. Check if our MaterialIds vector isn't empty (if empty, ignore PP)
    if(!m_PostProcessingMaterialIds.empty())
    {
        //2. Get the current (= INIT_RT) rendertarget from the game (OverlordGame::GetRenderTarget...)
        RenderTarget* pInitRt = SceneManager::GetInstance()->GetGame()->GetRenderTarget();
       
        //3. Create a new variable to hold our previous rendertarget (= PREV_RT) that holds the content of the previous drawcall 
        //   and want to use as a ShaderResource (Texture) for the next PP Effect.
        RenderTarget* pPrevRt = pInitRt;
        
        //4. Set PREV_RT = INIT_RT (The first pp effect uses the contents from the default rendertarget)
        //4. For each MaterialID
        for (unsigned int id : m_PostProcessingMaterialIds)
        {
            //4.1 Get the corresponding PostProcessingMaterial (= CURR_MAT) through MaterialManager
            auto ppMat = m_GameContext.pMaterialManager->GetMaterial_PP(id);
            //4.2 Get the RT from CURR_MAT (= TEMP_RT)
            RenderTarget* pTempRt = ppMat->GetRenderTarget();
            //4.3 Use TEMP_RT as current rendertarget (OverlordGame::SetRenderTarget)
            SceneManager::GetInstance()->GetGame()->SetRenderTarget(pTempRt);
            //4.4 Draw CURR_MAT (PREV_MAT provides the Texture used in the PPEffect)
            ppMat->Draw(m_GameContext, pPrevRt);
            //4.5 Change PREV_MAT to TEMP_MAT
            pPrevRt = pTempRt;
        }

        //5. Restore the current rendertarget with INIT_RT
        SceneManager::GetInstance()->GetGame()->SetRenderTarget(pInitRt);

        //6. Use SpriteRenderer::DrawImmediate to draw the content of the last postprocessed rendertarget > PREV_RT 
        SpriteRenderer::GetInstance()->DrawImmediate(m_GameContext, pPrevRt->GetShaderResourceView(), XMFLOAT2{ 0,0 });
    }
}

void GameScene::RootSceneActivated()
{
	//Start Timer
	m_GameContext.pGameTime->Start();
	SceneActivated();
}

void GameScene::RootSceneDeactivated()
{
	//Stop Timer
	m_GameContext.pGameTime->Stop();
	SceneDeactivated();
}

void GameScene::RootWindowStateChanged(int state, bool active)
{
	//TIMER
	if (state == 0)
	{
		if (active)m_GameContext.pGameTime->Start();
		else m_GameContext.pGameTime->Stop();
	}
}

void GameScene::AddPostProcessingMaterial(UINT materialId_PP)
{
	//Add the given ID to the MaterialIDs vector
    //Check for duplicates
    //Hint: std::find

    auto it = find(m_PostProcessingMaterialIds.begin(), m_PostProcessingMaterialIds.end(), materialId_PP);

    if(it == m_PostProcessingMaterialIds.end())
    {
        m_PostProcessingMaterialIds.emplace_back(materialId_PP);
    }
}

void GameScene::RemovePostProcessingMaterial(UINT materialId_PP)
{
	//Remove the given ID from the MaterialsIDs vector
	//Hint: std::erase

    auto it = find(m_PostProcessingMaterialIds.begin(), m_PostProcessingMaterialIds.end(), materialId_PP);

    m_PostProcessingMaterialIds.erase(it);
}

void GameScene::SetActiveCamera(CameraComponent* pCameraComponent)
{ 
	//nullptr for DefaultCamera

	if(m_pActiveCamera!=nullptr)
		m_pActiveCamera->m_IsActive = false;

	m_pActiveCamera = (pCameraComponent)?pCameraComponent:m_pDefaultCamera;
	m_pActiveCamera->m_IsActive = true;
}
