#include "stdafx.h"
#include "UberMaterial.h"

UberMaterial::UberMaterial() :
	Material<UberMaterial>(L"Effects/UberShader.fx")
{}

void UberMaterial::SetDiffuseTexture(const std::wstring& assetFile)
{
	m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile);
	SetVariable_Scalar(L"gUseTextureDiffuse", true);
	SetVariable_Vector(L"gColorDiffuse", {1, 0, 0, 1});
	SetVariable_Texture(L"gTextureDiffuse", m_pDiffuseTexture->GetShaderResourceView());

}

void UberMaterial::SetSpecularTexture(const std::wstring& assetFile)
{
	m_pSpecularTexture = ContentManager::Load<TextureData>(assetFile);
	SetVariable_Scalar(L"gUseTextureSpecularIntensity", true);
	SetVariable_Texture(L"gTextureSpecularIntensity", m_pDiffuseTexture->GetShaderResourceView());
}

void UberMaterial::SetNormalMap(const std::wstring& assetFile)
{
	m_pNormalMap = ContentManager::Load<TextureData>(assetFile);
	SetVariable_Scalar(L"gUseTextureNormal", true);
	SetVariable_Texture(L"gTextureNormal", m_pNormalMap->GetShaderResourceView());
}

void UberMaterial::SetOpacityMap(const std::wstring& assetFile)
{
	m_pOpacityMap = ContentManager::Load<TextureData>(assetFile);
	SetVariable_Scalar(L"gTextureOpacityIntensity", true);
	SetVariable_Texture(L"gTextureOpacity", m_pOpacityMap->GetShaderResourceView());
}


void UberMaterial::InitializeEffectVariables()
{
	//Fresnel Falloff
	SetVariable_Scalar(L"gUseFresnelFalloff", true);
	SetVariable_Scalar(L"gFresnelPower", 15);

	//Specular
	SetVariable_Scalar(L"gUseSpecularPhong", true);
	SetVariable_Vector(L"gColorSpecular", { 0, 0, 0, 1 });
}
