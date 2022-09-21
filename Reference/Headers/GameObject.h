#pragma once

#include "Transform.h"

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
public:
	typedef struct Wrap {
	CTransform* pTransform;
	CGameObject* pTarget;
	_float3 fColor;
}WRAP;

protected:
	explicit CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

public:
	class CComponent* Get_Component(const _tchar* pComponentTag) {
		return Find_Component(pComponentTag);
	}

	CTransform* Get_Transform() {
		return m_pTransformCom;
	}

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, CTransform::TRANSFORMDESC* pTransformDesc = nullptr);
	virtual _int Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render();

public:
	_bool SkyMoveOnNavi();

protected:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;

protected:
	static const _tchar*	m_pTransformTag;
	class CTransform*		m_pTransformCom = nullptr;

protected:
	map<const _tchar*, class CComponent*>			m_Components;
	typedef map<const _tchar*, class CComponent*>	COMPONENTS;

protected:
	HRESULT SetUp_Components(const _tchar* pComponentTag, _uint iLevelIndex, const _tchar* pPrototypeTag, CComponent** ppOut, void* pArg = nullptr);

private:
	CComponent* Find_Component(const _tchar* pComponentTag);

public:
	_bool Get_isColideBoxPicking();
	_bool Get_isCollideSphere(_float3 RayPos, _float3 RayDir);
	const _bool& Get_isCulling() {return m_Culling; }
	void Set_Dead(_bool flag) { m_Dead = flag; }
	void Set_isCollison(_bool flag) { m_isColllison = flag; }
	_bool Get_isCollison() { return m_isColllison; }
	_bool Get_isDead() { return m_Dead; }
	void Set_CameraDistance();
	_float Get_CamearaDistance() { return m_CameraDistance; }
	void ZBillBoard();
	void LookAtCameara();
	HRESULT Search_MyNaviIndex();
	void Create_ExpBall();
	void Create_Smoke(_fvector vPos);
	_bool Collide_Object(const _tchar* LayerTag); //Sphere Shpere

	_uint Get_LayerSize(const _tchar* pLayerTag);
	void Set_LayerIndex(const _tchar* pLayerTag);
	
public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;

protected:
	_uint m_iLevelIndex = 0;
	_bool m_Dead = false;
	_bool m_Culling = false;
	_bool m_IsPicking = false;
	_uint m_iCurrentCellIndex = 0;
	_bool m_isColllison = false;
	_float m_CameraDistance = 0;

	_uint m_LayerIndex = 0;

	_float m_Speed = 1.f;
};

END