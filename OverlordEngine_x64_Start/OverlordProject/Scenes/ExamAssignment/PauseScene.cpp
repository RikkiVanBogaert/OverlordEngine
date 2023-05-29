#include "stdafx.h"
#include "PauseScene.h"

PauseScene::~PauseScene()
{}

void PauseScene::Initialize()
{
}

void PauseScene::OnGUI()
{

}

void PauseScene::Update()
{
	SceneManager::Get()->SetActiveGameScene(L"SpongebobScene");

}