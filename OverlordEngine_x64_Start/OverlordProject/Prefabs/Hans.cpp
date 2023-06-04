#include "stdafx.h"
#include "Hans.h"

#include "Materials/Deferred/BasicMaterial_Deferred.h"


void Hans::Initialize(const SceneContext&)
{
	const auto pMat = MaterialManager::Get()->CreateMaterial<BasicMaterial_Deferred>();
	pMat->SetDiffuseMap(L"Exam/Textures/hans.png");

	constexpr float size{ 25 };

	const auto pModelObject = new GameObject();
	ModelComponent* pModel = new ModelComponent(L"Exam/Meshes/Hans.ovm");
	pModelObject->AddComponent<ModelComponent>(pModel);
	pModel->SetMaterial(pMat);
	AddChild(pModelObject);
	pModel->GetTransform()->Translate(0, 0.f, 0);

	//Collision
	auto& phys = PxGetPhysics();
	const auto pBouncyMaterial = phys.createMaterial(0, 0, 1.f);


	const auto pRigidBodyCp = AddComponent(new RigidBodyComponent(true));

	const auto pConvexMesh = ContentManager::Load<PxConvexMesh>(L"Exam/Meshes/Hans.ovpc");
	pRigidBodyCp->AddCollider(PxConvexMeshGeometry{ pConvexMesh, PxMeshScale{size} }, *pBouncyMaterial);

	
	GetTransform()->Scale(size);
}
