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
	void OnGUI() override;
	void Update() override;

private:
	
};

