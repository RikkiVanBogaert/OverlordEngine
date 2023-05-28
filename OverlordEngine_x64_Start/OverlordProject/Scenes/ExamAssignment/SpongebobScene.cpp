#include "stdafx.h"
#include "SpongebobScene.h"

#include "Prefabs/Character.h"
#include "Prefabs/Pickup.h"
#include"Prefabs/Tiki.h"
#include "Prefabs/ThreeTikis.h"

#include "Materials/ColorMaterial.h"
#include "Materials/DiffuseMaterial.h"
#include "Materials/DiffuseMaterial_Skinned.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include "Helpers.h"
#include "Prefabs/HUDPrefab.h"
#include "Prefabs/UIElement.h"

#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <vector>

#include <locale>
#include <codecvt>
#include <string>

#include "Materials/Post/PostGrayscale.h"
#include "Materials/Post/PostMyEffect.h"
#include "Materials/Shadow/DiffuseMaterial_Shadow.h"
#include "Materials/Shadow/DiffuseMaterial_Shadow_Skinned.h"
#include "Prefabs/BubbleParticles.h"
#include "Prefabs/ExitGate.h"
#include "Prefabs/Hans.h"
#include "Prefabs/Spongebob.h"
#include "Prefabs/Jellyfish.h"

SpongebobScene::~SpongebobScene()
{
	for (UINT i{ 0 }; i < m_ClipCount; ++i)
	{
		delete[] m_ClipNames[i];
	}

	delete[] m_ClipNames;
}

void SpongebobScene::Initialize()
{
	//Deferred rendering
	m_SceneContext.useDeferredRendering = true;

	m_SceneContext.settings.enableOnGUI = true;
	m_SceneContext.settings.drawPhysXDebug = false;

	//m_StartPos = { 493.961f, 147.045f, 153.425f }; //jellyfish
	m_StartPos = { 300.858f, 47.2426f, -865.813f }; //start
	CreateLevel();
	CreateObjects();

	//HUD
	auto pHud = new HUDPrefab();
	AddChild(pHud);

	//Character
	pSponge = new Spongebob(pHud);
	AddChild(pSponge);

	//PostProcessing
	auto m_pPostEffect = MaterialManager::Get()->CreateMaterial<PostMyEffect>();
	AddPostProcessingEffect(m_pPostEffect);

	//Music

	auto soundManager = SoundManager::Get();
	soundManager->GetSystem()->createSound("../OverlordProject/Resources/Exam/LevelMusic.mp3",
		FMOD_DEFAULT, nullptr, &m_pSound);

}

void SpongebobScene::OnGUI()
{
	
}

void SpongebobScene::OnSceneActivated()
{
	auto sponge = dynamic_cast<Spongebob*>(pSponge);
	sponge->SetControllerPosition(m_StartPos);
	sponge->ResetVariables();

	CreateItems();

	FMOD::System* fmodSystem = SoundManager::Get()->GetSystem();
	fmodSystem->playSound(m_pSound, nullptr, false, &m_pChannel);
	m_pChannel->setVolume(.4f);
}

void SpongebobScene::OnSceneDeactivated()
{
	for(auto c : GetChildren())
	{
		if(typeid(c) == typeid(Spatula*) || typeid(c) == typeid(Tiki*))
		{
			RemoveChild(c, true);
		}
	}
	m_pChannel->setPaused(true);
}

void SpongebobScene::Update()
{
	CheckDeletedObjects();
}


void SpongebobScene::CheckDeletedObjects()
{
	for (auto child : GetChildren())
	{
		if (child->NeedsDeleting())
			RemoveChild(child, true);
	}
}

void SpongebobScene::CreateLevel()
{
	const auto pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);

	//Simple Level
	const auto pLevelObject = AddChild(new GameObject());
	const auto pLevelMesh = pLevelObject->AddComponent(new ModelComponent(L"Exam/Meshes/Level2.ovm"));

	const auto pLevelActor = pLevelObject->AddComponent(new RigidBodyComponent(true));
	const auto pPxTriangleMesh = ContentManager::Load<PxTriangleMesh>(L"Exam/Meshes/Level.ovpt");
	const float levelScale = 10.f;
	pLevelActor->AddCollider(PxTriangleMeshGeometry(pPxTriangleMesh, PxMeshScale({ levelScale, levelScale, levelScale })), *pDefaultMaterial);

	pLevelObject->GetTransform()->Scale(levelScale);

	const auto pGroundMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pGroundMaterial->SetDiffuseTexture(L"Textures/GroundBrick.jpg");
	pLevelMesh->SetMaterial(pGroundMaterial);

	auto objInfo = ParseOBJFile("../OverlordProject/Resources/Exam/Meshes/jellyfishfields.obj");

	auto mtlInfo = mtlParser("../OverlordProject/Resources/Exam/Textures/Level/jellyfishfields.mtl");
	for(auto m : mtlInfo)
	{
		auto pMat = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();
		for(int i{}; i < objInfo.size(); ++i)
		{
			if(objInfo[i].name == m.meshName)
			{
				pMat->SetDiffuseTexture(L"Exam/Textures/Level/" + ConvertToWideString(m.textureName));
				pLevelMesh->SetMaterial(pMat, UINT8(i));
				std::cout << "Setting texture: " << m.textureName << " to mesh: " << objInfo[i].name << '\n';
			}
		}
	}

	//Skybox
	const auto pSkybox = AddChild(new GameObject());
	const auto pSkyboxMesh = pSkybox->AddComponent(new ModelComponent(L"Exam/Meshes/skybox.ovm"));
	const auto pSkyMat = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();
	pSkyMat->SetDiffuseTexture(L"Exam/Textures/jf_sky_color.png");
	pSkyboxMesh->SetMaterial(pSkyMat, 0);

	//make transparent
	const auto pFlowerMat = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();
	pFlowerMat->SetDiffuseTexture(L"Exam/Textures/jf_sky_flowers.png");
	//pSkyboxMesh->SetMaterial(pFlowerMat, 1);

	pSkybox->GetTransform()->Scale(1.5f);
	//pSkybox->GetTransform()->Rotate(0, 90, 0);
}

void SpongebobScene::CreateObjects()
{
	auto pGate = new ExitGate();
	pGate->GetTransform()->Translate(388.383f, 0.495f, -176.235f);
	pGate->GetTransform()->Rotate(0, -30, 0);
	AddChild(pGate);

	auto pJelly = new Jellyfish();
	pJelly->GetTransform()->Translate(489.977f, -400.204f, 106.555f);
	AddChild(pJelly);

	auto pHans = new Hans();
	pHans->GetTransform()->Translate(492.334f, 160.f, 185.318f);
	pHans->GetTransform()->Rotate(0, -30, 0);
	AddChild(pHans);
}

void SpongebobScene::CreateItems()
{
	//Objects
	auto spat1 = new Spatula();
	spat1->GetTransform()->Translate(174.363f, 38.097f, -590.433f);
	AddChild(spat1);

	auto spat2 = new Spatula();
	spat2->GetTransform()->Translate(428.641f, 65.725f, -558.302f);
	AddChild(spat2);

	auto spat3 = new Spatula();
	spat3->GetTransform()->Translate(493.961f, 147.045f, 153.425f);
	AddChild(spat3);

	auto pTiki = new Tiki();
	pTiki->GetTransform()->Translate(m_StartPos.x + 15, m_StartPos.y - 3, m_StartPos.z + 5);
	AddChild(pTiki);
}

std::wstring SpongebobScene::ConvertToWideString(const std::string& str)
{
	int length = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
	if (length == 0)
	{
		// Error handling if conversion fails
		std::cerr << "Failed to convert string to wide string." << std::endl;
		return L"";
	}

	std::wstring wideStr(length, L'\0');
	if (MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wideStr[0], length) == 0)
	{
		// Error handling if conversion fails
		std::cerr << "Failed to convert string to wide string." << std::endl;
		return L"";
	}

	return wideStr;
}

std::vector<SpongebobScene::MaterialInfo> SpongebobScene::mtlParser(const std::string& filename)
{
	std::ifstream file(filename);
	std::vector<MaterialInfo> materialInfoList;

	if (file.is_open())
	{
		std::string line;
		std::string currentTextureName;
		std::string currentMeshName;

		while (std::getline(file, line))
		{
			std::istringstream iss(line);
			std::string identifier;

			iss >> identifier;

			if (identifier == "newmtl") // Material identifier
			{
				if (!currentTextureName.empty() && !currentMeshName.empty())
				{
					materialInfoList.push_back({ currentTextureName, currentMeshName });
				}

				iss >> currentMeshName;
			}
			else if (identifier == "map_Kd") // Texture identifier
			{
				iss >> currentTextureName;
			}
		}

		// Handle the last material
		if (!currentTextureName.empty() && !currentMeshName.empty())
		{
			materialInfoList.push_back({ currentTextureName, currentMeshName });
		}

		file.close();
	}
	else
	{
		std::cerr << "Failed to open file: " << filename << std::endl;
	}

	return materialInfoList;
}


std::vector<SpongebobScene::MeshInfo> SpongebobScene::ParseOBJFile(const std::string& filepath)
{
	std::ifstream file(filepath);
	std::vector<MeshInfo> materialInfoList;

	if (file.is_open())
	{
		std::string line;
		std::string currentMaterial;
		std::string currentSubmeshId;

		while (std::getline(file, line))
		{
			std::istringstream iss(line);
			std::string identifier;

			iss >> identifier;

			if (identifier == "g") // Submesh group identifier
			{
				if (!currentMaterial.empty() && !currentSubmeshId.empty())
				{
					materialInfoList.push_back({ currentMaterial, currentSubmeshId });
					//std::cout << currentMaterial << '\n';
				}

				iss >> currentSubmeshId;
			}
			else if (identifier == "usemtl") // Material identifier
			{
				iss >> currentMaterial;
			}
		}

		// Handle the last submesh group
		if (!currentMaterial.empty() && !currentSubmeshId.empty())
		{
			materialInfoList.push_back({ currentMaterial, currentSubmeshId });
		}

		file.close();
	}
	else
	{
		std::cerr << "Failed to open file: " << filepath << std::endl;
	}

	materialInfoList.erase(
		std::remove_if(materialInfoList.begin(), materialInfoList.end(), [](const MeshInfo& mesh) {
			return mesh.submeshId == "default";
			}),
		materialInfoList.end()
				);

	return materialInfoList;
}

std::vector<XMFLOAT3> SpongebobScene::readObjFile(const std::string& filePath)
{
	std::vector<XMFLOAT3> pivotPositions;
	std::ifstream file(filePath);
	if (!file.is_open())
	{
		std::cerr << "Failed to open OBJ file: " << filePath << std::endl;
		return{};
	}

	std::string line;
	std::string currentObjectName;
	XMFLOAT3 currentPivotPosition;

	while (std::getline(file, line))
	{
		std::istringstream iss(line);
		std::string prefix;
		iss >> prefix;

		if (prefix == "o") // Object name
		{
			if (!currentObjectName.empty())
			{
				pivotPositions.push_back(currentPivotPosition);
			}

			iss >> currentObjectName;
			currentPivotPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
		}
		else if (prefix == "v") // Vertex position
		{
			float x, y, z;
			iss >> x >> y >> z;
			currentPivotPosition = XMFLOAT3(x, y, z);
		}
	}

	if (!currentObjectName.empty())
	{
		pivotPositions.push_back(currentPivotPosition);
	}

	file.close();

	return pivotPositions;
}