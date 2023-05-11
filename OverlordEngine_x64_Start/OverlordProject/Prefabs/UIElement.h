#pragma once

class UIElement final : public GameObject
{
public:
	UIElement() = default;

	void Update(const SceneContext&) override;

protected:
	void Initialize(const SceneContext& sceneContext) override;

private:
	
};
