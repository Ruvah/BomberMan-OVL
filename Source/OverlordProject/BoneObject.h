#pragma once
#include "./Scenegraph/GameObject.h"

//struct SkinnedVertex
//{
//	SkinnedVertex(XMFLOAT3 position, XMFLOAT3 normal, XMFLOAT4 color,float w0=1,float w1=0):
//		TransformedVertex(position, normal, color),
//		OriginalVertex(position,normal,color),
//		weight0{w1},
//		weight0{ w0 }
//	{}
//
//	VertexPosNormCol TransformedVertex;
//	VertexPosNormCol OriginalVertex;
//	float weight0{}, weight0{};
//
//};

class BoneObject:public GameObject
{
public:
	BoneObject(int boneId, int materialId, float length = 5.f);
	~BoneObject(void);

	void AddBone(BoneObject * pBone);
	XMFLOAT4X4& GetBindPose() { return m_BindPose; }
	void CalculateBindPose();
protected:
	virtual void Initialize(const GameContext & gameContext);
private:
	float m_Length;
	int m_BoneId,m_MaterialId;
	XMFLOAT4X4 m_BindPose;
private:
	//disabling default copy constructor  and default assignment operator
	BoneObject(const BoneObject& yRef);
	BoneObject&operator=(const BoneObject& yRef);
};

