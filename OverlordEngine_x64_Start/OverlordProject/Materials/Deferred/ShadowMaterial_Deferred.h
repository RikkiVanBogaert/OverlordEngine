#pragma once
class ShadowMaterial_Deferred : public Material<ShadowMaterial_Deferred>
{
public:
	ShadowMaterial_Deferred();
	~ShadowMaterial_Deferred() override = default;

	ShadowMaterial_Deferred(const ShadowMaterial_Deferred& other) = delete;
	ShadowMaterial_Deferred(ShadowMaterial_Deferred&& other) noexcept = delete;
	ShadowMaterial_Deferred& operator=(const ShadowMaterial_Deferred& other) = delete;
	ShadowMaterial_Deferred& operator=(ShadowMaterial_Deferred&& other) noexcept = delete;

	void SetDiffuseMap(const std::wstring& assetFile);
	void SetDiffuseMap(TextureData* pTextureData);

	void SetNormalMap(const std::wstring& assetFile);
	void SetNormalMap(TextureData* pTextureData);

	void SetSpecularMap(const std::wstring& assetFile);
	void SetSpecularMap(TextureData* pTextureData);

	void UseTransparency(bool enable);

protected:
	void InitializeEffectVariables() override;
	void OnUpdateModelVariables(const SceneContext& /*sceneContext*/, const ModelComponent* /*m_pModel*/) const;
};

