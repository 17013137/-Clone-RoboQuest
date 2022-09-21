#include "..\Public\HierarchyNode.h"

CHierarchyNode::CHierarchyNode()
{
}

HRESULT CHierarchyNode::SaveDatInfo(ofstream & Out)
{
	if (!Out)
		return E_FAIL;

	// 이름, 깊이, 부모 노드 정보
	Out << m_szName << endl;
	Out << m_iDepth << " " << m_iHierarchyNumber << endl;

	// 행렬 정보 저장
	Out.write((char*)m_TransformationMatrix.m, sizeof(_float4x4));
	Out << endl;

	return S_OK;
}

HRESULT CHierarchyNode::NativeConstruct(aiNode * pAINode, CHierarchyNode* pParent, _uint iDepth)
{
	strcpy_s(m_szName, pAINode->mName.data);

	m_pParent = pParent;

	memcpy(&m_TransformationMatrix, &pAINode->mTransformation, sizeof(_float4x4));

	XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));

	m_iDepth = iDepth;

	return S_OK;
}

HRESULT CHierarchyNode::NativeConstruct(void * pArg)
{
	return S_OK;
}

HRESULT CHierarchyNode::NativeConstruct(const char * szName, _matrix TransformationMatrixTP, _uint iDepth)
{
	// 이름 저장
	strcpy_s(m_szName, szName);

	// 이동행렬의 역행렬 값 저장
	XMStoreFloat4x4(&m_TransformationMatrix, TransformationMatrixTP);

	// 깊이 값 저장
	m_iDepth = iDepth;

	// 부모 노드의 정보는 외부에서 따로 처리한다.

	return S_OK;
}

void CHierarchyNode::Update_CombinedTransformationMatrix()
{
	if (nullptr != m_pParent)
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMLoadFloat4x4(&m_TransformationMatrix) * XMLoadFloat4x4(&m_pParent->m_CombinedTransformationMatrix));

	else
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMLoadFloat4x4(&m_TransformationMatrix));
}

CHierarchyNode * CHierarchyNode::Create(aiNode * pAINode, CHierarchyNode* pParent, _uint iDepth)
{
	CHierarchyNode*	pInstance = new CHierarchyNode();

	if (FAILED(pInstance->NativeConstruct(pAINode, pParent, iDepth)))
	{
		MSG_BOX(TEXT("Failed to Created CHierarchyNode"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CHierarchyNode * CHierarchyNode::Create(const char * szName, _matrix TransformationMatrixTP, _uint iDepth)
{
	CHierarchyNode*	pInstance = new CHierarchyNode();

	if (FAILED(pInstance->NativeConstruct(szName, TransformationMatrixTP, iDepth)))
	{
		MSG_BOX(TEXT("Failed to Created CHierarchyNode"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CHierarchyNode * CHierarchyNode::Clone(void * pArg)
{
	CHierarchyNode*	pInstance = new CHierarchyNode(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created CMeshContainer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHierarchyNode::Free()
{

}
