#pragma once

class BubbleParticles;
class Character;

class Tiki final : public GameObject
{
public:
	Tiki() = default;

	void Update(const SceneContext&) override;

protected:
	void Initialize(const SceneContext& sceneContext) override;

private:
	GameObject* pModelObject{};
	ModelComponent* pModel{};
	RigidBodyComponent* pRigidBody{};
	bool m_IsVulnerable{};
	Character* m_pPlayer{};
	BubbleParticles* pBubbles{};

	void SpawnFlowers();
	void SpawnBubbles();
};

