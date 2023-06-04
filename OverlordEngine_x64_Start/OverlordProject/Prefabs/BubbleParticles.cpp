#include "stdafx.h"
#include "BubbleParticles.h"

BubbleParticles::BubbleParticles(float lifeTime):
m_StopSpawningTime{lifeTime},
m_DeleteTime(4),
m_Timer(0)
{
	//Particle System
	ParticleEmitterSettings settings{};
	settings.velocity = { 0.f,15.f,0.f };
	settings.minSize = 1.f;
	settings.maxSize = 2.5f;
	settings.minEnergy = 4.f;
	settings.maxEnergy = 9.f;
	settings.minScale = 3.5f;
	settings.maxScale = 5.5f;
	settings.minEmitterRadius = 2.5f;
	settings.maxEmitterRadius = 15.f;
	settings.color = { 1.f,1.f,1.f, .6f };

	m_pParticles = AddComponent(new ParticleEmitterComponent(L"Exam/Textures/Bubble.png", settings, 100));
}

void BubbleParticles::Update(const SceneContext& scene_context)
{
	if (!m_IsActive) return;

	m_Timer += scene_context.pGameTime->GetElapsed();

	if(m_Timer > m_DeleteTime)
	{
		GetScene()->RemoveChild(this, true);
		return;
	}
	if(m_Timer > m_StopSpawningTime)
	{
		m_pParticles->StopSpawningParticles(true);
	}
}

void BubbleParticles::SetActive(bool isActive)
{
	m_IsActive = isActive;
	m_pParticles->SetActive(isActive);
}
