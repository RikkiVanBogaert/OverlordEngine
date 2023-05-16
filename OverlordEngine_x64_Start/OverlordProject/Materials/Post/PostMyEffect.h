#pragma once
class PostMyEffect final : public PostProcessingMaterial
{
public:
	PostMyEffect();
	~PostMyEffect() override = default;
	PostMyEffect(const PostMyEffect& other) = delete;
	PostMyEffect(PostMyEffect&& other) noexcept = delete;
	PostMyEffect& operator=(const PostMyEffect& other) = delete;
	PostMyEffect& operator=(PostMyEffect&& other) noexcept = delete;

protected:
	void Initialize(const GameContext& /*gameContext*/) override {}
};
