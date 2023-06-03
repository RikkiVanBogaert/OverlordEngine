#pragma once
class Spongebob;

class EndScreen final : public GameObject
{
public:
	EndScreen(Spongebob* sponge);

	void Update(const SceneContext&) override;

protected:
	void Initialize(const SceneContext& sceneContext) override;

private:
	Spongebob* m_pSponge{};

	GameObject* m_pScreen{};
	SpriteComponent* m_pScreenSprite{};

	SpriteComponent* m_pMainMenuButton{};
	SpriteComponent* m_pRestartSprite{};

	SpriteComponent* m_pActiveButton{};

	std::vector<SpriteComponent*> m_Buttons;

	void HoverOverButton(const SceneContext& sceneContext);
	void CheckControllerInput(const SceneContext& sceneContext);
	void CheckActiveButton(const SceneContext& sceneContext);

	bool MouseInRect(const SceneContext& sceneContext, const XMFLOAT2& pos, const XMFLOAT2& size) const;
};

