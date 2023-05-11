#include "stdafx.h"
#include "UIElement.h"

#include "Materials/DiffuseMaterial.h"

void UIElement::Initialize(const SceneContext&)
{
	auto pMat = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();
	pMat->SetDiffuseTexture(L"Exam/Textures/spatula_golden.png");

	auto pModelObject = new GameObject();
	ModelComponent* pModel = new ModelComponent(L"Exam/Meshes/Spatula.ovm");
	pModel->SetMaterial(pMat);
	pModelObject->AddComponent<ModelComponent>(pModel);
	AddChild(pModelObject);
	pModel->GetTransform()->Translate(0, -0.5f, 0);
}

void UIElement::Update(const SceneContext&)
{
	
}
