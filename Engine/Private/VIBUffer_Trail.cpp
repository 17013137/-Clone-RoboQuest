#include "..\Public\VIBuffer_Trail.h"

CVIBuffer_Trail::CVIBuffer_Trail(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer(pDevice, pDeviceContext)
{
}

CVIBuffer_Trail::CVIBuffer_Trail(const CVIBuffer_Trail & rhs)
	: CVIBuffer(rhs)
	, m_VBTrailSubResourceData(rhs.m_VBTrailSubResourceData)
	, m_VBTrailDesc(rhs.m_VBTrailDesc)
	, m_MaxVtx(rhs.m_MaxVtx)
{
	m_pDevice->CreateBuffer(&rhs.m_VBTrailDesc, &rhs.m_VBTrailSubResourceData, &m_pVBTrail);
}

HRESULT CVIBuffer_Trail::NativeConstruct_Prototype(_uint VtxCnt)
{
	m_MaxVtx = VtxCnt;

#pragma region VERTEX_BUFFER
	m_iStride = sizeof(VTXTEX);
	m_iNumVertices = m_MaxVtx;
	m_iNumVBuffers = 1;

#pragma endregion

	ZeroMemory(&m_VBTrailDesc, sizeof(D3D11_BUFFER_DESC));
	m_VBTrailDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_VBTrailDesc.StructureByteStride = m_iStride;
	m_VBTrailDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_VBTrailDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBTrailDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_VBTrailDesc.MiscFlags = 0;

	VTXTEX*		pTrailVertices = new VTXTEX[m_iNumVertices];

	for (_uint i = 0; i < m_iNumVertices; i++) {
		pTrailVertices[i].vPosition = _float3(1.f + i, 1.f + i, 1.f + i);
		pTrailVertices[i].vTexUV = _float2(0, 1);
	}

	ZeroMemory(&m_VBTrailSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_VBTrailSubResourceData.pSysMem = pTrailVertices;


	if (FAILED(m_pDevice->CreateBuffer(&m_VBTrailDesc, &m_VBTrailSubResourceData, &m_pVBTrail)))
		return E_FAIL;

	Safe_Delete_Array(pTrailVertices);
#pragma endregion


#pragma region INDEX_BUFFER

	m_iPrimitiveIndicesSize = sizeof(FACELISTINDICES16);
	m_iNumPrimitive = m_MaxVtx - 2;
	m_iNumIndicesPerPrimitive = 3;
	m_eFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;


	ZeroMemory(&m_IBDesc, sizeof(D3D11_BUFFER_DESC));
	m_IBDesc.ByteWidth = m_iPrimitiveIndicesSize * m_iNumPrimitive;
	m_IBDesc.StructureByteStride = 0;
	m_IBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_IBDesc.CPUAccessFlags = 0;
	m_IBDesc.MiscFlags = 0;

	FACELISTINDICES16*		pIndices = new FACELISTINDICES16[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(FACELISTINDICES16) * m_iNumPrimitive);

	for (_uint i = 0; i < m_iNumPrimitive; i += 2) {
		pIndices[i]._0 = i + 3;
		pIndices[i]._1 = i + 1;
		pIndices[i]._2 = i;

		pIndices[i + 1]._0 = i + 2;
		pIndices[i + 1]._1 = i + 3;
		pIndices[i + 1]._2 = i;
	}


	ZeroMemory(&m_IBSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_IBSubResourceData.pSysMem = pIndices;

	if (FAILED(Create_IndexBuffer()))
		return E_FAIL;
#pragma endregion

	Safe_Delete_Array(pIndices);

	return S_OK;
}

HRESULT CVIBuffer_Trail::NativeConstruct(void* pArg)
{
	m_flag = false;
	m_RenderOn = false;
	_uint m_VtxCount = 0;
	return S_OK;
}

void CVIBuffer_Trail::Set_TrailPoint(_fvector Point1, _fvector Point2)
{
	m_TrailPoint[0] = Point1;
	m_TrailPoint[1] = Point2;
}

HRESULT CVIBuffer_Trail::Update(_double TimeDelta, _fmatrix vWorldMatrix)
{
	if (m_flag == true)
		m_RenderOn = true;

	if (nullptr == m_pDeviceContext)
		return E_FAIL;

	if (m_RenderOn == true) {
		m_FrameCnt++;
		if (m_FrameCnt < m_MaxFrame) {
			return S_OK;
		}
		else
			m_FrameCnt = 0;
	}

	_vector vPosition = XMVector3TransformCoord(m_TrailPoint[0], vWorldMatrix);
	_vector vPosition2 = XMVector3TransformCoord(m_TrailPoint[1], vWorldMatrix);

	m_Distance = XMVectorGetX(XMVector3Length(m_vWorldPos-m_vPrevPos));

	//if (m_Distance >= 10.f) {
	//	m_RenderOn = false;
	//	return S_OK;
	//}
	
	D3D11_MAPPED_SUBRESOURCE			SubResource;

	m_pDeviceContext->Map(m_pVBTrail, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	XMStoreFloat3(&((VTXTEX*)SubResource.pData)[m_VtxCount].vPosition, vPosition);
	XMStoreFloat3(&((VTXTEX*)SubResource.pData)[m_VtxCount+1].vPosition, vPosition2);

	m_VtxCount += 2;

	// _float2((_float)i / ((_float)m_VtxCount - 2), 1.f);

	

	for (_uint i = 0; i < m_VtxCount - 1; i++) {
		((VTXTEX*)SubResource.pData)[i].vTexUV = _float2((_float)i / ((_float)m_VtxCount - 2), 1.f);
		((VTXTEX*)SubResource.pData)[i+1].vTexUV = _float2((_float)i / ((_float)m_VtxCount - 2), 0.f);
	}

	if (m_VtxCount >= m_MaxVtx) {
		m_flag = true;
		_uint iRemoveCount = m_VtxCount / 10;
		m_VtxCount -= iRemoveCount;
		for (_uint i = 0; i < m_VtxCount; i += 2) {
			((VTXTEX*)SubResource.pData)[i].vPosition = ((VTXTEX*)SubResource.pData)[iRemoveCount + i].vPosition;
			((VTXTEX*)SubResource.pData)[i+1].vPosition = ((VTXTEX*)SubResource.pData)[iRemoveCount + i + 1].vPosition;
		}
	}

	m_pDeviceContext->Unmap(m_pVBTrail, 0);


	return S_OK;
}

HRESULT CVIBuffer_Trail::Render()
{
	if (nullptr == m_pDeviceContext)
		return E_FAIL;

	if (m_RenderOn == false)
		return S_OK;

	ID3D11Buffer*	pVertexBuffers[] = {
		m_pVBTrail
	};

	_uint			iStrides[] = {
		m_iStride,
	};

	_uint			iOffset[] = {
		0
	};

	m_pDeviceContext->IASetVertexBuffers(0, m_iNumVBuffers, pVertexBuffers, iStrides, iOffset);
	m_pDeviceContext->IASetIndexBuffer(m_pIB, m_eFormat, 0);
	m_pDeviceContext->IASetPrimitiveTopology(m_eTopology);

	m_pDeviceContext->DrawIndexed(m_iNumIndicesPerPrimitive * m_iNumPrimitive, 0, 0);

	return S_OK;
}

CVIBuffer_Trail* CVIBuffer_Trail::Create(_uint VtxCnt, ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CVIBuffer_Trail*	pInstance = new CVIBuffer_Trail(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(VtxCnt)))
	{
		MSG_BOX(TEXT("Failed to Created CVIBuffer_Trail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_Trail::Clone(void * pArg)
{
	CVIBuffer_Trail*	pInstance = new CVIBuffer_Trail(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created CVIBuffer_Trail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Trail::Free()
{
	__super::Free();

	Safe_Release(m_pVBTrail);

}
