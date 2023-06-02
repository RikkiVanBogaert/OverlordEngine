#pragma once
class BubbleParticles final : public GameObject
{
public:
	BubbleParticles(float lifeTime = 1);

	void Update(const SceneContext&) override;
	void SetActive(bool isActive);

private:
	const float m_StopSpawningTime;
	const float m_DeleteTime;
	float m_Timer;
	bool m_IsActive{};

	ParticleEmitterComponent* m_pParticles;
};

