#pragma once

class MainMenuScene final : public GameScene
{
public:
	MainMenuScene() : GameScene(L"MainMenuScene") {}
	~MainMenuScene() override;
	MainMenuScene(const MainMenuScene& other) = delete;
	MainMenuScene(MainMenuScene&& other) noexcept = delete;
	MainMenuScene& operator=(const MainMenuScene& other) = delete;
	MainMenuScene& operator=(MainMenuScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;

private:
	SpriteComponent* m_pPlaySprite{};
	SpriteComponent* m_pQuitSprite{};

	SpriteComponent* m_pActiveButton{};

	std::vector<SpriteComponent*> m_Buttons;

	void HoverOverButton(const SceneContext& sceneContext);
	void CheckActiveButton();

	bool MouseInRect(const SceneContext& sceneContext, const XMFLOAT2& pos, const XMFLOAT2& size) const;
};

