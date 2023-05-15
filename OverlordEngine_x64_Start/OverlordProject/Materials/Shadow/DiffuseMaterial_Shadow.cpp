#include "stdafx.h"
#include "DiffuseMaterial_Shadow.h"

DiffuseMaterial_Shadow::DiffuseMaterial_Shadow():
	Material(L"Effects/Shadow/PosNormTex3D_Shadow.fx")
{}

void DiffuseMaterial_Shadow::SetDiffuseTexture(const std::wstring& assetFile)
{
	m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile);
	SetVariable_Texture(L"gDiffuseMap", m_pDiffuseTexture);
}

void DiffuseMaterial_Shadow::InitializeEffectVariables()
{
}

void DiffuseMaterial_Shadow::OnUpdateModelVariables(const SceneContext& sceneContext, const ModelComponent* pModel) const
{
	/*
	 * TODO_W8
	 * Update The Shader Variables
	 * 1. Update the LightWVP > Used to Transform a vertex into Light clipping space
	 * 	LightWVP = model_world * light_viewprojection
	 * 	(light_viewprojection [LightVP] can be acquired from the ShadowMapRenderer)
	 *
	 * 2. Update the ShadowMap texture
	 *
	 * 3. Update the Light Direction (retrieve the direction from the LightManager > sceneContext)
	*/

	//Update Shadow Variables
	//const auto pShadowMapRenderer = ShadowMapRenderer::Get();
	//...

	//// Update Shadow Variables
	//const auto pShadowMapRenderer = ShadowMapRenderer::Get();

	//// 1. Update LightWVP
	//const XMMATRIX modelWorldMatrix = XMLoadFloat4x4(&pModel->GetGameObject()->GetTransform()->GetWorld());
	//const XMMATRIX lightViewProjectionMatrix = XMLoadFloat4x4(&pShadowMapRenderer->GetLightVP());
	//XMMATRIX lightWVP = modelWorldMatrix * lightViewProjectionMatrix;

	//SetVariable_Matrix(L"gWorldViewProj_Light", (float*)&lightWVP);

	//SetVariable_Texture(L"gShadowMap", ShadowMapRenderer::Get()->GetShadowMap());

	//SetVariable_Vector(L"gLightDirection", sceneContext.pLights->GetDirectionalLight().direction);

	XMMATRIX lightWVPmatrix = (XMLoadFloat4x4(&pModel->GetGameObject()->GetTransform()->GetWorld()) * 
		XMLoadFloat4x4(&ShadowMapRenderer::Get()->GetLightVP()));
	XMFLOAT4X4 lightWVP{};
	XMStoreFloat4x4(&lightWVP, lightWVPmatrix);
	SetVariable_Matrix(L"gWorldViewProj_Light", (float*)&lightWVP);

	SetVariable_Texture(L"gShadowMap", ShadowMapRenderer::Get()->GetShadowMap());

	SetVariable_Vector(L"gLightDirection", sceneContext.pLights->GetDirectionalLight().direction);

	// Set the shader variables with the updated values
	// shaderVariable1->SetMatrix(reinterpret_cast<const float*>(&lightWVP));
	// shaderVariable2->SetResource(pShadowMapTexture);
	// shaderVariable3->SetFloat3(reinterpret_cast<const float*>(&lightDirection));

}
