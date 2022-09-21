#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CWeapons final : public CComponent
{
public:
	enum ATTACK_TYPE { ATTACK_DEFAULT, ATTACK_PUSH, ATTACK_KNOCKBACK, ATTACK_TYPE_FIRE, ATTACK_TYPEWEAPON_END };  //무기종류

public:
	typedef struct tagSocketDesc
	{
		class CModel*	pModelCom = nullptr;
		const char*		pBoneName = nullptr;

	}SOCKETDESC;

	

private:
	explicit CWeapons(ID3D11Device* ppDeviceOut, ID3D11DeviceContext* ppDeviceContextOut);
	virtual ~CWeapons() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);

	/*
	
	class CModel*	pModelCom = nullptr;
		const char*		pBoneName = nullptr;
	*/
public:
	static CWeapons* Create(ID3D11Device* ppDeviceOut, ID3D11DeviceContext* ppDeviceContextOut);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;

};

END