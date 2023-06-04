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
	GameObject* m_pModelObject{};
	ModelComponent* m_pModel{};
	RigidBodyComponent* m_pRigidBody{};
	bool m_IsVulnerable{};
	Character* m_pPlayer{};
	BubbleParticles* m_pBubbles{};
};

