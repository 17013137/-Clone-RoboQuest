#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
class CCollider;
END

BEGIN(Client)

class CExpBall final : public CGameObject
{
private:
	typedef struct JUMPDESC {
		_float fPower = 15.f;
		_float fTime = 0.0f;
		_float fSpeed = 1.0f;
		_vector vOriginPos;
	}JUMPDESC;
	
private:
	explicit CExpBall(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	explicit CExpBall(const CExpBall& rhs);
	virtual ~CExpBall() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Create_Trail();

private:
	CTexture*			m_pTextureCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CVIBuffer_Rect*		m_pVIBufferCom = nullptr;
	CCollider*			m_pSphereCom = nullptr;

private:
	_vector m_vDir;
	JUMPDESC m_JumpDesc;
	_double	m_TotalTime = 0.0;

	_bool IdleState = false;
	_float DefaultY = 0.f;

	_vector m_DownPos = _vector();
	_vector m_UpPos = _vector();
	_bool m_isDown = false;
	_float m_UpDownTime = 0.0f;
	_bool m_isTargetOn = false;
	_uint	m_ImgIndex = 6;

public:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static CExpBall* Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END