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
	m_pRestartSprite = new SpriteComponent(L"Exam/HUD/RestartButton.png");
	pRestartObj->AddComponent<SpriteComponent>(m_pRestartSprite);
	AddChild(pRestartObj);
	pRestartObj->GetTransform()->Translate(880, yPos, 0);
	pRestartObj->GetTransform()->Scale(scale);
	m_Buttons.emplace_back(m_pRestartSprite);

}


void PauseMenu::Update(const SceneContext& sceneContext)
{
	if (sceneContext.pInput->GetMouseMovement().x != 0 || sceneContext.pInput->GetMouseMovement().y != 0)
	{
		HoverOverButton(sceneContext);
	}
	else
	{
		CheckControllerInput(sceneContext);
	}

	CheckActiveButton(sceneContext);
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

void PauseMenu::CheckControllerInput(const SceneContext& sceneContext)
{
	if (sceneContext.pInput->IsGamepadButton(InputState::released, XINPUT_GAMEPAD_DPAD_RIGHT))
	{
		if (!m_pActiveButton)
		{
			m_pActiveButton = m_pResumeSprite;
			m_pResumeSprite->GetTransform()->Scale(.5f);
		}
		else if (m_pActiveButton == m_pResumeSprite)
		{
			m_pActiveButton = m_pQuitSprite;
			m_pResumeSprite->GetTransform()->Scale(.4f);
			m_pQuitSprite->GetTransform()->Scale(.5f);
		}
		else if (m_pActiveButton == m_pQuitSprite)
		{
			m_pActiveButton = m_pRestartSprite;
			m_pQuitSprite->GetTransform()->Scale(.4f);
			m_pRestartSprite->GetTransform()->Scale(.5f);
		}
		else if (m_pActiveButton == m_pRestartSprite)
		{
			m_pActiveButton = m_pResumeSprite;
			m_pRestartSprite->GetTransform()->Scale(.4f);
			m_pResumeSprite->GetTransform()->Scale(.5f);
		}
	}
	else if(sceneContext.pInput->IsGamepadButton(InputState::released, XINPUT_GAMEPAD_DPAD_LEFT))
	{
		if (!m_pActiveButton)
		{
			m_pActiveButton = m_pResumeSprite;
			m_pResumeSprite->GetTransform()->Scale(.5f);
		}
		else if (m_pActiveButton == m_pResumeSprite)
		{
			m_pActiveButton = m_pRestartSprite;
			m_pResumeSprite->GetTransform()->Scale(.4f);
			m_pRestartSprite->GetTransform()->Scale(.5f);
		}
		else if (m_pActiveButton == m_pRestartSprite)
		{
			m_pActiveButton = m_pQuitSprite;
			m_pRestartSprite->GetTransform()->Scale(.4f);
			m_pQuitSprite->GetTransform()->Scale(.5f);
		}
		else if (m_pActiveButton == m_pQuitSprite)
		{
			m_pActiveButton = m_pResumeSprite;
			m_pQuitSprite->GetTransform()->Scale(.4f);
			m_pResumeSprite->GetTransform()->Scale(.5f);
		}
	}
}

void PauseMenu::CheckActiveButton(const SceneContext& sceneContext)
{
	if (!m_pActiveButton) return;

	if (!InputManager::IsMouseButton(InputState::released, 1) &&
		!sceneContext.pInput->IsGamepadButton(InputState::released, XINPUT_GAMEPAD_A)) return;

	if(m_pActiveButton == m_pResumeSprite)
	{
		m_pSponge->TurnPauseMenuOnOff();
	}
	else if (m_pActiveButton == m_pQuitSprite)
	{
		std::exit(0);
	}
	else if (m_pActiveButton == m_pRestartSprite)
	{
		auto pSpongebobScene = dynamic_cast<SpongebobScene*>(GetScene());
		pSpongebobScene->ReloadScene();

		m_pSponge->TurnPauseMenuOnOff();
		m_pSponge->ResetVariables();
	}
}
