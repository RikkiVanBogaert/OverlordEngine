#pragma once
class PongScene final : public GameScene
{
public:
	PongScene();
	~PongScene() override = default;

	PongScene(const PongScene& other) = delete;
	PongScene(PongScene&& other) noexcept = delete;
	PongScene& operator=(const PongScene& other) = delete;
	PongScene& operator=(PongScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void OnGUI() override;

private:

	GameObject* m_pSphere{};
	RigidBodyComponent* m_pSphereCollider{};

	GameObject* m_pLeftPaddle{};
	GameObject* m_pRightPaddle{};

	float m_DistanceBoundaries{};
	float m_LengthPaddle{};

	bool m_IsBalMoving{};

	void MovePaddles();
	void ConstrainPaddles();
	void Reset();
};

