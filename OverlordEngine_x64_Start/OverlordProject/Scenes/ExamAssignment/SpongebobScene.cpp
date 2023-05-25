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
#include "Prefabs/Spongebob.h"

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
	m_SceneContext.settings.enableOnGUI = true;
	m_SceneContext.settings.drawPhysXDebug = false;


	//Ground Plane
	const auto pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);
	GameSceneExt::CreatePhysXGroundPlane(*this, pDefaultMaterial);

	//const XMFLOAT3 startPos{300, 35, -865};
	const XMFLOAT3 startPos{ 700, 20, 55 };
	CreateLevel();
	
	//Objects
	m_pSpatula = new Spatula();
	m_pSpatula->GetTransform()->Translate(startPos.x + 20, startPos.y - 3, startPos.z + 5);
	AddChild(m_pSpatula);

	m_pSpatula = new Spatula();
	m_pSpatula->GetTransform()->Translate(startPos.x - 20, startPos.y - 3, startPos.z + 5);
	AddChild(m_pSpatula);

	m_pSpatula = new Spatula();
	m_pSpatula->GetTransform()->Translate(startPos.x + 20, startPos.y - 3, startPos.z + 50);
	AddChild(m_pSpatula);

	auto pTiki = new Tiki();
	pTiki->GetTransform()->Translate(startPos.x + 15, startPos.y - 3, startPos.z + 5);
	AddChild(pTiki);

	auto pGate = new ExitGate();
	pGate->GetTransform()->Translate(625.9f, 2.6f, -130.2f);
	pGate->GetTransform()->Rotate(0, 50, 0);
	AddChild(pGate);

	//HUD
	auto pHud = new HUDPrefab();
	AddChild(pHud);

	//Character
	auto pSponge = new Spongebob(pHud);
	AddChild(pSponge);
	pSponge->SetControllerPosition(startPos);


	/*auto o = new GameObject();
	auto pSprite = new SpriteComponent(L"Exam/HUD/goldenSpatula.png");
	o->AddComponent<SpriteComponent>(pSprite);
	AddChild(o);*/

	////PostProcessing
	//auto m_pPostEffect = MaterialManager::Get()->CreateMaterial<PostMyEffect>();
	//AddPostProcessingEffect(m_pPostEffect);

}

void SpongebobScene::OnGUI()
{
	
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
	const auto pLevelMesh = pLevelObject->AddComponent(new ModelComponent(L"Exam/Meshes/NewLevel.ovm"));

	const auto pLevelActor = pLevelObject->AddComponent(new RigidBodyComponent(true));
	const auto pPxTriangleMesh = ContentManager::Load<PxTriangleMesh>(L"Exam/Meshes/NewLevel.ovpt");
	const float levelScale = 10.f;
	pLevelActor->AddCollider(PxTriangleMeshGeometry(pPxTriangleMesh, PxMeshScale({ levelScale, levelScale, levelScale })), *pDefaultMaterial);

	pLevelObject->GetTransform()->Scale(levelScale);

	const auto pGroundMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pGroundMaterial->SetDiffuseTexture(L"Textures/GroundBrick.jpg");
	pLevelMesh->SetMaterial(pGroundMaterial);

	auto objInfo = ParseOBJFile("../OverlordProject/Resources/Exam/Meshes/NewLevel.obj");

	auto mtlInfo = mtlParser("../OverlordProject/Resources/Exam/Meshes/NewLevel.mtl");
	for(auto m : mtlInfo)
	{
		auto pMat = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();
		for(int i{}; i < objInfo.size(); ++i)
		{
			if(objInfo[i].name == m.meshName)
			{
				pMat->SetDiffuseTexture(L"Exam/Textures/BikiniLevel/" + ConvertToWideString(m.textureName));
				pLevelMesh->SetMaterial(pMat, UINT8(i));
				std::cout << "Setting texture: " << m.textureName << " to mesh: " << objInfo[i].name << '\n';
			}
		}
	}

	//Skybox
	const auto pSkybox = AddChild(new GameObject());
	const auto pSkyboxMesh = pSkybox->AddComponent(new ModelComponent(L"Exam/Meshes/skybox.ovm"));
	const auto pSkyMat = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();
	pSkyMat->SetDiffuseTexture(L"Exam/Meshes/jf_sky_color.png");
	pSkyboxMesh->SetMaterial(pSkyMat, 0);

	//make transparent
	const auto pFlowerMat = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();
	pFlowerMat->SetDiffuseTexture(L"Exam/Meshes/jf_sky_flowers.png");
	//pSkyboxMesh->SetMaterial(pFlowerMat, 1);

	pSkybox->GetTransform()->Scale(1.5f);
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