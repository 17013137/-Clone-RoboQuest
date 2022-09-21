#include "..\Public\Picking.h"
#include "GameInstance.h"
#include "PIpeLine.h"

IMPLEMENT_SINGLETON(CPicking)

CPicking::CPicking()
{
}

HRESULT CPicking::NativeConstruct(HWND hWnd, ID3D11Device* pGraphic_Device, ID3D11DeviceContext* pDeviceContext)
{
	if (0 == hWnd || 
		nullptr == pGraphic_Device)
		return E_FAIL;

	m_hWnd = hWnd;

	m_pDevice = pGraphic_Device;
	m_pDevice_Context = pDeviceContext;
	

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDevice_Context);

	return S_OK;
}

void CPicking::Update_ToTransform()
{
	CPipeLine* pPipeLine = GET_INSTANCE(CPipeLine);

	if (nullptr == m_pDevice || nullptr == m_pDevice_Context)
		return;

	POINT		ptMouse;

	GetCursorPos(&ptMouse);

	/* 뷰 포트 영역상의 마우스 좌표를 구했다. */
	ScreenToClient(m_hWnd, &ptMouse);

	D3D11_VIEWPORT Viewport;
	_uint iIndex = 1;

	m_pDevice_Context->RSGetViewports(&iIndex, &Viewport);

	/* 투영스페이스 상의 마우스 좌표를 구한다. */
	_vector vMousePos = { ptMouse.x / (1280.f / 2.f) - 1 , ptMouse.y / (720.f/ -2.f) + 1 , 0.f, 1.f };
	_vector vCenterPos = { 640.f / (1280.f / 2.f) - 1 , 360.f / (720.f / -2.f) + 1 , 0.f, 1.f };

	vMousePos = XMVector3TransformCoord(vMousePos, XMMatrixInverse(nullptr, pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_PROJ)));
	vCenterPos = XMVector3TransformCoord(vCenterPos, XMMatrixInverse(nullptr, pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_PROJ)));
	XMStoreFloat3(&m_vCenterRay, vCenterPos);
	XMStoreFloat3(&m_vRay, vMousePos);
	XMStoreFloat3(&m_vRayPos, _vector());

	
	XMStoreFloat3(&m_vRayPos, XMVector3TransformCoord(XMLoadFloat3(&m_vRayPos), XMMatrixInverse(nullptr, pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))));
	XMStoreFloat3(&m_vRay, XMVector3TransformNormal(XMLoadFloat3(&m_vRay), XMMatrixInverse(nullptr, pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))));
	XMStoreFloat3(&m_vRay, XMVector3Normalize(XMLoadFloat3(&m_vRay)));

	XMStoreFloat3(&m_vCenterRay, XMVector3TransformNormal(XMLoadFloat3(&m_vCenterRay), XMMatrixInverse(nullptr, pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))));
	XMStoreFloat3(&m_vCenterRay, XMVector3Normalize(XMLoadFloat3(&m_vCenterRay)));
}

//_bool CPicking::Picking_OnLocalSpace(CCollider* pCollider)
//{
//	_float		fU, fV, fDist;
//	
//	if (TRUE == D3DXIntersectTri(&pLocalPoints[0], &pLocalPoints[1], &pLocalPoints[2], &m_vLocalRayPos, &m_vLocalRay,  &fU, &fV, &fDist))
//	{
//		*pPickingPointOut = m_vLocalRayPos + m_vLocalRay * fDist;
//	
//		return true;
//	}
//	return false;
//}

void CPicking::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pDevice_Context);
}
