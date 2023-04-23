#include "stdafx.h"
#include "Tiki.h"

#include "Materials/DiffuseMaterial.h"
#include "SpherePrefab.h"
#include "Character.h"
#include "Pickup.h"

void Tiki::Initialize(const SceneContext&)
{
	auto pMat = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();
	pMat->SetDiffuseTexture(L"Exam/Textures/tiki.png");

	ModelComponent* pModel = new ModelComponent(L"Exam/Meshes/Tiki.ovm");
	AddComponent<ModelComponent>(pModel);
	pModel->SetMaterial(pMat);

	//Collision
	auto& phys = PxGetPhysics();
	auto pBouncyMaterial = phys.createMaterial(0, 0, 1.f);

	auto pRB = new GameObject();
	auto pRigidBodyCp = pRB->AddComponent(new RigidBodyComponent(true));
	const XMFLOAT3 size{ 1, 1 ,1 };
	pRigidBodyCp->AddCollider(PxSphereGeometry(1.f), *pBouncyMaterial, true);
	AddChild(pRB);

	auto parentPos = this->GetTransform()->GetPosition();
	pRB->GetTransform()->Translate(parentPos.x, 1.f, parentPos.z);

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

	pRB->SetOnTriggerCallBack(onTrigger);
}

void Tiki::Update(const SceneContext& )
{
	if (!m_IsVulnerable) return;
	if (!m_pPlayer) return;

	if (m_pPlayer->IsAttacking())
	{
		auto spatula = new Spatula();
		auto pos = GetTransform()->GetPosition();
		spatula->GetTransform()->Translate(pos.x, pos.y + 5, pos.z + 5);
		GetScene()->AddChild(spatula);

		MarkForDeletion();
	}
}
