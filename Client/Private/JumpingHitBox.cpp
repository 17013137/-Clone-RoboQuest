#include "stdafx.h"
#include "..\Public\JumpingHitBox.h"
#include "GameInstance.h"
#include "Boss_Manager.h"

CJumpingHitBox::CJumpingHitBox(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
	: CBullet(pDeviceOut, pDeviceContextOut)
{
}

CJumpingHitBox::CJumpingHitBox(const CJumpingHitBox & rhs)
	: CBullet(rhs)
	, m_ProjMatrix(rhs.m_ProjMatrix)
{
}

HRESULT CJumpingHitBox::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CJumpingHitBox::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC		TransformDesc;
	TransformDesc.SpeedPerSec = 5.0f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);
	if (FAILED(__super::NativeConstruct(pArg, &TransformDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	CGameInstance* pGameinstance = GET_INSTANCE(CGameInstance);
	Safe_AddRef(pGameinstance);
	
	m_vDir = XMVector3Normalize(XMLoadFloat3(&_float3(_float(rand() % 20 - 10), 0.f, _float(rand() % 20 - 10))));
	
	m_Owner = (CTransform*)pGameinstance->Get_Component(m_iLevelIndex, L"Layer_Enemy", m_pTransformTag, 0);
	Safe_AddRef(m_Owner);

	if (m_Owner == nullptr)
		return E_FAIL;

	Safe_Release(pGameinstance);
	m_pSphereCom->Update(m_pTransformCom->Get_WorldMatrix());

	m_pTransformCom->Go_Y(1.f);

	m_JumpDesc.vOriginPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	return S_OK;

}

_int CJumpingHitBox::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	m_TotalTime += TimeDelta;
	
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_float fY = (m_JumpDesc.fPower * m_JumpDesc.fTime * (1 + (m_JumpDesc.fSpeed * 0.2f))) - ((9.81f * m_JumpDesc.fTime * m_JumpDesc.fTime) / 2.f) + XMVectorGetY(m_JumpDesc.vOriginPos);
	
	// m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_vDir*0.1f

	if (fY < 1.f) {
		m_JumpDesc.fPower *= (0.7f);
		m_JumpDesc.fTime = 0.0;
		m_JumpDesc.vOriginPos = XMLoadFloat4(&_float4(XMVectorGetX(vPos), fY, XMVectorGetZ(vPos), 1.f));

		if (m_JumpDesc.fPower < 0.5f) {
			return 1;
		}
	}

	_vector Result = XMLoadFloat4(&_float4(XMVectorGetX(vPos) + XMVectorGetX(m_vDir) * 0.1f, fY, XMVectorGetZ(vPos) + XMVectorGetZ(m_vDir) * 0.1f, 1));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, Result);
	
	m_JumpDesc.fTime += (_float)TimeDelta;

	m_pSphereCom->Update(m_pTransformCom->Get_WorldMatrix());

	return 0;
}

void CJumpingHitBox::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::GROUP_NONBLEND, this);
}

HRESULT CJumpingHitBox::Render()
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
//	m_pSphereCom->Render();
#endif // _DEBUG

	return S_OK;
}

HRESULT CJumpingHitBox::SetUp_Components()
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
	ColliderDesc.fRadius = 1.f;
	if (FAILED(__super::SetUp_Components(TEXT("Com_Sphere"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"), (CComponent**)&m_pSphereCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

CJumpingHitBox * CJumpingHitBox::Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
{
	CJumpingHitBox*	pInstance = new CJumpingHitBox(pDeviceOut, pDeviceContextOut);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created CJumpingHitBox"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CJumpingHitBox::Clone(void * pArg)
{
	CJumpingHitBox*	pInstance = new CJumpingHitBox(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created CJumpingHitBox"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CJumpingHitBox::Free()
{
	__super::Free();
	Safe_Release(m_Owner);
	Safe_Release(m_pSphereCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);

}
