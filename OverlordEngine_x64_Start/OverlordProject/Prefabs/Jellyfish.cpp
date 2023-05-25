#include "stdafx.h"
#include "Jellyfish.h"

#include "Character.h"
#include "Materials/DiffuseMaterial.h"
#include "Prefabs/Spongebob.h"


void Jellyfish::Initialize(const SceneContext&)
{
	auto pMat = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();
	pMat->SetDiffuseTexture(L"Exam/Textures/jellyfish_pink.RW3.png");

	constexpr float size{ 40 };

	auto pModelObject = new GameObject();
	ModelComponent* pModel = new ModelComponent(L"Exam/Meshes/Jellyfish.ovm");
	pModelObject->AddComponent<ModelComponent>(pModel);
	pModel->SetMaterial(pMat);
	AddChild(pModelObject);
	pModel->GetTransform()->Translate(0, 0.f, 0);

	//Collision
	auto& phys = PxGetPhysics();
	auto pBouncyMaterial = phys.createMaterial(0, 0, 1.f);


	auto pRigidBodyCp = AddComponent(new RigidBodyComponent(true));

	auto pConvexMesh = ContentManager::Load<PxConvexMesh>(L"Exam/Meshes/Jellyfish.ovpc");
	pRigidBodyCp->AddCollider(PxConvexMeshGeometry{ pConvexMesh, PxMeshScale{size} }, *pBouncyMaterial, true);

	auto onTrigger = [&](GameObject*, GameObject* other, PxTriggerAction action)
	{
		if (other->GetTag() != L"Player") return;

		

		if (action == PxTriggerAction::ENTER)
		{
			auto sponge = dynamic_cast<Character*>(other);
			sponge->SetVelocity({0, 300, 0});
		}

	};

	SetOnTriggerCallBack(onTrigger);

	GetTransform()->Scale(size);
}

void Jellyfish::Update(const SceneContext&)
{
	if (NeedsDeleting()) return;

}

