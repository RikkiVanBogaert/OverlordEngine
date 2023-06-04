#include "stdafx.h"
#include "Jellyfish.h"

#include "Character.h"
#include "Materials/DiffuseMaterial.h"
#include "Materials/Deferred/BasicMaterial_Deferred.h"
#include "Prefabs/Spongebob.h"


void Jellyfish::Initialize(const SceneContext&)
{
	auto pMat = MaterialManager::Get()->CreateMaterial<BasicMaterial_Deferred>();
	pMat->SetDiffuseMap(L"Exam/Textures/jellyfish_pink.RW3.png");

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

			FMOD::Sound* m_pSound{};
			FMOD::Channel* m_pChannel{};
			auto soundManager = SoundManager::Get();
			soundManager->GetSystem()->createStream("Resources/Exam/Boing.mp3",
				FMOD_DEFAULT, nullptr, &m_pSound);
			FMOD::System* fmodSystem = soundManager->GetSystem();
			fmodSystem->playSound(m_pSound, nullptr, false, &m_pChannel);
		}

	};

	SetOnTriggerCallBack(onTrigger);

	GetTransform()->Scale(size);
}

void Jellyfish::Update(const SceneContext&)
{
	if (NeedsDeleting()) return;

}

