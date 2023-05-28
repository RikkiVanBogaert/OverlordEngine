#include "stdafx.h"
#include "DeferredRenderer.h"

#include "DeferredLightRenderer.h"

DeferredRenderer::~DeferredRenderer()
{
	//Release Resources
	//Delete Objects
}

void DeferredRenderer::Initialize()
{
	//Create & Collect GBuffer RTs
	//...

	//Collect References of RTVs
	//...

	//Collect SRVs (Ambient/Diffuse/Specular/Normal) + DepthSRV
	//...

	//LightPassRenderer Init
	//...
}

RenderTarget* DeferredRenderer::CreateRenderTarget(UINT width, UINT height, DXGI_FORMAT format) const
{
	RENDERTARGET_DESC rtDesc{};

	rtDesc.width = width;
	rtDesc.height = height;
	rtDesc.colorFormat = format;

	rtDesc.enableColorBuffer = true;
	rtDesc.enableColorSRV = true;

	const auto pRenderTarget = new RenderTarget(m_GameContext.d3dContext);
	HANDLE_ERROR(pRenderTarget->Create(rtDesc))

	return pRenderTarget;
}

void DeferredRenderer::Begin(const SceneContext& /*sceneContext*/) const
{
	//Ignore this function if DeferredRendering is not 'active'
	//if (!sceneContext.useDeferredRendering) return;


	//1. Clear the G-Buffer RTVs
	//...

	//2. Bind GBuffer RTVs + DSV
	//...

	//3. DRAW GEOMETRY
	//... (Render Scene)
}

void DeferredRenderer::End(const SceneContext& /*sceneContext*/) const
{
	//Ignore this function if DeferredRendering is not 'active'
	//if (!sceneContext.useDeferredRendering) return;

	//1. Geometry Pass is finished > Unbind GBuffer RTVs
	//...

	//2. Restore Main RenderTarget WITHOUT DSV (DepthBuffer SRV is used in pipeline)
	//...

	//3. Directional Light Pass
	//...

	//4. Volumetric Light Pass
	//...

	//5. Unbind G-Buffer SRVs (Diffuse, Specular, Normal & Depth)
	//...

	//6. Reset Game RenderTarget (back to normal)
	//...


	//DEBUG >> Visualize GBUFFER
	//Draw ImGui (to default RT)
	if(m_DrawImGui)
	{
		Debug_DrawGBuffer();
	}
}

void DeferredRenderer::DrawImGui()
{
	ImGui::Checkbox("Draw GBuffer", &m_DrawImGui);

	if(m_DrawImGui)
		ImGui::SliderInt("RTV ID", &m_VizRTVid, 0, RT_COUNT - 1);
}

void DeferredRenderer::Debug_DrawGBuffer() const
{
	//FULL SCREEN
	if(m_VizRTVid >= 1)
		QuadRenderer::Get()->Draw(m_ShaderResourceViews[m_VizRTVid]);

	const auto aspectRatio = m_GameContext.windowWidth / float(m_GameContext.windowHeight);
	const auto width = static_cast<int>(m_GameContext.windowWidth / 3.f);
	const auto height = static_cast<int>(width / aspectRatio);

	Quad renderQuad{
		0,
		int(m_GameContext.windowHeight) - height,
		width,
		height
	};

	//GBUFFER SRVs (only Diffuse/Specular/Normal) - skip Ambient & Depth
	for (auto i{ 1 }; i < SRV_COUNT-1; ++i)
	{
		QuadRenderer::Get()->Draw(m_ShaderResourceViews[i], renderQuad);
		renderQuad.x += width;
	}
}
