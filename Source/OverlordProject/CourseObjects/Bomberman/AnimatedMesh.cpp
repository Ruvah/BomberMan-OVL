#include "stdafx.h"
#include "AnimatedMesh.h"
#include "../OverlordEngine/Components/ModelComponent.h"
#include "Graphics/ModelAnimator.h"
#include "Materials/SkinnedDiffuseMaterial.h"
#include "CourseObjects/Bomberman/Helpers.h"


AnimatedMesh::AnimatedMesh(const wstring& path): m_FilePath(path),
m_CurrentClip(0)
{

}


void AnimatedMesh::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);

	m_pModel = new ModelComponent(m_FilePath);
	AddComponent(m_pModel);
	
}

void AnimatedMesh::NextAnim()
{
	if (m_pModel->GetAnimator()->GetClipCount() - 1 > m_CurrentClip)
		m_pModel->GetAnimator()->SetAnimation(++m_CurrentClip);
}

void AnimatedMesh::PreviousAnim()
{
	if ( m_CurrentClip!=0)
		m_pModel->GetAnimator()->SetAnimation(--m_CurrentClip);
}

void AnimatedMesh::Play()
{
	m_pModel->GetAnimator()->Play();
}

void AnimatedMesh::Pause()
{
	m_pModel->GetAnimator()->Pause();

}

void AnimatedMesh::SetAnim(unsigned int anim)
{
	if(anim <m_pModel->GetAnimator()->GetClipCount() )
	m_CurrentClip = anim;
	m_pModel->GetAnimator()->SetAnimation(anim);
}

void AnimatedMesh::SetAnimationSpeed(float speedPercentage)
{
	m_pModel->GetAnimator()->SetAnimationSpeed(speedPercentage);
}
