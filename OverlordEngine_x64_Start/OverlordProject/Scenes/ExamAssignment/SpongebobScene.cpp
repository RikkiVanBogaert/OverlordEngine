#include "stdafx.h"
#include "SpongebobScene.h"

#include "Prefabs/Character.h"
#include "Materials/ColorMaterial.h"
#include "Materials/DiffuseMaterial.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include "Helpers.h"

SpongebobScene::~SpongebobScene()
{
	
}

void SpongebobScene::Initialize()
{
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

	m_pCharacter = AddChild(new Character(characterDesc, {0, 0, -10}));
	m_pCharacter->GetTransform()->Translate(0.f, 5.f, 0.f);
	
	//Spongebob
	
	m_pSpongebobMesh = new GameObject();
	
	auto pMat = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();
	pMat->SetDiffuseTexture(L"Exam/Textures/Spongebob.png");

	ModelComponent* pModel = new ModelComponent(L"Exam/Meshes/Spongebob.ovm");
	m_pSpongebobMesh->AddComponent<ModelComponent>(pModel);
	pModel->SetMaterial(pMat);
	m_pSpongebobMesh->GetTransform()->Scale(0.2f);
	
	AddChild(m_pSpongebobMesh);

	//Simple Level
	//auto& phys = PxGetPhysics();
	//auto pPhysicsMat = phys.createMaterial(0, 0, 0.4f);

	const auto pLevelObject = AddChild(new GameObject());
	const auto pLevelMesh = pLevelObject->AddComponent(new ModelComponent(L"Meshes/SimpleLevel.ovm"));
	pLevelMesh->SetMaterial(MaterialManager::Get()->CreateMaterial<ColorMaterial>());

	const auto pLevelActor = pLevelObject->AddComponent(new RigidBodyComponent(true));
	const auto pPxTriangleMesh = ContentManager::Load<PxTriangleMesh>(L"Meshes/SimpleLevel.ovpt");
	pLevelActor->AddCollider(PxTriangleMeshGeometry(pPxTriangleMesh, PxMeshScale({ .5f,.5f,.5f })), *pDefaultMaterial);

	//auto pConvexMesh = ContentManager::Load<PxConvexMesh>(L"Exam/Meshes/Jellyfishfields.ovpc");
	//auto pRigidBody = pLevelObject->AddComponent<RigidBodyComponent>(new RigidBodyComponent());
	//pRigidBody->AddCollider(PxConvexMeshGeometry{ pConvexMesh }, *pPhysicsMat);

	pLevelObject->GetTransform()->Scale(0.5f);

	//Input
	auto inputAction = InputAction(CharacterMoveLeft, InputState::down, 'A');
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterMoveRight, InputState::down, 'D');
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterMoveForward, InputState::down, 'W');
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterMoveBackward, InputState::down, 'S');
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterJump, InputState::pressed, VK_SPACE, -1, XINPUT_GAMEPAD_A);
	m_SceneContext.pInput->AddInputAction(inputAction);
	
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
}