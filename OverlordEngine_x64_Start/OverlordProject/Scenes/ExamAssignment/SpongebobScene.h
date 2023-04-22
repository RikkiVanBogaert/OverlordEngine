#pragma once
class Character;

class SpongebobScene : public GameScene
{
public:
	SpongebobScene() : GameScene(L"SpongebobScene") {}
	~SpongebobScene() override;
	SpongebobScene(const SpongebobScene& other) = delete;
	SpongebobScene(SpongebobScene&& other) noexcept = delete;
	SpongebobScene& operator=(const SpongebobScene& other) = delete;
	SpongebobScene& operator=(SpongebobScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void OnGUI() override;
	void Update() override;

private:
	enum InputIds
	{
		CharacterMoveLeft,
		CharacterMoveRight,
		CharacterMoveForward,
		CharacterMoveBackward,
		CharacterJump
	};

	Character* m_pCharacter{};
	GameObject* m_pSpongebobMesh{}; //maybe local
};

