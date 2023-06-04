#include "stdafx.h"
#include "MainMenuScene.h"


MainMenuScene::~MainMenuScene()
{}

void MainMenuScene::Initialize()
{
	m_SceneContext.settings.enableOnGUI = true;

	
	constexpr float scale{ 0.7f };
	constexpr float xPos{ 840 };

	const auto pPlayObj = new GameObject();
	m_pPlaySprite = new SpriteComponent(L"Exam/HUD/MainPlayButton.png");
	pPlayObj->AddComponent<SpriteComponent>(m_pPlaySprite);
	AddChild(pPlayObj);
	pPlayObj->GetTransform()->Translate(xPos, 340, 0);
	pPlayObj->GetTransform()->Scale(scale);
	m_pButtons.emplace_back(m_pPlaySprite);

	const auto pQuitObj = new GameObject();
	m_pQuitSprite = new SpriteComponent(L"Exam/HUD/MainQuitButton.png");
	pQuitObj->AddComponent<SpriteComponent>(m_pQuitSprite);
	AddChild(pQuitObj);
	pQuitObj->GetTransform()->Translate(xPos, 430, 0);
	pQuitObj->GetTransform()->Scale(scale);
	m_pButtons.emplace_back(m_pQuitSprite);

	const auto pControlsObj = new GameObject();
	const auto pControlsSprite = new SpriteComponent(L"Exam/HUD/MainMenuControls.png");
	pControlsObj->AddComponent<SpriteComponent>(pControlsSprite);
	AddChild(pControlsObj);
	pControlsObj->GetTransform()->Translate(xPos - 85, 480, 0);

	const auto pGameObject = new GameObject();
	const auto pSprite = new SpriteComponent(L"Exam/HUD/MainMenu.png");
	pGameObject->AddComponent<SpriteComponent>(pSprite);
	AddChild(pGameObject);
}

void MainMenuScene::Update()
{
	if(m_SceneContext.pInput->GetMouseMovement().x != 0 || m_SceneContext.pInput->GetMouseMovement().y != 0)
	{
		HoverOverButton();
	}
	else
	{
		CheckControllerInput();
	}

	CheckActiveButton();
}

void MainMenuScene::HoverOverButton()
{
	for (auto b : m_pButtons)
	{
		const XMFLOAT2 pos{ b->GetTransform()->GetPosition().x, b->GetTransform()->GetPosition().y };
		const XMFLOAT2 size{ b->GetDimensions().x * b->GetTransform()->GetScale().x,
			b->GetDimensions().y * b->GetTransform()->GetScale().y };
		if (MouseInRect( pos, size))
		{
			b->GetTransform()->Scale(.8f);
			m_pActiveButton = b;
			return;
		}
		else
		{
			b->GetTransform()->Scale(.7f);
		}
	}

	m_pActiveButton = nullptr;
}

void MainMenuScene::CheckControllerInput()
{
	if(m_SceneContext.pInput->IsGamepadButton(InputState::released, XINPUT_GAMEPAD_DPAD_DOWN) ||
		m_SceneContext.pInput->IsGamepadButton(InputState::released, XINPUT_GAMEPAD_DPAD_UP))
	{
		if(!m_pActiveButton)
		{
			m_pActiveButton = m_pPlaySprite;
			m_pPlaySprite->GetTransform()->Scale(.8f);
		}
		else if (m_pActiveButton == m_pPlaySprite)
		{
			m_pActiveButton = m_pQuitSprite;
			m_pPlaySprite->GetTransform()->Scale(.7f);
			m_pQuitSprite->GetTransform()->Scale(.8f);
		}
		else if (m_pActiveButton == m_pQuitSprite)
		{
			m_pActiveButton = m_pPlaySprite;
			m_pQuitSprite->GetTransform()->Scale(.7f);
			m_pPlaySprite->GetTransform()->Scale(.8f);
		}
	}
}

void MainMenuScene::CheckActiveButton() const
{
	if (!m_pActiveButton) return;

	if (!InputManager::IsMouseButton(InputState::released, 1) &&
		!m_SceneContext.pInput->IsGamepadButton(InputState::released, XINPUT_GAMEPAD_A)) return;

	if (m_pActiveButton == m_pPlaySprite)
	{
		SceneManager::Get()->SetActiveGameScene(L"SpongebobScene");
	}
	else if (m_pActiveButton == m_pQuitSprite)
	{
		std::exit(0);
	}
}

bool MainMenuScene::MouseInRect(const XMFLOAT2& pos, const XMFLOAT2& size) const
{
	const auto mousePos = m_SceneContext.pInput->GetMousePosition();

	if (mousePos.x > pos.x && mousePos.x < pos.x + size.x
		&& mousePos.y > pos.y && mousePos.y < pos.y + size.y)
	{
		return true;
	}

	return false;
}
