#include "stdafx.h"
#include "HUDPrefab.h"


void HUDPrefab::Initialize(const SceneContext& sceneContext)
{
	pSprite = new SpriteComponent(L"Exam/HUD/goldenSpatula.png");
	AddComponent<SpriteComponent>(pSprite);
	constexpr float scale{ 0.4f };
	pSprite->GetTransform()->Scale(0, scale,scale);
	m_SpatulaPos.x = sceneContext.windowWidth / 2 - pSprite->GetDimensions().x * scale / 2;
	pSprite->GetTransform()->Translate(m_SpatulaPos.x, 100, 100);

	m_pFont = ContentManager::Load<SpriteFont>(L"SpriteFonts/Consolas_32.fnt");
}


void HUDPrefab::Update(const SceneContext&)
{
	const XMFLOAT2 spatTextPos = { m_SpatulaPos.x + pSprite->GetDimensions().x * pSprite->GetTransform()->GetScale().x,
		m_SpatulaPos.y + pSprite->GetDimensions().y * pSprite->GetTransform()->GetScale().y / 2 - 10 };
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
