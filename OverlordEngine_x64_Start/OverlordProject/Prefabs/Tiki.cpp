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
	const auto pMat = MaterialManager::Get()->CreateMaterial<BasicMaterial_Deferred>();
	pMat->SetDiffuseMap(L"Exam/Textures/tiki.png");

	constexpr float size{ 5 };

	m_pModelObject = new GameObject();
	m_pModel = new ModelComponent(L"Exam/Meshes/Tiki.ovm");
	m_pModelObject->AddComponent<ModelComponent>(m_pModel);
	m_pModel->SetMaterial(pMat);
	AddChild(m_pModelObject);
	m_pModel->GetTransform()->Translate(0, 0.f, 0);
	m_pModel->GetTransform()->Rotate(0, float(rand() % 360), 0);

	//Collision
	auto& phys = PxGetPhysics();
	const auto pBouncyMaterial = phys.createMaterial(0, 0, 1.f);


	m_pRigidBody = AddComponent(new RigidBodyComponent(true));
	m_pRigidBody->AddCollider(PxSphereGeometry(1.8f * size), *pBouncyMaterial, true);
	
	const auto pConvexMesh = ContentManager::Load<PxConvexMesh>(L"Exam/Meshes/Tiki.ovpc");
	m_pRigidBody->AddCollider(PxConvexMeshGeometry{ pConvexMesh, PxMeshScale{size} }, *pBouncyMaterial);

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

	m_pBubbles = new BubbleParticles();
	const XMFLOAT3 pos = { GetTransform()->GetPosition().x - 10, GetTransform()->GetPosition().y + 5, GetTransform()->GetPosition().z };
	m_pBubbles->GetTransform()->Translate(pos);
	GetScene()->AddChild(m_pBubbles);
}

void Tiki::Update(const SceneContext& )
{
	if (!m_IsVulnerable) return;
	if (!m_pPlayer) return;
	
	if (m_pPlayer->IsAttacking())
	{
		m_pBubbles->SetActive(true);

		FMOD::Sound* m_pSound{};
		FMOD::Channel* m_pChannel{};
		const auto soundManager = SoundManager::Get();
		soundManager->GetSystem()->createStream("Resources/Exam/TikiBreak.mp3",
			FMOD_DEFAULT, nullptr, &m_pSound);
		FMOD::System* fmodSystem = soundManager->GetSystem();
		fmodSystem->playSound(m_pSound, nullptr, false, &m_pChannel);

		GetTransform()->Translate(0, -500, 0);
	}
}