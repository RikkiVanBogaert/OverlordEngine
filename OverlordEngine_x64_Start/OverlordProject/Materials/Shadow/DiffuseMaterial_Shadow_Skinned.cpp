#include "stdafx.h"
#include "DiffuseMaterial_Shadow_Skinned.h"

DiffuseMaterial_Shadow_Skinned::DiffuseMaterial_Shadow_Skinned():
	Material(L"Effects/Shadow/PosNormTex3D_Shadow_Skinned.fx")
{}

void DiffuseMaterial_Shadow_Skinned::SetDiffuseTexture(const std::wstring& assetFile)
{
	m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile);
	SetVariable_Texture(L"gDiffuseMap", m_pDiffuseTexture);
}

void DiffuseMaterial_Shadow_Skinned::InitializeEffectVariables()
{
}

void DiffuseMaterial_Shadow_Skinned::OnUpdateModelVariables(const SceneContext& sceneContext, const ModelComponent* pModel) const
{
	//TODO_W8

	XMMATRIX lightWVPmatrix = (XMLoadFloat4x4(&pModel->GetGameObject()->GetTransform()->GetWorld()) * XMLoadFloat4x4(&ShadowMapRenderer::Get()->GetLightVP()));
	XMFLOAT4X4 lightWVP{};
	XMStoreFloat4x4(&lightWVP, lightWVPmatrix);
	SetVariable_Matrix(L"gWorldViewProj_Light", (float*)&lightWVP);

	SetVariable_Texture(L"gShadowMap", ShadowMapRenderer::Get()->GetShadowMap());

	SetVariable_Vector(L"gLightDirection", sceneContext.pLights->GetDirectionalLight().direction);

	ModelAnimator* pAnim = pModel->GetAnimator();
	ASSERT_NULL(pAnim, L"animator is null");
	auto bones = pAnim->GetBoneTransforms();
	SetVariable_MatrixArray(L"gBones", (float*)bones.data(), (UINT)bones.size());

}
