#include "stdafx.h"
#include "SpongebobScene.h"

#include "Prefabs/Character.h"
#include "Prefabs/Pickup.h"
#include "Prefabs/ThreeTikis.h"

#include "Materials/ColorMaterial.h"
#include "Materials/DiffuseMaterial.h"
#include "Materials/DiffuseMaterial_Skinned.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include "Helpers.h"
#include "Prefabs/HUDPrefab.h"
#include "Prefabs/UIElement.h"

SpongebobScene::~SpongebobScene()
{
	for (UINT i{ 0 }; i < m_ClipCount; ++i)
	{
		delete[] m_ClipNames[i];
	}

	delete[] m_ClipNames;
}

void SpongebobScene::Initialize()
{
	m_SceneContext.settings.enableOnGUI = true;

	//Ground Plane
	const auto pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);
	GameSceneExt::CreatePhysXGroundPlane(*this, pDefaultMaterial);

	//Character
	CharacterDesc characterDesc{ pDefaultMaterial };
	characterDesc.actionId_MoveForward = CharacterMoveForward;
	characterDesc.actionId_MoveBackward = CharacterMoveBackward;
	characterDesc.actionId_MoveLeft = CharacterMoveLeft;
	characterDesc.actionId_MoveRight = CharacterMoveRight;
	characterDesc.actionId_Jump = CharacterJump;
	characterDesc.actionId_Attack = Attack;

	m_pCharacter = AddChild(new Character(characterDesc, {0, 0, -10}));
	const XMFLOAT3 startPos{65, 10, -190};
	m_pCharacter->GetTransform()->Translate(startPos);
	
	//Spongebob
	m_pSpongebobMesh = new GameObject();
	
	auto pSpongeMat = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Skinned>();
	pSpongeMat->SetDiffuseTexture(L"Exam/Textures/Spongebob.png");

	ModelComponent* pModel = new ModelComponent(L"Exam/Meshes/Sponge.ovm");
	m_pSpongebobMesh->AddComponent<ModelComponent>(pModel);
	pModel->SetMaterial(pSpongeMat);
	m_pSpongebobMesh->GetTransform()->Scale(0.2f);
	
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

	CreateLevel();
	
	//Objects
	m_pSpatula = new Spatula();
	m_pSpatula->GetTransform()->Translate(2, 0, 5);
	AddChild(m_pSpatula);

	auto pUnderwear = new Underwear();
	pUnderwear->GetTransform()->Translate(-2, 0, 5);
	AddChild(pUnderwear);


	//Input
	auto inputAction = InputAction(CharacterMoveLeft, InputState::down, 'A');
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterMoveRight, InputState::down, 'D');
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterMoveForward, InputState::down, 'W');
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterMoveBackward, InputState::down, 'S');
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(Attack, InputState::down, 'E');
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterJump, InputState::pressed, VK_SPACE, -1, XINPUT_GAMEPAD_A);
	m_SceneContext.pInput->AddInputAction(inputAction);


	//HUD
	auto pHud = new HUDPrefab();
	AddChild(pHud);
}

void SpongebobScene::OnGUI()
{
	
}

void SpongebobScene::Update()
{
	//used manual position adjustement instead of childing mesh to parent, 
	//because the mesh always pointed to the camera when it was a child of the characterComponent

	auto pos = m_pCharacter->GetTransform()->GetPosition();
	pos.y -= 1.3f; //offset to put spongebob on same height as capsule
	m_pSpongebobMesh->GetTransform()->Translate(pos);
	
	auto rot = std::atan2f(m_pCharacter->GetVelocity().z,-m_pCharacter->GetVelocity().x) * float(180 / M_PI) + 90;
	m_pSpongebobMesh->GetTransform()->Rotate(0, rot, 0);

	PlayCorrectAnimation();
	UpdateHUDElements();

	CheckDeletedObjects();
}

void SpongebobScene::PlayCorrectAnimation()
{
	if (m_pCharacter->IsAttacking() &&
		m_AnimationClipId != 2)
	{
		pAnimator->Pause();
		m_AnimationClipId = 2;
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

void SpongebobScene::UpdateHUDElements()
{
	/*auto pos = m_pCharacter->GetCamera()->GetTransform()->GetWorldPosition();
	XMFLOAT3 toPlayer = { m_pCharacter->GetTransform()->GetPosition().x - pos.x,
		m_pCharacter->GetTransform()->GetPosition().y - pos.y,
		m_pCharacter->GetTransform()->GetPosition().z - pos.z };

	const float toPlayerLength = sqrtf(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y + toPlayer.z * toPlayer.z);
	XMFLOAT3 camToPlayerNorm = { toPlayer.x };
	const float length = 1.5f;
	const XMFLOAT3 newPos{ pos.x + toPlayer.x * length, pos.y + toPlayer.y * length ,
	pos.z + toPlayer.z * length };
	
	m_pUISpatula->GetTransform()->Translate(newPos);*/
}

void SpongebobScene::CheckDeletedObjects()
{
	for (auto child : this->GetChildren())
	{
		if (child->NeedsDeleting())
			this->RemoveChild(child);
	}
}

void SpongebobScene::CreateLevel()
{
	const auto pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);

	//Simple Level
	const auto pLevelObject = AddChild(new GameObject());
	const auto pLevelMesh = pLevelObject->AddComponent(new ModelComponent(L"Exam/Meshes/Level.ovm"));


	const auto pLevelActor = pLevelObject->AddComponent(new RigidBodyComponent(true));
	const auto pPxTriangleMesh = ContentManager::Load<PxTriangleMesh>(L"Exam/Meshes/Level2.ovpt");
	const float levelScale = 2.f;
	pLevelActor->AddCollider(PxTriangleMeshGeometry(pPxTriangleMesh, PxMeshScale({ levelScale, levelScale, levelScale })), *pDefaultMaterial);

	pLevelObject->GetTransform()->Scale(levelScale);


	auto pDefault = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();
	pLevelMesh->SetMaterial(pDefault);

	auto pMat = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();
	pMat->SetDiffuseTexture(L"Exam/Textures/Level/t11.png");
	//pLevelMesh->SetMaterial(pMat);

	auto pMat2 = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();
	pMat2->SetDiffuseTexture(L"Exam/Textures/Level/t0092_0.png");

	for (int i{}; i < 195; ++i)
	{
		if(i % 2 == 0)
			pLevelMesh->SetMaterial(pMat, UINT8(i));
		else
			pLevelMesh->SetMaterial(pMat2, UINT8(i));
	}

	//pLevelMesh->SetMaterial(pMat, UINT8(33));
	

}
