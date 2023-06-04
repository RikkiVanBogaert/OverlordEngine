#include "stdafx.h"
#include "HUDPrefab.h"


void HUDPrefab::Initialize(const SceneContext& sceneContext)
{
	constexpr float scale{ 0.5f };

	m_pSpriteObj = new GameObject();
	m_pSpriteCp = new SpriteComponent(L"Exam/HUD/goldenSpatula.png");
	m_pSpriteObj->AddComponent<SpriteComponent>(m_pSpriteCp);
	AddChild(m_pSpriteObj);
	m_pSpriteObj->GetTransform()->Scale(scale);
	m_SpatulaPos.x = sceneContext.windowWidth / 2 - m_pSpriteCp->GetDimensions().x * scale / 2;
	m_pSpriteObj->GetTransform()->Translate(m_SpatulaPos.x, 0, 0);

	m_pFont = ContentManager::Load<SpriteFont>(L"SpriteFonts/Consolas_32.fnt");
}

void HUDPrefab::Update(const SceneContext&)
{
}

void HUDPrefab::Draw(const SceneContext&)
{
	const XMFLOAT2 spatTextPos = { m_SpatulaPos.x + m_pSpriteCp->GetDimensions().x * m_pSpriteCp->GetTransform()->GetScale().x,
		m_SpatulaPos.y + m_pSpriteCp->GetDimensions().y * m_pSpriteCp->GetTransform()->GetScale().y / 2 - 10 };
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
