#pragma once

class HUDPrefab final : public GameObject
{
public:
	HUDPrefab() = default;
	~HUDPrefab() override = default;

	HUDPrefab(const HUDPrefab& other) = delete;
	HUDPrefab(HUDPrefab&& other) noexcept = delete;
	HUDPrefab& operator=(const HUDPrefab& other) = delete;
	HUDPrefab& operator=(HUDPrefab&& other) noexcept = delete;

	void Update(const SceneContext&) override;

	int GetAmountSpatulas() const { return m_SpatulaAmount; }
	void SetAmountSpatulas(int amount);
	void IncreaseAmountSpatulas(int value);

protected:
	void Initialize(const SceneContext& sceneContext) override;


private:
	SpriteFont* m_pFont{};

	GameObject* spriteObj{};
	SpriteComponent* spriteCp{};
	XMFLOAT2 m_SpatulaPos{};
	int m_SpatulaAmount{};
};
