#pragma once
class UIElement;
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
		CharacterJump,
		Attack
	};

	Character* m_pCharacter{};
	GameObject* m_pSpongebobMesh{};

	GameObject* m_pSpatula{};

	//Animations
	ModelAnimator* pAnimator{};

	int m_AnimationClipId{ 0 };
	float m_AnimationSpeed{ 0.5f };

	char** m_ClipNames{};
	UINT m_ClipCount{};

	//HUD
	UIElement* m_pUISpatula;

	void PlayCorrectAnimation();
	void UpdateHUDElements();

	void CheckDeletedObjects();
	void CreateLevel();

	std::vector<std::string> mtlParser(const std::string& filename);
};

