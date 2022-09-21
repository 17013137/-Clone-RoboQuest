#pragma once

#include "Base.h"

BEGIN(Engine)

class CHierarchyNode final : public CBase
{
private:
	CHierarchyNode();
	virtual ~CHierarchyNode() = default;

public:
	const char* Get_Name() {
		return m_szName;
	}

	_uint Get_Depth() const {
		return m_iDepth;
	}

	_matrix Get_OffsetMatrix() const {
		return XMLoadFloat4x4(&m_OffsetMatrix);
	}
	_matrix Get_CombinedMatrix() const {
		return XMLoadFloat4x4(&m_CombinedTransformationMatrix);
	}
	_float4x4* Get_CombinedFloat4x4Ptr() {
		return &m_CombinedTransformationMatrix;
	}

public:
	void Set_TransformationMatrix(_fmatrix TransformationMatrix) {
		XMStoreFloat4x4(&m_TransformationMatrix, TransformationMatrix);
	}

	void Set_OffsetMatrix(_fmatrix OffsetMatrix) {
		XMStoreFloat4x4(&m_OffsetMatrix, OffsetMatrix);
	}

public:
	void Set_HierarchyNumber(_uint iNumber) { m_iHierarchyNumber = iNumber; }	// 노드의 정보를 받아 자신의 부모로 설정하는 함수
	_uint Get_HierarchyNumber(void) { return m_iHierarchyNumber; }				// 부모 노드의 정보를 가져오는 함수
	CHierarchyNode* GetParent(void) { return m_pParent; }				// 부모 노드의 정보를 가져오는 함수
	CHierarchyNode* SetParent(CHierarchyNode* pNode) { return m_pParent = pNode; }	// 부모 노드를 설정하는 함수


	_uint Get_ParentIndex(void) { return m_iParentIndex; }
	void Set_ParentIndex(_uint iIndex) { m_iParentIndex = iIndex; }
public:
	HRESULT SaveDatInfo(ofstream& Out);


public:
	HRESULT NativeConstruct(aiNode* pAINode, CHierarchyNode* pParent, _uint iDepth);
	HRESULT NativeConstruct(void* pArg = nullptr);
	HRESULT NativeConstruct(const char * szName, _matrix TransformationMatrixTP, _uint iDepth);
	void Update_CombinedTransformationMatrix();

private:
	char			m_szName[MAX_PATH] = "";
	_uint			m_iDepth = 0;
	_uint			m_iHierarchyNumber = 0;
	_uint			m_iParentIndex = 0;

	CHierarchyNode*	m_pParent = nullptr;
	_float4x4		m_OffsetMatrix;
	_float4x4		m_TransformationMatrix; /* 부모기준 나의 상태. */
	_float4x4		m_CombinedTransformationMatrix; /* 나의 상태  * 부모상태 */

public:
	static CHierarchyNode* Create(aiNode* pAINode, CHierarchyNode* pParent, _uint iDepth);
	static CHierarchyNode* Create(const char* szName, _matrix TransformationMatrixTP, _uint iDepth);
	virtual CHierarchyNode* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END