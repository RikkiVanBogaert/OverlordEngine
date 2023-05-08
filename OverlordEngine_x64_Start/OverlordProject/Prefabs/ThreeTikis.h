#pragma once
class Tiki;

class ThreeTikis final : public GameObject
{
public:
	ThreeTikis();

	virtual void Update(const SceneContext& sceneContext) override;

private:
	std::vector<std::shared_ptr<Tiki>> m_pTikis;
};

