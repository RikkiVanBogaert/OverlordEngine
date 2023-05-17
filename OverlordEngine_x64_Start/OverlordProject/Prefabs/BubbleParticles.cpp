#include "stdafx.h"
#include "BubbleParticles.h"

BubbleParticles::BubbleParticles(float lifeTime):
	m_LifeTime{lifeTime}
{
	//Particle System
	ParticleEmitterSettings settings{};
	settings.velocity = { 0.f,20.f,0.f };
	settings.minSize = 1.f;
	settings.maxSize = 2.5f;
	settings.minEnergy = 2.f;
	settings.maxEnergy = 4.f;
	settings.minScale = 3.5f;
	settings.maxScale = 5.5f;
	settings.minEmitterRadius = 2.5f;
	settings.maxEmitterRadius = 10.f;
	settings.color = { 1.f,1.f,1.f, .6f };

	AddComponent(new ParticleEmitterComponent(L"Textures/Bubble.png", settings, 50));
}

void BubbleParticles::Update(const SceneContext& scene_context)
{
	m_Timer += scene_context.pGameTime->GetElapsed();

	if(m_Timer > m_LifeTime)
	{
		MarkForDeletion();
	}
}
