#include "..\Public\Channel.h"
#include "HierarchyNode.h"

CChannel::CChannel()
	: m_isCloned(false)
{

}

CChannel::CChannel(const CChannel & rhs)
	: m_KeyFrames(rhs.m_KeyFrames)
	, m_iNumKeyFrames(rhs.m_iNumKeyFrames)
	, m_iCurrentKeyFrame(rhs.m_iCurrentKeyFrame)
	, m_TransformationMatrix(rhs.m_TransformationMatrix)
	, m_isCloned(true)
	, m_RelatedNodeIndex(rhs.m_RelatedNodeIndex)

{
	strcpy_s(m_szName, rhs.m_szName);
}

CHierarchyNode * CChannel::Get_HierarchyNode()
{
	return m_pHierarchyNode;
}

HRESULT CChannel::NativeConstruct_Prototype(aiNodeAnim * pAIChannel)
{
	strcpy_s(m_szName, pAIChannel->mNodeName.data);

	if (FAILED(Ready_KeyFrames(pAIChannel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CChannel::NativeConstruct_Prototype(ifstream & In)
{
	if (!In)
		return E_FAIL;

	char temp[MAX_PATH];

	In.getline(m_szName, MAX_PATH);
	//In.getline(temp, MAX_PATH);

	In >> m_iNumKeyFrames;
	int iKeyFrameCnt = 0;
	In >> iKeyFrameCnt;
	In >> m_RelatedNodeIndex;
	In.getline(temp, MAX_PATH);

	KEYFRAME* pKeyFrames = new KEYFRAME[iKeyFrameCnt];
	In.read((char*)pKeyFrames, sizeof(KEYFRAME)*iKeyFrameCnt);

	for (int i = 0; i < iKeyFrameCnt; ++i)
		m_KeyFrames.push_back(&pKeyFrames[i]);

	In.getline(temp, MAX_PATH);

	return S_OK;
}

HRESULT CChannel::NativeConstruct()
{

	return S_OK;
}

void  CChannel::Compute_TransformationMatrix(_double PlayTime)
{
	while (m_iCurrentKeyFrame + 1 < m_KeyFrames.size())
	{
		if (PlayTime > m_KeyFrames[m_iCurrentKeyFrame + 1]->Time)
			++m_iCurrentKeyFrame;
		else
			break;
	}
	m_TempKeyFrame = m_iCurrentKeyFrame;

	KEYFRAME*	pLastKeyFrame = m_KeyFrames.back();

	_vector		vScale, vRotation, vTranslation;

	if (PlayTime > pLastKeyFrame->Time)
	{
		vScale = XMLoadFloat3(&pLastKeyFrame->vScale);
		vRotation = XMLoadFloat4(&pLastKeyFrame->vRotation);
		vTranslation = XMLoadFloat3(&pLastKeyFrame->vTranslation);
		vTranslation = XMVectorSetW(vTranslation, 1.f);
	}

	else
	{
		_double		Ratio = (PlayTime - m_KeyFrames[m_iCurrentKeyFrame]->Time) / (m_KeyFrames[m_iCurrentKeyFrame + 1]->Time - m_KeyFrames[m_iCurrentKeyFrame]->Time);

		_vector		vSourScale = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrame]->vScale);
		_vector		vSourRotation = XMLoadFloat4(&m_KeyFrames[m_iCurrentKeyFrame]->vRotation);
		_vector		vSourTranslation = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrame]->vTranslation);

		_vector		vDestScale = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrame + 1]->vScale);
		_vector		vDestRotation = XMLoadFloat4(&m_KeyFrames[m_iCurrentKeyFrame + 1]->vRotation);
		_vector		vDestTranslation = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrame + 1]->vTranslation);

		vScale = XMVectorLerp(vSourScale, vDestScale, (_float)Ratio);
		vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, (_float)Ratio);
		vTranslation = XMVectorLerp(vSourTranslation, vDestTranslation, (_float)Ratio);
		vTranslation = XMVectorSetW(vTranslation, 1.f);

	}
	if (nullptr != m_pHierarchyNode)
		m_pHierarchyNode->Set_TransformationMatrix(XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation));

}

void CChannel::Linear_TransMatrix(_double PlayTime, _double Duration, KEYFRAME* PrevKeyFrame)
{
	_vector		vScale, vRotation, vTranslation;

	if (PlayTime > Duration)
	{
		vScale = XMLoadFloat3(&m_KeyFrames[0]->vScale);
		vRotation = XMLoadFloat4(&m_KeyFrames[0]->vRotation);
		vTranslation = XMLoadFloat3(&m_KeyFrames[0]->vTranslation);
		vTranslation = XMVectorSetW(vTranslation, 1.f);
	}
	else
	{
		_double		Ratio = PlayTime / Duration;

		_vector		vSourScale = XMLoadFloat3(&PrevKeyFrame->vScale);
		_vector		vSourRotation = XMLoadFloat4(&PrevKeyFrame->vRotation);
		_vector		vSourTranslation = XMLoadFloat3(&PrevKeyFrame->vTranslation);

		_vector		vDestScale = XMLoadFloat3(&m_KeyFrames[0]->vScale);
		_vector		vDestRotation = XMLoadFloat4(&m_KeyFrames[0]->vRotation);
		_vector		vDestTranslation = XMLoadFloat3(&m_KeyFrames[0]->vTranslation);

		vScale = XMVectorLerp(vSourScale, vDestScale, (_float)Ratio);
		vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, (_float)Ratio);
		vTranslation = XMVectorLerp(vSourTranslation, vDestTranslation, (_float)Ratio);
		vTranslation = XMVectorSetW(vTranslation, 1.f);

	}

	if (nullptr != m_pHierarchyNode)
		m_pHierarchyNode->Set_TransformationMatrix(XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation));
}

HRESULT CChannel::Save_ChannelInfo(ostream & Out)
{
	if (!Out)
		return E_FAIL;

	Out << m_szName << endl;
	Out << m_iNumKeyFrames << " " << m_KeyFrames.size() << " " << m_RelatedNodeIndex << endl;

	KEYFRAME* KeyFrames = nullptr;
	KeyFrames = new KEYFRAME[m_KeyFrames.size()];

	for (int i = 0; i<m_KeyFrames.size(); ++i) {
		KeyFrames[i] = *m_KeyFrames[i];
	}

	Out.write((char*)KeyFrames, sizeof(KEYFRAME) * m_KeyFrames.size());

	Out << endl;

	Safe_Delete_Array(KeyFrames);

	return S_OK;
}

HRESULT CChannel::Ready_KeyFrames(aiNodeAnim * pAIChannel)
{
	m_iNumKeyFrames = max(pAIChannel->mNumScalingKeys, pAIChannel->mNumRotationKeys);
	m_iNumKeyFrames = max(m_iNumKeyFrames, pAIChannel->mNumPositionKeys);

	_float3			vScale;
	_float4			vRotation;
	_float3			vTranslation;


	for (_uint i = 0; i < m_iNumKeyFrames; ++i)
	{
		KEYFRAME*			pKeyFrame = new KEYFRAME;
		ZeroMemory(pKeyFrame, sizeof(KEYFRAME));

		if (pAIChannel->mNumScalingKeys > i)
		{
			memcpy(&vScale, &pAIChannel->mScalingKeys[i].mValue, sizeof(_float3));
			pKeyFrame->Time = pAIChannel->mScalingKeys[i].mTime;
		}

		if (pAIChannel->mNumRotationKeys > i)
		{
			vRotation.x = pAIChannel->mRotationKeys[i].mValue.x;
			vRotation.y = pAIChannel->mRotationKeys[i].mValue.y;
			vRotation.z = pAIChannel->mRotationKeys[i].mValue.z;
			vRotation.w = pAIChannel->mRotationKeys[i].mValue.w;
			pKeyFrame->Time = pAIChannel->mRotationKeys[i].mTime;
		}

		if (pAIChannel->mNumPositionKeys > i)
		{
			memcpy(&vTranslation, &pAIChannel->mPositionKeys[i].mValue, sizeof(_float3));
			pKeyFrame->Time = pAIChannel->mPositionKeys[i].mTime;
		}

		pKeyFrame->vScale = vScale;
		pKeyFrame->vRotation = vRotation;
		pKeyFrame->vTranslation = vTranslation;

		m_KeyFrames.push_back(pKeyFrame);
	}

	return S_OK;
}

CChannel * CChannel::Create(aiNodeAnim* pAIChannel)
{
	CChannel*	pInstance = new CChannel();

	if (FAILED(pInstance->NativeConstruct_Prototype(pAIChannel)))
	{
		MSG_BOX(TEXT("Failed to Created CChannel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CChannel * CChannel::Create(ifstream & In)
{
	CChannel*	pInstance = new CChannel();

	if (FAILED(pInstance->NativeConstruct_Prototype(In)))
	{
		MSG_BOX(TEXT("Failed to Created CChannel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CChannel * CChannel::Clone()
{
	CChannel*	pInstance = new CChannel(*this);

	if (FAILED(pInstance->NativeConstruct()))
	{
		MSG_BOX(TEXT("Failed to Created CChannel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CChannel::Free()
{
	if (false == m_isCloned)
	{
		//for (auto pKeyFrame : m_KeyFrames)
		//	Safe_Release(pKeyFrame);

		m_KeyFrames.clear();

		Safe_Release(m_pHierarchyNode);
	}
}



