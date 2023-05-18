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
	if (m_pVertexBuffer) m_pVertexBuffer->Release();

	D3D11_BUFFER_DESC buffDesc{};
	buffDesc.Usage = D3D11_USAGE_DYNAMIC;
	buffDesc.ByteWidth = m_ParticleCount * sizeof(VertexParticle);
	buffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	buffDesc.MiscFlags = 0;

	HRESULT hResult = sceneContext.d3dContext.pDevice->CreateBuffer(&buffDesc, nullptr, &m_pVertexBuffer);
	HANDLE_ERROR(hResult);
}

void ParticleEmitterComponent::Update(const SceneContext& sceneContext)
{
	//TODO_W9(L"Implement Update")
	
	const float elapsedSec{ sceneContext.pGameTime->GetElapsed() };

	float particleInterval{};
	const float averageEnergy = (m_EmitterSettings.maxEnergy + m_EmitterSettings.minEnergy) / 2.0f;
	particleInterval = averageEnergy / m_ParticleCount;

	m_LastParticleInit += elapsedSec;

	m_ActiveParticles = 0;

	D3D11_MAPPED_SUBRESOURCE vertexBuffer{};
	sceneContext.d3dContext.pDeviceContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &vertexBuffer);

	VertexParticle* pBuffer{};
	pBuffer = static_cast<VertexParticle*>(vertexBuffer.pData);

	for (int i{}; i < int(m_ParticleCount); ++i)
	{
		Particle& particle{ m_ParticlesArray[i] };

		if (particle.isActive) UpdateParticle(particle, elapsedSec);

		if (!particle.isActive && m_LastParticleInit >= particleInterval)
		{
			SpawnParticle(particle);
		}

		if (particle.isActive)
		{
			pBuffer[m_ActiveParticles] = particle.vertexInfo;

			++m_ActiveParticles;
		}

	}

	sceneContext.d3dContext.pDeviceContext->Unmap(m_pVertexBuffer, 0);
}

void ParticleEmitterComponent::UpdateParticle(Particle& p, float elapsedTime) const
{
	//TODO_W9(L"Implement UpdateParticle")
	if (!p.isActive) return;

	p.currentEnergy -= elapsedTime;
	if (p.currentEnergy < 0.f)
	{
		p.isActive = false;
		return;
	}

	const XMVECTOR newPos = XMLoadFloat3(&p.vertexInfo.Position) + (XMLoadFloat3(&m_EmitterSettings.velocity) * elapsedTime);
	XMStoreFloat3(&p.vertexInfo.Position, newPos);

	const float lifePercent = p.currentEnergy / p.totalEnergy;

	p.vertexInfo.Color = m_EmitterSettings.color;
	p.vertexInfo.Color.w = (p.vertexInfo.Color.w * lifePercent);

	p.vertexInfo.Size = std::lerp(p.initialSize, p.initialSize * p.sizeChange, (1 - lifePercent));
}

void ParticleEmitterComponent::SpawnParticle(Particle& p)
{
	//TODO_W9(L"Implement SpawnParticle")
	p.isActive = true;

	const float rndEnergy = MathHelper::randF(m_EmitterSettings.minEnergy, m_EmitterSettings.maxEnergy);
	p.totalEnergy = rndEnergy;
	p.currentEnergy = rndEnergy;

	XMVECTOR randomDirection = { 1,0,0 };
	auto randomRotationMatrix = XMMatrixRotationRollPitchYaw(MathHelper::randF(-XM_PI, XM_PI), 
		MathHelper::randF(-XM_PI, XM_PI), MathHelper::randF(-XM_PI, XM_PI));
	XMVector2TransformNormal(randomDirection, randomRotationMatrix);

	float startDistance = MathHelper::randF(m_EmitterSettings.minEmitterRadius, m_EmitterSettings.maxEmitterRadius);
	XMStoreFloat3(&p.vertexInfo.Position, XMLoadFloat3(&GetTransform()->GetWorldPosition()) + randomDirection * startDistance);


	const float rndSize = MathHelper::randF(m_EmitterSettings.minSize, m_EmitterSettings.maxSize);
	p.initialSize = rndSize;
	p.vertexInfo.Size = rndSize;

	p.sizeChange = MathHelper::randF(m_EmitterSettings.minScale, m_EmitterSettings.maxScale);


	p.vertexInfo.Rotation = MathHelper::randF(-XM_PI, XM_PI);


	p.vertexInfo.Color = m_EmitterSettings.color;

}

void ParticleEmitterComponent::PostDraw(const SceneContext& sceneContext)
{
	//TODO_W9(L"Implement PostDraw")
	m_pParticleMaterial->SetVariable_Matrix(L"gWorldViewProj", sceneContext.pCamera->GetViewProjection());
	m_pParticleMaterial->SetVariable_Matrix(L"gViewInverse", sceneContext.pCamera->GetViewInverse());
	m_pParticleMaterial->SetVariable_Texture(L"gParticleTexture", m_pParticleTexture);

	MaterialTechniqueContext techniqueContext = m_pParticleMaterial->GetTechniqueContext();

	sceneContext.d3dContext.pDeviceContext->IASetInputLayout(techniqueContext.pInputLayout);

	sceneContext.d3dContext.pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	const UINT offset = 0;
	const UINT stride = sizeof(VertexParticle);
	sceneContext.d3dContext.pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	D3DX11_TECHNIQUE_DESC techniqueDesc{};
	techniqueContext.pTechnique->GetDesc(&techniqueDesc);
	for (int p{}; p < int(techniqueDesc.Passes); ++p)
	{
		techniqueContext.pTechnique->GetPassByIndex(p)->Apply(0, sceneContext.d3dContext.pDeviceContext);
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