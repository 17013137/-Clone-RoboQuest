#include "stdafx.h"
#include "..\Public\ShotgunIcon.h"
#include "GameInstance.h"
#include "Player_Manager.h"
#include "Event_Manager.h"

CShotgunIcon::CShotgunIcon(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
	: CGameObject(pDeviceOut, pDeviceContextOut)
{
}

CShotgunIcon::CShotgunIcon(const CShotgunIcon & rhs)
	: CGameObject(rhs)
	, m_ProjMatrix(rhs.m_ProjMatrix)	
{
}

HRESULT CShotgunIcon::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.f, 1.f));
	
	return S_OK;
}

HRESULT CShotgunIcon::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC		TransformDesc;
	TransformDesc.SpeedPerSec = 5.0f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::NativeConstruct(pArg, &TransformDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;
	
	m_fX = 1220.f;
	m_fY = 620.f;
	m_fSizeX = 100.f;
	m_fSizeY = 46.f;

	return S_OK;
}

_int CShotgunIcon::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	
	Update_WeaponInfo();

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX - g_iWinCX * 0.5f, -m_fY + g_iWinCY * 0.5f, 0.f, 1.f));
	m_pTransformCom->Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));

	return 0;
}

void CShotgunIcon::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (nullptr != m_pRendererCom&& CEvent_Manager::Get_Instance()->Get_Air() == false)
		m_pRendererCom->Add_RenderGroup(CRenderer::GROUP_UI, this);
}

HRESULT CShotgunIcon::Render()
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

	if (FAILED(m_pShaderCom->Set_RawValue("fAlpha", &m_Alpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(17)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CShotgunIcon::Update_WeaponInfo()
{
	if (CPlayer_Manager::Get_Instance()->Get_WeaponType() == TYPE_SHOTGUN) {
		if (m_Alpha == 0.4f) {
			m_Alpha = 0.8f;
			m_fX = 1210.f;
			m_fY = 620.f;
			m_fSizeX = 120.f;
			m_fSizeY = 55.2f;
		}
	}
	else
		if (m_Alpha == 0.8f) {
			m_Alpha = 0.4f;
			m_fX = 1220.f;
			m_fY = 620.f;
			m_fSizeX = 100.f;
			m_fSizeY = 46.f;
		}

	if (CPlayer_Manager::Get_Instance()->Get_NowWeapon() == SHOTGUN0)
		m_ImgIndex = 0;
	else if (CPlayer_Manager::Get_Instance()->Get_NowWeapon() == SHOTGUN1)
		m_ImgIndex = 1;
	else if (CPlayer_Manager::Get_Instance()->Get_NowWeapon() == SHOTGUN2)
		m_ImgIndex = 2;
}


HRESULT CShotgunIcon::SetUp_Components()
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
	if (FAILED(__super::SetUp_Components(TEXT("Com_Texture"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ShotgunIcon"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

CShotgunIcon * CShotgunIcon::Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
{
	CShotgunIcon*	pInstance = new CShotgunIcon(pDeviceOut, pDeviceContextOut);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created CShotgunIcon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CShotgunIcon::Clone(void * pArg)
{
	CShotgunIcon*	pInstance = new CShotgunIcon(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created CShotgunIcon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShotgunIcon::Free()
{
	__super::Free();
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);

}
