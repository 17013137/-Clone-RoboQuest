#include "..\Public\Cell.h"
#include "VIBuffer_Line.h"
#include "Shader.h"
#include "Frustum.h"

CCell::CCell(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

_bool CCell::isInCollider(_fvector Position, _float Radius)
{
	_float flag = XMVectorGetX(m_Plane) * XMVectorGetX(Position) + XMVectorGetY(m_Plane) * XMVectorGetY(Position) + XMVectorGetZ(m_Plane) * XMVectorGetZ(Position) + XMVectorGetW(m_Plane);

	if (flag <= 0)
		return true;

	return false;
}

HRESULT CCell::NativeConstruct(CELLDESC* CellDesc, _int iIndex)
{
	m_iIndex = iIndex;

	if (CellDesc == nullptr)
		return E_FAIL;

	memcpy(m_vPoint, CellDesc, sizeof(_float3) * POINT_END);
	m_TypeIndex = CellDesc->Index;

	for (int i = 0; i < 3; i++) {
		if (m_vPoint[i].y > m_MaxHegiht)
			m_MaxHegiht = m_vPoint[i].y;

		if (m_vPoint[i].y < m_MinHeight)
			m_MinHeight = m_vPoint[i].y;
	}

	_vector vPt[3];
	_vector vCellNormal;

	for (int i = 0; i < 3; i++) {
		vPt[i] = XMLoadFloat3(&m_vPoint[i]);
		m_vLine[i] = vPt[(i + 1)%3] - vPt[i];
	}

	vCellNormal = XMVector3Cross(vPt[1] - vPt[0], vPt[2] - vPt[1]);

	_vector Dot = XMVector3Dot(XMVectorSet(0.f, 1.f, 0.f, 0.f), vCellNormal);
	
	if (XMVectorGetX(Dot) <= 0.f) {
		_float3 temp = m_vPoint[1];
		m_vPoint[1] = m_vPoint[2];
		m_vPoint[2] = temp;
		_vector vtemp = vPt[1];
		vPt[1] = vPt[2];
		vPt[2] = vtemp;
	}
	m_Plane = XMPlaneFromPoints(vPt[0], vPt[1], vPt[2]);
	m_vNormal = XMVector3Cross(vPt[1] - vPt[0], vPt[2] - vPt[1]);

#ifdef _DEBUG
	_float3		vPoints[4] = {
		m_vPoint[0], 
		m_vPoint[1],
		m_vPoint[2],
		m_vPoint[0]
	};

	m_pDebugBuffer = CVIBuffer_Line::Create(m_pDevice, m_pDeviceContext, vPoints, 4, &_float4(1.f, 1.f, 1.f, 1.f));
	if (nullptr == m_pDebugBuffer)
		return E_FAIL;
#endif // _DEBUG

	return S_OK;
}

_bool CCell::isIn(_float3* vPosition, _fmatrix WorldMatrix, _int* pNeighborIndex, _fvector PlayerDir ,_bool isJump)
{
	_vector		vPointInWorld[POINT_END + 1];
	_float Height = 0.0f;
	_vector PrevPos = XMLoadFloat3(vPosition) - (PlayerDir);

	for (_uint i = 0; i < POINT_END; ++i)
		vPointInWorld[i] = XMVector3TransformCoord(XMLoadFloat3(&m_vPoint[i]), WorldMatrix);
	vPointInWorld[POINT_END] = vPointInWorld[0];

	_vector vPlane = XMPlaneFromPoints(vPointInWorld[0], vPointInWorld[1], vPointInWorld[2]);
	_float4 fPlane; XMStoreFloat4(&fPlane, vPlane);

	_vector		vLine[LINE_END];

	for (_uint i = 0; i < LINE_END; ++i)
	{
		vLine[i] = vPointInWorld[i + 1] - vPointInWorld[i];

		_vector vNormal = XMVector3Normalize(XMVectorSet(XMVectorGetZ(vLine[i]) * -1.f, 0.f, XMVectorGetX(vLine[i]), 0.f));
		_vector vDir = XMVector3Normalize(XMLoadFloat3(vPosition) - vPointInWorld[i]);

		if (0 < XMVectorGetX(XMVector3Dot(vNormal, vDir)))
		{
			if (m_iNeighbor[i] == -1) { 
				_float LineSpeed = XMVectorGetX(XMVector3Dot(XMVector3Normalize(vLine[i]), PlayerDir));

				_vector NearPoint = vPointInWorld[i] + XMVector3Normalize(vLine[i]) * XMVectorGetX(XMVector3Dot(XMVector3Normalize(vLine[i]), PrevPos - vPointInWorld[i]));

				_vector result = NearPoint + XMVector3Normalize(vLine[i]) * LineSpeed;

				if (LineSpeed > 0) {
					if (XMVectorGetX(XMVector3Length(vLine[i])) < XMVectorGetX(XMVector3Length(result - vPointInWorld[i]))) {
						*pNeighborIndex = m_iNeighbor[i];
						return false;
					}
				}
				else {
					if (XMVectorGetX(XMVector3Length(vLine[i])) < XMVectorGetX(XMVector3Length(result - vPointInWorld[(i + 1) % 3]))) {
						*pNeighborIndex = m_iNeighbor[i];
						return false;
					}
				}

				vPosition->x = XMVectorGetX(result);
				vPosition->y = XMVectorGetY(result);
				vPosition->z = XMVectorGetZ(result);

				return true;
			}

			*pNeighborIndex = m_iNeighbor[i];
			return false;
		}
	}
	//if (m_TypeIndex == 2)
	//	return false;

	if (isJump == true) {
		Height = (-XMVectorGetX(vPlane) * vPosition->x - XMVectorGetZ(vPlane) *  vPosition->z - XMVectorGetW(vPlane)) / XMVectorGetY(vPlane);
		if (Height + 2.3f > vPosition->y) {
			vPosition->y = Height + 2.3f;
		}

		return true;
	}

	Height = (-XMVectorGetX(vPlane) * vPosition->x - XMVectorGetZ(vPlane) *  vPosition->z - XMVectorGetW(vPlane)) / XMVectorGetY(vPlane);

	//if (m_TypeIndex == 1) {
	//	if (vPosition->y < m_MaxHegiht+2.3f){
	//		return false;
	//	}
	//}

	//if (Height+2.3f < vPosition->y) {
	//}
	//else
	vPosition->y = Height;

	return true;
}

_int CCell::isOn(_fvector vPosition, _fmatrix WorldMatrix, _int * pNeighborIndex)
{
	_vector vPos = vPosition;
	_vector	vPointInWorld[POINT_END + 1];


	for (_uint i = 0; i < POINT_END; ++i)
		vPointInWorld[i] = XMVector3TransformCoord(XMLoadFloat3(&m_vPoint[i]), WorldMatrix);
	vPointInWorld[POINT_END] = vPointInWorld[0];

	_vector vPlane = XMPlaneFromPoints(vPointInWorld[0], vPointInWorld[1], vPointInWorld[2]);
	_float4 fPlane; XMStoreFloat4(&fPlane, vPlane);

	for (_uint i = 0; i < POINT_END; ++i)
		vPointInWorld[i] = XMVectorSetY(vPointInWorld[i], 0.f);

	vPos = XMVectorSetY(vPos, 0.f);

	_vector		vLine[LINE_END];

	for (_uint i = 0; i < LINE_END; ++i)
	{
		vLine[i] = vPointInWorld[i + 1] - vPointInWorld[i];

		_vector vNormal = XMVector3Normalize(XMVectorSet(XMVectorGetZ(vLine[i]) * -1.f, 0.f, XMVectorGetX(vLine[i]), 0.f));
		_vector vDir = XMVector3Normalize(vPos - vPointInWorld[i]);

		if (0 < XMVectorGetX(XMVector3Dot(vNormal, vDir)))
		{
			*pNeighborIndex = m_iNeighbor[i];

			return 1;
		}
	}

	_float Height = (-XMVectorGetX(vPlane) * XMVectorGetX(vPos) - XMVectorGetZ(vPlane) *  XMVectorGetZ(vPos) - XMVectorGetW(vPlane)) / XMVectorGetY(vPlane);
	if (Height > XMVectorGetY(vPosition))
		return 2;

	return 0;
}

_bool CCell::Compare_Points(_vector vSour, _vector vDest)
{
	if (true == XMVector3Equal(vSour, XMLoadFloat3(&m_vPoint[POINT_A])))
	{
		if (true == XMVector3Equal(vDest, XMLoadFloat3(&m_vPoint[POINT_B])))
			return true;
		
		if (true == XMVector3Equal(vDest, XMLoadFloat3(&m_vPoint[POINT_C])))
			return true;
	}

	if (true == XMVector3Equal(vSour, XMLoadFloat3(&m_vPoint[POINT_B])))
	{
		if (true == XMVector3Equal(vDest, XMLoadFloat3(&m_vPoint[POINT_A])))
			return true;

		if (true == XMVector3Equal(vDest, XMLoadFloat3(&m_vPoint[POINT_C])))
			return true;
	}

	if (true == XMVector3Equal(vSour, XMLoadFloat3(&m_vPoint[POINT_C])))
	{
		if (true == XMVector3Equal(vDest, XMLoadFloat3(&m_vPoint[POINT_A])))
			return true;

		if (true == XMVector3Equal(vDest, XMLoadFloat3(&m_vPoint[POINT_B])))
			return true;
	}

	return false;	
}


#ifdef _DEBUG

HRESULT CCell::Render(CShader * pShader)
{
	_vector vAvg = (XMLoadFloat3(&m_vPoint[0]) + XMLoadFloat3(&m_vPoint[1]) + XMLoadFloat3(&m_vPoint[2])) / 3;

	if (CFrustum::GetInstance()->isIn_WorldSpace(vAvg, 5.f) == false)
		return S_OK;

	if (nullptr == m_pDebugBuffer)
		return E_FAIL;		

	pShader->Begin(0);

	m_pDebugBuffer->Render();
	
	return S_OK;
}

#endif // _DEBUG
CCell * CCell::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, CELLDESC* CellDesc, _int iIndex)
{
	CCell*	pInstance = new CCell(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct(CellDesc, iIndex)))
	{
		MSG_BOX(TEXT("Failed to Created CSky"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCell::Free()
{
	
#ifdef _DEBUG
	Safe_Release(m_pDebugBuffer);
#endif // _DEBUG

	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);

	
}
