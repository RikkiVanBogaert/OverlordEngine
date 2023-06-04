#include "stdafx.h"
#include "MainMenuScene.h"

#include "SpongebobScene.h"
#include "Materials/ColorMaterial.h"
#include "Materials/DiffuseMaterial.h"


MainMenuScene::~MainMenuScene()
{}

void MainMenuScene::Initialize()
{
	m_SceneContext.settings.enableOnGUI = true;

	//GameSettings
	//GameScene::GetSceneSettings().clearColor = {};
	//GameScene::GetSceneSettings().drawGrid = false;
	//GameScene::GetSceneSettings().drawPhysXDebug = false;

	auto pCamera = new FixedCamera();
	AddChild(pCamera);
	pCamera->GetTransform()->Rotate(90, 0, 0);
	pCamera->GetTransform()->Translate(0, 40, 0);
	pCamera->GetComponent<CameraComponent>()->SetActive();


	constexpr float scale{ 0.7f };
	constexpr float xPos{ 840 };

	auto pPlayObj = new GameObject();
	m_pPlaySprite = new SpriteComponent(L"Exam/HUD/MainPlayButton.png");
	pPlayObj->AddComponent<SpriteComponent>(m_pPlaySprite);
	AddChild(pPlayObj);
	pPlayObj->GetTransform()->Translate(xPos, 340, 0);
	pPlayObj->GetTransform()->Scale(scale);
	m_Buttons.emplace_back(m_pPlaySprite);

	auto pQuitObj = new GameObject();
	m_pQuitSprite = new SpriteComponent(L"Exam/HUD/MainQuitButton.png");
	pQuitObj->AddComponent<SpriteComponent>(m_pQuitSprite);
	AddChild(pQuitObj);
	pQuitObj->GetTransform()->Translate(xPos, 430, 0);
	pQuitObj->GetTransform()->Scale(scale);
	m_Buttons.emplace_back(m_pQuitSprite);

	auto pControlsObj = new GameObject();
	auto pControlsSprite = new SpriteComponent(L"Exam/HUD/ControlsText.png");
	pControlsObj->AddComponent<SpriteComponent>(pControlsSprite);
	AddChild(pControlsObj);
	pControlsObj->GetTransform()->Translate(xPos - 50, 510, 0);

	auto pGameObject = new GameObject();
	auto pSprite = new SpriteComponent(L"Exam/HUD/MainMenu.png");
	pGameObject->AddComponent<SpriteComponent>(pSprite);
	AddChild(pGameObject);

	/*static FMOD::Sound* pSound = nullptr;
	if (!pSound)
	{
		SoundManager::Get()->GetSystem()->createStream("Resources/Exam/LevelMusic.mp3", FMOD_DEFAULT, nullptr, &pSound);
		pSound->setMode(FMOD_LOOP_NORMAL);
	}
	SoundManager::Get()->GetSystem()->playSound(pSound, nullptr, true, &m_pMainMenuChannel);
	m_pMainMenuChannel->setVolume(0.2f);*/
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

bool MainMenuScene::MouseInRect(const XMFLOAT2& pos, const XMFLOAT2& size) const
{
	auto mousePos = m_SceneContext.pInput->GetMousePosition();

	if (mousePos.x > pos.x && mousePos.x < pos.x + size.x
		&& mousePos.y > pos.y && mousePos.y < pos.y + size.y)
	{
		return true;
	}

	return false;
}

void MainMenuScene::HoverOverButton()
{
	for (auto b : m_Buttons)
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

void MainMenuScene::CheckActiveButton()
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
