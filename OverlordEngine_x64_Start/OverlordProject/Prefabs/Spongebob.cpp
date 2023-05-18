#include "stdafx.h"
#include "Spongebob.h"

#include <corecrt_math_defines.h>

#include "Character.h"
#include "Materials/Shadow/DiffuseMaterial_Shadow_Skinned.h"

Spongebob::~Spongebob()
{
	for (UINT i{ 0 }; i < m_ClipCount; ++i)
	{
		delete[] m_ClipNames[i];
	}

	delete[] m_ClipNames;
}

void Spongebob::Initialize(const SceneContext& sceneContext)
{
	const auto pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);

	//Character
	CharacterDesc characterDesc{ pDefaultMaterial, 5.5f, 2.f };
	characterDesc.actionId_MoveForward = CharacterMoveForward;
	characterDesc.actionId_MoveBackward = CharacterMoveBackward;
	characterDesc.actionId_MoveLeft = CharacterMoveLeft;
	characterDesc.actionId_MoveRight = CharacterMoveRight;
	characterDesc.actionId_Jump = CharacterJump;
	characterDesc.actionId_Attack = Attack;

	characterDesc.moveAccelerationTime = 0.1f;
	characterDesc.maxMoveSpeed = 70.f;
	characterDesc.fallAccelerationTime = 0.5f;
	characterDesc.maxFallSpeed = 50.f;
	characterDesc.JumpSpeed = 40.f;

	m_pCharacter = AddChild(new Character(characterDesc, { 0, 0, -50 }));
	m_pCharacter->SetTag(L"Player");
	
	//Mesh
	m_pSpongebobMesh = new GameObject();

	auto pSpongeMat = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow_Skinned>();
	pSpongeMat->SetDiffuseTexture(L"Exam/Textures/Spongebob.png");

	ModelComponent* pModel = new ModelComponent(L"Exam/Meshes/SpongebobNew.ovm");
	m_pSpongebobMesh->AddComponent<ModelComponent>(pModel);
	pModel->SetMaterial(pSpongeMat);
	m_pSpongebobMesh->GetTransform()->Scale(1.f);
	m_pSpongebobMesh->GetTransform()->Translate(0,0,0);

	AddChild(m_pSpongebobMesh);
	
	//Animations------------------------
	pAnimator = pModel->GetAnimator();
	pAnimator->SetAnimation(m_AnimationClipId);
	pAnimator->SetAnimationSpeed(m_AnimationSpeed);

	//Gather Clip Names
	m_ClipCount = pAnimator->GetClipCount();
	m_ClipNames = new char* [m_ClipCount];
	for (UINT i{ 0 }; i < m_ClipCount; ++i)
	{
		auto clipName = StringUtil::utf8_encode(pAnimator->GetClip(static_cast<int>(i)).name);
		const auto clipSize = clipName.size();
		m_ClipNames[i] = new char[clipSize + 1];
		strncpy_s(m_ClipNames[i], clipSize + 1, clipName.c_str(), clipSize);
	}


	//Input
	auto inputAction = InputAction(CharacterMoveLeft, InputState::down, 'A');
	sceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterMoveRight, InputState::down, 'D');
	sceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterMoveForward, InputState::down, 'W');
	sceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterMoveBackward, InputState::down, 'S');
	sceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(Attack, InputState::down, 'E');
	sceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterJump, InputState::pressed, VK_SPACE, -1, XINPUT_GAMEPAD_A);
	sceneContext.pInput->AddInputAction(inputAction);

	//Light
	sceneContext.pLights->SetDirectionalLight({ -5.6139526f,5.1346436f,-4.1850471f },
		{ 0.740129888f, -0.597205281f, 0.309117377f });
}

void Spongebob::PlayCorrectAnimation()
{
	if (m_pCharacter->IsAttacking() &&
		m_AnimationClipId != 3)
	{
		pAnimator->Pause();
		m_AnimationClipId = 3;
	}
	else if (!m_pCharacter->IsAttacking())
	{
		if ((abs(m_pCharacter->GetVelocity().x) > 0 || abs(m_pCharacter->GetVelocity().z) > 0) &&
			m_AnimationClipId != 1)
		{
			pAnimator->Pause();
			m_AnimationClipId = 1;
		}
		else if ((abs(m_pCharacter->GetVelocity().x) == 0 && abs(m_pCharacter->GetVelocity().z) == 0) &&
			m_AnimationClipId != 0)
		{
			pAnimator->Pause();
			m_AnimationClipId = 0;
		}
	}

	if (!pAnimator->IsPlaying())
	{
		pAnimator->SetAnimation(m_AnimationClipId);
		pAnimator->Play();
	}
}

void Spongebob::Update(const SceneContext& sceneContext)
{
	//Light
	sceneContext.pLights->GetDirectionalLight().position.x = m_pSpongebobMesh->GetTransform()->GetPosition().x - 10;
	sceneContext.pLights->GetDirectionalLight().position.y = m_pSpongebobMesh->GetTransform()->GetPosition().y + 5;
	sceneContext.pLights->GetDirectionalLight().position.z = m_pSpongebobMesh->GetTransform()->GetPosition().z - 5;

	//used manual position adjustement instead of childing mesh to parent, 
	//because the mesh always pointed to the camera when it was a child of the characterComponent

	auto pos = m_pCharacter->GetTransform()->GetPosition();
	pos.y -= 4.f; //offset to put spongebob on same height as capsule
	m_pSpongebobMesh->GetTransform()->Translate(pos);

	auto rot = std::atan2f(m_pCharacter->GetVelocity().z, -m_pCharacter->GetVelocity().x) * float(180 / M_PI) + 90;
	m_pSpongebobMesh->GetTransform()->Rotate(0, rot, 0);

	PlayCorrectAnimation();
	//UpdateHUDElements();

	//CheckDeletedObjects();
}

void Spongebob::SetControllerPosition(const XMFLOAT3& pos)
{
	m_pCharacter->GetTransform()->Translate(pos);
}
