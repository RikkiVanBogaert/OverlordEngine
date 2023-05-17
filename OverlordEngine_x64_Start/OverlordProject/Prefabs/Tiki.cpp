#include "stdafx.h"
#include "Tiki.h"

#include "BubbleParticles.h"
#include "Materials/DiffuseMaterial.h"
#include "SpherePrefab.h"
#include "Character.h"
#include "Pickup.h"

void Tiki::Initialize(const SceneContext&)
{
	auto pMat = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();
	pMat->SetDiffuseTexture(L"Exam/Textures/tiki.png");

	const float size{ 5 };

	auto pModelObject = new GameObject();
	ModelComponent* pModel = new ModelComponent(L"Exam/Meshes/Tiki.ovm");
	pModelObject->AddComponent<ModelComponent>(pModel);
	pModel->SetMaterial(pMat);
	AddChild(pModelObject);
	pModel->GetTransform()->Translate(0, 0.f, 0);

	//Collision
	auto& phys = PxGetPhysics();
	auto pBouncyMaterial = phys.createMaterial(0, 0, 1.f);


	auto pRigidBodyCp = AddComponent(new RigidBodyComponent(true));

	pRigidBodyCp->AddCollider(PxSphereGeometry(1.8f * size), *pBouncyMaterial, true);
	
	auto pConvexMesh = ContentManager::Load<PxConvexMesh>(L"Exam/Meshes/Tiki.ovpc");
	pRigidBodyCp->AddCollider(PxConvexMeshGeometry{ pConvexMesh, PxMeshScale{size} }, *pBouncyMaterial);

	auto onTrigger = [&](GameObject*, GameObject* other, PxTriggerAction action)
	{
		if (other->GetTag() != L"Player") return;

		if (action == PxTriggerAction::ENTER)
		{
			m_IsVulnerable = true;
			m_pPlayer = static_cast<Character*>(other);
		}

		if (action == PxTriggerAction::LEAVE)
		{
			m_IsVulnerable = false;
		}
	};

	SetOnTriggerCallBack(onTrigger);

	GetTransform()->Scale(size);
}

void Tiki::Update(const SceneContext& )
{
	if (!m_IsVulnerable) return;
	if (!m_pPlayer) return;

	if (m_pPlayer->IsAttacking())
	{
		SpawnFlowers();
		SpawnBubbles();

		MarkForDeletion();
	}
}

void Tiki::SpawnFlowers()
{
	for (int i{}; i < 5; ++i)
	{
		auto pFlower = new Flower();
		auto pos = GetTransform()->GetPosition();

		const int rndX{rand() % 13 - 6};
		const int rndZ{ rand() % 13 - 6 };

		pFlower->GetTransform()->Translate(pos.x + rndX, pos.y + 3, pos.z + rndZ);
		GetScene()->AddChild(pFlower);
	}
}

void Tiki::SpawnBubbles()
{
	auto pBubbles = new BubbleParticles();
	pBubbles->GetTransform()->Translate(GetTransform()->GetPosition());
	pBubbles->GetTransform()->Scale(5);
	GetScene()->AddChild(pBubbles);
}
