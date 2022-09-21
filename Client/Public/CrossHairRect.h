#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CCrossHairRect final : public CGameObject
{
	typedef struct CrossHairDesc {
		_bool Start = false;
		_double Max = 1.0;
		_double Now = 0.0;
		_double Speed = 0.2;
	}CROSSHAIRDESC;

private:
	explicit CCrossHairRect(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	explicit CCrossHairRect(const CCrossHairRect& rhs);
	virtual ~CCrossHairRect() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	CTexture*			m_pTextureCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CVIBuffer_Rect*		m_pVIBufferCom = nullptr;

private:
	_vector				m_OriginPos;
	_float4x4			m_ProjMatrix;
	_float				m_fX, m_fY, m_fSizeX, m_fSizeY;
	CROSSHAIRDESC		m_CrossHairDesc;
	_bool				m_isAttack = false;
public:
	HRESULT SetUp_Components();

public:
	static CCrossHairRect* Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END