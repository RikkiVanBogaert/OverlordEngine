#include "stdafx.h"
#include "Spongebob.h"

#include <corecrt_math_defines.h>

#include "Character.h"
#include "HUDPrefab.h"
#include "PauseMenu.h"
#include "Materials/Deferred/SkinnedMaterial_Deferred.h"
#include "Materials/Shadow/DiffuseMaterial_Shadow_Skinned.h"
#include "Scenes/ExamAssignment/SpongebobScene.h"

Spongebob::Spongebob(HUDPrefab* hud):
m_pHud(hud)
{}

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
	characterDesc.fallAccelerationTime = 0.3f;
	characterDesc.maxFallSpeed = 70.f;
	characterDesc.JumpSpeed = 80.f;

	m_pCharacter = AddChild(new Character(characterDesc, { 0, 0, -50 }));
	m_pCharacter->SetTag(L"Player");


	//Mesh
	m_pSpongebobMesh = new GameObject();

	auto pSpongeMat = MaterialManager::Get()->CreateMaterial<SkinnedMaterial_Deferred>();
	pSpongeMat->SetDiffuseMap(L"Exam/Textures/Spongebob.png");

	ModelComponent* pModel = new ModelComponent(L"Exam/Meshes/SpongeMesh.ovm");
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

	inputAction = InputAction(CharacterJump, InputState::pressed, VK_SPACE /*, -1, XINPUT_GAMEPAD_A*/);
	sceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(Pause, InputState::pressed, 'P');
	sceneContext.pInput->AddInputAction(inputAction);

	////Light
	sceneContext.pLights->SetDirectionalLight({ -5.6139526f,5.1346436f,-4.1850471f },
		{ 0.740129888f, -0.597205281f, 0.309117377f });


	//Sound
	// Load the m_pWalkSound
	auto soundManager = SoundManager::Get();
	auto path = ContentManager::GetFullAssetPath(L"Exam/WalkingSound.mp3").string().c_str();
	soundManager->GetSystem()->createSound(path, 
		FMOD_DEFAULT, nullptr, &m_pWalkSound);

	soundManager->GetSystem()->playSound(m_pWalkSound, nullptr, false, &m_pSoundChannel);
	m_pSoundChannel->setPaused(true);
	m_IsSoundPlaying = false;
}

void Spongebob::Update(const SceneContext& sceneContext)
{
	CheckPausePress();

	if (m_IsPaused) return;

	////Light
	sceneContext.pLights->GetDirectionalLight().position.x = m_pSpongebobMesh->GetTransform()->GetPosition().x - 50;
	sceneContext.pLights->GetDirectionalLight().position.y = m_pSpongebobMesh->GetTransform()->GetPosition().y + 100;
	sceneContext.pLights->GetDirectionalLight().position.z = m_pSpongebobMesh->GetTransform()->GetPosition().z - 50;
	//Spotlight
	//auto meshPos = m_pSpongebobMesh->GetTransform()->GetPosition();
	//XMFLOAT4 pos{};
	//pos.x = meshPos.x - 10;
	//pos.y = meshPos.y + 30;
	//pos.z = meshPos.z - 80;
	//pos.w = 1.0f;
	//sceneContext.pLights->GetLight(0).position = pos;


	//used manual position adjustement instead of childing mesh to parent, 
	//because the mesh always pointed to the camera when it was a child of the characterComponent
	auto characterPos = m_pCharacter->GetTransform()->GetPosition();
	characterPos.y -= 6.4f; //offset to put spongebob on same height as capsule
	m_pSpongebobMesh->GetTransform()->Translate(characterPos);

	if(abs(m_pCharacter->GetVelocity().x) > 0 || abs(m_pCharacter->GetVelocity().z) > 0)
	{
		//Update Rotation if not standing still
		auto rot = std::atan2f(m_pCharacter->GetVelocity().z, -m_pCharacter->GetVelocity().x) * float(180 / M_PI) + 90;
		m_pSpongebobMesh->GetTransform()->Rotate(0, rot, 0);
	}

	UpdateAnimations();
	UpdateSounds();
}

void Spongebob::UpdateAnimations()
{
	if (m_pCharacter->IsAttacking() &&
		m_AnimationClipId != 2)
	{
		pAnimator->Pause();
		m_AnimationClipId = 2;
	}

	if(!m_pCharacter->IsOnGround() && !m_pCharacter->IsAttacking())
	{
		if(m_AnimationClipId != 1)
		{
			pAnimator->Pause();
			m_AnimationClipId = 1;
		}
		
	}
	else
	{
		if (!m_pCharacter->IsAttacking())
		{
			if ((abs(m_pCharacter->GetVelocity().x) > 0 || abs(m_pCharacter->GetVelocity().z) > 0) &&
				m_AnimationClipId != 3)
			{
				pAnimator->Pause();
				m_AnimationClipId = 3;
			}
			else if ((abs(m_pCharacter->GetVelocity().x) == 0 && abs(m_pCharacter->GetVelocity().z) == 0) &&
				m_AnimationClipId != 0)
			{
				pAnimator->Pause();
				m_AnimationClipId = 0;
			}
		}
	}

	if (!pAnimator->IsPlaying())
	{
		pAnimator->SetAnimation(m_AnimationClipId);
		pAnimator->Play();
	}
}

void Spongebob::UpdateSounds()
{
	/*if (abs(m_pCharacter->GetVelocity().x) > 0 || abs(m_pCharacter->GetVelocity().z) > 0)
	{
		auto rot = std::atan2f(m_pCharacter->GetVelocity().z, -m_pCharacter->GetVelocity().x) * float(180 / M_PI) + 90;
		m_pSpongebobMesh->GetTransform()->Rotate(0, rot, 0);

		if (m_pCharacter->IsOnGround())
		{
			if (!m_IsSoundPlaying)
			{
				m_pSoundChannel->setPaused(false);
				m_IsSoundPlaying = true;
			}
		}
		else if (m_IsSoundPlaying)
		{
			m_pSoundChannel->setPaused(true);
			m_IsSoundPlaying = false;
		}
	}
	else if (m_IsSoundPlaying)
	{
		m_pSoundChannel->setPaused(true);
		m_IsSoundPlaying = false;
	}*/

	if(m_AnimationClipId == 3)
	{
		m_pSoundChannel->setPaused(false);
		m_IsSoundPlaying = true;
	}
	else
	{
		m_pSoundChannel->setPaused(true);
		m_IsSoundPlaying = false;
	}
}

void Spongebob::CheckPausePress()
{
	if (GetScene()->GetSceneContext().pInput->IsActionTriggered(Pause))
	{
		TurnPauseMenuOnOff();
	}
}

void Spongebob::TurnPauseMenuOnOff()
{
	if (!m_IsPaused)
	{
		pPauseMenu = new PauseMenu(this);
		AddChild(pPauseMenu);
		m_IsPaused = true;
	}
	else
	{
		RemoveChild(pPauseMenu, true);
		m_IsPaused = false;
	}

	PauseCharacter(m_IsPaused);
	PauseScene(m_IsPaused);
}

void Spongebob::PauseCharacter(bool isPaused)
{
	m_pCharacter->SetPaused(isPaused);

	if(isPaused) pAnimator->Pause();
	else pAnimator->Play();

	m_pSoundChannel->setPaused(isPaused);
	m_IsSoundPlaying = !isPaused;

	m_IsPaused = isPaused;
}

void Spongebob::PauseScene(bool isPaused)
{
	auto pSpongebobScene = dynamic_cast<SpongebobScene*>(GetScene());
	pSpongebobScene->SetPaused(isPaused);
}

void Spongebob::SetControllerPosition(const XMFLOAT3& pos)
{
	m_pCharacter->GetTransform()->Translate(pos);
}

void Spongebob::ResetVariables()
{
	m_pCharacter->SetPaused(false);
	
	m_pSpongebobMesh->GetTransform()->Rotate(0,180,0);
	m_pCharacter->SetCameraPitchYaw(0, 0);

	m_pHud->SetAmountSpatulas(0);

	m_pSoundChannel->setPaused(true);
	m_IsSoundPlaying = false;
	m_IsPaused = false;
}
