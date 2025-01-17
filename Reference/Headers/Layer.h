#pragma once

#include "Base.h"

BEGIN(Engine)

class CLayer final : public CBase
{
public:
	CLayer();
	virtual ~CLayer() = default;

public:
	class CComponent* Get_Component(const _tchar* pComponentTag, _uint iIndex = 0);
	class CTransform* Get_Transform(_uint iIndex = 0);

public:
	HRESULT Add_GameObject(class CGameObject* pGameObject);
	CGameObject* Add_GameObj(class CGameObject* pGameObject);
	void Tick(_double TimeDelta);
	void LateTick(_double TimeDelta);

public:
	list<class CGameObject*>* Get_ObjectList() { return &m_ObjectList; }
	CGameObject* Get_PickingObject();
	CGameObject* Get_CrossHair_Near_Object(_float3 RayPos, _float3 RayDir);
	_uint Get_LayerIndex(CGameObject* Object);
	_uint Get_LayerSize() { return m_ObjectList.size(); }
private:
	list<class CGameObject*>			m_ObjectList;
	typedef list<class CGameObject*>	OBJECTLIST;

public:
	static CLayer* Create();
	virtual void Free() override;
};

END