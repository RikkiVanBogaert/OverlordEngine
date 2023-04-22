#include "stdafx.h"
#include "ModelAnimator.h"
#include <DirectXMath.h>

ModelAnimator::ModelAnimator(MeshFilter* pMeshFilter):
	m_pMeshFilter{pMeshFilter}
{
	SetAnimation(0);
}

void ModelAnimator::Update(const SceneContext& sceneContext)
{
	TODO_W6_();

	//We only update the transforms if the animation is running and the clip is set
	if (m_IsPlaying && m_ClipSet)
	{
		//1. 
		//Calculate the passedTicks (see the lab document)
		//auto passedTicks = ...
		auto passedTicks = sceneContext.pGameTime->GetElapsed() * m_CurrentClip.ticksPerSecond * m_AnimationSpeed;
		//Make sure that the passedTicks stay between the m_CurrentClip.Duration bounds (fmod)
		if (passedTicks > m_CurrentClip.duration) 
			passedTicks = fmodf(passedTicks, m_CurrentClip.duration);

		//2. 
		//IF m_Reversed is true
		//	Subtract passedTicks from m_TickCount
		//	If m_TickCount is smaller than zero, add m_CurrentClip.Duration to m_TickCount
		//ELSE
		//	Add passedTicks to m_TickCount
		//	if m_TickCount is bigger than the clip duration, subtract the duration from m_TickCount
		if (m_Reversed)
		{
			m_TickCount -= passedTicks;
			if (m_TickCount < 0) 
				m_TickCount += m_CurrentClip.duration;
		}
		else
		{
			m_TickCount += passedTicks;
			if (m_TickCount > m_CurrentClip.duration) 
				m_TickCount -= m_CurrentClip.duration;
		}

		//3.
		//Find the enclosing keys
		AnimationKey keyA, keyB;
		//Iterate all the keys of the clip and find the following keys:
		//keyA > Closest Key with Tick before/smaller than m_TickCount
		//keyB > Closest Key with Tick after/bigger than m_TickCount
		for (int i{}; i < m_CurrentClip.keys.size(); ++i)
		{

			if (m_CurrentClip.keys[i].tick > m_TickCount)
			{
				keyB = m_CurrentClip.keys[i];
				keyA = m_CurrentClip.keys[i - 1];
				break;
			}
		}

		//4.
		//Interpolate between keys
		//Figure out the BlendFactor (See lab document)
		//Clear the m_Transforms vector
		//FOR every boneTransform in a key (So for every bone)
		//	Retrieve the transform from keyA (transformA)
		//	auto transformA = ...
		// 	Retrieve the transform from keyB (transformB)
		//	auto transformB = ...
		//	Decompose both transforms
		//	Lerp between all the transformations (Position, Scale, Rotation)
		//	Compose a transformation matrix with the lerp-results
		//	Add the resulting matrix to the m_Transforms vector
		const float blendFactor{ 1 - (keyB.tick - m_TickCount) / (m_CurrentClip.duration / (m_CurrentClip.keys.size() - 1)) };

		for (int i{}; i < m_pMeshFilter->m_BoneCount; ++i)
		{
			const auto& transformA{ keyA.boneTransforms[i] };
			const auto& transformB{ keyB.boneTransforms[i] };

			XMVECTOR scaleA;
			XMVECTOR rotA;
			XMVECTOR transA;
			XMMatrixDecompose(&scaleA, &rotA, &transA, XMLoadFloat4x4(&transformA));

			XMVECTOR scaleB;
			XMVECTOR rotB;
			XMVECTOR transB;
			XMMatrixDecompose(&scaleB, &rotB, &transB, XMLoadFloat4x4(&transformB));

			const auto scaleLerp{ XMVectorLerp(scaleA, scaleB, blendFactor) };
			const auto rotSlerp{ XMQuaternionSlerp(rotA, rotB, blendFactor) };
			const auto translationLerp{ XMVectorLerp(transA, transB, blendFactor) };

			const XMMATRIX transform{ XMMatrixScalingFromVector(scaleLerp) * XMMatrixRotationQuaternion(rotSlerp) * XMMatrixTranslationFromVector(translationLerp) };
			
			XMStoreFloat4x4(&m_Transforms[i], transform);
		}
	}
}

void ModelAnimator::SetAnimation(const std::wstring& clipName)
{
	TODO_W6_()
		//Set m_ClipSet to false
		//Iterate the m_AnimationClips vector and search for an AnimationClip with the given name (clipName)
		//If found,
		//	Call SetAnimation(Animation Clip) with the found clip
		//Else
		//	Call Reset
		//	Log a warning with an appropriate message

	m_ClipSet = false;
		
	for (auto& animationClip : m_pMeshFilter->GetAnimationClips())
	{
		if (animationClip.name == clipName)
		{
			SetAnimation(animationClip);
			return;
		}
	}

	Reset();
}

void ModelAnimator::SetAnimation(UINT clipNumber)
{
	TODO_W6_()
	//Set m_ClipSet to false
	//Check if clipNumber is smaller than the actual m_AnimationClips vector size
	//If not,
		//	Call Reset
		//	Log a warning with an appropriate message
		//	return
	//else
		//	Retrieve the AnimationClip from the m_AnimationClips vector based on the given clipNumber
		//	Call SetAnimation(AnimationClip clip)

	m_ClipSet = false;

	const auto animatioClips{ m_pMeshFilter->GetAnimationClips() };
	if (clipNumber < animatioClips.size() && clipNumber >= 0)
	{
		SetAnimation(animatioClips[clipNumber]);
		return;
	}

	Reset();
}

void ModelAnimator::SetAnimation(const AnimationClip& clip)
{
	TODO_W6_()
	//Set m_ClipSet to true
	//Set m_CurrentClip

	//Call Reset(false)

	m_ClipSet = true;
	m_CurrentClip = clip;
	Reset(false);
}

void ModelAnimator::Reset(bool pause)
{
	TODO_W6_()
	//If pause is true, set m_IsPlaying to false
	if (pause) 
		m_IsPlaying = false;

	//Set m_TickCount to zero
	//Set m_AnimationSpeed to 1.0f
	m_TickCount = 0;
	m_AnimationSpeed = 1.0f;

	//If m_ClipSet is true
	//	Retrieve the BoneTransform from the first Key from the current clip (m_CurrentClip)
	//	Refill the m_Transforms vector with the new BoneTransforms (have a look at vector::assign)
	//Else
	//	Create an IdentityMatrix 
	//	Refill the m_Transforms vector with this IdenityMatrix (Amount = BoneCount) (have a look at vector::assign)

	if (m_ClipSet)
	{
		m_Transforms.assign(m_CurrentClip.keys[0].boneTransforms.begin(), m_CurrentClip.keys[0].boneTransforms.end());
	}
	else
	{
		XMFLOAT4X4 identityMatrix{};
		XMStoreFloat4x4(&identityMatrix, XMMatrixIdentity());
		m_Transforms.assign(m_pMeshFilter->m_BoneCount, identityMatrix);
	}
}
