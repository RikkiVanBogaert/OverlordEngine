#include "stdafx.h"
#include "PauseMenu.h"

#include "Spongebob.h"
#include "Scenes/ExamAssignment/SpongebobScene.h"

PauseMenu::PauseMenu(Spongebob* sponge):
m_pSponge(sponge)
{}

void PauseMenu::Initialize(const SceneContext& )
{
	constexpr float scale{ 0.4f };
	constexpr float yPos{ 240 };

	auto pResumeObj = new GameObject();
	m_pResumeSprite = new SpriteComponent(L"Exam/HUD/ResumeButton.png");
	pResumeObj->AddComponent<SpriteComponent>(m_pResumeSprite);
	AddChild(pResumeObj);
	pResumeObj->GetTransform()->Translate(220, yPos, 0);
	pResumeObj->GetTransform()->Scale(scale);
	m_Buttons.emplace_back(m_pResumeSprite);

	auto pQuitObj = new GameObject();
	m_pQuitSprite = new SpriteComponent(L"Exam/HUD/QuitButton.png");
	pQuitObj->AddComponent<SpriteComponent>(m_pQuitSprite);
	AddChild(pQuitObj);
	pQuitObj->GetTransform()->Translate(550, yPos, 0);
	pQuitObj->GetTransform()->Scale(scale);
	m_Buttons.emplace_back(m_pQuitSprite);

	auto pRestartObj = new GameObject();
	m_pRestartSprite = new SpriteComponent(L"Exam/HUD/ResumeButton.png");
	pRestartObj->AddComponent<SpriteComponent>(m_pRestartSprite);
	AddChild(pRestartObj);
	pRestartObj->GetTransform()->Translate(880, yPos, 0);
	pRestartObj->GetTransform()->Scale(scale);
	m_Buttons.emplace_back(m_pRestartSprite);

	/*m_pScreen = new GameObject();
	m_pScreenSprite = new SpriteComponent(L"Exam/HUD/PauseMenu.jpg");
	m_pScreen->AddComponent<SpriteComponent>(m_pScreenSprite);
	AddChild(m_pScreen);
	m_pScreen->GetTransform()->Translate(120, 100, 0);
	m_pScreen->GetTransform()->Scale(.8f);*/

}


void PauseMenu::Update(const SceneContext& sceneContext)
{
	HoverOverButton(sceneContext);
	CheckActiveButton();
}

bool PauseMenu::MouseInRect(const SceneContext& sceneContext, const XMFLOAT2& pos, const XMFLOAT2& size) const
{
	auto mousePos = sceneContext.pInput->GetMousePosition();

	if (mousePos.x > pos.x && mousePos.x < pos.x + size.x
		&& mousePos.y > pos.y && mousePos.y < pos.y + size.y)
	{
		return true;
	}

	return false;
}

void PauseMenu::HoverOverButton(const SceneContext& sceneContext)
{
	for (auto b : m_Buttons)
	{
		const XMFLOAT2 pos{ b->GetTransform()->GetPosition().x, b->GetTransform()->GetPosition().y };
		const XMFLOAT2 size{ b->GetDimensions().x * b->GetTransform()->GetScale().x,
			b->GetDimensions().y * b->GetTransform()->GetScale().y };
		if (MouseInRect(sceneContext, pos, size))
		{
			b->GetTransform()->Scale(.5f);
			m_pActiveButton = b;
			return;
		}
		else
		{
			b->GetTransform()->Scale(.4f);
		}
	}
	m_pActiveButton = nullptr;
}

void PauseMenu::CheckActiveButton()
{
	if (!m_pActiveButton) return;

	if (!InputManager::IsMouseButton(InputState::down, 1)) return;

	if(m_pActiveButton == m_pResumeSprite)
	{
		m_pSponge->TurnPauseOnOff();
	}
	else if (m_pActiveButton == m_pQuitSprite)
	{
		std::exit(0);
	}
	else if (m_pActiveButton == m_pRestartSprite)
	{
		auto pSpongebobScene = dynamic_cast<SpongebobScene*>(GetScene());
		pSpongebobScene->ReloadScene();
		m_pSponge->TurnPauseOnOff();
	}
}
