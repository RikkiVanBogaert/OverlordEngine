#include "stdafx.h"
#include "HUDPrefab.h"

#include "Materials/DiffuseMaterial.h"

void HUDPrefab::Initialize(const SceneContext& sceneContext)
{
	auto pSprite = new SpriteComponent(L"Exam/HUD/goldenSpatula.png");
	AddComponent<SpriteComponent>(pSprite);
	pSprite->GetTransform()->Scale(0.6f);
	const float newX = sceneContext.windowWidth - 200;
	pSprite->GetTransform()->Translate(newX, 0, 0);

}

void HUDPrefab::Update(const SceneContext&)
{

}
