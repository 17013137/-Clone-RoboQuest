#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Terrain final : public CVIBuffer
{
protected:
	CVIBuffer_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CVIBuffer_Terrain(const CVIBuffer_Terrain& rhs);
	virtual ~CVIBuffer_Terrain() = default;
public:
	virtual HRESULT NativeConstruct_Prototype(const _tchar* pHeightMapPath);
	virtual HRESULT NativeConstruct(void* pArg);

public:
	_uint				m_iNumVerticesX = 0;
	_uint				m_iNumVerticesZ = 0;

public:
	static CVIBuffer_Terrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pHeightMapPath);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END

