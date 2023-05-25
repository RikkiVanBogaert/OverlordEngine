#pragma once

class EndScene final : public GameScene
{
public:
	EndScene() : GameScene(L"EndScene") {}
	~EndScene() override;
	EndScene(const EndScene& other) = delete;
	EndScene(EndScene&& other) noexcept = delete;
	EndScene& operator=(const EndScene& other) = delete;
	EndScene& operator=(EndScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void OnGUI() override;
	void Update() override;

private:

};

