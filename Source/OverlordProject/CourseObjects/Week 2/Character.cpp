//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "Character.h"
#include "Components\Components.h"
#include "Prefabs\Prefabs.h"
#include "Physx/PhysxManager.h"
#include "Scenegraph\GameScene.h"

Character::Character(float radius, float height, float moveSpeed) :
	m_Radius(radius),
	m_Height(height),
	m_MoveSpeed(moveSpeed),
	m_pCamera(nullptr),
	m_pController(nullptr),
	m_TotalPitch(0),
	m_TotalYaw(0),
	m_RotationSpeed(90.f),
	//Running
	m_MaxRunVelocity(50.0f),
	m_TerminalVelocity(20),
	m_Gravity(9.81f),
	m_RunAccelerationTime(0.3f),
	m_JumpAccelerationTime(0.8f),
	m_RunAcceleration(m_MaxRunVelocity / m_RunAccelerationTime),
	m_JumpAcceleration(m_Gravity / m_JumpAccelerationTime),
	m_RunVelocity(0),
	m_JumpVelocity(0),
	m_Velocity(0, 0, 0),
	m_PlayerNumber(GamepadIndex::PlayerOne)
{
}


Character::~Character(void)
{
}

void Character::Initialize(const GameContext& gameContext)
{
	
	auto physX = PhysxManager::GetInstance()->GetPhysics();
	// Create controller
	m_pController = new ControllerComponent(physX->createMaterial(0.5f,0.5f,0.f),m_Radius,m_Height );
	AddComponent(m_pController);

	// Add a fixed camera as child
	

	auto camera = new FixedCamera();
	AddChild(camera);
	// Register all Input Actions
	gameContext.pInput->AddInputAction(InputAction(LEFT, Down, VK_LEFT));
	gameContext.pInput->AddInputAction(InputAction(RIGHT, Down, VK_RIGHT));
	gameContext.pInput->AddInputAction(InputAction(FORWARD, Down, VK_UP));
	gameContext.pInput->AddInputAction(InputAction(BACKWARD, Down, VK_DOWN));
	gameContext.pInput->AddInputAction(InputAction(JUMP, Pressed, VK_SPACE));


}

void Character::PostInitialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	// Set the camera as active
	// We need to do this in the PostInitialize because child game objects only get initialized after the Initialize of the current object finishes
	auto cam = GetChild<FixedCamera>();
	m_pController->GetGameObject()->GetScene()->SetActiveCamera(cam->GetComponent<CameraComponent>());
}


void Character::Update(const GameContext& gameContext)
{
	UpdateMovement(gameContext);
}

void Character::UpdateMovement(const GameContext& gameContext)
{

	UNREFERENCED_PARAMETER(gameContext);
	//Update the character (Camera rotation, Character Movement, Character Gravity)

	//HANDLE INPUT
	XMFLOAT2 move = XMFLOAT2(0, 0);
	move.y = gameContext.pInput->IsActionTriggered(FORWARD) ? 1.0f : 0.0f;
	if (move.y == 0) move.y = -(gameContext.pInput->IsActionTriggered(BACKWARD) ? 1.0f : 0.0f);
	if (move.y == 0) move.y = gameContext.pInput->GetThumbstickPosition().y;

	move.x = gameContext.pInput->IsActionTriggered(RIGHT) ? 1.0f : 0.0f;
	if (move.x == 0) move.x = -(gameContext.pInput->IsActionTriggered(LEFT) ? 1.0f : 0.0f);
	if (move.x == 0) move.x = gameContext.pInput->GetThumbstickPosition().x;

	auto currSpeed = m_MoveSpeed;
	if (gameContext.pInput->IsKeyboardKeyDown(VK_LSHIFT))
		currSpeed *= m_RunAcceleration;

	XMFLOAT2 look = XMFLOAT2(0, 0);
	if (gameContext.pInput->IsMouseButtonDown(VK_LBUTTON))
	{
		auto mouseMove = gameContext.pInput->GetMouseMovement();
		look.x = static_cast<float>(mouseMove.x);
		look.y = static_cast<float>(mouseMove.y);
	}

	if (look.x == 0 && look.y == 0)
	{
		look = gameContext.pInput->GetThumbstickPosition(false);
	}
	//CALCULATE TRANSFORMS
	XMVECTOR forward = XMLoadFloat3(&GetTransform()->GetForward());
	XMVECTOR right = XMLoadFloat3(&GetTransform()->GetRight());
	XMVECTOR currPos = XMLoadFloat3(&GetTransform()->GetPosition());
	//XMVECTOR currRot = XMLoadFloat4(&GetTransform()->GetRotation());
	//XMVECTOR finalQuat;

	//currPos += forward * move.y * currSpeed * gameContext.pGameTime->GetElapsed();
	//currPos += right * move.x * currSpeed * gameContext.pGameTime->GetElapsed();

	XMVECTOR direction = XMVector3Normalize((forward*move.y + right * move.x));
	if (move.x != 0 || move.y != 0)
	{
		m_RunVelocity += m_RunAcceleration * gameContext.pGameTime->GetElapsed();
		Clamp(m_RunVelocity, m_MaxRunVelocity, 0.f);
		//auto component = m_Velocity.y;
		auto newVel = direction * m_RunVelocity;
		XMStoreFloat3(&m_Velocity, newVel);
		m_Velocity.y = m_JumpVelocity;

	}
	else
	{
		m_Velocity.x = 0;
		m_Velocity.z = 0;
		m_Velocity.y = m_JumpVelocity;

	}


	if ((GetComponent<ControllerComponent>()->GetCollisionFlags()) != PxControllerCollisionFlag::eCOLLISION_DOWN)
	{
		m_JumpVelocity -= m_JumpAcceleration * gameContext.pGameTime->GetElapsed();
		//Clamp(m_JumpVelocity, m_MaxRunVelocity, 0.f);

	}
	else if (gameContext.pInput->IsActionTriggered(JUMP))
	{
		m_JumpVelocity = 0;
		m_Velocity.y = 200.f;
	}
	else
	{
		m_Velocity.y = 0.f;
	}
	m_Velocity.y += m_JumpVelocity;


	m_TotalYaw += look.x * m_RotationSpeed * gameContext.pGameTime->GetElapsed();
	m_TotalPitch += look.y * m_RotationSpeed * gameContext.pGameTime->GetElapsed();

	gameContext.pCamera->GetTransform()->Rotate(m_TotalPitch, m_TotalYaw, 0);

	XMFLOAT3 displacement;
	XMStoreFloat3(&displacement, XMLoadFloat3(&m_Velocity)*gameContext.pGameTime->GetElapsed());

	GetComponent<ControllerComponent>()->Move(displacement);

}
