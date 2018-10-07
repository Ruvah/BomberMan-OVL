//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "MainGame.h"
#include "Base\GeneralStructs.h"
#include "Scenegraph\SceneManager.h"
#include "Physx\PhysxProxy.h"
#include "Diagnostics\DebugRenderer.h"


//#define W1
#define exam

#ifdef W1
#include "CourseObjects/Week 1/TestScene.h"
#include "CourseObjects/Week 1/ComponentTestScene.h"
#endif

#ifdef W2
#include "CourseObjects/Week 2/ModelTestScene.h"
#include "CourseObjects/Week 2/CharacterTestScene.h"
#endif
#ifdef W3
#include "CourseObjects/Week3/SpriteTestScene.h"

#endif
#ifdef W4
#include "CourseObjects/Week4/PickTestScene.h"

#endif
#ifdef W5
#include "CourseObjects/Week5/SkyBoxTestScene.h"

#endif
#ifdef W6
#include "CourseObjects/Week6/SoftwareSkinningScene_1.h"
#include "CourseObjects/Week6/SoftwareSkinningScene_2.h"
#include "CourseObjects/Week6/SoftwareSkinningScene_3.h"


#endif
#ifdef W7
#include "CourseObjects/Week 7/HardwareSkinningScene.h"

#endif

#ifdef W8
#include "CourseObjects\Week 8\PostProcessingScene.h"
#include "CourseObjects/Week 8/ParticleTestScene.h"

#endif
#ifdef W9

#include "CourseObjects\Week 9\ShadowMappingScene.h"

#endif

#ifdef geom
#include "CourseObjects/Geom Shader/FurShaderTest.h"


#endif

#ifdef exam
#include "CourseObjects/Bomberman/MainMenu.h"
#include "CourseObjects/Bomberman/BManLevel1.h"
#include "CourseObjects/Bomberman/EndScreen.h";

#endif
MainGame::MainGame(void)
{
}


MainGame::~MainGame(void)
{
}

//Game is preparing
void MainGame::OnGamePreparing(GameSettings& gameSettings)
{
	UNREFERENCED_PARAMETER(gameSettings);
	//Nothing to do atm.
}

void MainGame::Initialize()
{

#ifdef W1
	SceneManager::GetInstance()->AddGameScene(new TestScene());
	SceneManager::GetInstance()->AddGameScene(new ComponentTestScene());

	//SceneManager::GetInstance()->SetActiveGameScene(L"TestScene");
#endif

#ifdef W2
	SceneManager::GetInstance()->AddGameScene(new ModelTestScene());
	SceneManager::GetInstance()->AddGameScene(new CharacterTestScene());


	SceneManager::GetInstance()->SetActiveGameScene(L"CharacterTestScene");
#endif

#ifdef W3
	SceneManager::GetInstance()->AddGameScene(new SpriteTestScene());
	//ceneManager::GetInstance()->AddGameScene(new CharacterTestScene());


	//SceneManager::GetInstance()->SetActiveGameScene(L"CharacterTestScene");
#endif
#ifdef W4
	SceneManager::GetInstance()->AddGameScene(new PickTestScene());
	//ceneManager::GetInstance()->AddGameScene(new CharacterTestScene());


	//SceneManager::GetInstance()->SetActiveGameScene(L"CharacterTestScene");
#endif
#ifdef W5
	SceneManager::GetInstance()->AddGameScene(new SkyBoxTestScene());
	//ceneManager::GetInstance()->AddGameScene(new CharacterTestScene());


	//SceneManager::GetInstance()->SetActiveGameScene(L"CharacterTestScene");
#endif
#ifdef W6
	SceneManager::GetInstance()->AddGameScene(new SoftwareSkinningScene_3());

	SceneManager::GetInstance()->AddGameScene(new SoftwareSkinningScene_2());
	SceneManager::GetInstance()->AddGameScene(new SoftwareSkinningScene_1());


#endif
#ifdef W7
	SceneManager::GetInstance()->AddGameScene(new HardwareSkinningScene());

#endif

#ifdef W8
	//SceneManager::GetInstance()->AddGameScene(new ParticleTestScene());

	SceneManager::GetInstance()->AddGameScene(new PostProcessingScene());


#endif

#ifdef W9
	SceneManager::GetInstance()->AddGameScene(new ShadowMappingScene());

	

#endif
#ifdef geom


	SceneManager::GetInstance()->AddGameScene(new FurShaderTest());

#endif

#ifdef exam
	SceneManager::GetInstance()->AddGameScene(new MainMenu());

	SceneManager::GetInstance()->AddGameScene(new BManLevel1());
	SceneManager::GetInstance()->AddGameScene(new EndScreen());


#endif
}

LRESULT MainGame::WindowProcedureHook(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(hWnd);
	UNREFERENCED_PARAMETER(message);
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	switch (message)
	{
		case WM_KEYUP:
		{
			if ((lParam & 0x80000000) != 0x80000000)
				return -1;

			//NextScene
			if (wParam == VK_F3)
			{
				SceneManager::GetInstance()->NextScene();
				return 0;
			}
			//PreviousScene
			else if (wParam == VK_F2)
			{
				SceneManager::GetInstance()->PreviousScene();
				return 0;
			}
			else if (wParam == VK_F4)
			{
				DebugRenderer::ToggleDebugRenderer();
				return 0;
			}
			else if (wParam == VK_F6)
			{
				auto activeScene = SceneManager::GetInstance()->GetActiveScene();
				activeScene->GetPhysxProxy()->NextPhysXFrame();
			}
		}
	}

	return -1;
}
