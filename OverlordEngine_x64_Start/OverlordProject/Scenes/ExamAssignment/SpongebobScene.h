#pragma once

class EndScreen;
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

	void SetPaused(bool isPaused) const;
	void ReloadScene(bool pauseMusic = false);

protected:
	void Initialize() override;
	void Update() override;
	void OnSceneActivated() override;
	void OnSceneDeactivated() override;

private:
	
	GameObject* m_pSponge{};
	Character* m_pCharacter{};
	GameObject* m_pSpongebobMesh{};
	XMFLOAT3 m_StartPos{};

	//Animations
	ModelAnimator* m_pAnimator{};

	int m_AnimationClipId{ 0 };
	float m_AnimationSpeed{ 0.5f };

	char** m_ClipNames{};
	UINT m_ClipCount{};

	
	FMOD::Sound* m_pSound{};
	FMOD::Channel* m_pChannel{};
	bool m_MusicPlaying{};

	bool m_IsLoaded{};

	GameObject* m_pControlsObj{};
	SpriteComponent* m_pControlsSprite{};
	float m_ShowControlsTimer{};
	bool m_ShowControls{ true };

	EndScreen* m_pEndScreen{};

	enum State
	{
		Playing,
		Paused
	};
	void CheckDeletedObjects();
	void CheckControlSchemeTimer();
	void CreateLevel();
	void CreateObjects();
	void CreateItems();

	void ReadCreatedTextureFile(ModelComponent* levelMesh);

	struct MaterialInfo
	{
		std::string textureName;
		std::string meshName;
	};
	std::vector<MaterialInfo> mtlParser(const std::string& filename) const;

	struct MeshInfo
	{
		std::string name;
		std::string submeshId;
	};
	std::vector<MeshInfo> ParseOBJFile(const std::string& filepath) const;

	std::wstring ConvertToWideString(const std::string& str) const;
	std::vector<XMFLOAT3> readObjFile(const std::string& filePath) const;
};

