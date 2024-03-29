#include "stdafx.h"
#include "TestScene.h"


#include "Prefabs/Pickup.h"
#include "Prefabs/Tiki.h"


#include "Materials/ColorMaterial.h"


#define _USE_MATH_DEFINES
#include <math.h>

#include "Materials/Shadow/DiffuseMaterial_Shadow.h"


TestScene::~TestScene()
{
}

void TestScene::Initialize()
{
	m_SceneContext.settings.enableOnGUI = true;

	//Ground Plane
	const auto pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);
	GameSceneExt::CreatePhysXGroundPlane(*this, pDefaultMaterial);


	//Materials
	//*********
	const auto pGroundMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pGroundMaterial->SetDiffuseTexture(L"Textures/GroundBrick.jpg");

	//Ground Mesh
	//***********
	const auto pGroundObj = new GameObject();
	const auto pGroundModel = new ModelComponent(L"Meshes/UnitPlane.ovm");
	pGroundModel->SetMaterial(pGroundMaterial);

	pGroundObj->AddComponent(pGroundModel);
	pGroundObj->GetTransform()->Scale(10.f);

	AddChild(pGroundObj);

	/*auto pSponge = new Spongebob(new HUDPrefab());
	AddChild(pSponge);
	pSponge->SetControllerPosition({ 5, 10, 0 });*/

	//CreateLevel();

	//Objects
	m_pSpatula = new Spatula();
	m_pSpatula->GetTransform()->Translate(20, 25, 0);
	AddChild(m_pSpatula);

	
	auto pTiki = new Tiki();
	AddChild(pTiki);
	pTiki->GetTransform()->Translate(0, 0, 12);

}

void TestScene::OnGUI()
{
}

void TestScene::Update()
{
	
	CheckDeletedObjects();
}


void TestScene::CheckDeletedObjects()
{
	for (auto child : GetChildren())
	{
		if (child->NeedsDeleting())
			this->RemoveChild(child, true);
	}
}

void TestScene::CreateLevel()
{
	const auto pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);

	//Simple Level
	const auto pLevelObject = AddChild(new GameObject());
	const auto pLevelMesh = pLevelObject->AddComponent(new ModelComponent(L"Meshes/SimpleLevel.ovm"));
	pLevelMesh->SetMaterial(MaterialManager::Get()->CreateMaterial<ColorMaterial>());

	const auto pLevelActor = pLevelObject->AddComponent(new RigidBodyComponent(true));
	const auto pPxTriangleMesh = ContentManager::Load<PxTriangleMesh>(L"Meshes/SimpleLevel.ovpt");
	const float levelScale = 0.5f;
	pLevelActor->AddCollider(PxTriangleMeshGeometry(pPxTriangleMesh, PxMeshScale({ levelScale, levelScale, levelScale })), *pDefaultMaterial);

	pLevelObject->GetTransform()->Scale(levelScale);



}
