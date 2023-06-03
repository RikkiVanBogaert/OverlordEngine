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

	void SetPaused(bool isPaused);
	void ReloadScene(bool pauseMusic = false);

protected:
	void Initialize() override;
	void OnGUI() override;
	void Update() override;
	void OnSceneActivated() override;
	void OnSceneDeactivated() override;

private:
	enum InputIds
	{
		Pause
	};

	GameObject* pSponge;
	Character* m_pCharacter{};
	GameObject* m_pSpongebobMesh{};
	XMFLOAT3 m_StartPos{};

	//GameObject* m_pSpatula{};

	//Animations
	ModelAnimator* pAnimator{};

	int m_AnimationClipId{ 0 };
	float m_AnimationSpeed{ 0.5f };

	char** m_ClipNames{};
	UINT m_ClipCount{};

	//HUD
	UIElement* m_pUISpatula;

	FMOD::Sound* m_pSound{};
	FMOD::Channel* m_pChannel{};
	bool m_MusicPlaying{};

	bool m_IsLoaded;


	enum State
	{
		Playing,
		Paused
	};
	void CheckDeletedObjects();
	void CreateLevel();
	void CreateObjects();
	void CreateItems();

	void ReadCreatedTextureFile(ModelComponent* levelMesh);

	struct MaterialInfo
	{
		std::string textureName;
		std::string meshName;
	};
	std::vector<MaterialInfo> mtlParser(const std::string& filename);

	struct MeshInfo
	{
		std::string name;
		std::string submeshId;
	};
	std::vector<MeshInfo> ParseOBJFile(const std::string& filepath);

	std::wstring ConvertToWideString(const std::string& str);
	std::vector<XMFLOAT3> readObjFile(const std::string& filePath);
};

