#include "stdafx.h"
#include "BubbleParticles.h"

BubbleParticles::BubbleParticles(float lifeTime):
	m_LifeTime{lifeTime}
{
	//Particle System
	ParticleEmitterSettings settings{};
	settings.velocity = { 0.f,6.f,0.f };
	settings.minSize = 0.2f;
	settings.maxSize = 0.5f;
	settings.minEnergy = 1.f;
	settings.maxEnergy = 3.f;
	settings.minScale = 3.5f;
	settings.maxScale = 5.5f;
	settings.minEmitterRadius = 0.5f;
	settings.maxEmitterRadius = 3.f;
	settings.color = { 1.f,1.f,1.f, .6f };

	AddComponent(new ParticleEmitterComponent(L"Textures/Bubble.png", settings, 100));
}

void BubbleParticles::Update(const SceneContext& scene_context)
{
	m_Timer += scene_context.pGameTime->GetElapsed();

	if(m_Timer > m_LifeTime)
	{
		MarkForDeletion();
	}
}
