#include "stdafx.h"
#include "ShadowMaterial_Deferred.h"

ShadowMaterial_Deferred::ShadowMaterial_Deferred() :
	Material(L"Effects/Deferred/Shadow_Deferred.fx")
{
}

void ShadowMaterial_Deferred::SetDiffuseMap(const std::wstring& assetFile)
{
	SetDiffuseMap(ContentManager::Load<TextureData>(assetFile));
}

void ShadowMaterial_Deferred::SetDiffuseMap(TextureData* pTextureData)
{
	SetVariable_Scalar(L"gUseDiffuseMap", pTextureData != nullptr);
	SetVariable_Texture(L"gDiffuseMap", pTextureData);
}

void ShadowMaterial_Deferred::SetNormalMap(const std::wstring& assetFile)
{
	SetNormalMap(ContentManager::Load<TextureData>(assetFile));

}

void ShadowMaterial_Deferred::SetNormalMap(TextureData* pTextureData)
{
	SetVariable_Scalar(L"gUseNormalMap", pTextureData != nullptr);
	SetVariable_Texture(L"gNormalMap", pTextureData);
}

void ShadowMaterial_Deferred::SetSpecularMap(const std::wstring& assetFile)
{
	SetSpecularMap(ContentManager::Load<TextureData>(assetFile));

}

void ShadowMaterial_Deferred::SetSpecularMap(TextureData* pTextureData)
{
	SetVariable_Scalar(L"gUseSpecularMap", pTextureData != nullptr);
	SetVariable_Texture(L"gSpecularMap", pTextureData);
}

void ShadowMaterial_Deferred::UseTransparency(bool /*enable*/)
{
	//SetTechnique(enable ? L"Default_Alpha" : L"Default");
}

void ShadowMaterial_Deferred::InitializeEffectVariables()
{
	//For SpongebobScene----
	SetVariable_Scalar(L"gShininess", 100.f);
}

void ShadowMaterial_Deferred::OnUpdateModelVariables(const SceneContext& sceneContext, const ModelComponent* pModel) const
{
	XMMATRIX lightWVPmatrix = (XMLoadFloat4x4(&pModel->GetGameObject()->GetTransform()->GetWorld()) *
		XMLoadFloat4x4(&ShadowMapRenderer::Get()->GetLightVP()));
	XMFLOAT4X4 lightWVP{};
	XMStoreFloat4x4(&lightWVP, lightWVPmatrix);
	SetVariable_Matrix(L"gWorldViewProj_Light", (float*)&lightWVP);

	SetVariable_Texture(L"gShadowMap", ShadowMapRenderer::Get()->GetShadowMap());

	SetVariable_Vector(L"gLightDirection", sceneContext.pLights->GetDirectionalLight().direction);
}
