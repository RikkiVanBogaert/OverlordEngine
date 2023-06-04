#pragma once
class Pickup : public GameObject
{
public:
	Pickup() = default;

	float GetScale() const { return m_Size; };

private:
	const float m_Size{5};
};

class Spatula final : public Pickup
{
public:
	Spatula() = default;

protected:
	void Initialize(const SceneContext& sceneContext) override;

private:
	int m_LightId{};
};

//class Underwear final : public Pickup
//{
//public:
//	Underwear() = default;
//
//protected:
//	void Initialize(const SceneContext& sceneContext) override;
//};
//
//class Flower final : public Pickup
//{
//public:
//	Flower(int type = rand() % 5);
//	void Update(const SceneContext&) override;
//
//protected:
//	void Initialize(const SceneContext& sceneContext) override;
//	
//private:
//	int m_Type{};
//	bool m_HitGround{};
//};


