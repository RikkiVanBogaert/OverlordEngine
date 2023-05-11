#include "stdafx.h"
#include "MainMenuScene.h"

#include "Materials/ColorMaterial.h"
#include "Materials/DiffuseMaterial.h"
#include "Prefabs/UIElement.h"


MainMenuScene::~MainMenuScene()
{}

void MainMenuScene::Initialize()
{
	m_SceneContext.settings.enableOnGUI = true;

	//GameSettings
	//GameScene::GetSceneSettings().clearColor = {};
	//GameScene::GetSceneSettings().drawGrid = false;
	//GameScene::GetSceneSettings().drawPhysXDebug = false;

	auto pCamera = new FixedCamera();
	AddChild(pCamera);
	pCamera->GetTransform()->Rotate(90, 0, 0);
	pCamera->GetTransform()->Translate(0, 40, 0);
	pCamera->GetComponent<CameraComponent>()->SetActive();

	auto pGameObject = new GameObject();
	auto pSprite = new SpriteComponent(L"Exam/HUD/MainMenu.png");
	pGameObject->AddComponent<SpriteComponent>(pSprite);

	AddChild(pGameObject);

	//Buttons-----
	const auto pButtonObject = new GameObject();
	const auto pButtonPlane = new ModelComponent(L"Exam/HUD/Plane.ovm");
	pButtonObject->AddComponent<ModelComponent>(pButtonPlane);

	auto pButtonMat = MaterialManager::Get()->CreateMaterial<ColorMaterial>();
	pButtonPlane->SetMaterial(pButtonMat);


	pButtonPlane->GetTransform()->Scale(0.4f, 1.f, .1f);
	pButtonObject->GetTransform()->Translate(12, 1, -1.8f);

	const auto pMaterial = PxGetPhysics().createMaterial(.5f, .5f, .5f);
	const auto pRigidBody = pButtonObject->AddComponent(new RigidBodyComponent(true));
	const auto scale = pButtonPlane->GetTransform()->GetScale();
	pRigidBody->AddCollider(PxBoxGeometry{ 5, 1, 1 }, *pMaterial);

	AddChild(pButtonObject);
}

void MainMenuScene::OnGUI()
{
	
}

void MainMenuScene::Update()
{
	if (InputManager::IsMouseButton(InputState::down, 1))
	{
		if (const auto pPickedObject = m_SceneContext.pCamera->Pick())
		{
			//RemoveChild(pPickedObject, true);

			SceneManager::Get()->SetActiveGameScene(L"TestScene");
		}
	}

}