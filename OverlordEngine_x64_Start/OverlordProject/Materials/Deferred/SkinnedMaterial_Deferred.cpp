#include "stdafx.h"
#include "SkinnedMaterial_Deferred.h"

SkinnedMaterial_Deferred::SkinnedMaterial_Deferred() :
	Material(L"Effects/Deferred/Skinned_Deferred.fx")
{
}

void SkinnedMaterial_Deferred::SetDiffuseMap(const std::wstring& assetFile)
{
	SetDiffuseMap(ContentManager::Load<TextureData>(assetFile));
}

void SkinnedMaterial_Deferred::SetDiffuseMap(TextureData* pTextureData)
{
	SetVariable_Scalar(L"gUseDiffuseMap", pTextureData != nullptr);
	SetVariable_Texture(L"gDiffuseMap", pTextureData);
}

void SkinnedMaterial_Deferred::SetNormalMap(const std::wstring& assetFile)
{
	SetNormalMap(ContentManager::Load<TextureData>(assetFile));

}

void SkinnedMaterial_Deferred::SetNormalMap(TextureData* pTextureData)
{
	SetVariable_Scalar(L"gUseNormalMap", pTextureData != nullptr);
	SetVariable_Texture(L"gNormalMap", pTextureData);
}

void SkinnedMaterial_Deferred::SetSpecularMap(const std::wstring& assetFile)
{
	SetSpecularMap(ContentManager::Load<TextureData>(assetFile));

}

void SkinnedMaterial_Deferred::SetSpecularMap(TextureData* pTextureData)
{
	SetVariable_Scalar(L"gUseSpecularMap", pTextureData != nullptr);
	SetVariable_Texture(L"gSpecularMap", pTextureData);
}

void SkinnedMaterial_Deferred::UseTransparency(bool /*enable*/)
{
	//SetTechnique(enable ? L"Default_Alpha" : L"Default");
}

void SkinnedMaterial_Deferred::InitializeEffectVariables()
{
	//For SpongebobScene----
	SetVariable_Scalar(L"gShininess", 70.f);
}

void SkinnedMaterial_Deferred::OnUpdateModelVariables(const SceneContext& sceneContext, const ModelComponent* pModel) const
{
	SetVariable_Vector(L"gLightDirection", sceneContext.pLights->GetDirectionalLight().direction);

	//Retrieve The Animator from the ModelComponent
	//Make sure the animator is not NULL (ASSERT_NULL_)
	//Retrieve the BoneTransforms from the Animator
	//Set the 'gBones' variable of the effect (MatrixArray) > BoneTransforms

	auto pAnimator = pModel->GetAnimator();
	ASSERT_NULL_(pAnimator);
	auto& boneTransforms = pAnimator->GetBoneTransforms();
	auto data = boneTransforms.data();
	const float* dataFl = reinterpret_cast<const float*>(data);
	SetVariable_MatrixArray(L"gBones", dataFl, static_cast<UINT>(boneTransforms.size()));
}
