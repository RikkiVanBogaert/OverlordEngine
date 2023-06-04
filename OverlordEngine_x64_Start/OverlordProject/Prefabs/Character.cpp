#include "stdafx.h"
#include "Character.h"

#include <corecrt_math_defines.h>

Character::Character(const CharacterDesc& characterDesc, const XMFLOAT3& cameraOffset) :
	m_CharacterDesc{ characterDesc },
	m_MoveAcceleration(characterDesc.maxMoveSpeed / characterDesc.moveAccelerationTime),
	m_FallAcceleration(characterDesc.maxFallSpeed / characterDesc.fallAccelerationTime),
	m_CameraOffset{cameraOffset}
{}

void Character::Initialize(const SceneContext& /*sceneContext*/)
{
	//Controller
	m_pControllerComponent = AddComponent(new ControllerComponent(m_CharacterDesc.controller));
	
	//Camera
	const auto pCamera = AddChild(new FixedCamera());
	m_pCameraComponent = pCamera->GetComponent<CameraComponent>();
	m_pCameraComponent->SetActive(true); //Uncomment to make this camera the active camera

	if (m_CameraOffset.y == 0) m_CameraOffset.y = m_CharacterDesc.controller.height * .5f;
	pCamera->GetTransform()->Translate(m_CameraOffset.x, m_CameraOffset.y, m_CameraOffset.z);
}

void Character::Update(const SceneContext& sceneContext)
{
	if (m_IsPaused) return;

	if (m_pCameraComponent->IsActive())
	{
		//constexpr float epsilon{ 0.01f }; //Constant that can be used to compare if a float is near zero

		//***************
		//HANDLE INPUT

		//## Input Gathering (move)
		XMFLOAT2 move{}; //Uncomment
		bool isMoving = false;

		const float stickX = sceneContext.pInput->GetThumbstickPosition(true).x;
		const float stickY = sceneContext.pInput->GetThumbstickPosition(true).y;
		//move.y should contain a 1 (Forward) or -1 (Backward) based on the active input (check corresponding actionId in m_CharacterDesc)
		if(GetScene()->GetSceneContext().pInput->IsActionTriggered(m_CharacterDesc.actionId_MoveForward))
		{
			move.y = 1;
			isMoving = true;
		}
		else if (GetScene()->GetSceneContext().pInput->IsActionTriggered(m_CharacterDesc.actionId_MoveBackward))
		{
			move.y = -1;
			isMoving = true;
		}
		else if(move.y == 0 && stickY != 0)
		{
			if (stickY > 0) move.y = 1;
			else if (stickY < 0) move.y = -1;
			isMoving = true;
		}
		//Optional: if move.y is near zero (abs(move.y) < epsilon), you could use the ThumbStickPosition.y for movement

		//move.x should contain a 1 (Right) or -1 (Left) based on the active input (check corresponding actionId in m_CharacterDesc)
		if(GetScene()->GetSceneContext().pInput->IsActionTriggered(m_CharacterDesc.actionId_MoveRight))
		{
			move.x = 1;
			isMoving = true;
		}
		else if (GetScene()->GetSceneContext().pInput->IsActionTriggered(m_CharacterDesc.actionId_MoveLeft))
		{
			move.x = -1;
			isMoving = true;
		}
		else if (move.x == 0 && stickX != 0)
		{
			if (stickX > 0) move.x = 1;
			else if (stickX < 0) move.x = -1;
			isMoving = true;
		}
		//Optional: if move.x is near zero (abs(move.x) < epsilon), you could use the Left ThumbStickPosition.x for movement

		//## Input Gathering (look)
		XMFLOAT2 look{ 0.f, 0.f }; //Uncomment
		//Only if the Left Mouse Button is Down >
			// Store the MouseMovement in the local 'look' variable (cast is required)
		const auto rightStickX = sceneContext.pInput->GetThumbstickPosition(false).x;
		const auto rightStickY = sceneContext.pInput->GetThumbstickPosition(false).y;

		if (InputManager::IsMouseButton(InputState::down, 1))
		{
			look.x = float(InputManager::GetMouseMovement().x);

			look.y = float(InputManager::GetMouseMovement().y);
		}
		else if(rightStickX != 0 || rightStickY != 0)
		{
			constexpr float rotationSpeedX = 3;
			constexpr float rotationSpeedY = 1.5f;
			look.x += rightStickX * rotationSpeedX;
			look.y -= rightStickY * rotationSpeedY;
		}
	
		//Optional: in case look.x AND look.y are near zero, you could use the Right ThumbStickPosition for look

		//************************
		//GATHERING TRANSFORM INFO

		//Retrieve the TransformComponent
		const auto transform = m_pControllerComponent->GetTransform();
		//Retrieve the forward & right vector (as XMVECTOR) from the TransformComponent
		const XMVECTOR forward = XMLoadFloat3(&transform->GetForward());
		const XMVECTOR right = XMLoadFloat3(&transform->GetRight());

		//***************
		//CAMERA ROTATION

		const auto elapsedTime = GetScene()->GetSceneContext().pGameTime->GetElapsed();
		//Adjust the TotalYaw (m_TotalYaw) & TotalPitch (m_TotalPitch) based on the local 'look' variable
		//Make sure this calculated on a framerate independent way and uses CharacterDesc::rotationSpeed.
		m_TotalYaw += m_CharacterDesc.rotationSpeed * elapsedTime * look.x;
		m_TotalPitch += m_CharacterDesc.rotationSpeed * elapsedTime * look.y;

		//clamp pitch
		if (m_TotalPitch > 40) m_TotalPitch = 40;
		if (m_TotalPitch < 0) m_TotalPitch = 0;

		//Rotate this character based on the TotalPitch (X) and TotalYaw (Y)
		GetTransform()->Rotate(m_TotalPitch, m_TotalYaw, 0);


		//********
		//MOVEMENT

		//## Horizontal Velocity (Forward/Backward/Right/Left)
		//Calculate the current move acceleration for this frame (m_MoveAcceleration * ElapsedTime)
		const auto moveAcc = m_MoveAcceleration * elapsedTime;

		//If the character is moving (= input is pressed)

		//ATTACK LOGIC
		if (GetScene()->GetSceneContext().pInput->IsActionTriggered(m_CharacterDesc.actionId_Attack))
		{
			isMoving = false;
			m_IsAttacking = true;
			std::cout << GetTransform()->GetPosition().x << ", "
				<< GetTransform()->GetPosition().y << ", "
				<< GetTransform()->GetPosition().z << "\n";
		}
		else
		{
			m_IsAttacking = false;
		}

		if(isMoving)
		{
			//Calculate & Store the current direction (m_CurrentDirection) >> based on the forward/right vectors and the pressed input
			XMStoreFloat3( &m_CurrentDirection, { move.y * forward + move.x * right });
			//Increase the current MoveSpeed with the current Acceleration (m_MoveSpeed)
			m_MoveSpeed += moveAcc;
			//Make sure the current MoveSpeed stays below the maximum MoveSpeed (CharacterDesc::maxMoveSpeed)
			if (m_MoveSpeed > m_CharacterDesc.maxMoveSpeed)
			{
				m_MoveSpeed = m_CharacterDesc.maxMoveSpeed;
			}
		}
		else
		{
			//Else (character is not moving, or stopped moving)
			//Decrease the current MoveSpeed with the current Acceleration (m_MoveSpeed)
			m_MoveSpeed -= moveAcc;
			//Make sure the current MoveSpeed doesn't get smaller than zero
			if (m_MoveSpeed < 0)
			{
				m_MoveSpeed = 0;
			}
		}

		//Now we can calculate the Horizontal Velocity which should be stored in m_TotalVelocity.xz
		//Calculate the horizontal velocity (m_CurrentDirection * MoveSpeed)
		//Set the x/z component of m_TotalVelocity (horizontal_velocity x/z)
		m_TotalVelocity.x = m_CurrentDirection.x * m_MoveSpeed;
		m_TotalVelocity.z = m_CurrentDirection.z * m_MoveSpeed;
		//It's important that you don't overwrite the y component of m_TotalVelocity (contains the vertical velocity)

		////## Vertical Movement (Jump/Fall) //Old System
		////If the Controller Component is NOT grounded (= freefall)
		//if (!(m_pControllerComponent->GetCollisionFlags() & PxControllerCollisionFlag::eCOLLISION_DOWN))
		//{
		//	//Decrease the y component of m_TotalVelocity with a fraction (ElapsedTime) of the Fall Acceleration (m_FallAcceleration)
		//	m_TotalVelocity.y -= m_FallAcceleration * elapsedTime;
		//	//Make sure that the minimum speed stays above -CharacterDesc::maxFallSpeed (negative!)
		//	if (m_TotalVelocity.y <= -m_CharacterDesc.maxFallSpeed)
		//	{
		//		m_TotalVelocity.y = -m_CharacterDesc.maxFallSpeed;
		//	}
		//}
		////Else If the jump action is triggered
		//else if(GetScene()->GetSceneContext().pInput->IsActionTriggered(m_CharacterDesc.actionId_Jump))
		//{
		//	//Set m_TotalVelocity.y equal to CharacterDesc::JumpSpeed
		//	m_TotalVelocity.y = m_CharacterDesc.JumpSpeed;
		//}
		////Else (=Character is grounded, no input pressed)
		//else
		//{
		//	//m_TotalVelocity.y is zero
		//	m_TotalVelocity.y = 0;
		//}

		//Raycast check
		PxRaycastBuffer hit{};
		const PxVec3 origin = { GetTransform()->GetPosition().x, GetTransform()->GetPosition().y - 6, GetTransform()->GetPosition().z };
		if (GetScene()->GetPhysxProxy()->Raycast(origin, { 0,-1,0 }, 3, hit))
		{
			m_OnGround = true;
			if (GetScene()->GetSceneContext().pInput->IsActionTriggered(m_CharacterDesc.actionId_Jump))
			{
				//Set m_TotalVelocity.y equal to CharacterDesc::JumpSpeed
				m_TotalVelocity.y = m_CharacterDesc.JumpSpeed;
			}
			//std::cout << "ON FLOOR\n";
			m_TimeOffGround = 0;
		}
		else
		{
			//Decrease the y component of m_TotalVelocity with a fraction (ElapsedTime) of the Fall Acceleration (m_FallAcceleration)
			m_TotalVelocity.y -= m_FallAcceleration * elapsedTime;
			//Make sure that the minimum speed stays above -CharacterDesc::maxFallSpeed (negative!)
			if (m_TotalVelocity.y <= -m_CharacterDesc.maxFallSpeed)
			{
				m_TotalVelocity.y = -m_CharacterDesc.maxFallSpeed;
			}
			//std::cout << "NOT ON FLOOR\n";
			m_TimeOffGround += elapsedTime;
			if(m_TimeOffGround > .1f)
			{
				m_OnGround = false;
			}
		}

		//************
		//DISPLACEMENT

		//The displacement required to move the Character Controller (ControllerComponent::Move) can be calculated using our TotalVelocity (m/s)
		//Calculate the displacement (m) for the current frame and move the ControllerComponent
		XMFLOAT3 displacement = { m_TotalVelocity.x * elapsedTime, m_TotalVelocity.y * elapsedTime,
			m_TotalVelocity.z * elapsedTime };
		m_pControllerComponent->Move(displacement);

		//The above is a simple implementation of Movement Dynamics, adjust the code to further improve the movement logic and behaviour.
		//Also, it can be usefull to use a seperate RayCast to check if the character is grounded (more responsive)

		
	}
}

void Character::DrawImGui()
{
	if (ImGui::CollapsingHeader("Character"))
	{
		ImGui::Text(std::format("Move Speed: {:0.1f} m/s", m_MoveSpeed).c_str());
		ImGui::Text(std::format("Fall Speed: {:0.1f} m/s", m_TotalVelocity.y).c_str());

		ImGui::Text(std::format("Move Acceleration: {:0.1f} m/s2", m_MoveAcceleration).c_str());
		ImGui::Text(std::format("Fall Acceleration: {:0.1f} m/s2", m_FallAcceleration).c_str());

		const float jumpMaxTime = m_CharacterDesc.JumpSpeed / m_FallAcceleration;
		const float jumpMaxHeight = (m_CharacterDesc.JumpSpeed * jumpMaxTime) - (0.5f * (m_FallAcceleration * powf(jumpMaxTime, 2)));
		ImGui::Text(std::format("Jump Height: {:0.1f} m", jumpMaxHeight).c_str());

		ImGui::Dummy({ 0.f,5.f });
		if (ImGui::DragFloat("Max Move Speed (m/s)", &m_CharacterDesc.maxMoveSpeed, 0.1f, 0.f, 0.f, "%.1f") ||
			ImGui::DragFloat("Move Acceleration Time (s)", &m_CharacterDesc.moveAccelerationTime, 0.1f, 0.f, 0.f, "%.1f"))
		{
			m_MoveAcceleration = m_CharacterDesc.maxMoveSpeed / m_CharacterDesc.moveAccelerationTime;
		}

		ImGui::Dummy({ 0.f,5.f });
		if (ImGui::DragFloat("Max Fall Speed (m/s)", &m_CharacterDesc.maxFallSpeed, 0.1f, 0.f, 0.f, "%.1f") ||
			ImGui::DragFloat("Fall Acceleration Time (s)", &m_CharacterDesc.fallAccelerationTime, 0.1f, 0.f, 0.f, "%.1f"))
		{
			m_FallAcceleration = m_CharacterDesc.maxFallSpeed / m_CharacterDesc.fallAccelerationTime;
		}

		ImGui::Dummy({ 0.f,5.f });
		ImGui::DragFloat("Jump Speed", &m_CharacterDesc.JumpSpeed, 0.1f, 0.f, 0.f, "%.1f");
		ImGui::DragFloat("Rotation Speed (deg/s)", &m_CharacterDesc.rotationSpeed, 0.1f, 0.f, 0.f, "%.1f");

		bool isActive = m_pCameraComponent->IsActive();
		if(ImGui::Checkbox("Character Camera", &isActive))
		{
			m_pCameraComponent->SetActive(isActive);
		}
	}
}