#include "stdafx.h"
#include "PauseScene.h"

#include "Materials/ColorMaterial.h"
#include "Materials/DiffuseMaterial.h"
#include "Prefabs/UIElement.h"


PauseScene::~PauseScene()
{}

void PauseScene::Initialize()
{
	m_SceneContext.settings.enableOnGUI = true;

	//GameSettings
	//GameScene::GetSceneSettings().drawPhysXDebug = false;

	auto pCamera = new FixedCamera();
	AddChild(pCamera);
	pCamera->GetTransform()->Rotate(90, 0, 0);
	pCamera->GetTransform()->Translate(0, 40, 0);
	pCamera->GetComponent<CameraComponent>()->SetActive();

	auto pGameObject = new GameObject();
	auto pSprite = new SpriteComponent(L"Exam/HUD/PauseMenu.png");
	pGameObject->AddComponent<SpriteComponent>(pSprite);
	
	AddChild(pGameObject);

	//Buttons-----
	const auto pButtonObject = new GameObject();
	pButtonObject->GetTransform()->Translate(-19, 1, 2);

	const auto pMaterial = PxGetPhysics().createMaterial(.5f, .5f, .5f);
	const auto pRigidBody = pButtonObject->AddComponent(new RigidBodyComponent(true));
	pRigidBody->SetCollisionGroup(CollisionGroup::Group0);
	pRigidBody->AddCollider(PxSphereGeometry{ 5 }, *pMaterial);

	AddChild(pButtonObject);


	const auto pQuitObject = new GameObject();
	pQuitObject->GetTransform()->Translate(0, 1, 2);

	//const auto pMaterial = PxGetPhysics().createMaterial(.5f, .5f, .5f);
	const auto pQuitRigidBody = pQuitObject->AddComponent(new RigidBodyComponent(true));
	pQuitRigidBody->SetCollisionGroup(CollisionGroup::Group1);
	pQuitRigidBody->AddCollider(PxSphereGeometry{ 5 }, *pMaterial);

	AddChild(pQuitObject);
}

void PauseScene::OnGUI()
{

}

void PauseScene::Update()
{
	if (InputManager::IsMouseButton(InputState::down, 1))
	{
		if (m_SceneContext.pCamera->Pick(CollisionGroup::Group1))
		{
			SceneManager::Get()->SetActiveGameScene(L"SpongebobScene");
		}
		if (m_SceneContext.pCamera->Pick(CollisionGroup::Group0))
		{
			std::exit(0);
		}
	}

}