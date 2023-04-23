#pragma once
class Character;

class Tiki final : public GameObject
{
public:
	Tiki() = default;

protected:
	void Initialize(const SceneContext& sceneContext) override;
	void Update(const SceneContext&) override;

private:
	bool m_IsVulnerable{};
	Character* m_pPlayer{};
};

