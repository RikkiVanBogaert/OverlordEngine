#pragma once

class EndScreen;

class ExitGate final : public GameObject
{
public:
	ExitGate() = default;

	void Update(const SceneContext&) override;

protected:
	void Initialize(const SceneContext& sceneContext) override;

private:
	GameObject* m_Text{};
	EndScreen* pEndScreen{};
};

