#include "stdafx.h"
#include "Pickup.h"

#include "HUDPrefab.h"
#include "Spongebob.h"
#include "Materials/Deferred/BasicMaterial_Deferred.h"

void Spatula::Initialize(const SceneContext& sceneContext)
{
	const auto pMat = MaterialManager::Get()->CreateMaterial<BasicMaterial_Deferred>();
	pMat->SetDiffuseMap(L"Exam/Textures/spatula_golden.png");

	const auto pModelObject = new GameObject();
	ModelComponent* pModel = new ModelComponent(L"Exam/Meshes/Spatula.ovm");
	pModel->SetMaterial(pMat);
	pModelObject->AddComponent<ModelComponent>(pModel);
	AddChild(pModelObject);
	pModel->GetTransform()->Translate(0, -0.5f, 0);
	pModel->GetTransform()->Scale(GetScale());

	//Point Light
	Light light = {};
	light.isEnabled = true;
	light.position = { GetTransform()->GetPosition().x, GetTransform()->GetPosition().y,
		GetTransform()->GetPosition().z, 1 };
	light.color = { 1.f,1.f,0.f,1.f };
	light.intensity = 1.f;
	light.range = 30.0f;
	light.type = LightType::Point;
	m_LightId = int(sceneContext.pLights->GetLights().size());
	sceneContext.pLights->AddLight(light);

	//Collision
	auto& phys = PxGetPhysics();
	const auto pBouncyMaterial = phys.createMaterial(0, 0, 1.f);

	const auto pRigidBodyCp = AddComponent(new RigidBodyComponent(true));
	pRigidBodyCp->AddCollider(PxSphereGeometry(GetScale()), *pBouncyMaterial, true);

	auto onTrigger = [&](GameObject*, const GameObject* other, PxTriggerAction action)
	{
		if (other->GetTag() != L"Player") return;

		if (action != PxTriggerAction::ENTER) return;

		if (auto sponge = dynamic_cast<Spongebob*>(other->GetParent()))
		{
			sponge->GetHUD()->IncreaseAmountSpatulas(1);

			std::cout << "SPATULA PICKED UP\n";

			FMOD::Sound* m_pSound{};
			FMOD::Channel* m_pChannel{};
			const auto soundManager = SoundManager::Get();
			soundManager->GetSystem()->createStream("Resources/Exam/Sounds/PickupBling.mp3",
				FMOD_DEFAULT, nullptr, &m_pSound);
			FMOD::System* fmodSystem = soundManager->GetSystem();
			fmodSystem->playSound(m_pSound, nullptr, false, &m_pChannel);

			sceneContext.pLights->GetLight(m_LightId).isEnabled = false;

			MarkForDeletion();
		}
	};

	SetOnTriggerCallBack(onTrigger);
}

//void Underwear::Initialize(const SceneContext&)
//{
//	auto pMat = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();
//	pMat->SetDiffuseTexture(L"Exam/Textures/underwear_health.png");
//
//	auto pModelObject = new GameObject();
//	ModelComponent* pModel = new ModelComponent(L"Exam/Meshes/Underwear.ovm");
//	pModel->SetMaterial(pMat);
//	pModelObject->AddComponent<ModelComponent>(pModel);
//	AddChild(pModelObject);
//	pModel->GetTransform()->Translate(0, -0.5f, 0);
//	pModel->GetTransform()->Scale(GetScale());
//
//	//Collision
//	auto& phys = PxGetPhysics();
//	auto pBouncyMaterial = phys.createMaterial(0, 0, 1.f);
//
//	auto pRigidBodyCp = AddComponent(new RigidBodyComponent(true));
//	pRigidBodyCp->AddCollider(PxSphereGeometry(GetScale()), *pBouncyMaterial, true);
//
//	auto parentPos = this->GetTransform()->GetPosition();
//
//	auto onTrigger = [&](GameObject*, GameObject* other, PxTriggerAction action)
//	{
//		if (other->GetTag() != L"Player") return;
//
//		if (action == PxTriggerAction::ENTER)
//		{
//			std::cout << "UNDERWEAR PICKED UP\n";
//			MarkForDeletion();
//		}
//	};
//
//	SetOnTriggerCallBack(onTrigger);
//}
//
//Flower::Flower(int type):
//	m_Type{type}
//{}
//
//void Flower::Initialize(const SceneContext&)
//{
//	auto pMat = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();
//	pMat->SetDiffuseTexture(L"Exam/Textures/flower.png");
//
//	std::wstring meshName{};
//	switch (m_Type)
//	{
//	case 0:
//		meshName = L"Exam/Meshes/FlowerBlue.ovm";
//		break;
//	case 1:
//		meshName = L"Exam/Meshes/FlowerGreen.ovm";
//		break;
//	case 2:
//		meshName = L"Exam/Meshes/FlowerOrange.ovm";
//		break;
//	case 3:
//		meshName = L"Exam/Meshes/FlowerPurple.ovm";
//		break;
//	case 4:
//		meshName = L"Exam/Meshes/FlowerYellow.ovm";
//		break;
//	default:
//		break;
//	}
//
//	auto pModelObject = new GameObject();
//	ModelComponent* pModel = new ModelComponent(meshName);
//	pModel->SetMaterial(pMat);
//	pModelObject->AddComponent<ModelComponent>(pModel);
//	AddChild(pModelObject);
//	pModel->GetTransform()->Translate(0, 0, 0);
//	pModel->GetTransform()->Scale(GetScale());
//
//	//Collision
//	auto& phys = PxGetPhysics();
//	auto pBouncyMaterial = phys.createMaterial(0, 0, 1.f);
//
//
//	auto pRigidBodyCp = AddComponent(new RigidBodyComponent(true));
//
//	const float size{ 5 };
//	pRigidBodyCp->AddCollider(PxSphereGeometry(1.8f * size), *pBouncyMaterial, true);
//
//	auto onTrigger = [&](GameObject*, GameObject* other, PxTriggerAction action)
//	{
//		if (other->GetTag() != L"Player") 
//		{
//			m_HitGround = true;
//			return;
//		}
//
//		if (action == PxTriggerAction::ENTER)
//		{
//			std::cout << "FLOWER PICKED UP\n";
//			MarkForDeletion();
//
//		}
//
//	};
//
//	SetOnTriggerCallBack(onTrigger);
//}
//
//void Flower::Update(const SceneContext&)
//{
//
//}
