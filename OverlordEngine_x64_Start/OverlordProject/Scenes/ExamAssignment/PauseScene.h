#pragma once

class PauseScene final : public GameScene
{
public:
	PauseScene() : GameScene(L"PauseScene") {}
	~PauseScene() override;
	PauseScene(const PauseScene& other) = delete;
	PauseScene(PauseScene&& other) noexcept = delete;
	PauseScene& operator=(const PauseScene& other) = delete;
	PauseScene& operator=(PauseScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void OnGUI() override;
	void Update() override;

private:

};

