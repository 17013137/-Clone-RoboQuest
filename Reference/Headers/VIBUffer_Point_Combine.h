#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Point_Combine final : public CVIBuffer
{
protected:
	CVIBuffer_Point_Combine(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CVIBuffer_Point_Combine(const CVIBuffer_Point_Combine& rhs);
	virtual ~CVIBuffer_Point_Combine() = default;
public:
	virtual HRESULT NativeConstruct_Prototype(_uint iNumInstance);
	virtual HRESULT NativeConstruct(void* pArg);

public:
	HRESULT Update(_double TimeDelta);

public:
	virtual HRESULT Render() override;

private:
	ID3D11Buffer*			m_pVBInstance = nullptr;
	D3D11_BUFFER_DESC		m_VBInstDesc;
	D3D11_SUBRESOURCE_DATA	m_VBInstSubResourceData;
	_uint					m_iInstanceStride = 0;
	_uint					m_iNumInstance = 0;

private:
	_float*					m_pInstanceSpeed = nullptr;
public:
	static CVIBuffer_Point_Combine* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iNumInstance = 1);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END

