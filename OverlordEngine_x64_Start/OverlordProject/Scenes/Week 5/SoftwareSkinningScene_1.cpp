//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"
#include "SoftwareSkinningScene_1.h"

#include "Materials/ColorMaterial.h"
#include "Prefabs/BoneObject.h"

SoftwareSkinningScene_1::SoftwareSkinningScene_1() :
	GameScene(L"SoftwareSkinningScene_1") {}

void SoftwareSkinningScene_1::Initialize()
{
	m_SceneContext.settings.enableOnGUI = true;

	auto pColor = MaterialManager::Get()->CreateMaterial<ColorMaterial>();

	auto pRoot = new GameObject();

	const float length = 15.f;
	m_pBone0 = new BoneObject(pColor, length);
	pRoot->AddChild(m_pBone0);

	m_pBone1 = new BoneObject(pColor, length);
	m_pBone0->AddBone(m_pBone1);

	AddChild(pRoot);
}

void SoftwareSkinningScene_1::Update()
{
	if (m_AutomaticRotation)
	{
		const float rotSpeed = 45 * m_SceneContext.pGameTime->GetElapsed();
		m_BoneRotation0 += rotSpeed * m_RotationSign;
		m_BoneRotation1 += rotSpeed * -m_RotationSign * 2;
	}
	ClampRotations();


	m_pBone0->GetTransform()->Rotate(0, 0, m_BoneRotation0);
	m_pBone1->GetTransform()->Rotate(0, 0, m_BoneRotation1);
}


void SoftwareSkinningScene_1::OnGUI()
{
	ImGui::BeginDisabled(m_AutomaticRotation);
	ImGui::DragFloat("Bone 0: Rotation Z", &m_BoneRotation0, 1, -45, 45 );
	ImGui::DragFloat("Bone 1: Rotation Z", &m_BoneRotation1, 1, -45, 45);
	ImGui::EndDisabled();

	ImGui::Checkbox("Auto Rotate", &m_AutomaticRotation);
}

void SoftwareSkinningScene_1::ClampRotations()
{
	if (m_BoneRotation0 >= 45)
	{
		m_BoneRotation0 = 45;
		m_RotationSign *= -1;
	}
	else if (m_BoneRotation0 <= -45)
	{
		m_BoneRotation0 = -45;
		m_RotationSign *= -1;
	}
}

