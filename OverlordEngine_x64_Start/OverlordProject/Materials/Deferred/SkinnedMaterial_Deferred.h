#pragma once
class SkinnedMaterial_Deferred : public Material<SkinnedMaterial_Deferred>
{
public:
	SkinnedMaterial_Deferred();
	~SkinnedMaterial_Deferred() override = default;

	SkinnedMaterial_Deferred(const SkinnedMaterial_Deferred& other) = delete;
	SkinnedMaterial_Deferred(SkinnedMaterial_Deferred&& other) noexcept = delete;
	SkinnedMaterial_Deferred& operator=(const SkinnedMaterial_Deferred& other) = delete;
	SkinnedMaterial_Deferred& operator=(SkinnedMaterial_Deferred&& other) noexcept = delete;

	void SetDiffuseMap(const std::wstring& assetFile);
	void SetDiffuseMap(TextureData* pTextureData);

	void SetNormalMap(const std::wstring& assetFile);
	void SetNormalMap(TextureData* pTextureData);

	void SetSpecularMap(const std::wstring& assetFile);
	void SetSpecularMap(TextureData* pTextureData);

	void UseTransparency(bool enable);

protected:
	void InitializeEffectVariables() override;
	void OnUpdateModelVariables(const SceneContext& /*sceneContext*/, const ModelComponent* /*pModel*/) const override;
};

