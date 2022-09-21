#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Trail final : public CVIBuffer
{
protected:
	CVIBuffer_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CVIBuffer_Trail(const CVIBuffer_Trail& rhs);
	virtual ~CVIBuffer_Trail() = default;
public:
	virtual HRESULT NativeConstruct_Prototype(_uint VtxCnt);
	virtual HRESULT NativeConstruct(void* pArg);

public:
	HRESULT Update(_double TimeDelta, _fmatrix vWorldMatrix);
	void Set_TrailPoint(_fvector Point1, _fvector Point2);
	void Set_MaxFrame(_uint flag) { m_MaxFrame = flag; }
	virtual HRESULT Render() override;

private:
	ID3D11Buffer*			m_pVBTrail = nullptr;
	D3D11_BUFFER_DESC		m_VBTrailDesc;
	D3D11_SUBRESOURCE_DATA	m_VBTrailSubResourceData;
	_bool					m_flag = false;
private:
	_uint m_MaxVtx = 0;
	_uint m_VtxCount = 0;

	_uint m_MaxFrame = 1;
	_uint m_FrameCnt = m_MaxFrame;

	_float m_Distance = 0.f;
	_bool m_RenderOn = false;

	_vector m_vWorldPos = _vector();
	_vector m_vPrevPos = _vector();

	vector<_vector> PosVec;

	_vector m_TrailPoint[2] = { XMVectorSet(-1.f, 0.f, -1.f, 0.f),  XMVectorSet(1.f, 0.f, -1.f, 0.f) };
public:
	static CVIBuffer_Trail* Create(_uint VtxCnt, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END

