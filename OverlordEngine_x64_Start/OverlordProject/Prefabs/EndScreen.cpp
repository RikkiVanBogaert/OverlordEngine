#include "stdafx.h"
#include "EndScreen.h"

#include "Spongebob.h"
#include "Scenes/ExamAssignment/SpongebobScene.h"

EndScreen::EndScreen(Spongebob* sponge) :
	m_pSponge(sponge)
{}

void EndScreen::Initialize(const SceneContext&)
{
	constexpr float scale{ 0.4f };
	constexpr float yPos{ 240 };

	m_pEndObj = new GameObject();
	AddChild(m_pEndObj);
	m_pEndObj->GetTransform()->Translate(470, 170, 0);
	m_pEndObj->GetTransform()->Scale(.5f);

	m_pMainMenuObj = new GameObject();
	AddChild(m_pMainMenuObj);
	m_pMainMenuObj->GetTransform()->Translate(220, yPos, 0);
	m_pMainMenuObj->GetTransform()->Scale(scale);

	m_pRestartObj = new GameObject();
	AddChild(m_pRestartObj);
	m_pRestartObj->GetTransform()->Translate(880, yPos, 0);
	m_pRestartObj->GetTransform()->Scale(scale);
}


void EndScreen::Update(const SceneContext& sceneContext)
{
	if (!m_IsPaused) return;

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

void EndScreen::Activate()
{
	//Pause Game
	m_pSponge->PauseCharacter(true);
	m_pSponge->PauseScene(true);

	m_pEndSprite = new SpriteComponent(L"Exam/HUD/TheEndFrame.png");
	m_pEndObj->AddComponent<SpriteComponent>(m_pEndSprite);

	m_pMainMenuSprite = new SpriteComponent(L"Exam/HUD/MainMenuButton.png");
	m_pMainMenuObj->AddComponent<SpriteComponent>(m_pMainMenuSprite);
	m_pButtons.emplace_back(m_pMainMenuSprite);

	m_pRestartSprite = new SpriteComponent(L"Exam/HUD/RestartButton.png");
	m_pRestartObj->AddComponent<SpriteComponent>(m_pRestartSprite);
	m_pButtons.emplace_back(m_pRestartSprite);

	m_IsPaused = true;
}

void EndScreen::Deactivate()
{
	m_pEndObj->RemoveComponent(m_pEndSprite, true);
	m_pMainMenuObj->RemoveComponent(m_pMainMenuSprite, true);
	m_pRestartObj->RemoveComponent(m_pRestartSprite, true);

	m_pButtons.clear();
	m_pActiveButton = nullptr;

	m_IsPaused = false;
}

bool EndScreen::MouseInRect(const SceneContext& sceneContext, const XMFLOAT2& pos, const XMFLOAT2& size) const
{
	auto mousePos = sceneContext.pInput->GetMousePosition();

	if (mousePos.x > pos.x && mousePos.x < pos.x + size.x
		&& mousePos.y > pos.y && mousePos.y < pos.y + size.y)
	{
		return true;
	}

	return false;
}

void EndScreen::HoverOverButton(const SceneContext& sceneContext)
{
	for (auto b : m_pButtons)
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

void EndScreen::CheckControllerInput(const SceneContext& sceneContext)
{
	if (sceneContext.pInput->IsGamepadButton(InputState::released, XINPUT_GAMEPAD_DPAD_RIGHT) ||
		sceneContext.pInput->IsGamepadButton(InputState::released, XINPUT_GAMEPAD_DPAD_LEFT))
	{
		if (!m_pActiveButton)
		{
			m_pActiveButton = m_pMainMenuSprite;
			m_pMainMenuSprite->GetTransform()->Scale(.5f);
		}
		else if (m_pActiveButton == m_pMainMenuSprite)
		{
			m_pActiveButton = m_pRestartSprite;
			m_pMainMenuSprite->GetTransform()->Scale(.4f);
			m_pRestartSprite->GetTransform()->Scale(.5f);
		}
		else if (m_pActiveButton == m_pRestartSprite)
		{
			m_pActiveButton = m_pMainMenuSprite;
			m_pRestartSprite->GetTransform()->Scale(.4f);
			m_pMainMenuSprite->GetTransform()->Scale(.5f);
		}
	}
}

void EndScreen::CheckActiveButton(const SceneContext& sceneContext)
{
	if (!m_pActiveButton) return;

	if (!InputManager::IsMouseButton(InputState::released, 1) &&
		!sceneContext.pInput->IsGamepadButton(InputState::released, XINPUT_GAMEPAD_A)) return;

	if (m_pActiveButton == m_pMainMenuSprite)
	{
		auto pSpongebobScene = dynamic_cast<SpongebobScene*>(GetScene());
		pSpongebobScene->ReloadScene(true);

		m_pSponge->ResetVariables();
		Deactivate();

		SceneManager::Get()->SetActiveGameScene(L"MainMenuScene");
	}
	else if (m_pActiveButton == m_pRestartSprite)
	{
		auto pSpongebobScene = dynamic_cast<SpongebobScene*>(GetScene());
		pSpongebobScene->ReloadScene();

		m_pSponge->ResetVariables();

		Deactivate();
	}
}
