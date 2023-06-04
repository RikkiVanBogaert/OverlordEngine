#pragma once
class Spongebob;

class PauseMenu final : public GameObject
{
public:
	PauseMenu();

	void Update(const SceneContext&) override;
	void Activate();
	void Deactivate();
	void SetSpongebob(Spongebob* sponge) { m_pSponge = sponge; };

protected:
	void Initialize(const SceneContext& sceneContext) override;

private:
	Spongebob* m_pSponge{};

	GameObject* m_pResumeObj{};
	SpriteComponent* m_pResumeSprite{};
	GameObject* m_pQuitObj{};
	SpriteComponent* m_pQuitSprite{};
	GameObject* m_pRestartObj{};
	SpriteComponent* m_pRestartSprite{};

	SpriteComponent* m_pActiveButton{};

	std::vector<SpriteComponent*> m_pButtons;
	bool m_IsPaused{};

	void HoverOverButton(const SceneContext& sceneContext);
	void CheckControllerInput(const SceneContext& sceneContext);
	void CheckActiveButton(const SceneContext& sceneContext) const;

	bool MouseInRect(const SceneContext& sceneContext, const XMFLOAT2& pos, const XMFLOAT2& size) const;
};

