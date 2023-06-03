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

#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <vector>

#include <locale>
#include <codecvt>
#include <string>

#include "Materials/Deferred/BasicMaterial_Deferred.h"
#include "Materials/Deferred/ShadowMaterial_Deferred.h"
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
	//Deferred rendering //Causes pink effect on postProcessing
	m_SceneContext.useDeferredRendering = true;

	m_SceneContext.settings.enableOnGUI = true;
	m_SceneContext.settings.drawPhysXDebug = false;

	m_StartPos = { 300.858f, 47.2426f, -865.813f }; //start
	//m_StartPos = { 493.961f, 147.045f, 153.425f }; //jellyfish
	//m_StartPos = { 378.383f, 10.495f, -166.235f }; //Gate

	CreateLevel();
	CreateObjects();

	//HUD
	auto pHud = new HUDPrefab();
	AddChild(pHud);

	//Character
	pSponge = new Spongebob(pHud);
	AddChild(pSponge);
	auto sponge = dynamic_cast<Spongebob*>(pSponge);
	sponge->SetControllerPosition(m_StartPos);

	//PostProcessing
	auto m_pPostEffect = MaterialManager::Get()->CreateMaterial<PostMyEffect>();
	AddPostProcessingEffect(m_pPostEffect);

	//Music
	auto soundManager = SoundManager::Get();
	auto path = ContentManager::GetFullAssetPath(L"Exam/LevelMusic.mp3").string().c_str();
	soundManager->GetSystem()->createSound(path,
		FMOD_LOOP_NORMAL | FMOD_DEFAULT, nullptr, &m_pSound);
	FMOD::System* fmodSystem = SoundManager::Get()->GetSystem();
	fmodSystem->playSound(m_pSound, nullptr, false, &m_pChannel);
	m_pChannel->setVolume(.4f);
	m_pChannel->setPaused(true);

	//Lights
	auto& dirLight = m_SceneContext.pLights->GetDirectionalLight();
	dirLight.isEnabled = true;

	dirLight.position = { -5.6139526f,5.1346436f,-4.1850471f, 1.f };
	dirLight.direction = { 0.740129888f, -0.597205281f, 0.309117377f, 0.f };


	//Point Light
	Light light = {};
	light.isEnabled = true;
	light.position = { 0.f,700.f,0.f,1.0f };
	light.color = { 0.f,0.f,0.6f,1.f };
	light.intensity = 0.5f;
	light.range = 5000.0f;
	light.type = LightType::Point;
	m_SceneContext.pLights->AddLight(light);
}

void SpongebobScene::OnGUI()
{
	
}

void SpongebobScene::OnSceneActivated()
{
	m_pChannel->setPaused(false);

	auto sponge = dynamic_cast<Spongebob*>(pSponge);
	sponge->PauseCharacter(false);
}

void SpongebobScene::OnSceneDeactivated()
{
	m_pChannel->setPaused(true);
	auto sponge = dynamic_cast<Spongebob*>(pSponge);
	sponge->PauseCharacter(true);
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

	/*const auto pGroundMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pGroundMaterial->SetDiffuseTexture(L"Textures/GroundBrick.jpg");
	pLevelMesh->SetMaterial(pGroundMaterial);*/

	//USED THIS TO GET THE INFO, THEN WROTE IT TO A FILE FOR QUICK ACCESS AND FOR CHANGING WRONG TEXTURES
	//auto path = ContentManager::GetFullAssetPath(L"Exam/Meshes/jellyfishfields.obj");
	//auto objInfo = ParseOBJFile(path.string());
	//path = ContentManager::GetFullAssetPath(L"Exam/Textures/Level/jellyfishfields.mtl");
	//auto mtlInfo = mtlParser(path.string());
	//std::ofstream outFile("texture_names.txt"); // Open the file for writing
	//if (outFile.is_open())
	//{
	//	for (auto m : mtlInfo)
	//	{
	//		auto pMat = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	//		for (int i{}; i < objInfo.size(); ++i)
	//		{
	//			if (objInfo[i].name == m.meshName)
	//			{
	//				pMat->SetDiffuseTexture(L"Exam/Textures/Level/" + ConvertToWideString(m.textureName));
	//				pLevelMesh->SetMaterial(pMat, UINT8(i));
	//				std::cout << "Setting texture: " << m.textureName << " to mesh: " << i << '\n';
	//				outFile << i << ": " << m.textureName << '\n'; // Write the textureName to the file
	//			}
	//		}
	//	}
	//	outFile.close(); // Close the file after writing
	//}
	//else
	//{
	//	std::cout << "Failed to open the file for writing.\n";
	//}

	ReadCreatedTextureFile(pLevelMesh);


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
	pGate->GetTransform()->Rotate(0, -45, 0);
	AddChild(pGate);

	auto pJelly = new Jellyfish();
	pJelly->GetTransform()->Translate(489.977f, -400.204f, 106.555f);
	AddChild(pJelly);

	auto pHans = new Hans();
	pHans->GetTransform()->Translate(492.334f, 160.f, 185.318f);
	pHans->GetTransform()->Rotate(0, -30, 0);
	AddChild(pHans);

	CreateItems();
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

	std::vector<XMFLOAT3> tikiPositions;
	tikiPositions.emplace_back(XMFLOAT3{ 325.298f, 36.6f, -855.795f }); //start
	tikiPositions.emplace_back(XMFLOAT3{ 374.88f, 18.0335f, -660.487f });
	tikiPositions.emplace_back(XMFLOAT3{ 151.844f, 35.384f, -603.422f });

	tikiPositions.emplace_back(XMFLOAT3{ 301.575f, 14.4461f, -397.826f }); //rocks left
	tikiPositions.emplace_back(XMFLOAT3{ 629.043f, 41.9164f, -192.13f }); //back
	tikiPositions.emplace_back(XMFLOAT3{ 620.043f, 41.9164f, -190.13f }); //back 2
	tikiPositions.emplace_back(XMFLOAT3{ 625.043f, 41.9164f, -182.13f }); //back 3
	tikiPositions.emplace_back(XMFLOAT3{ 543.827f, 42.1792f, -465.691f }); //up right


	for(auto t : tikiPositions)
	{
		auto pTiki = new Tiki();
		t.y -= 6.6f;
		pTiki->GetTransform()->Translate(t);
		AddChild(pTiki);
	}
}

void SpongebobScene::ReadCreatedTextureFile(ModelComponent* levelMesh)
{
	auto path = ContentManager::GetFullAssetPath(L"Exam/texture_names.txt").string();
	std::ifstream inputFile(path);

	if (inputFile.is_open())
	{
		std::vector<int> numbers;
		std::vector<std::string> strings;

		std::string line;
		while (std::getline(inputFile, line)) 
		{
			size_t colonPos = line.find(' ');
			if (colonPos != std::string::npos) 
			{
				int number = std::stoi(line.substr(0, colonPos));
				std::string str = line.substr(colonPos + 1);
				numbers.push_back(number);
				strings.push_back(str);
			}
		}

		inputFile.close();

		// Access the numbers and strings
		for (size_t i = 0; i < numbers.size(); ++i) 
		{
			int number = numbers[i];
			std::string str = strings[i];
			std::cout << "Number: " << number << ", String: " << str << std::endl;

			const auto pMat = MaterialManager::Get()->CreateMaterial<ShadowMaterial_Deferred>();
			pMat->SetDiffuseMap(L"Exam/Textures/Level/" + ConvertToWideString(str));
			levelMesh->SetMaterial(pMat, UINT8(number));
		}
	}
	else 
	{
		std::cerr << "Failed to open the file." << std::endl;
	}
}

void SpongebobScene::SetPaused(bool isPaused)
{
	//pause/unpause music
	//stop moving items

	//m_MusicPlaying = !m_MusicPlaying;
	m_pChannel->setPaused(isPaused);
}

void SpongebobScene::ReloadScene(bool pauseMusic)
{
	//Deactivate scene
	for (auto c : GetChildren())
	{
		if (dynamic_cast<Tiki*>(c) ||
			dynamic_cast<Spatula*>(c) ||
			c->GetComponent<ParticleEmitterComponent>())
		{
			RemoveChild(c, true);
		}
	}


	auto sponge = dynamic_cast<Spongebob*>(pSponge);
	sponge->SetControllerPosition(m_StartPos);

	//Reload scene
	CreateItems();


	m_pChannel->stop();
	FMOD::System* fmodSystem = SoundManager::Get()->GetSystem();
	fmodSystem->playSound(m_pSound, nullptr, false, &m_pChannel);
	m_pChannel->setVolume(.4f);
	m_pChannel->setPaused(pauseMusic);
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