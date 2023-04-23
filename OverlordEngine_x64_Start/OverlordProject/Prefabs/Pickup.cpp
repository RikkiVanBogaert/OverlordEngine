#include "stdafx.h"
#include "Pickup.h"
#include "Materials/DiffuseMaterial.h"
#include "SpherePrefab.h"

void Spatula::Initialize(const SceneContext&)
{
	auto pMat = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();
	pMat->SetDiffuseTexture(L"Exam/Textures/spatula_golden.png");


	ModelComponent* pModel = new ModelComponent(L"Exam/Meshes/Spatula.ovm");
	AddComponent<ModelComponent>(pModel);
	pModel->SetMaterial(pMat);

	//Collision
	auto& phys = PxGetPhysics();
	auto pBouncyMaterial = phys.createMaterial(0, 0, 1.f);

	//auto pRB = new GameObject();
	auto pRigidBodyCp = AddComponent(new RigidBodyComponent(true));
	const XMFLOAT3 size{ 1, 1 ,1 };
	pRigidBodyCp->AddCollider(PxSphereGeometry(1.f), *pBouncyMaterial, true);

	auto parentPos = this->GetTransform()->GetPosition();

	auto onTrigger = [&](GameObject*, GameObject* other, PxTriggerAction action)
	{
		if (other->GetTag() != L"Player") return;

		if (action == PxTriggerAction::ENTER)
		{
			std::cout << "SPATULA PICKED UP\n";
			MarkForDeletion();
		}
	};

	SetOnTriggerCallBack(onTrigger);
}

void Underwear::Initialize(const SceneContext&)
{
	auto pMat = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();
	pMat->SetDiffuseTexture(L"Exam/Textures/underwear_health.png");

	ModelComponent* pModel = new ModelComponent(L"Exam/Meshes/Underwear.ovm");
	AddComponent<ModelComponent>(pModel);
	pModel->SetMaterial(pMat);

	//Collision
	auto& phys = PxGetPhysics();
	auto pBouncyMaterial = phys.createMaterial(0, 0, 1.f);

	auto pRB = new GameObject();
	auto pRigidBodyCp = pRB->AddComponent(new RigidBodyComponent(true));
	const XMFLOAT3 size{ 1, 1 ,1 };
	pRigidBodyCp->AddCollider(PxSphereGeometry(1.f), *pBouncyMaterial, true);
	this->AddChild(pRB);

	auto parentPos = this->GetTransform()->GetPosition();
	pRB->GetTransform()->Translate(parentPos.x, 1.f, parentPos.z);

	auto onTrigger = [&](GameObject*, GameObject* other, PxTriggerAction action)
	{
		if (other->GetTag() != L"Player") return;
		
		if (action == PxTriggerAction::ENTER)
		{
			std::cout << "UNDERWEAR PICKED UP\n";
			MarkForDeletion();
		}
	};

	pRB->SetOnTriggerCallBack(onTrigger);
}
