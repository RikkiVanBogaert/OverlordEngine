#include "stdafx.h"
#include "ParticleEmitterComponent.h"
#include "Misc/ParticleMaterial.h"

ParticleMaterial* ParticleEmitterComponent::m_pParticleMaterial{};

ParticleEmitterComponent::ParticleEmitterComponent(const std::wstring& assetFile, const ParticleEmitterSettings& emitterSettings, UINT particleCount):
	m_ParticlesArray(new Particle[particleCount]),
	m_ParticleCount(particleCount), //How big is our particle buffer?
	m_MaxParticles(particleCount), //How many particles to draw (max == particleCount)
	m_AssetFile(assetFile),
	m_EmitterSettings(emitterSettings)
{
	m_enablePostDraw = true; //This enables the PostDraw function for the component
}

ParticleEmitterComponent::~ParticleEmitterComponent()
{
	//TODO_W9(L"Implement Destructor")
	SafeDelete(m_ParticlesArray);
	m_pVertexBuffer->Release();
}

void ParticleEmitterComponent::Initialize(const SceneContext& sceneContext)
{
	//TODO_W9(L"Implement Initialize")
	if (m_pParticleMaterial == nullptr)
	{
		m_pParticleMaterial = MaterialManager::Get()->CreateMaterial<ParticleMaterial>();
	}
	CreateVertexBuffer(sceneContext);
	m_pParticleTexture = ContentManager::Load<TextureData>(m_AssetFile);

}

void ParticleEmitterComponent::CreateVertexBuffer(const SceneContext& sceneContext)
{
	//TODO_W9(L"Implement CreateVertexBuffer")
	D3D11_BUFFER_DESC buffDesc{};
	buffDesc.Usage = D3D11_USAGE_DYNAMIC;
	buffDesc.ByteWidth = m_ParticleCount * sizeof(VertexParticle);
	buffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	buffDesc.MiscFlags = 0;

	HRESULT hr = sceneContext.d3dContext.pDevice->CreateBuffer(&buffDesc, nullptr, &m_pVertexBuffer);
	HANDLE_ERROR(hr);
}

void ParticleEmitterComponent::Update(const SceneContext& sceneContext)
{
	//TODO_W9(L"Implement Update")
	//COmmentsssss

	float deltaT{ sceneContext.pGameTime->GetElapsed() };

	int totalActiveParticles{};
	//count how many active particles are in the particle emitter already!
	for (int i{}; i < static_cast<int>(m_ParticleCount); ++i)
	{
		if (m_ParticlesArray[i].isActive) ++totalActiveParticles;
	}

	int missingParts{ (int)m_ParticleCount - totalActiveParticles };
	//particle interval is low when there are few particles
	float particleInterval{ ((m_EmitterSettings.minEnergy + m_EmitterSettings.maxEnergy) / 2) /
							(missingParts * missingParts + 1) };

	m_LastParticleSpawn += deltaT;
	m_ActiveParticles = 0;

	//map the subresource
	D3D11_MAPPED_SUBRESOURCE pSubResource{ nullptr };
	sceneContext.d3dContext.pDeviceContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &pSubResource);

	VertexParticle* pBuffer{};
	pBuffer = static_cast<VertexParticle*>(pSubResource.pData);

	//add  particle vertices to the buffer if the particle is active and update it
	for (int i{}; i < static_cast<int>(m_ParticleCount); ++i)
	{
		Particle& particle{ m_ParticlesArray[i] };

		// If the particle is currently Active > UpdateParticle function.
		if (particle.isActive) UpdateParticle(particle, deltaT);

		// If the particle is NOT Active (do not use ‘else if’), and the value of m_LastParticleInit is
		//	bigger than or equal to particleInterval > Call SpawnParticle.
		if (!particle.isActive && m_LastParticleSpawn >= particleInterval)
		{
			SpawnParticle(particle);
			m_LastParticleSpawn -= particleInterval;
		}

		// If (after potential update and/or spawn) the particle is Active, add the particle to the VertexBuffer
		if (particle.isActive)
		{
			pBuffer[m_ActiveParticles] = particle.vertexInfo;

			++m_ActiveParticles;
		}

	}

	//unmap
	sceneContext.d3dContext.pDeviceContext->Unmap(m_pVertexBuffer, 0);

}

void ParticleEmitterComponent::UpdateParticle(Particle& p, float elapsedTime) const
{
	//TODO_W9(L"Implement UpdateParticle")
	if (!p.isActive) return;

	//check the lifespan
	p.currentEnergy -= elapsedTime;
	if (p.currentEnergy < 0.f)
	{
		p.isActive = false;
		return;
	}

	//update the particleVertex info

	//position
	XMVECTOR newPosition = XMLoadFloat3(&p.vertexInfo.Position) + (XMLoadFloat3(&m_EmitterSettings.velocity) * elapsedTime);
	XMStoreFloat3(&p.vertexInfo.Position, newPosition);

	float lifePercent = p.currentEnergy / p.totalEnergy; //1 at the start, 0 at the end

	//color & transparancy
	p.vertexInfo.Color = m_EmitterSettings.color;
	p.vertexInfo.Color.w = (p.vertexInfo.Color.w * lifePercent);

	//size
	float startSize = p.initialSize;
	float endSize = p.initialSize * p.sizeChange;

	p.vertexInfo.Size = std::lerp(startSize, endSize, (1 - lifePercent));

}

void ParticleEmitterComponent::SpawnParticle(Particle& p)
{
	//TODO_W9(L"Implement SpawnParticle")
	p.isActive = true;

	//init energy
	p.totalEnergy = MathHelper::randF(m_EmitterSettings.minEnergy, m_EmitterSettings.maxEnergy);
	p.currentEnergy = p.totalEnergy;


	//init pos
	XMFLOAT3 randomVector = XMFLOAT3(MathHelper::randF(-1.f, 1.f), MathHelper::randF(-1.f, 1.f), MathHelper::randF(-1.f, 1.f));
	XMVECTOR RandUnitVector = XMVector3Normalize(XMLoadFloat3(&randomVector));

	float spawnDistance = MathHelper::randF(m_EmitterSettings.minEmitterRadius, m_EmitterSettings.maxEmitterRadius);

	XMStoreFloat3(&p.vertexInfo.Position, XMLoadFloat3(&GetGameObject()->GetTransform()->GetPosition()) + (RandUnitVector * spawnDistance));

	//init size
	p.initialSize = MathHelper::randF(m_EmitterSettings.minSize, m_EmitterSettings.maxSize);
	p.vertexInfo.Size = p.initialSize;

	p.sizeChange = MathHelper::randF(m_EmitterSettings.minScale, m_EmitterSettings.maxScale);

	//rotation
	p.vertexInfo.Rotation = MathHelper::randF(-XM_PI, XM_PI);

	//color initialization
	p.vertexInfo.Color = m_EmitterSettings.color;

}

void ParticleEmitterComponent::PostDraw(const SceneContext& sceneContext)
{
	//TODO_W9(L"Implement PostDraw")
	m_pParticleMaterial->SetVariable_Matrix(L"gWorldViewProj", sceneContext.pCamera->GetViewProjection());
	m_pParticleMaterial->SetVariable_Matrix(L"gViewInverse", sceneContext.pCamera->GetViewInverse());
	m_pParticleMaterial->SetVariable_Texture(L"gParticleTexture", m_pParticleTexture);

	//get technique context 
	MaterialTechniqueContext techContext = m_pParticleMaterial->GetTechniqueContext();

	//set inputlayout
	sceneContext.d3dContext.pDeviceContext->IASetInputLayout(techContext.pInputLayout);

	//prim topology
	sceneContext.d3dContext.pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	//vetex buffer	
	UINT stride = sizeof(VertexParticle);
	UINT offset = 0;
	sceneContext.d3dContext.pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	//draw calls
	D3DX11_TECHNIQUE_DESC techDesc{};
	techContext.pTechnique->GetDesc(&techDesc);
	for (unsigned int p{}; p < techDesc.Passes; ++p)
	{
		techContext.pTechnique->GetPassByIndex(p)->Apply(0, sceneContext.d3dContext.pDeviceContext);
		sceneContext.d3dContext.pDeviceContext->Draw(m_ActiveParticles, 0);
	}
}

void ParticleEmitterComponent::DrawImGui()
{
	if(ImGui::CollapsingHeader("Particle System"))
	{
		ImGui::SliderUInt("Count", &m_ParticleCount, 0, m_MaxParticles);
		ImGui::InputFloatRange("Energy Bounds", &m_EmitterSettings.minEnergy, &m_EmitterSettings.maxEnergy);
		ImGui::InputFloatRange("Size Bounds", &m_EmitterSettings.minSize, &m_EmitterSettings.maxSize);
		ImGui::InputFloatRange("Scale Bounds", &m_EmitterSettings.minScale, &m_EmitterSettings.maxScale);
		ImGui::InputFloatRange("Radius Bounds", &m_EmitterSettings.minEmitterRadius, &m_EmitterSettings.maxEmitterRadius);
		ImGui::InputFloat3("Velocity", &m_EmitterSettings.velocity.x);
		ImGui::ColorEdit4("Color", &m_EmitterSettings.color.x, ImGuiColorEditFlags_NoInputs);
	}
}