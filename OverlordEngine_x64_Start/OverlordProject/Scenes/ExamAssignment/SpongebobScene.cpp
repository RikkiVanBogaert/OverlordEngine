#include "stdafx.h"
#include "SpongebobScene.h"

#include "Prefabs/Character.h"
#include "Prefabs/Pickup.h"
#include"Prefabs/Tiki.h"
#include "Prefabs/ThreeTikis.h"

#include "Materials/ColorMaterial.h"
#include "Materials/DiffuseMaterial.h"
#include "Materials/DiffuseMaterial_Skinned.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include "Helpers.h"
#include "Prefabs/HUDPrefab.h"
#include "Prefabs/UIElement.h"

#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <vector>

#include <locale>
#include <codecvt>
#include <string>

#include "Materials/Post/PostGrayscale.h"
#include "Materials/Post/PostMyEffect.h"
#include "Materials/Shadow/DiffuseMaterial_Shadow.h"
#include "Materials/Shadow/DiffuseMaterial_Shadow_Skinned.h"
#include "Prefabs/BubbleParticles.h"

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
	//m_SceneContext.settings.drawPhysXDebug = false;


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

	m_pCharacter = AddChild(new Character(characterDesc, {0, 0, -30}));
	const XMFLOAT3 startPos{60, 50, -175};
	m_pCharacter->GetTransform()->Translate(startPos);
	m_pCharacter->SetTag(L"Player");

	/*auto pHUD = new HUDPrefab();
	m_pCharacter->AddChild(pHUD);*/
	
	//Spongebob
	m_pSpongebobMesh = new GameObject();
	
	auto pSpongeMat = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow_Skinned>();
	pSpongeMat->SetDiffuseTexture(L"Exam/Textures/Spongebob.png");

	ModelComponent* pModel = new ModelComponent(L"Exam/Meshes/SpongebobNew.ovm");
	m_pSpongebobMesh->AddComponent<ModelComponent>(pModel);
	pModel->SetMaterial(pSpongeMat);
	m_pSpongebobMesh->GetTransform()->Scale(1.f);
	
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
	m_pSpatula->GetTransform()->Translate(startPos.x + 2, startPos.y - 3, startPos.z + 5);
	AddChild(m_pSpatula);

	auto pTiki = new Tiki();
	pTiki->GetTransform()->Translate(startPos.x - 2, startPos.y - 3, startPos.z + 5);
	AddChild(pTiki);


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

	////TEST
	auto m_pPostEffect = MaterialManager::Get()->CreateMaterial<PostMyEffect>();
	AddPostProcessingEffect(m_pPostEffect);


	m_SceneContext.pLights->SetDirectionalLight({ -95.6139526f,66.1346436f,-41.1850471f }, 
		{ 0.740129888f, -0.597205281f, 0.309117377f });
}

void SpongebobScene::OnGUI()
{
	
}

void SpongebobScene::Update()
{
	//Light
	m_SceneContext.pLights->GetDirectionalLight().position.x = m_pSpongebobMesh->GetTransform()->GetPosition().x - 95;
	m_SceneContext.pLights->GetDirectionalLight().position.z = m_pSpongebobMesh->GetTransform()->GetPosition().z - 41;

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
	for (auto child : GetChildren())
	{
		if (child->NeedsDeleting())
			RemoveChild(child, true);
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
	const float levelScale = 10.f;
	pLevelActor->AddCollider(PxTriangleMeshGeometry(pPxTriangleMesh, PxMeshScale({ levelScale, levelScale, levelScale })), *pDefaultMaterial);

	pLevelObject->GetTransform()->Scale(levelScale);


	//auto pDefault = MaterialManager::Get()->CreateMaterial<ColorMaterial>();
	//pDefault->SetColor({ 0.5f,0.5f,0.5f,1 });
	//pLevelMesh->SetMaterial(pDefault);

	const auto pGroundMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pGroundMaterial->SetDiffuseTexture(L"Textures/GroundBrick.jpg");
	pLevelMesh->SetMaterial(pGroundMaterial);

	//auto pMat = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();
	//pMat->SetDiffuseTexture(L"Exam/Textures/Level/t11.png");
	////pLevelMesh->SetMaterial(pMat);

	//auto pMat2 = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();
	//pMat2->SetDiffuseTexture(L"Exam/Textures/Level/t0092_0.png");

	//for (int i{}; i < 195; ++i)
	//{
	//	if(i % 2 == 0)
	//		pLevelMesh->SetMaterial(pMat, UINT8(i));
	//	else
	//		pLevelMesh->SetMaterial(pMat2, UINT8(i));
	//}

	//pLevelMesh->SetMaterial(pMat, UINT8(33));

	/*auto mtl_contents = mtlParser("../OverlordProject/Resources/Exam/Textures/Level/jellyfishfields.mtl");

	for (UINT8 i{}; i < mtl_contents.size(); ++i)
	{
		std::cout << mtl_contents[i] << '\n';

		int wstr_size = MultiByteToWideChar(CP_UTF8, 0, mtl_contents[i].c_str(), -1, NULL, 0);
		std::wstring wstr(wstr_size, 0);
		MultiByteToWideChar(CP_UTF8, 0, mtl_contents[i].c_str(), -1, &wstr[0], wstr_size);

		auto pMat = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();
		pMat->SetDiffuseTexture(L"Exam/Textures/Level/" + wstr);
		pLevelMesh->SetMaterial(pMat, i);
	}

	std::cout << "Size of mtl_contents: " << mtl_contents.size() << std::endl;*/


	//for (UINT8 i{1}; i < 195; ++i)
	//{
	//	//t0177_0

	//	auto pMat = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();
	//	pMat->SetDiffuseTexture(L"Exam/Textures/Level/t0" + std::to_wstring(i) + L"_0.png");
	//	pLevelMesh->SetMaterial(pMat, i);
	//}

	auto pMat = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();
	pMat->SetDiffuseTexture(L"Exam/Textures/Level/t0090_0.png");
	for (UINT8 i{ 80 }; i < 90; ++i)
	{
		pLevelMesh->SetMaterial(pMat, i);
	}

	auto pMat2 = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();
	pMat2->SetDiffuseTexture(L"Exam/Textures/Level/t0022_0.png");
	pLevelMesh->SetMaterial(pMat2, 0);
}

std::vector<std::string> SpongebobScene::mtlParser(const std::string& filename)
{
	std::ifstream file(filename);
	std::vector<std::string> map_kds;
	std::string line;
	while (std::getline(file, line)) {
		size_t map_kd_pos = line.find("map_Kd ");
		if (map_kd_pos != std::string::npos) {
			std::string map_kd_value = line.substr(map_kd_pos + 7);
			map_kds.push_back(map_kd_value);
		}
	}
	return map_kds;
}
