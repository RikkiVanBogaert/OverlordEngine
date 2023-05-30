#pragma once
class HUDPrefab;
class Character;

class Spongebob final : public GameObject
{
public:
	Spongebob(HUDPrefab* hud);
	~Spongebob() override;

	void Initialize(const SceneContext&) override;
	void Update(const SceneContext&) override;

	void SetControllerPosition(const XMFLOAT3& pos);
	void ResetVariables();
	HUDPrefab* GetHUD() { return m_pHud; };
	void TurnPauseMenuOnOff();
	void PauseCharacter(bool isPaused);
	void PauseScene(bool isPaused);

private:
	enum InputIds
	{
		CharacterMoveLeft,
		CharacterMoveRight,
		CharacterMoveForward,
		CharacterMoveBackward,
		CharacterJump,
		Attack,
		Pause
	};

	Character* m_pCharacter{};
	GameObject* m_pSpongebobMesh{};

	RigidBodyComponent* pRigidBody{};

	HUDPrefab* m_pHud;
	//Animations
	ModelAnimator* pAnimator{};

	int m_AnimationClipId{ 0 };
	float m_AnimationSpeed{ 0.5f };

	char** m_ClipNames{};
	UINT m_ClipCount{};

	FMOD::Sound* m_pWalkSound{};
	FMOD::Channel* m_pSoundChannel{};
	bool m_IsSoundPlaying{};

	GameObject* pPauseMenu{};
	bool m_IsPaused{};

	void UpdateAnimations();
	void UpdateSounds();
	void CheckPausePress();
};

