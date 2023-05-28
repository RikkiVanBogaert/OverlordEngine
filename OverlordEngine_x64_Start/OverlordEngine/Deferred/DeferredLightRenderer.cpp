#include "stdafx.h"
#include "DeferredLightRenderer.h"

DeferredLightRenderer::~DeferredLightRenderer()
{
	//Release Resources
}

void DeferredLightRenderer::Initialize(const D3D11Context& /*d3dContext*/)
{
	//Directional LightPass
	//...

	//Volumetric LightPass
	//...

	//Sphere Light Mesh
	//...


	//Cone Light Mesh
	//...
}

void DeferredLightRenderer::DirectionalLightPass(const SceneContext& sceneContext, ID3D11ShaderResourceView* const /*gbufferSRVs*/[]) const
{
	//Retrieve Directional light
	const auto& light = sceneContext.pLights->GetDirectionalLight();

	if(light.isEnabled)
	{
		//Prepare Effect

		//Ambient SRV > Already on Main RenderTarget
		//...

		//Draw Effect (Full Screen Quad)
		//...
	}
}

void DeferredLightRenderer::VolumetricLightPass(const SceneContext& /*sceneContext*/, ID3D11ShaderResourceView* const /*gbufferSRVs*/[], ID3D11RenderTargetView* /*pDefaultRTV*/) const
{
	//Set DefaultRTV WITH Read-Only DSV (no write access to DepthBuffer, but still able to write to StencilBuffer)
	//...

	//Prepare Effect

	//Ambient SRV > Already on Main RenderTarget
	//...

	//Iterate Lights & Render Volumes
	//...
}

void DeferredLightRenderer::DrawVolumetricLight(const SceneContext& /*sceneContext*/, const Light& /*light*/) const
{
	//Draw Light Volume & Apply Shading
}

void DeferredLightRenderer::CreateReadOnlyDSV(const D3D11Context& /*d3dContext*/, ID3D11Resource* /*pDepthResource*/, DXGI_FORMAT /*format*/)
{
	//Create DSV with Read-Only Depth (m_pReadOnlyDepthStencilView)
}