#pragma once

#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CBullet abstract : public CGameObject
{
public:
	typedef struct BulletDesc{
		_vector vLook;
		_vector vPos;
		_float Damage;
		_float CriticalPer;
		_float Speed = 1.f;
		_float Size = 1.f;
}BULLETDESC;

protected:
	explicit CBullet(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	explicit CBullet(const CBullet& rhs);
	virtual ~CBullet() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* pArg, CTransform::TRANSFORMDESC* pTransformDesc = nullptr) override;
	virtual _int Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

public:
	_bool Collide_Object(const _tchar* LayerTag);
	_bool AABBCollide_Object(const _tchar* LayerTag);

protected:
	_vector m_vDir;
	_float m_Damage;
	_float m_CriticalPer;
	_float m_Size = 0.f;
	
	_int m_BoundCnt = 0;  //0 상태에서 충돌시 소멸
	_bool m_Penetration = false;
	_vector m_PrevPos = _vector();

	list<CGameObject*> m_isCollisonlist;  //해당 리스트에 들어오면 이미 맞은애들

public:	
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END