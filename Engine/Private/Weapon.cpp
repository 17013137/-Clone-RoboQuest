#include "..\Public\Weapon.h"
#include "Transform.h"
#include "Model.h"



CWeapons::CWeapons(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
	: CComponent(pDeviceOut, pDeviceContextOut)
	
{
}


HRESULT CWeapons::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CWeapons::NativeConstruct(void * pArg)
{

	return S_OK;
}



CWeapons * CWeapons::Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
{
	CWeapons*	pInstance = new CWeapons(pDeviceOut, pDeviceContextOut);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created CWeapons"));
		Safe_Release(pInstance);
	}
	 
	return pInstance;
}

CComponent * CWeapons::Clone(void * pArg)
{
	CWeapons*	pInstance = new CWeapons(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created CWeapons"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWeapons::Free()
{
	__super::Free();
}
