#pragma once
class Character;

class Tiki final : public GameObject
{
public:
	Tiki() = default;

	void Update(const SceneContext&) override;

protected:
	void Initialize(const SceneContext& sceneContext) override;

private:
	bool m_IsVulnerable{};
	Character* m_pPlayer{};

	void SpawnFlowers();
	void SpawnBubbles();
};

