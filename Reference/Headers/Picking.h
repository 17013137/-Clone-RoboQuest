#pragma once

#include "Base.h"

BEGIN(Engine)

class CPicking final : public CBase
{
	DECLARE_SINGLETON(CPicking)
public:
	CPicking();
	virtual ~CPicking() = default;

public:
	HRESULT NativeConstruct(HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);

	/* 매 프레임마다 호출을 하여, 월드 스페이스 상의 m_vRay, m_vRayPos 구해놓는다. */
	void Update_ToTransform();

	const _float3& Get_WorldRayPos() { return m_vRayPos; }
	const _float3& Get_WorldRay() { return m_vRay; }
	const _float3& Get_WorldCenterRay() { return m_vCenterRay; }

	//_bool Picking_OnLocalSpace(_fvector* pLocalPoints, _fvector* pPickingPointOut);

private:
	HWND				m_hWnd;
	ID3D11Device*		m_pDevice = nullptr;
	ID3D11DeviceContext* m_pDevice_Context = nullptr;
private:
	_float3 m_vRay;
	_float3 m_vRayPos;
	_float3 m_vCenterRay;


public:
	virtual void Free() override;
};

END