#pragma once

#include "Base.h"

BEGIN(Engine)

class CRenderTarget final : public CBase
{
public:
	CRenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CRenderTarget() = default;

public:
	ID3D11RenderTargetView* Get_RTV() const {
		return m_pRTV;
	}

	ID3D11ShaderResourceView* Get_SRV() const {
		return m_pSRV;
	}

public:
	HRESULT NativeConstruct(_uint iWidth, _uint iHeight, DXGI_FORMAT eFormat, _float4 vColor);
	HRESULT Clear_RenderTarget();

#ifdef _DEBUG
public:
	HRESULT Ready_DebugDesc(_float fTopX, _float fTopY, _float fWidth, _float fHeight);
	HRESULT Bind_WorldMatrix(class CShader* pShader);
#endif // _DEBUG

private:
	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pDeviceContext = nullptr;

private:	
	ID3D11ShaderResourceView*	m_pSRV = nullptr;
	ID3D11RenderTargetView*		m_pRTV = nullptr;
	_float4						m_vClearColor;



#ifdef _DEBUG
private:
	_float			m_fX, m_fY, m_fSizeX, m_fSizeY;
	_float4x4		m_WorldMatrix;
#endif // _DEBUG



public:
	static CRenderTarget* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iWidth, _uint iHeight, DXGI_FORMAT eFormat, _float4 vColor);
	virtual void Free() override;
};

END