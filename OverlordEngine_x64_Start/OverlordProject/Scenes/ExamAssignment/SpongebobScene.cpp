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

	//Character
	const XMFLOAT3 startPos{300, 35, -865};
	auto pSponge = new Spongebob();
	AddChild(pSponge);
	pSponge->SetControllerPosition(startPos);

	CreateLevel();
	
	//Objects
	m_pSpatula = new Spatula();
	m_pSpatula->GetTransform()->Translate(startPos.x + 2, startPos.y - 3, startPos.z + 5);
	AddChild(m_pSpatula);

	auto pTiki = new Tiki();
	pTiki->GetTransform()->Translate(startPos.x + 15, startPos.y - 3, startPos.z + 5);
	AddChild(pTiki);

	//HUD
	auto pHud = new HUDPrefab();
	AddChild(pHud);

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
	const auto pLevelMesh = pLevelObject->AddComponent(new ModelComponent(L"Exam/Meshes/Level.ovm"));


	const auto pLevelActor = pLevelObject->AddComponent(new RigidBodyComponent(true));
	const auto pPxTriangleMesh = ContentManager::Load<PxTriangleMesh>(L"Exam/Meshes/Level2.ovpt");
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
		auto pMat = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
		for(int i{}; i < objInfo.size(); ++i)
		{
			if(objInfo[i].name == m.meshName)
			{
				pMat->SetDiffuseTexture(L"Exam/Textures/Level/" + ConvertToWideString(m.textureName));
				pLevelMesh->SetMaterial(pMat, UINT8(i));

				//std::cout << "Setting texture: " << m.textureName << '\n';
			}
		}
	}

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

//std::vector<std::string> SpongebobScene::mtlParser(const std::string& filename)
//{
//	std::ifstream file(filename);
//	std::vector<std::string> map_kds;
//	std::string line;
//	while (std::getline(file, line)) 
//	{
//		size_t map_kd_pos = line.find("map_Kd ");
//		if (map_kd_pos != std::string::npos) 
//		{
//			std::string map_kd_value = line.substr(map_kd_pos + 7);
//			map_kds.push_back(map_kd_value);
//		}
//	}
//	return map_kds;
//}

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


//TEST
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

	return materialInfoList;
}
