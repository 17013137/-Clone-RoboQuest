#include "stdafx.h"
#include "..\Public\BossHpBar.h"
#include "GameInstance.h"
#include "Player_Manager.h"
#include "KeyMgr.h"
#include "Event_Manager.h"

CBossHpBar::CBossHpBar(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
	: CGameObject(pDeviceOut, pDeviceContextOut)
{
}

CBossHpBar::CBossHpBar(const CBossHpBar & rhs)
	: CGameObject(rhs)
	, m_ProjMatrix(rhs.m_ProjMatrix)	
{
}

HRESULT CBossHpBar::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.f, 1.f));
	
	return S_OK;
}

HRESULT CBossHpBar::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC		TransformDesc;
	TransformDesc.SpeedPerSec = 5.0f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::NativeConstruct(pArg, &TransformDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;


	m_fSizeX = 608.f;
	m_fSizeY = 22.f;

	/*
	m_fSizeX = 612.f;
	m_fSizeY = 24.f;
	
	*/

	m_fX = g_iWinCX/2.f;
	m_fY = 50.f;//680

	CGameInstance* pGameinstance = GET_INSTANCE(CGameInstance);
	Safe_AddRef(pGameinstance);

	m_TargetStatus = (CStatus*)pGameinstance->Get_Component(m_iLevelIndex, L"Layer_Enemy", L"Com_Status", 0);
	if (m_TargetStatus == nullptr)
		return E_FAIL;

	Safe_AddRef(m_TargetStatus);

	Safe_Release(pGameinstance);

	m_MaxHP = m_TargetStatus->Get_MaxHP();
	m_HP = m_MaxHP;
	m_PrevHP = m_HP;
	return S_OK;
}

_int CBossHpBar::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);


	m_HP = m_TargetStatus->Get_HP();
	
	if (m_HP < m_PrevHP) {
		m_PrevHP -= (_float)TimeDelta * (5.f);

		if (m_HP >= m_PrevHP) {
			m_PrevHP = m_HP;
		}
	}

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX - g_iWinCX * 0.5f, -m_fY + g_iWinCY * 0.5f, 0.f, 1.f));
	m_pTransformCom->Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
	return 0;
}

void CBossHpBar::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);
	if (nullptr != m_pRendererCom&& CEvent_Manager::Get_Instance()->Get_Air() == false)
		m_pRendererCom->Add_RenderGroup(CRenderer::GROUP_UI, this);
}

HRESULT CBossHpBar::Render()
{
	if (nullptr == m_pShaderCom || 
		nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(__super::Render()))
		return E_FAIL;
	
	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->SetUp_ShaderResourceView(m_pShaderCom, "g_Texture", 1)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(12)))
		return E_FAIL;
	
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBossHpBar::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::SetUp_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Texture"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_HealthBar"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBossHpBar::SetUp_ConstantTable()
{
	if (FAILED(m_pTransformCom->Bind_WorldMatrixOnShader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	_float4x4	ViewMatrix;
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;

	_float4x4	ProjMatrixTP;
	XMStoreFloat4x4(&ProjMatrixTP, XMMatrixTranspose(XMLoadFloat4x4(&m_ProjMatrix)));
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &ProjMatrixTP, sizeof(_float4x4))))
		return E_FAIL;

	_float Hp = m_HP / m_MaxHP;
	if (FAILED(m_pShaderCom->Set_RawValue("Hp", &Hp, sizeof(_float))))
		return E_FAIL;

	_float PrevHP = m_PrevHP / m_MaxHP;
	if (FAILED(m_pShaderCom->Set_RawValue("PrevHp", &PrevHP, sizeof(_float))))
		return E_FAIL;


	return S_OK;
}

CBossHpBar * CBossHpBar::Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
{
	CBossHpBar*	pInstance = new CBossHpBar(pDeviceOut, pDeviceContextOut);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created CBossHpBar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBossHpBar::Clone(void * pArg)
{
	CBossHpBar*	pInstance = new CBossHpBar(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created CBossHpBar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBossHpBar::Free()
{
	__super::Free();
	Safe_Release(m_TargetStatus);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);

}
