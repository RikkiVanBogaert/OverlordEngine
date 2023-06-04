#include "stdafx.h"
#include "Pickup.h"

#include "Character.h"
#include "HUDPrefab.h"
#include "Spongebob.h"
#include "Materials/DiffuseMaterial.h"
#include "Materials/Deferred/BasicMaterial_Deferred.h"

void Spatula::Initialize(const SceneContext&)
{
	auto pMat = MaterialManager::Get()->CreateMaterial<BasicMaterial_Deferred>();
	pMat->SetDiffuseMap(L"Exam/Textures/spatula_golden.png");

	auto pModelObject = new GameObject();
	ModelComponent* pModel = new ModelComponent(L"Exam/Meshes/Spatula.ovm");
	pModel->SetMaterial(pMat);
	pModelObject->AddComponent<ModelComponent>(pModel);
	AddChild(pModelObject);
	pModel->GetTransform()->Translate(0, -0.5f, 0);
	pModel->GetTransform()->Scale(GetScale());

	//Collision
	auto& phys = PxGetPhysics();
	auto pBouncyMaterial = phys.createMaterial(0, 0, 1.f);

	auto pRigidBodyCp = AddComponent(new RigidBodyComponent(true));
	pRigidBodyCp->AddCollider(PxSphereGeometry(GetScale()), *pBouncyMaterial, true);

	auto parentPos = GetTransform()->GetPosition();

	auto onTrigger = [&](GameObject*, GameObject* other, PxTriggerAction action)
	{
		if (other->GetTag() != L"Player") return;

		if (action != PxTriggerAction::ENTER) return;

		if (auto sponge = dynamic_cast<Spongebob*>(other->GetParent()))
		{
			sponge->GetHUD()->IncreaseAmountSpatulas(1);

			std::cout << "SPATULA PICKED UP\n";

			FMOD::Sound* m_pSound{};
			FMOD::Channel* m_pChannel{};
			auto soundManager = SoundManager::Get();
			soundManager->GetSystem()->createStream("Resources/Exam/PickupBling.mp3",
				FMOD_DEFAULT, nullptr, &m_pSound);
			FMOD::System* fmodSystem = soundManager->GetSystem();
			fmodSystem->playSound(m_pSound, nullptr, false, &m_pChannel);
			
			MarkForDeletion();
		}
	};

	SetOnTriggerCallBack(onTrigger);
}

void Underwear::Initialize(const SceneContext&)
{
	auto pMat = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();
	pMat->SetDiffuseTexture(L"Exam/Textures/underwear_health.png");

	auto pModelObject = new GameObject();
	ModelComponent* pModel = new ModelComponent(L"Exam/Meshes/Underwear.ovm");
	pModel->SetMaterial(pMat);
	pModelObject->AddComponent<ModelComponent>(pModel);
	AddChild(pModelObject);
	pModel->GetTransform()->Translate(0, -0.5f, 0);
	pModel->GetTransform()->Scale(GetScale());

	//Collision
	auto& phys = PxGetPhysics();
	auto pBouncyMaterial = phys.createMaterial(0, 0, 1.f);

	auto pRigidBodyCp = AddComponent(new RigidBodyComponent(true));
	pRigidBodyCp->AddCollider(PxSphereGeometry(GetScale()), *pBouncyMaterial, true);

	auto parentPos = this->GetTransform()->GetPosition();

	auto onTrigger = [&](GameObject*, GameObject* other, PxTriggerAction action)
	{
		if (other->GetTag() != L"Player") return;

		if (action == PxTriggerAction::ENTER)
		{
			std::cout << "UNDERWEAR PICKED UP\n";
			MarkForDeletion();
		}
	};

	SetOnTriggerCallBack(onTrigger);
}

Flower::Flower(int type):
	m_Type{type}
{}

void Flower::Initialize(const SceneContext&)
{
	auto pMat = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();
	pMat->SetDiffuseTexture(L"Exam/Textures/flower.png");

	std::wstring meshName{};
	switch (m_Type)
	{
	case 0:
		meshName = L"Exam/Meshes/FlowerBlue.ovm";
		break;
	case 1:
		meshName = L"Exam/Meshes/FlowerGreen.ovm";
		break;
	case 2:
		meshName = L"Exam/Meshes/FlowerOrange.ovm";
		break;
	case 3:
		meshName = L"Exam/Meshes/FlowerPurple.ovm";
		break;
	case 4:
		meshName = L"Exam/Meshes/FlowerYellow.ovm";
		break;
	default:
		break;
	}

	auto pModelObject = new GameObject();
	ModelComponent* pModel = new ModelComponent(meshName);
	pModel->SetMaterial(pMat);
	pModelObject->AddComponent<ModelComponent>(pModel);
	AddChild(pModelObject);
	pModel->GetTransform()->Translate(0, 0, 0);
	pModel->GetTransform()->Scale(GetScale());

	//Collision
	auto& phys = PxGetPhysics();
	auto pBouncyMaterial = phys.createMaterial(0, 0, 1.f);


	auto pRigidBodyCp = AddComponent(new RigidBodyComponent(true));

	const float size{ 5 };
	pRigidBodyCp->AddCollider(PxSphereGeometry(1.8f * size), *pBouncyMaterial, true);

	auto onTrigger = [&](GameObject*, GameObject* other, PxTriggerAction action)
	{
		if (other->GetTag() != L"Player") 
		{
			m_HitGround = true;
			return;
		}

		if (action == PxTriggerAction::ENTER)
		{
			std::cout << "FLOWER PICKED UP\n";
			MarkForDeletion();

		}

	};

	SetOnTriggerCallBack(onTrigger);
}

void Flower::Update(const SceneContext&)
{

}
