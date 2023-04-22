//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"
#include "ModelTestScene.h"

#include "Materials/ColorMaterial.h"
#include "Materials/DiffuseMaterial.h"

ModelTestScene::ModelTestScene() :
	GameScene(L"ModelTestScene") {}

void ModelTestScene::Initialize()
{
	auto& phys = PxGetPhysics();
	auto pPhysicsMat = phys.createMaterial(0, 0, 0.4f);

	GameSceneExt::CreatePhysXGroundPlane(*this);

	auto pCamera = new FixedCamera();
	AddChild(pCamera);
	pCamera->GetTransform()->Rotate(30, 0, 0);
	pCamera->GetTransform()->Translate(0, 40, -60);
	pCamera->GetComponent<CameraComponent>()->SetActive();

	//Chair
	m_pChair = new GameObject();
	m_pChair->GetTransform()->Translate(0, 10, 0);

	auto pConvexMesh = ContentManager::Load<PxConvexMesh>(L"Meshes/Chair.ovpc");
	auto pRigidBody = m_pChair->AddComponent<RigidBodyComponent>(new RigidBodyComponent());
	pRigidBody->AddCollider(PxConvexMeshGeometry{ pConvexMesh }, *pPhysicsMat);

	auto pMat = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();
	pMat->SetDiffuseTexture(L"Textures/Chair_Dark.dds");
	ModelComponent* pModel = new ModelComponent(L"Meshes/Chair.ovm");
	m_pChair->AddComponent<ModelComponent>(pModel);
	pModel->SetMaterial(pMat);
	
	AddChild(m_pChair);

}

void ModelTestScene::Update()
{
	
}



void ModelTestScene::Draw()
{

}

void ModelTestScene::OnGUI()
{

}

