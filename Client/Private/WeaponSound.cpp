#include "stdafx.h"
#include "..\Public\WeaponSound.h"
#include "GameInstance.h"
#include "Player_Manager.h"

CWeaponSound::CWeaponSound(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
	: CGameObject(pDeviceOut, pDeviceContextOut)
{
}

CWeaponSound::CWeaponSound(const CWeaponSound & rhs)
	: CGameObject(rhs)
	, m_ProjMatrix(rhs.m_ProjMatrix)	
{
}

HRESULT CWeaponSound::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.f, 1.f));
	
	return S_OK;
}

HRESULT CWeaponSound::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC		TransformDesc;
	TransformDesc.SpeedPerSec = 5.0f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::NativeConstruct(pArg, &TransformDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_fX = g_iWinCX * 0.7f;
	m_fY = g_iWinCY * 0.55f;

	m_fSizeX = 100.f;
	m_fSizeY = 100.f;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX - g_iWinCX * 0.5f, -m_fY + g_iWinCY * 0.5f, 0.f, 1.f));
	m_pTransformCom->Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));

	return S_OK;
}

_int CWeaponSound::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	Update_WeaponInfo();

	if (CPlayer_Manager::Get_Instance()->Get_isAttack() == true) {
		m_isRender = true;
		if (m_isRandom == false) {
			m_isRandom = true;
			m_fX = g_iWinCX * 0.7f + rand() % 50 + 10.f;
			m_fY = g_iWinCY * 0.55f + rand() % 30;
			m_rand = rand() % 2;
			m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(_float(rand() % 10)));
		}
	}
	else {
		m_isRender = false;
		m_isRandom = false;
	}

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX - g_iWinCX * 0.5f, -m_fY + g_iWinCY * 0.5f, 0.f, 1.f));
	m_pTransformCom->Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));

	return 0;
}

void CWeaponSound::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (nullptr != m_pRendererCom && m_isRender == true)
		m_pRendererCom->Add_RenderGroup(CRenderer::GROUP_UI, this);
}

HRESULT CWeaponSound::Render()
{
	if (nullptr == m_pShaderCom || 
		nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(__super::Render()))
		return E_FAIL;

	
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

	if (FAILED(m_pTextureCom->SetUp_ShaderResourceView(m_pShaderCom, "g_Texture", m_ImgIndex)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(16)))
		return E_FAIL;
	
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CWeaponSound::Update_WeaponInfo()
{
	if (CPlayer_Manager::Get_Instance()->Get_WeaponType() == TYPE_RIFLE) {
		m_ImgIndex = m_rand + 10;
	}
	else if (CPlayer_Manager::Get_Instance()->Get_WeaponType() == TYPE_SHOTGUN) {
		m_ImgIndex = m_rand % 2;
	}
	else if (CPlayer_Manager::Get_Instance()->Get_WeaponType() == TYPE_BOW) {
		m_ImgIndex = 7;
	}

}

HRESULT CWeaponSound::SetUp_Components()
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
	if (FAILED(__super::SetUp_Components(TEXT("Com_Texture"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_WeaponSound"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

CWeaponSound * CWeaponSound::Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
{
	CWeaponSound*	pInstance = new CWeaponSound(pDeviceOut, pDeviceContextOut);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created CWeaponSound"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CWeaponSound::Clone(void * pArg)
{
	CWeaponSound*	pInstance = new CWeaponSound(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created CWeaponSound"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWeaponSound::Free()
{
	__super::Free();
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);

}
