#pragma once

class EndScreen;

class ExitGate final : public GameObject
{
public:
	ExitGate(EndScreen* pEndScreen);

	void Update(const SceneContext&) override;


protected:
	void Initialize(const SceneContext& sceneContext) override;

private:
	GameObject* m_Text{};
	EndScreen* m_pEndScreen{};
};

