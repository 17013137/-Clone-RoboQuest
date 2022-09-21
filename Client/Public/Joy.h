#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CCollider;
class CModel;
END

BEGIN(Client)

class CJoy final : public CGameObject
{
public:
	enum NORMAL_STATE{ STATE_BASE, STATE_SPECAIL1, STATE_SPECAIL2, STATE_SPECAIL3, STATE_END};

private:
	explicit CJoy(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	explicit CJoy(const CJoy& rhs);
	virtual ~CJoy() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Anim_Tick(_double TImeDelta);
	void Update_Alpha(_double Timedelta);

private:	
	CCollider*			m_pSphereCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;	
	CModel*				m_pModelCom = nullptr;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();	

private:
	NORMAL_STATE m_State = STATE_BASE;
	_double m_TotalTime = 0.0;
	_int m_ShaderCnt = 0;
	_bool m_flag = false;
	_float m_fAlpha = 0.f;

public:
	static CJoy* Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END