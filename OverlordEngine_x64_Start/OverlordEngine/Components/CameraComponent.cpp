﻿#include "stdafx.h"
#include "CameraComponent.h"

CameraComponent::CameraComponent() :
	m_FarPlane(2500.0f),
	m_NearPlane(0.1f),
	m_FOV(XM_PIDIV4),
	m_Size(25.0f),
	m_PerspectiveProjection(true)
{
	XMStoreFloat4x4(&m_Projection, XMMatrixIdentity());
	XMStoreFloat4x4(&m_View, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ViewInverse, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ViewProjection, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ViewProjectionInverse, XMMatrixIdentity());
}

void CameraComponent::Update(const SceneContext& sceneContext)
{
	// see https://stackoverflow.com/questions/21688529/binary-directxxmvector-does-not-define-this-operator-or-a-conversion
	using namespace DirectX;

	XMMATRIX projection{};

	if (m_PerspectiveProjection)
	{
		projection = XMMatrixPerspectiveFovLH(m_FOV, sceneContext.aspectRatio, m_NearPlane, m_FarPlane);
	}
	else
	{
		const float viewWidth = (m_Size > 0) ? m_Size * sceneContext.aspectRatio : sceneContext.windowWidth;
		const float viewHeight = (m_Size > 0) ? m_Size : sceneContext.windowHeight;
		projection = XMMatrixOrthographicLH(viewWidth, viewHeight, m_NearPlane, m_FarPlane);
	}

	const XMVECTOR worldPosition = XMLoadFloat3(&GetTransform()->GetWorldPosition());
	const XMVECTOR lookAt = XMLoadFloat3(&GetTransform()->GetForward());
	const XMVECTOR upVec = XMLoadFloat3(&GetTransform()->GetUp());

	const XMMATRIX view = XMMatrixLookAtLH(worldPosition, worldPosition + lookAt, upVec);
	const XMMATRIX viewInv = XMMatrixInverse(nullptr, view);
	const XMMATRIX viewProjectionInv = XMMatrixInverse(nullptr, view * projection);

	XMStoreFloat4x4(&m_Projection, projection);
	XMStoreFloat4x4(&m_View, view);
	XMStoreFloat4x4(&m_ViewInverse, viewInv);
	XMStoreFloat4x4(&m_ViewProjection, view * projection);
	XMStoreFloat4x4(&m_ViewProjectionInverse, viewProjectionInv);
}

void CameraComponent::SetActive(bool active)
{
	if (m_IsActive == active) return;

	const auto pGameObject = GetGameObject();
	ASSERT_IF(!pGameObject, L"Failed to set active camera. Parent game object is null");

	if (!pGameObject) return; //help the compiler... (C6011)
	const auto pScene = pGameObject->GetScene();
	ASSERT_IF(!pScene, L"Failed to set active camera. Parent game scene is null");

	m_IsActive = active;
	pScene->SetActiveCamera(active?this:nullptr); //Switch to default camera if active==false
}

GameObject* CameraComponent::Pick(CollisionGroup ignoreGroups) const
{
	TODO_W7(L"Implement Picking Logic");

	const POINT mousePos = InputManager::GetMousePosition();
	const float halfWidth = this->GetScene()->GetSceneContext().windowWidth / 2;
	const float halfHeight = this->GetScene()->GetSceneContext().windowHeight / 2;
	const float xMouseNDC = (mousePos.x - halfWidth) / halfWidth;
	const float yMouseNDC = (halfHeight - mousePos.y) / halfHeight;

	const XMFLOAT3 mousePosNear = { xMouseNDC, yMouseNDC, 0 };
	const XMFLOAT3 mousePosFar = { xMouseNDC, yMouseNDC, 1 };

	const auto& inverseViewProj{ XMLoadFloat4x4(&m_ViewProjectionInverse) };

	const XMVECTOR nearPointVector{ XMVector3TransformCoord(XMVECTOR{ xMouseNDC, yMouseNDC, 0.0f }, inverseViewProj) };
	const XMVECTOR farPointVector{ XMVector3TransformCoord(XMVECTOR{ xMouseNDC, yMouseNDC, 1.0f }, inverseViewProj) };
	XMFLOAT3 nearPoint{};
	XMFLOAT3 farPoint{};
	XMStoreFloat3(&nearPoint, nearPointVector);
	XMStoreFloat3(&farPoint, farPointVector);

	const PxVec3 rayOrigin{ nearPoint.x, nearPoint.y, nearPoint.z };
	const PxVec3 rayDirection{ farPoint.x - nearPoint.x, farPoint.y - nearPoint.y, farPoint.z - nearPoint.z };
	

	PxQueryFilterData filterData{};
	filterData.data.word0 = ~UINT(ignoreGroups);

	PxRaycastBuffer hit{};
	if (this->GetScene()->GetPhysxProxy()->Raycast(rayOrigin, rayDirection.getNormalized(), PX_MAX_F32,
		hit, PxHitFlag::eDEFAULT, filterData))
	{
		BaseComponent* pComponent{ static_cast<BaseComponent*>(hit.block.actor->userData) };
		return pComponent->GetGameObject();
	}

	return nullptr;
}
