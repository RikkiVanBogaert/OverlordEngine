#include "stdafx.h"
#include "ExitGate.h"

#include "BubbleParticles.h"
#include "Materials/DiffuseMaterial.h"
#include "Character.h"
#include "HUDPrefab.h"
#include "Pickup.h"
#include "Spongebob.h"
#include "EndScreen.h"
#include "Materials/Deferred/BasicMaterial_Deferred.h"

void ExitGate::Initialize(const SceneContext&)
{
	auto pMat = MaterialManager::Get()->CreateMaterial<BasicMaterial_Deferred>();
	pMat->SetDiffuseMap(L"Exam/Textures/Gate.png");

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

		auto pSponge = dynamic_cast<Spongebob*>(other->GetParent());
		//if(pSponge->GetHUD()->GetAmountSpatulas() >= 3)
		{
			if (action == PxTriggerAction::ENTER)
			{
				pEndScreen = new EndScreen(pSponge);
				AddChild(pEndScreen);
			}

			return;
			
		}

		if (action == PxTriggerAction::ENTER)
		{
			m_Text = new GameObject();
			auto spatTextSprite = new SpriteComponent(L"Exam/HUD/CollectSpatulasText.png");
			AddChild(m_Text);
			m_Text->AddComponent<SpriteComponent>(spatTextSprite);
			m_Text->GetTransform()->Translate(410, 160, 0);
		}

		if (action == PxTriggerAction::LEAVE)
		{
			if (m_Text)
			{
				RemoveChild(m_Text, true);
				m_Text = nullptr;
			}
		}
	};

	SetOnTriggerCallBack(onTrigger);

	GetTransform()->Scale(size);
}

void ExitGate::Update(const SceneContext&)
{
	if (NeedsDeleting()) return;

}

