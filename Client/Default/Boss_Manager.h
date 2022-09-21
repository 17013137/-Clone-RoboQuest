#pragma once

#include "Client_Defines.h"
#include "Base.h"

class CBoss_Manager
{
private:
	CBoss_Manager();
	~CBoss_Manager();

public:
	static CBoss_Manager*		Get_Instance(void)
	{
		if (nullptr == m_pInstance)
			m_pInstance = new CBoss_Manager;

		return m_pInstance;
	}
	static void			Destroy_Instance(void)
	{
		if (nullptr != m_pInstance)
		{
			delete m_pInstance;
			m_pInstance = nullptr;
		}
	}



	void Set_CanonFinish(_bool temp) { m_CanonFisish = temp; }
	const _bool& Get_CanonFinish() { return m_CanonFisish; }

	void Set_isLaser(_bool temp) { m_isLaser = temp; }
	const _bool& Get_isLaser() { return m_isLaser; }

	void Set_CanonPos(_float4* temp) { m_CanonPos = temp; }
	_float4* Get_CanonPos() { return m_CanonPos; }

	void Set_IK_Mtx(_fmatrix flag) { R_Canon_IKMtx = flag; }
	_matrix Get_IK_Mtx() { return R_Canon_IKMtx; }
	
	void Set_isFlameShoot(_bool flag) { m_FlameShoot= flag; }
	const _bool& Get_isFlameShoot() { return m_FlameShoot; }

private:
	_bool m_FlameShoot = false;
	_bool m_CanonFisish = false;
	_bool m_isLaser = false;
	_float4*	m_CanonPos = nullptr;
	_matrix R_Canon_IKMtx = _matrix();
private:
	static	CBoss_Manager*	m_pInstance;
};

