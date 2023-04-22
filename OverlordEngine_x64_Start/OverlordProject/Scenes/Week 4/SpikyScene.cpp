//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"
#include "SpikyScene.h"

#include "Materials/ColorMaterial.h"
#include "Materials/SpikyMaterial.h"


SpikyScene::SpikyScene() :
	GameScene(L"SpikyScene") {}

void SpikyScene::Initialize()
{
	//GameSettings
	GameScene::GetSceneSettings().drawGrid = false;
	GameScene::GetSceneSettings().enableOnGUI = true;

	//Camera
	m_SceneContext.pCamera->GetTransform()->Translate(0.f, 30.f, -50.f);

	//Sphere
	m_pSphere = new GameObject();
	m_pSpikyMat = MaterialManager::Get()->CreateMaterial<SpikyMaterial>();

	ModelComponent* pModel = new ModelComponent(L"Meshes/OctaSphere.ovm");
	m_pSphere->AddComponent<ModelComponent>(pModel);
	pModel->SetMaterial(m_pSpikyMat);


	m_pSphere->GetTransform()->Scale(15.f);

	AddChild(m_pSphere);
}

void SpikyScene::Update()
{
	const float rotSpeed = 20.f * m_SceneContext.pGameTime->GetElapsed();
	m_RotAngle += rotSpeed;

	m_pSphere->GetTransform()->Rotate(0, m_RotAngle, 0);

	if (m_RotAngle >= 360)
		m_RotAngle = 0;
}



void SpikyScene::Draw()
{

}

void SpikyScene::OnGUI()
{
	m_pSpikyMat->DrawImGui();
}

