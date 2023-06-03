#include "stdafx.h"
#include "BoneObject.h"

BoneObject::BoneObject(BaseMaterial* pMaterial, float length):
	m_pMaterial{pMaterial},
	m_Length{length}
{}


void BoneObject::Initialize(const SceneContext&)
{
	GameObject* pEmpty = new GameObject();

	AddChild(pEmpty);

	ModelComponent* pModel = new ModelComponent(L"Meshes/Bone.ovm");
	pEmpty->AddComponent<ModelComponent>(pModel);

	pModel->SetMaterial(m_pMaterial);

	pEmpty->GetTransform()->Rotate(0, -90, 0);

	pEmpty->GetTransform()->Scale(m_Length);
}

void BoneObject::AddBone(BoneObject* pBone)
{
	pBone->GetTransform()->Translate(m_Length, 0, 0);

	AddChild(pBone);
}

void BoneObject::CalculateBindPose()
{
	const auto invWorldMatrix = XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4 ( & GetTransform()->GetWorld()));

	XMStoreFloat4x4(&m_BindPose, invWorldMatrix);

	for (auto pChild : GetChildren<BoneObject>())
	{
		pChild->CalculateBindPose();
	}
}