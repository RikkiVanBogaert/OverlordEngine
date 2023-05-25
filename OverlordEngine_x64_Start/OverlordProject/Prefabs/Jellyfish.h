#pragma once

class Jellyfish final : public GameObject
{
public:
	Jellyfish() = default;

	void Update(const SceneContext&) override;

protected:
	void Initialize(const SceneContext& sceneContext) override;

private:
	
};

