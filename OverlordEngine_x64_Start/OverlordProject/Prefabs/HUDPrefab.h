#pragma once

class HUDPrefab final : public GameObject
{
public:
	HUDPrefab() = default;

	void Update(const SceneContext&) override;

protected:
	void Initialize(const SceneContext& sceneContext) override;

private:

};
