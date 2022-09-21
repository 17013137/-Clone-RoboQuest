#pragma once

#include "Client_Defines.h"
#include "Camera.h"
#include "Event_Manager.h"

BEGIN(Client)

class CCamera_First final : public CCamera
{
private:
	explicit CCamera_First(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	explicit CCamera_First(const CCamera_First& rhs);
	virtual ~CCamera_First() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	void Shaking(_double TimeDelta);
	void Boss_Tower_Event(_double TimeDelta);
	void Boss_Disco_Event(_double TimeDelta);

public:
	static CCamera_First* Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

private:
	CEvent_Manager* m_EventMgr = CEvent_Manager::Get_Instance();
	_bool flag = false;
	_bool m_isShaking = false;
	_int m_SceneType = 0;
};

END