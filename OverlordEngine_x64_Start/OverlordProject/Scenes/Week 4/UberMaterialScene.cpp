//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"
#include "UberMaterialScene.h"

#include "Materials/ColorMaterial.h"
#include "Materials/UberMaterial.h"


UberMaterialScene::UberMaterialScene() :
	GameScene(L"ModelTestScene") {}

void UberMaterialScene::Initialize()
{
	//GameSettings
	GameScene::GetSceneSettings().drawGrid = false;
	GameScene::GetSceneSettings().enableOnGUI = true;

	//Camera
	m_SceneContext.pCamera->GetTransform()->Translate(0.f, 3.f, -5.f);

	//Sphere
	m_pSphere = new GameObject();
	m_pUberMat = MaterialManager::Get()->CreateMaterial<UberMaterial>();
	m_pUberMat->SetDiffuseTexture(L"Textures/Skulls/Skulls_Diffusemap.tga");
	m_pUberMat->SetNormalMap(L"Textures/Skulls/Skulls_Normalmap.tga");

	ModelComponent* pModel = new ModelComponent(L"Meshes/Sphere.ovm");
	m_pSphere->AddComponent<ModelComponent>(pModel);
	pModel->SetMaterial(m_pUberMat);

	AddChild(m_pSphere);
}

void UberMaterialScene::Update()
{

}



void UberMaterialScene::Draw()
{

}

void UberMaterialScene::OnGUI()
{
	m_pUberMat->DrawImGui();
}

