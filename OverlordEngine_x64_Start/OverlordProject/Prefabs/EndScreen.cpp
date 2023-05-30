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

	auto pEndObj = new GameObject();
	auto pEndSprite = new SpriteComponent(L"Exam/HUD/TheEndFrame.png");
	pEndObj->AddComponent<SpriteComponent>(pEndSprite);
	AddChild(pEndObj);
	pEndObj->GetTransform()->Translate(470, 170, 0);
	pEndObj->GetTransform()->Scale(.5f);

	auto pResumeObj = new GameObject();
	m_pResumeSprite = new SpriteComponent(L"Exam/HUD/MainMenuButton.png");
	pResumeObj->AddComponent<SpriteComponent>(m_pResumeSprite);
	AddChild(pResumeObj);
	pResumeObj->GetTransform()->Translate(220, yPos, 0);
	pResumeObj->GetTransform()->Scale(scale);
	m_Buttons.emplace_back(m_pResumeSprite);

	auto pRestartObj = new GameObject();
	m_pRestartSprite = new SpriteComponent(L"Exam/HUD/RestartButton.png");
	pRestartObj->AddComponent<SpriteComponent>(m_pRestartSprite);
	AddChild(pRestartObj);
	pRestartObj->GetTransform()->Translate(880, yPos, 0);
	pRestartObj->GetTransform()->Scale(scale);
	m_Buttons.emplace_back(m_pRestartSprite);

	//Pause Game
	m_pSponge->PauseCharacter(true);
	m_pSponge->PauseScene(true);
}


void EndScreen::Update(const SceneContext& sceneContext)
{
	HoverOverButton(sceneContext);
	CheckActiveButton();
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

void EndScreen::CheckActiveButton()
{
	if (!m_pActiveButton) return;

	if (!InputManager::IsMouseButton(InputState::released, 1)) return;

	if (m_pActiveButton == m_pResumeSprite)
	{
		auto pSpongebobScene = dynamic_cast<SpongebobScene*>(GetScene());
		pSpongebobScene->ReloadScene(true);

		m_pSponge->ResetVariables();

		SceneManager::Get()->SetActiveGameScene(L"MainMenuScene");

		GetParent()->RemoveChild(this, true);
	}
	else if (m_pActiveButton == m_pRestartSprite)
	{
		auto pSpongebobScene = dynamic_cast<SpongebobScene*>(GetScene());
		pSpongebobScene->ReloadScene();

		m_pSponge->ResetVariables();

		GetParent()->RemoveChild(this, true);

	}
}
