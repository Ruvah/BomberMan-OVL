//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "ModelAnimator.h"
#include "../Diagnostics/Logger.h"


ModelAnimator::ModelAnimator(MeshFilter* pMeshFilter):
m_pMeshFilter(pMeshFilter),
m_Transforms(vector<XMFLOAT4X4>()),
m_IsPlaying(false), 
m_Reversed(false),
m_ClipSet(false),
m_TickCount(0),
m_AnimationSpeed(1.0f)
{
	SetAnimation(0);
}


ModelAnimator::~ModelAnimator()
{
}

void ModelAnimator::SetAnimation(UINT clipNumber)
{
	//Set m_ClipSet to false
	m_ClipSet = false;
	//Check if clipNumber is smaller than the actual m_AnimationClips vector size
	//If not,
	//	Call Reset
	//	Log a warning with an appropriate message
	//	return
	if(clipNumber >= m_pMeshFilter->m_AnimationClips.size())
	{
		Reset();
		Logger::LogWarning(L"ModelAnimatior: > Clipnumber >= AnimationClips!");
		return;
	}
	//else
	//	Retrieve the AnimationClip from the m_AnimationClips vector based on the given clipNumber
	//	Call SetAnimation(AnimationClip clip)
	else
		SetAnimation(m_pMeshFilter->m_AnimationClips[clipNumber]);
}

void ModelAnimator::SetAnimation(wstring clipName)
{
	//Set m_ClipSet to false
	m_ClipSet = false;
	//Iterate the m_AnimationClips vector and search for an AnimationClip with the given name (clipName)
	for (size_t i = 0; i < m_pMeshFilter->m_AnimationClips.size(); i++)
	{
		if(m_pMeshFilter->m_AnimationClips[i].Name == clipName)
		{
			SetAnimation(m_pMeshFilter->m_AnimationClips[i]);
			return;
		}
	}
	Reset();
	Logger::LogFormat(LogLevel::Warning, L"ModelAnimator: > %s clip not found!", clipName);
	//If found,
	//	Call SetAnimation(Animation Clip) with the found clip
	//Else
	//	Call Reset
	//	Log a warning with an appropriate message
}

void ModelAnimator::SetAnimation(AnimationClip clip)
{
	//Set m_ClipSet to true
	m_ClipSet = true;
	//Set m_CurrentClip
	m_CurrentClip = clip;
	//Call Reset(false)
	Reset();
}

void ModelAnimator::Reset(bool pause)
{
	//If pause is true, set m_IsPlaying to false
	if (pause)
	{
		m_IsPlaying = false;
		//Set m_TickCount to zero
		m_TickCount = 0.0f;
		//Set m_AnimationSpeed to 1.0f
		m_AnimationSpeed = 1.0f;
	}
	//If m_ClipSet is true
	if (m_ClipSet == true)
	{
		//	Retrieve the BoneTransform from the first Key from the current clip (m_CurrentClip)
		vector<XMFLOAT4X4> transforms = m_CurrentClip.Keys[0].BoneTransforms;
		//	Refill the m_Transforms vector with the new BoneTransforms (have a look at vector::assign)
		m_Transforms.assign(transforms.begin(), transforms.end());
	}
	//Else
	else
	{
		//	Create an IdentityMatrix
		XMMATRIX id = XMMatrixIdentity();
		XMFLOAT4X4 identity;
		XMStoreFloat4x4(&identity, id);
		//	Refill the m_Transforms vector with this IdenityMatrix (Amount = BoneCount) (have a look at vector::assign)
		m_Transforms.assign(m_Transforms.size(), identity);
	}
}

void ModelAnimator::Update(const GameContext& gameContext)
{
	//We only update the transforms if the animation is running and the clip is set
	if (m_IsPlaying && m_ClipSet)
	{
		//1. 
		//Calculate the passedTicks (see the lab document)
		auto passedTicks = gameContext.pGameTime->GetElapsed() * m_CurrentClip.TicksPerSecond * m_AnimationSpeed;
		//Make sure that the passedTicks stay between the m_CurrentClip.Duration bounds (fmod)
		passedTicks = fmod(passedTicks, m_CurrentClip.Duration);

		//2. 
		//IF m_Reversed is true
		//	Subtract passedTicks from m_TickCount
		if (m_Reversed)
		{
			m_TickCount -= passedTicks;
			//	If m_TickCount is smaller than zero, add m_CurrentClip.Duration to m_TickCount
			if (m_TickCount < 0.0f)
				m_TickCount += m_CurrentClip.Duration;
		}
		//ELSE
		//	Add passedTicks to m_TickCount
		//	if m_TickCount is bigger than the clip duration, subtract the duration from m_TickCount

		else
		{
			m_TickCount += passedTicks;
			if (m_TickCount > m_CurrentClip.Duration)
				m_TickCount -= m_CurrentClip.Duration;
		}
		
		//3.
		//Find the enclosing keys
		AnimationKey keyA, keyB;
		//Iterate all the keys of the clip and find the following keys:
		
		//keyA > Closest Key with Tick before/smaller than m_TickCount
		//keyB > Closest Key with Tick after/bigger than m_TickCount
		for (size_t i = 0; i < m_CurrentClip.Keys.size(); i++)
		{
			if(m_TickCount > m_CurrentClip.Keys[i].Tick)
			{
				keyA = m_CurrentClip.Keys[i];
				keyB = m_CurrentClip.Keys[(i + 1) % m_CurrentClip.Keys.size()];
			}
		}

		//4.
		//Interpolate between keys
		//Figure out the BlendFactor (See lab document)
		float length = m_CurrentClip.Duration / m_CurrentClip.Keys.size();
		float blendA = (keyB.Tick - m_TickCount) / length;
		if (blendA < 0)
			blendA = m_TickCount;

		//Clear the m_Transforms vector
		m_Transforms.clear();
		//FOR every boneTransform in a key (So for every bone)
		for (size_t i = 0; i < keyA.BoneTransforms.size(); i++)
		{
			auto transformA = keyA.BoneTransforms[i];
			auto transformB = keyB.BoneTransforms[i];
			//	Retrieve the transform from keyA (transformA)
			XMMATRIX matA = XMLoadFloat4x4(&transformA);
			// 	Retrieve the transform from keyB (transformB)
			XMMATRIX matB = XMLoadFloat4x4(&transformB);
			XMVECTOR scaleA, rotQuatA, transA,
				scaleB, rotQuatB, transB;
			//	Decompose both transforms
			XMMatrixDecompose(&scaleA, &rotQuatA, &transA, matA);
			XMMatrixDecompose(&scaleB, &rotQuatB, &transB, matB);

			XMVECTOR translation, rotation, scale;
			//	Lerp between all the transformations (Position, Scale, Rotation)
			translation = XMVectorLerp(transB, transA, blendA);
			scale = XMVectorLerp(scaleB, scaleA, blendA);
			rotation = XMQuaternionSlerp(rotQuatB, rotQuatA, blendA);
		
			//	Compose a transformation matrix with the lerp-results
			XMMATRIX transformation = XMMatrixScalingFromVector(scale) *
				XMMatrixRotationQuaternion(rotation) *
				XMMatrixTranslationFromVector(translation);

			XMFLOAT4X4 t;
			XMStoreFloat4x4(&t, transformation);
			//	Add the resulting matrix to the m_Transforms vector
			m_Transforms.push_back(t);
		}
		
	}
}
