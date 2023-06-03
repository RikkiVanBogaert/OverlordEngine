#include "stdafx.h"
#include "HUDPrefab.h"


void HUDPrefab::Initialize(const SceneContext& sceneContext)
{
	constexpr float scale{ 0.5f };

	spriteObj = new GameObject();
	spriteCp = new SpriteComponent(L"Exam/HUD/goldenSpatula.png");
	spriteObj->AddComponent<SpriteComponent>(spriteCp);
	AddChild(spriteObj);
	spriteObj->GetTransform()->Scale(scale);
	m_SpatulaPos.x = sceneContext.windowWidth / 2 - spriteCp->GetDimensions().x * scale / 2;
	spriteObj->GetTransform()->Translate(m_SpatulaPos.x, 0, 0);

	m_pFont = ContentManager::Load<SpriteFont>(L"SpriteFonts/Consolas_32.fnt");
}

void HUDPrefab::Update(const SceneContext&)
{
}

void HUDPrefab::Draw(const SceneContext&)
{
	const XMFLOAT2 spatTextPos = { m_SpatulaPos.x + spriteCp->GetDimensions().x * spriteCp->GetTransform()->GetScale().x,
		m_SpatulaPos.y + spriteCp->GetDimensions().y * spriteCp->GetTransform()->GetScale().y / 2 - 10 };
	TextRenderer::Get()->DrawText(m_pFont, std::to_wstring(m_SpatulaAmount), spatTextPos);
}

void HUDPrefab::SetAmountSpatulas(int amount)
{
	m_SpatulaAmount = amount;
}

void HUDPrefab::IncreaseAmountSpatulas(int value)
{
	m_SpatulaAmount += value;
}
