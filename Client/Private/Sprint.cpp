#include "stdafx.h"
#include "..\Public\Sprint.h"
#include "GameInstance.h"
#include "Player_Manager.h"

CSprint::CSprint(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
	: CGameObject(pDeviceOut, pDeviceContextOut)
{
}

CSprint::CSprint(const CSprint & rhs)
	: CGameObject(rhs)
	, m_ProjMatrix(rhs.m_ProjMatrix)	
{
}

HRESULT CSprint::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.f, 1.f));
	
	return S_OK;
}

HRESULT CSprint::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC		TransformDesc;
	TransformDesc.SpeedPerSec = 5.0f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::NativeConstruct(pArg, &TransformDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;
	
	m_fX = g_iWinCX * 0.5f;
	m_fY = g_iWinCY * 0.5f;

	m_fSizeX = g_iWinCX*1.5f;
	m_fSizeY = g_iWinCY*1.5f;

	return S_OK;
}

_int CSprint::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	//m_Alpha -= (_float)TimeDelta;
	//
	//if (m_Alpha <= 0.0f) {
	//	m_Alpha = 0.f;
	//}
	_bool isSprint = CPlayer_Manager::Get_Instance()->Get_Sprint();

	if (isSprint == true) {
		m_Alpha += TimeDelta*5.f;
		if (m_Alpha > 1.0f)
			m_Alpha = 1.f;

		m_AccTime += TimeDelta;
	}
	else {
		m_Alpha -= TimeDelta*5.f;
		if (m_Alpha < 0.0f) {
			m_Alpha = 0.f;
			m_AccTime = 0.0;
		}
	}

	Set_ColorAlpha(TimeDelta);

	m_pTransformCom->Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX - g_iWinCX * 0.5f, -m_fY + g_iWinCY * 0.5f, 0.f, 1.f));

	return 0;
}

void CSprint::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::GROUP_UI, this);
}

HRESULT CSprint::Render()
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

	if (FAILED(m_pTextureCom->SetUp_ShaderResourceView(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("fAlpha", &m_Alpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("red", &m_RedAlpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("blue", &m_BlueAlpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("green", &m_GreenAlpha, sizeof(_float))))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Begin(13)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CSprint::Set_ColorAlpha(_double TimeDelta)
{
	if (m_RedAlpha > 1.0f) {
		m_RedAlpha = 1.f;
		m_ColorFlag[0] = true;
	}
	else if (m_RedAlpha < 0.f) {
		m_RedAlpha = 0.f;
		m_ColorFlag[0] = false;
	}

	if (m_BlueAlpha > 1.0f) {
		m_BlueAlpha = 1.f;
		m_ColorFlag[1] = true;
	}
	else if (m_BlueAlpha < 0.f) {
		m_BlueAlpha = 0.f;
		m_ColorFlag[1] = false;
	}

	if (m_GreenAlpha > 1.0f) {
		m_GreenAlpha = 1.f;
		m_ColorFlag[2] = true;
	}
	else if (m_GreenAlpha < 0.f) {
		m_GreenAlpha = 0.f;
		m_ColorFlag[2] = false;
	}

	if (m_ColorFlag[0] == false)
		m_RedAlpha += (_float)TimeDelta;
	else
		m_RedAlpha -= (_float)TimeDelta;

	if (m_ColorFlag[1] == false)
		m_BlueAlpha += (_float)TimeDelta;
	else
		m_BlueAlpha -= (_float)TimeDelta;

	if (m_ColorFlag[2] == false)
		m_GreenAlpha += (_float)TimeDelta;
	else
		m_GreenAlpha -= (_float)TimeDelta;
}

HRESULT CSprint::SetUp_Components()
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
	if (FAILED(__super::SetUp_Components(TEXT("Com_Texture"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Sprint"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

CSprint * CSprint::Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
{
	CSprint*	pInstance = new CSprint(pDeviceOut, pDeviceContextOut);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created CSprint"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSprint::Clone(void * pArg)
{
	CSprint*	pInstance = new CSprint(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created CSprint"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSprint::Free()
{
	__super::Free();
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);

}
