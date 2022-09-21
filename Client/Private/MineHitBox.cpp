#include "stdafx.h"
#include "..\Public\MineHitBox.h"
#include "GameInstance.h"
#include "Boss_Manager.h"

CMineHitBox::CMineHitBox(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
	: CBullet(pDeviceOut, pDeviceContextOut)
{
}

CMineHitBox::CMineHitBox(const CMineHitBox & rhs)
	: CBullet(rhs)
{
}

HRESULT CMineHitBox::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMineHitBox::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC		TransformDesc;
	TransformDesc.SpeedPerSec = 5.0f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);
	if (FAILED(__super::NativeConstruct(pArg, &TransformDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;
	/* 처음정해진 디렉션이있고(플레이어 방향쪽)	*/
	m_pSphereCom->Update(m_pTransformCom->Get_WorldMatrix());

	m_JumpDesc.fPower = _float((rand() % 8) + 8);
	m_JumpDesc.vOriginPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	m_pTransformCom->Scaled(_float3(2.f, 2.f, 2.f));

	Create_Trail();

	return S_OK;

}

_int CMineHitBox::Tick(_double TimeDelta)
{
	if (m_Dead == true) {
		//Create_Explosion(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		return 1;
	}
	Status_Tick(TimeDelta);

	_float fY = (m_JumpDesc.fPower * m_JumpDesc.fTime * (1 + (m_JumpDesc.fSpeed * 0.2f))) - ((9.81f * m_JumpDesc.fTime * m_JumpDesc.fTime) / 2.f) + XMVectorGetY(m_JumpDesc.vOriginPos);
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_vDir*0.2f;
	vPos = XMVectorSetY(vPos, fY);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	
	Collide_Object(L"Layer_Player");

	m_JumpDesc.fTime += (_float)TimeDelta;
	m_TotalTime += TimeDelta;
	if (m_TotalTime >= 10.f)
		m_Dead = true;

	return 0;
}

void CMineHitBox::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);
	m_pSphereCom->Update(m_pTransformCom->Get_WorldMatrix());
	LookAtCameara();

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::GROUP_NONLIGHT, this);
}

HRESULT CMineHitBox::Render()
{
	if (nullptr == m_pShaderCom || 
		nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(__super::Render()))
		return S_OK;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(9)))
		return E_FAIL;
	
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

#ifdef _DEBUG
//	m_pSphereCom->Render();
#endif // _DEBUG

	return S_OK;
}

_int CMineHitBox::Status_Tick(_double TimeDelta)
{
	int flag = m_pStatus->Status_Tick(TimeDelta);
	if (flag == 1)
		m_Dead = true;

	return 0;
}

void CMineHitBox::Create_Trail()
{
	CGameInstance* pGameinstance = GET_INSTANCE(CGameInstance);
	WRAP temp;
	temp.pTransform = m_pTransformCom;
	temp.pTarget = this;


	if (FAILED(pGameinstance->Add_GameObjectToLayer(m_iLevelIndex, L"Trail_Purple", L"Prototype_Effect_Trail_Purple", &temp)))
		int a = 10;

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CMineHitBox::SetUp_Components()
{
	if (FAILED(__super::SetUp_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::SetUp_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::SetUp_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	if (FAILED(__super::SetUp_Components(TEXT("Com_Texture"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Bullet"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	CStatus::STATUSDESC temp;
	temp.fMaxHp = 20.f;
	temp.fMaxShield = 0.f;
	temp.fSpeed = 1.f;
	if (FAILED(__super::SetUp_Components(TEXT("Com_Status"), LEVEL_STATIC, TEXT("Prototype_Component_Status"), (CComponent**)&m_pStatus, &temp)))
		return E_FAIL;


	CCollider::COLLIDERDESC			ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
	ColliderDesc.fRadius = 0.5f;
	if (FAILED(__super::SetUp_Components(TEXT("Com_Sphere"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"), (CComponent**)&m_pSphereCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMineHitBox::SetUp_ConstantTable()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_WorldMatrixOnShader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("MyColor", &m_MyColor, sizeof(_float3))))
		return E_FAIL;
	
	if (FAILED(m_pShaderCom->Set_RawValue("fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->SetUp_ShaderResourceView(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CMineHitBox * CMineHitBox::Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
{
	CMineHitBox*	pInstance = new CMineHitBox(pDeviceOut, pDeviceContextOut);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created CMineHitBox"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMineHitBox::Clone(void * pArg)
{
	CMineHitBox*	pInstance = new CMineHitBox(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created CMineHitBox"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMineHitBox::Free()
{
	__super::Free();
	Safe_Release(m_pSphereCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);

}
