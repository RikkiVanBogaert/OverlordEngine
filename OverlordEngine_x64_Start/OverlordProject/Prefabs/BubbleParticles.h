#pragma once
class BubbleParticles final : public GameObject
{
public:
	BubbleParticles(float lifeTime = 1);

	void Update(const SceneContext&) override;

private:
	const float m_LifeTime;
	float m_Timer;
};

