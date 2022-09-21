#include "stdafx.h"
#include "..\Public\MortarHitBox.h"
#include "GameInstance.h"
#include "Boss_Manager.h"

CMortarHitBox::CMortarHitBox(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
	: CBullet(pDeviceOut, pDeviceContextOut)
{
}

CMortarHitBox::CMortarHitBox(const CMortarHitBox & rhs)
	: CBullet(rhs)
	, m_ProjMatrix(rhs.m_ProjMatrix)
{
}

HRESULT CMortarHitBox::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMortarHitBox::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC		TransformDesc;
	TransformDesc.SpeedPerSec = 5.0f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);
	if (FAILED(__super::NativeConstruct(pArg, &TransformDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	CGameInstance* pGameinstance = GET_INSTANCE(CGameInstance);
	
	
	m_Player = (CTransform*)pGameinstance->Get_Component(m_iLevelIndex, L"Layer_Player", m_pTransformTag);
	
	RELEASE_INSTANCE(CGameInstance);

	if (m_Player == nullptr)
		return E_FAIL;
	_vector RandPos = XMVectorSet((_float)(rand() % 101 - 50) / 10.f, 0.f, (_float)(rand() % 101 - 50) / 10.f, 0.f);
	_vector vPos = m_Player->Get_State(CTransform::STATE_POSITION) + RandPos;
	
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	m_pSphere->Update(m_pTransformCom->Get_WorldMatrix());
	Create_OverHit();
	m_Damage = 1.f;
	return S_OK;
}

_int CMortarHitBox::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	if (m_Duration < m_AccDuration) {
		return 1;
	}
	

	m_pSphere->Update(m_pTransformCom->Get_WorldMatrix());
	AABBCollide_Object(L"Layer_Player");
	return 0;
}

void CMortarHitBox::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::GROUP_NONBLEND, this);
}

HRESULT CMortarHitBox::Render()
{
	if (nullptr == m_pShaderCom || 
		nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(__super::Render()))
		return S_OK;

	
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

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;
	
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

#ifdef _DEBUG
//	m_pSphere->Render();
#endif // _DEBUG

	return S_OK;
}

void CMortarHitBox::Create_OverHit()
{
	CGameInstance* pGameinstance = GET_INSTANCE(CGameInstance);

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	pGameinstance->Add_GameObjectToLayer(m_iLevelIndex, L"Effect", L"Prototype_GameObject_OverHit", &vPos);
	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CMortarHitBox::SetUp_Components()
{
	if (FAILED(__super::SetUp_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::SetUp_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::SetUp_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	if (FAILED(__super::SetUp_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_MainMenu"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC			ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vSize = _float3(1.f, 1.f, 1.f);
	ColliderDesc.fRadius = 2.f;

	if (FAILED(__super::SetUp_Components(TEXT("Com_Sphere"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"), (CComponent**)&m_pSphere, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

CMortarHitBox * CMortarHitBox::Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
{
	CMortarHitBox*	pInstance = new CMortarHitBox(pDeviceOut, pDeviceContextOut);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created CMortarHitBox"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMortarHitBox::Clone(void * pArg)
{
	CMortarHitBox*	pInstance = new CMortarHitBox(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created CMortarHitBox"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMortarHitBox::Free()
{
	__super::Free();
	Safe_Release(m_pSphere);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);

}
