#include "stdafx.h"
#include "Tiki.h"

#include "BubbleParticles.h"
#include "Materials/DiffuseMaterial.h"
#include "EnginePrefabs/SpherePrefab.h"
#include "Character.h"
#include "Pickup.h"
#include "Materials/Deferred/BasicMaterial_Deferred.h"

void Tiki::Initialize(const SceneContext&)
{
	auto pMat = MaterialManager::Get()->CreateMaterial<BasicMaterial_Deferred>();
	pMat->SetDiffuseMap(L"Exam/Textures/tiki.png");

	constexpr float size{ 5 };

	pModelObject = new GameObject();
	pModel = new ModelComponent(L"Exam/Meshes/Tiki.ovm");
	pModelObject->AddComponent<ModelComponent>(pModel);
	pModel->SetMaterial(pMat);
	AddChild(pModelObject);
	pModel->GetTransform()->Translate(0, 0.f, 0);
	pModel->GetTransform()->Rotate(0, float(rand() % 360), 0);

	//Collision
	auto& phys = PxGetPhysics();
	auto pBouncyMaterial = phys.createMaterial(0, 0, 1.f);


	pRigidBody = AddComponent(new RigidBodyComponent(true));
	pRigidBody->AddCollider(PxSphereGeometry(1.8f * size), *pBouncyMaterial, true);
	
	auto pConvexMesh = ContentManager::Load<PxConvexMesh>(L"Exam/Meshes/Tiki.ovpc");
	pRigidBody->AddCollider(PxConvexMeshGeometry{ pConvexMesh, PxMeshScale{size} }, *pBouncyMaterial);

	auto onTrigger = [&](GameObject*, GameObject* other, PxTriggerAction action)
	{
		if (other->GetTag() != L"Player") return;
		if(NeedsDeleting()) return;

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

	pBubbles = new BubbleParticles();
	XMFLOAT3 pos = { GetTransform()->GetPosition().x - 10, GetTransform()->GetPosition().y + 5, GetTransform()->GetPosition().z };
	pBubbles->GetTransform()->Translate(pos);
	GetScene()->AddChild(pBubbles);
}

void Tiki::Update(const SceneContext& )
{
	if (!m_IsVulnerable) return;
	if (!m_pPlayer) return;
	//if (NeedsDeleting()) return;
	if (!pModelObject->HasComponent<ModelComponent>()) return;

	if (m_pPlayer->IsAttacking())
	{
		//SpawnFlowers();
		//SpawnBubbles();
		pBubbles->SetActive(true);

		FMOD::Sound* m_pSound{};
		FMOD::Channel* m_pChannel{};
		auto soundManager = SoundManager::Get();
		auto path = ContentManager::GetFullAssetPath(L"Exam/TikiBreak.mp3").string().c_str();
		soundManager->GetSystem()->createSound(path,
			FMOD_DEFAULT, nullptr, &m_pSound);
		FMOD::System* fmodSystem = soundManager->GetSystem();
		fmodSystem->playSound(m_pSound, nullptr, false, &m_pChannel);

		//MarkForDeletion();
		pModelObject->RemoveComponent(pModel);
		GetTransform()->Translate(0, 0, 0);
		//pRigidBody->RemoveCollider(pRigidBody->GetCollider(0));
		//GetScene()->RemoveChild(this, true);
	}
}

void Tiki::SpawnFlowers()
{
	auto pFlower = new Flower();
	auto pos = GetTransform()->GetPosition();

	const int rndX{rand() % 53 - 12};
	const int rndZ{ rand() % 53 - 12 };

	pFlower->GetTransform()->Translate(pos.x + rndX, pos.y + 15, pos.z + rndZ);
	SceneManager::Get()->GetActiveScene()->AddChild(pFlower);
}

void Tiki::SpawnBubbles()
{}
