#include "stdafx.h"
#include "ExitGate.h"

#include "BubbleParticles.h"
#include "Materials/DiffuseMaterial.h"
#include "SpherePrefab.h"
#include "Character.h"
#include "HUDPrefab.h"
#include "Pickup.h"
#include "Spongebob.h"

void ExitGate::Initialize(const SceneContext&)
{
	auto pMat = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();
	pMat->SetDiffuseTexture(L"Exam/Textures/Gate.png");

	constexpr float size{ 8 };

	auto pModelObject = new GameObject();
	ModelComponent* pModel = new ModelComponent(L"Exam/Meshes/Gate.ovm");
	pModelObject->AddComponent<ModelComponent>(pModel);
	pModel->SetMaterial(pMat);
	AddChild(pModelObject);
	pModel->GetTransform()->Translate(0, 0.f, 0);

	//Collision
	auto& phys = PxGetPhysics();
	auto pBouncyMaterial = phys.createMaterial(0, 0, 1.f);


	auto pRigidBodyCp = AddComponent(new RigidBodyComponent(true));

	auto pConvexMesh = ContentManager::Load<PxConvexMesh>(L"Exam/Meshes/Gate.ovpc");
	pRigidBodyCp->AddCollider(PxConvexMeshGeometry{ pConvexMesh, PxMeshScale{size} }, *pBouncyMaterial, true);

	auto onTrigger = [&](GameObject*, GameObject* other, PxTriggerAction action)
	{
		if (other->GetTag() != L"Player") return;

		auto sponge = dynamic_cast<Spongebob*>(other->GetParent());
		if(sponge->GetHUD()->GetAmountSpatulas() >= 3)
		{
			if (action != PxTriggerAction::ENTER) return;

			SceneManager::Get()->SetActiveGameScene(L"EndScene");
			return;
			
		}

		if (action == PxTriggerAction::ENTER)
		{
			m_Text = new GameObject();
			auto spatTextSprite = new SpriteComponent(L"Exam/HUD/CollectSpatulasText.png");
			AddChild(m_Text);
			m_Text->AddComponent<SpriteComponent>(spatTextSprite);
			//m_Text->GetTransform()->Translate(300, 100, 0);
			//m_Text->GetTransform()->Scale(5);
		}

		if (action == PxTriggerAction::LEAVE)
		{
			RemoveChild(m_Text, true);
		}
	};

	SetOnTriggerCallBack(onTrigger);

	GetTransform()->Scale(size);
}

void ExitGate::Update(const SceneContext&)
{
	if (NeedsDeleting()) return;

}

