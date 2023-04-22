//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"
#include "PongScene.h"

#include "Prefabs/CubePrefab.h"
#include "Prefabs/SpherePrefab.h"

PongScene::PongScene() :
	GameScene(L"PongScene") {}

void PongScene::Initialize()
{
	auto& phys = PxGetPhysics();
	auto pBouncyMaterial = phys.createMaterial(0, 0, 1.f);

	auto pCamera = new FixedCamera();
	AddChild(pCamera);
	pCamera->GetTransform()->Rotate(90, 0, 0);
	pCamera->GetTransform()->Translate(0, 25, 0);
	pCamera->GetComponent<CameraComponent>()->SetActive();

	//GameSettings
	GameScene::GetSceneSettings().clearColor = {};
	GameScene::GetSceneSettings().drawGrid = false;
	GameScene::GetSceneSettings().drawPhysXDebug = false;

	//Red sphere
	m_pSphere = new SpherePrefab(0.5f, 10, XMFLOAT4(Colors::Red));
	AddChild(m_pSphere);

	m_pSphere->GetTransform()->Translate(0, 0, 0);

	m_pSphereCollider = m_pSphere->AddComponent(new RigidBodyComponent(false));
	m_pSphereCollider->AddCollider(PxSphereGeometry(0.3f), *pBouncyMaterial);
	m_pSphereCollider->SetConstraint(RigidBodyConstraint::TransY, false);


	//Boundaries
	const XMFLOAT3 boundSize = { 35, 1, 1 };
	m_DistanceBoundaries = 10;
	auto pTopBound = new CubePrefab(boundSize.x, boundSize.y, boundSize.z, XMFLOAT4{ Colors::Black });
	AddChild(pTopBound);
	pTopBound->GetTransform()->Translate(0, 0, m_DistanceBoundaries);
	auto pRigidBodyTop = pTopBound->AddComponent(new RigidBodyComponent(true));
	pRigidBodyTop->AddCollider(PxBoxGeometry(boundSize.x / 2, boundSize.y / 2, boundSize.z / 2), *pBouncyMaterial);


	auto pBottomBound = new CubePrefab(boundSize.x, boundSize.y, boundSize.z, XMFLOAT4{ Colors::Black });
	AddChild(pBottomBound);
	pBottomBound->GetTransform()->Translate(0, 0, -m_DistanceBoundaries);
	auto pRigidBodyBottom = pBottomBound->AddComponent(new RigidBodyComponent(true));
	pRigidBodyBottom->AddCollider(PxBoxGeometry(boundSize.x / 2, boundSize.y / 2, boundSize.z / 2), *pBouncyMaterial);

	m_DistanceBoundaries -= boundSize.z / 2; // minus half boundSize to get inner z-pos for boundary checking on peddles

	//Goals
	{
		const XMFLOAT3 goalSize = { 1, 1, 25 };
		const float goalDistance = 18;

		auto pLeftGoal = new CubePrefab(goalSize.x, goalSize.y, goalSize.z, XMFLOAT4{ Colors::Black });
		AddChild(pLeftGoal);
		pLeftGoal->GetTransform()->Translate(-goalDistance, 0, 0);
		auto pRigidBodyLeftGoal = pLeftGoal->AddComponent(new RigidBodyComponent(true));
		pRigidBodyLeftGoal->AddCollider(PxBoxGeometry(goalSize.x / 2, goalSize.y / 2, goalSize.z / 2), *pBouncyMaterial, true);


		auto pRightGoal = new CubePrefab(goalSize.x, goalSize.y, goalSize.z, XMFLOAT4{ Colors::Black });
		AddChild(pRightGoal);
		pRightGoal->GetTransform()->Translate(goalDistance, 0, 0);
		auto pRigidBodyRightGoal = pRightGoal->AddComponent(new RigidBodyComponent(true));
		pRigidBodyRightGoal->AddCollider(PxBoxGeometry(goalSize.x / 2, goalSize.y / 2, goalSize.z / 2), *pBouncyMaterial, true);


		auto onTrigger = [&](GameObject*, GameObject*, PxTriggerAction action)
		{
			if (action == PxTriggerAction::ENTER)
			{
				Reset();
			}
		};

		pLeftGoal->SetOnTriggerCallBack(onTrigger);
		pRightGoal->SetOnTriggerCallBack(onTrigger);
	}

	//Paddles
	const XMFLOAT3 paddleSize = { 1.3f, 1, 5.6f };
	m_LengthPaddle = paddleSize.z;
	const float distancePaddles = 14;

	m_pLeftPaddle = new CubePrefab(paddleSize.x, paddleSize.y, paddleSize.z, XMFLOAT4{ Colors::White });
	AddChild(m_pLeftPaddle);
	m_pLeftPaddle->GetTransform()->Translate(-distancePaddles, 0, 0);
	auto pRigidBodyPaddleLeft = m_pLeftPaddle->AddComponent(new RigidBodyComponent(true));
	pRigidBodyPaddleLeft->AddCollider(PxBoxGeometry(paddleSize.x / 2, paddleSize.y / 2, paddleSize.z / 2), *pBouncyMaterial);

	m_pRightPaddle = new CubePrefab(paddleSize.x, paddleSize.y, paddleSize.z, XMFLOAT4{ Colors::White });
	AddChild(m_pRightPaddle);
	m_pRightPaddle->GetTransform()->Translate(distancePaddles, 0, 0);
	auto pRigidBodyPaddleRight = m_pRightPaddle->AddComponent(new RigidBodyComponent(true));
	pRigidBodyPaddleRight->AddCollider(PxBoxGeometry(paddleSize.x / 2, paddleSize.y / 2, paddleSize.z / 2), *pBouncyMaterial);

	
}

void PongScene::Update()
{
	if (m_SceneContext.pInput->IsKeyboardKey(InputState::down, VK_SPACE) &&
		!m_IsBalMoving)
	{
		m_pSphereCollider->AddForce({ 3000, 0, 2600 });
		m_IsBalMoving = true;
	}

	MovePaddles();
}



void PongScene::Draw()
{

}

void PongScene::OnGUI()
{

}

void PongScene::MovePaddles()
{
	const auto speed = 20.f * m_SceneContext.pGameTime->GetElapsed(); //speed not tested

	if (m_SceneContext.pInput->IsKeyboardKey(InputState::down, VK_UP))
	{
		auto pos = m_pRightPaddle->GetTransform()->GetPosition();
		m_pRightPaddle->GetTransform()->Translate(pos.x, pos.y, pos.z + speed);

		if (m_pRightPaddle->GetTransform()->GetPosition().z + m_LengthPaddle / 2 >= m_DistanceBoundaries)
		{
			m_pRightPaddle->GetTransform()->Translate(m_pRightPaddle->GetTransform()->GetPosition().x,
				m_pRightPaddle->GetTransform()->GetPosition().y, m_DistanceBoundaries - m_LengthPaddle / 2);
		}
	}
	if (m_SceneContext.pInput->IsKeyboardKey(InputState::down, VK_DOWN))
	{
		auto pos = m_pRightPaddle->GetTransform()->GetPosition();
		m_pRightPaddle->GetTransform()->Translate(pos.x, pos.y, pos.z - speed);

		if (m_pRightPaddle->GetTransform()->GetPosition().z - m_LengthPaddle / 2 <= -m_DistanceBoundaries)
		{
			m_pRightPaddle->GetTransform()->Translate(m_pRightPaddle->GetTransform()->GetPosition().x,
				m_pRightPaddle->GetTransform()->GetPosition().y, -m_DistanceBoundaries + m_LengthPaddle / 2);

		}
	}

	//Left
	if (m_SceneContext.pInput->IsKeyboardKey(InputState::down, VK_SHIFT))
	{
		auto pos = m_pLeftPaddle->GetTransform()->GetPosition();
		m_pLeftPaddle->GetTransform()->Translate(pos.x, pos.y, pos.z + speed);

		if (m_pLeftPaddle->GetTransform()->GetPosition().z + m_LengthPaddle / 2 >= m_DistanceBoundaries)
		{
			m_pLeftPaddle->GetTransform()->Translate(m_pLeftPaddle->GetTransform()->GetPosition().x,
				m_pLeftPaddle->GetTransform()->GetPosition().y, m_DistanceBoundaries - m_LengthPaddle / 2);
		}
	}
	if (m_SceneContext.pInput->IsKeyboardKey(InputState::down, VK_CONTROL))
	{
		auto pos = m_pLeftPaddle->GetTransform()->GetPosition();
		m_pLeftPaddle->GetTransform()->Translate(pos.x, pos.y, pos.z - speed);

		if (m_pLeftPaddle->GetTransform()->GetPosition().z - m_LengthPaddle / 2 <= -m_DistanceBoundaries)
		{
			m_pLeftPaddle->GetTransform()->Translate(m_pLeftPaddle->GetTransform()->GetPosition().x,
				m_pLeftPaddle->GetTransform()->GetPosition().y, -m_DistanceBoundaries + m_LengthPaddle / 2);

		}
	}
}



void PongScene::Reset()
{
	m_pSphere->GetTransform()->Translate(0, 0, 0);
	m_IsBalMoving = false;
}
