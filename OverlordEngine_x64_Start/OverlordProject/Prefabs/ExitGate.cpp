#include "stdafx.h"
#include "ExitGate.h"


#include "HUDPrefab.h"
#include "Spongebob.h"
#include "EndScreen.h"
#include "Materials/Deferred/BasicMaterial_Deferred.h"

ExitGate::ExitGate(EndScreen* pEndScreen):
m_pEndScreen(pEndScreen)
{}


void ExitGate::Initialize(const SceneContext&)
{
	const auto pMat = MaterialManager::Get()->CreateMaterial<BasicMaterial_Deferred>();
	pMat->SetDiffuseMap(L"Exam/Textures/Gate.png");

	constexpr float size{ 8 };

	const auto pModelObject = new GameObject();
	ModelComponent* pModel = new ModelComponent(L"Exam/Meshes/Gate.ovm");
	pModelObject->AddComponent<ModelComponent>(pModel);
	pModel->SetMaterial(pMat);
	AddChild(pModelObject);
	pModel->GetTransform()->Translate(0, 0.f, 0);

	//Collision
	auto& phys = PxGetPhysics();
	const auto pBouncyMaterial = phys.createMaterial(0, 0, 1.f);


	const auto pRigidBodyCp = AddComponent(new RigidBodyComponent(true));

	const auto pConvexMesh = ContentManager::Load<PxConvexMesh>(L"Exam/Meshes/Gate.ovpc");
	pRigidBodyCp->AddCollider(PxConvexMeshGeometry{ pConvexMesh, PxMeshScale{size} }, *pBouncyMaterial, true);

	auto onTrigger = [&](GameObject*, const GameObject* other, PxTriggerAction action)
	{
		if (other->GetTag() != L"Player") return;

		auto pSponge = dynamic_cast<Spongebob*>(other->GetParent());
		if(pSponge->GetHUD()->GetAmountSpatulas() >= 3)
		{
			if (action == PxTriggerAction::ENTER)
			{
				m_pEndScreen->Activate();
			}

			return;
			
		}

		if (action == PxTriggerAction::ENTER)
		{
			m_pTextObj = new GameObject();
			const auto spatTextSprite = new SpriteComponent(L"Exam/HUD/CollectSpatulasText.png");
			AddChild(m_pTextObj);
			m_pTextObj->AddComponent<SpriteComponent>(spatTextSprite);
			m_pTextObj->GetTransform()->Translate(410, 160, 0);
		}

		if (action == PxTriggerAction::LEAVE)
		{
			if (m_pTextObj)
			{
				RemoveChild(m_pTextObj, true);
				m_pTextObj = nullptr;
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
