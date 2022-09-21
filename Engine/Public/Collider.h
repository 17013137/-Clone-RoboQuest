#pragma once

#include "Component.h"
#include "DebugDraw.h"

BEGIN(Engine)

class ENGINE_DLL CCollider final : public CComponent
{
public:
	enum TYPE { TYPE_AABB, TYPE_OBB, TYPE_SPHERE, TYPEWEAPON_END };

public:
	typedef struct tagColliderDesc
	{
		_float3			vPosition = _float3();
		_float3			vSize  = _float3();
		_float			fRadius = 0.f;
		_float3			vWorldPosition = _float3();
	}COLLIDERDESC;

	typedef struct tagObbDesc
	{
		_float3			vCenter;
		_float3			vAlignAxis[3];
		_float3			vCenterAxis[3];
	}OBBDESC;


public:
	CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CCollider(const CCollider& rhs);
	virtual ~CCollider() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(TYPE eType);
	virtual HRESULT NativeConstruct(void* pArg) override;

public:
	_bool Collsion_AABB(CCollider* pTargetCollider);
	_bool Collsion_OBB(CCollider* pTargetCollider);
	_bool Collision_Point(_fvector vPos1, _fvector vPos2, _fvector vPos3);
	_vector InterSectsRay(_float3 RayPos, _float3 RayDir);
	_bool InterSectSphere(const BoundingSphere& Src, _float* Dist = nullptr);
	_bool InterSectOBB(const BoundingOrientedBox& Src, _float* Dist = nullptr);
	_bool InterSectAABB(const BoundingBox& Src, _float* Dist = nullptr);

public:
	BoundingBox* Get_AABBWorld() { return m_pAABBInWorld; }
	BoundingOrientedBox* Get_OBBWorld() { return m_pOBBInWorld; }
	BoundingSphere* Get_SphereWorld() { return m_pSPHEREInWorld; }

public:
	void Set_LineColor(_vector vColor) { m_LineColor = vColor; }
	void Update(_fmatrix WorldMatrix);

public:
	_float3 Get_Position() { return m_ColliderDesc.vWorldPosition; }
	_float3 Get_Size() { return m_ColliderDesc.vSize; }
	_float Get_Radius() { return m_ColliderDesc.fRadius; }

#ifdef _DEBUG
public:
	HRESULT Render();
#endif // _DEBUG

private:
	TYPE						m_eType = TYPEWEAPON_END;
	_bool						m_isCollision = false;
	_vector						m_LineColor = XMLoadFloat4(&_float4(0.f, 1.f, 0.f, 1.f));
	COLLIDERDESC				m_ColliderDesc;

	BoundingBox*				m_pAABB = nullptr;
	BoundingBox*				m_pAABBInWorld = nullptr;
	BoundingOrientedBox*		m_pOBB = nullptr;
	BoundingOrientedBox*		m_pOBBInWorld = nullptr;
	BoundingSphere*				m_pSPHERE = nullptr;
	BoundingSphere*				m_pSPHEREInWorld = nullptr;
	
#ifdef _DEBUG
private:
	BasicEffect*								m_pBasicEffect = nullptr;
	PrimitiveBatch<VertexPositionColor>*		m_pBatch = nullptr;
	ID3D11InputLayout*							m_pInputLayout = nullptr;
#endif // _DEBUG

private:
	_matrix Remove_Rotation(_fmatrix TransformMatrix);
	_vector Compute_Min();
	_vector Compute_Max();
	OBBDESC Compute_OBB();

private:
	_bool m_isPicking = false;
	_bool m_isNavi;
public:
	static CCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, TYPE eType);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END