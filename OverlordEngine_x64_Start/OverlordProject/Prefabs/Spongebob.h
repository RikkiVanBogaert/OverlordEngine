#pragma once
class Character;

class Spongebob final : public GameObject
{
public:
	Spongebob() = default;
	~Spongebob() override;

	void Initialize(const SceneContext&) override;
	void Update(const SceneContext&) override;

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

	//Animations
	ModelAnimator* pAnimator{};

	int m_AnimationClipId{ 0 };
	float m_AnimationSpeed{ 0.5f };

	char** m_ClipNames{};
	UINT m_ClipCount{};

	void PlayCorrectAnimation();
};

