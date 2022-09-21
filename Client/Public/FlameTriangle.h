#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CCollider;
class CTexture;
END

BEGIN(Client)

class CFlameTriangle final : public CGameObject
{
private:
	explicit CFlameTriangle(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	explicit CFlameTriangle(const CFlameTriangle& rhs);
	virtual ~CFlameTriangle() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	void Set_Position();
	void Collide_Target(const _tchar* LayerTag);

private:	
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;	
	CModel*				m_pModelCom = nullptr;
	CCollider*			m_pOBBCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;

	_float				m_Damage = 1.f;
	_float				m_AccTime = 0.0f;
	_double				m_Duration = 20.0;
	_double				m_AccDuration = 0.0;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();


public:
	static CFlameTriangle* Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END