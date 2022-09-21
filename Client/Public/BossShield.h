#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CCollider;
class CStatus;
END

BEGIN(Client)

class CBossShield final : public CGameObject
{
private:
	explicit CBossShield(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	explicit CBossShield(const CBossShield& rhs);
	virtual ~CBossShield() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	_int Status_Tick(_double TimeDelta);
	void Set_Position();

private:	
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;	
	CModel*				m_pModelCom = nullptr;
	CStatus*			m_pStatus = nullptr;
	CCollider*			m_pSphereCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;
	CTexture*			m_DisolveTexCom = nullptr;


private:
	_float m_Ambient = 0.f;
	_bool m_Damaged = false;
	_float m_DisolveTime = 0.f;
	_bool m_isDisovle = false;

	_double m_Duration = 20.0;
	_double m_AccDuration = 0.0;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();
	
public:
	static CBossShield* Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END