#pragma once
#include <DirectXMath.h>
using namespace DirectX;

inline XMFLOAT3 operator+(const XMFLOAT3& lhs, const XMFLOAT3& rhs)
{
    XMVECTOR lhsVec = XMLoadFloat3(&lhs);
    XMVECTOR rhsVec = XMLoadFloat3(&rhs);
    lhsVec += rhsVec;
    XMFLOAT3 temp{};
    XMStoreFloat3(&temp, lhsVec);
    return temp;
}

inline XMFLOAT3 operator*(const XMFLOAT3& lhs, const float& rhs)
{
    XMVECTOR lhsVec = XMLoadFloat3(&lhs);
    lhsVec *= rhs;
    XMFLOAT3 temp{};
    XMStoreFloat3(&temp, lhsVec);
    return temp;
}
