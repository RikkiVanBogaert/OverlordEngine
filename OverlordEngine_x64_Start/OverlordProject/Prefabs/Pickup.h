#pragma once
class Pickup : public GameObject
{
public:
	Pickup() = default;

};

class Spatula final : public Pickup
{
public:
	Spatula() = default;

protected:
	void Initialize(const SceneContext& sceneContext) override;
};

class Underwear final : public Pickup
{
public:
	Underwear() = default;

protected:
	void Initialize(const SceneContext& sceneContext) override;
};


