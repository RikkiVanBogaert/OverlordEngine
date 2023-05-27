#pragma once
class Character;

class TestScene final : public GameScene
{
public:
	TestScene() : GameScene(L"TestScene") {}
	~TestScene() override;
	TestScene(const TestScene& other) = delete;
	TestScene(TestScene&& other) noexcept = delete;
	TestScene& operator=(const TestScene& other) = delete;
	TestScene& operator=(TestScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void OnGUI() override;
	void Update() override;

private:
	//enum InputIds
	//{
	//	CharacterMoveLeft,
	//	CharacterMoveRight,
	//	CharacterMoveForward,
	//	CharacterMoveBackward,
	//	CharacterJump,
	//	Attack
	//};

	//Character* m_pCharacter{};
	//GameObject* m_pSpongebobMesh{};

	GameObject* m_pSpatula{};

	////Animations
	//ModelAnimator* pAnimator{};

	//int m_AnimationClipId{ 0 };
	//float m_AnimationSpeed{ 0.5f };

	//char** m_ClipNames{};
	//UINT m_ClipCount{};

	//void UpdateAnimations();

	void CheckDeletedObjects();
	void CreateLevel();
};

