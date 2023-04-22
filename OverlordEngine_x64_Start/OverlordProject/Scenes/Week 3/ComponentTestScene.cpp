//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"
#include "ComponentTestScene.h"

#include "Prefabs/CubePrefab.h"
#include "Prefabs/SpherePrefab.h"

ComponentTestScene::ComponentTestScene() :
	GameScene(L"ComponentTestScene") {}

void ComponentTestScene::Initialize()
{
	auto& phys = PxGetPhysics();
	auto pBouncyMaterial = phys.createMaterial(.5f, .5f, 1.f);

	//ground plane
	GameSceneExt::CreatePhysXGroundPlane(*this, pBouncyMaterial);

	//Red sphere
	auto pSphereRed = new SpherePrefab(1, 10, XMFLOAT4(Colors::Red));
	AddChild(pSphereRed);

	pSphereRed->GetTransform()->Translate(0, 30, 0);

	auto pRigidBody  = pSphereRed->AddComponent(new RigidBodyComponent(false));
	pRigidBody->AddCollider(PxSphereGeometry(1.f), *pBouncyMaterial);
	pRigidBody->SetCollisionGroup(CollisionGroup::Group0);
	pRigidBody->SetCollisionIgnoreGroups(CollisionGroup::Group1 | CollisionGroup::Group2);

	//Green sphere
	auto pSphereGreen = new SpherePrefab(1, 10, XMFLOAT4(Colors::Green));
	AddChild(pSphereGreen);

	pSphereGreen->GetTransform()->Translate(0, 20, 0);

	auto pRigidBodyGreen = pSphereGreen->AddComponent(new RigidBodyComponent(false));
	pRigidBodyGreen->AddCollider(PxSphereGeometry(1.f), *pBouncyMaterial);
	pRigidBodyGreen->SetCollisionGroup(CollisionGroup::Group1);
	pRigidBodyGreen->SetConstraint(RigidBodyConstraint::TransX | RigidBodyConstraint::TransZ, false);

	//Blue sphere
	auto pSphereBlue = new SpherePrefab(1, 10, XMFLOAT4(Colors::Blue));
	AddChild(pSphereBlue);

	pSphereBlue->GetTransform()->Translate(0, 10, 0);

	auto pRigidBodyBlue = pSphereBlue->AddComponent(new RigidBodyComponent(false));
	pRigidBodyBlue->AddCollider(PxSphereGeometry(1.f), *pBouncyMaterial);
	pRigidBodyBlue->SetCollisionGroup(CollisionGroup::Group2);

}

void ComponentTestScene::Update()
{

}

void ComponentTestScene::Draw()
{
	
}

void ComponentTestScene::OnGUI()
{}
