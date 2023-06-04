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
	std::vector<SpriteComponent*> m_pButtons;

	FMOD::Channel* m_pMainMenuChannel{};

	void HoverOverButton();
	void CheckControllerInput();
	void CheckActiveButton() const;

	bool MouseInRect(const XMFLOAT2& pos, const XMFLOAT2& size) const;

};

