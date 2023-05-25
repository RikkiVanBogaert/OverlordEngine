#include "stdafx.h"
#include "Hans.h"

#include "Character.h"
#include "Materials/DiffuseMaterial.h"


void Hans::Initialize(const SceneContext&)
{
	auto pMat = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();
	pMat->SetDiffuseTexture(L"Exam/Textures/hans.png");

	constexpr float size{ 25 };

	auto pModelObject = new GameObject();
	ModelComponent* pModel = new ModelComponent(L"Exam/Meshes/Hans.ovm");
	pModelObject->AddComponent<ModelComponent>(pModel);
	pModel->SetMaterial(pMat);
	AddChild(pModelObject);
	pModel->GetTransform()->Translate(0, 0.f, 0);

	//Collision
	auto& phys = PxGetPhysics();
	auto pBouncyMaterial = phys.createMaterial(0, 0, 1.f);


	auto pRigidBodyCp = AddComponent(new RigidBodyComponent(true));

	auto pConvexMesh = ContentManager::Load<PxConvexMesh>(L"Exam/Meshes/Hans.ovpc");
	pRigidBodyCp->AddCollider(PxConvexMeshGeometry{ pConvexMesh, PxMeshScale{size} }, *pBouncyMaterial);

	
	GetTransform()->Scale(size);
}
