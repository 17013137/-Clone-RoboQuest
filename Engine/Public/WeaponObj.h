#pragma once

#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CWeaponObj abstract : public CGameObject
{
public:
	typedef struct tagSocketDesc {
	class CModel*	pModelCom = nullptr;
	const char*		pBoneName = nullptr;
	const char*		pBoneName2 = nullptr;
	CTransform*		pPlayerTransCom = nullptr;
}SOCKETDESC;

	typedef struct GROWTHDESC {
		_uint iLevel = 0;
		_float Exp = 0;
	}GROWTHDESC;

protected:
	explicit CWeaponObj(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	explicit CWeaponObj(const CWeaponObj& rhs);
	virtual ~CWeaponObj() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* pArg, CTransform::TRANSFORMDESC* pTransformDesc = nullptr) override;
	virtual _int Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	void Update_WorldState();
	void Get_PlayerBone();
	void Get_Exp(_float _flag) { m_GrowDesc.Exp += _flag; }
	void Create_UpgradeEff();

protected:
	_float m_fAlpha = 0.f;

	_bool m_isAppear = false;
	_uint m_Type = 0;
	_bool m_isShot = false;
	_bool m_isReroad = false;
	_matrix m_AttachmentMatrix = _matrix();
	GROWTHDESC m_GrowDesc;
	SOCKETDESC			m_SocketDesc;
	_float4x4*			m_pSocketMatrix = nullptr;
	_float4x4			m_PivotMatrix;


public:	
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END