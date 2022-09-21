#include "..\Public\Transform.h"
#include "Shader.h"
#include "Navigation.h"
#include "Sound_Manager.h"

CTransform::CTransform(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{

}

_float3 CTransform::Get_Scale()
{
	_float3		vScale;

	vScale.x = XMVectorGetX(XMVector3Length(Get_State(STATE_RIGHT)));
	vScale.y = XMVectorGetX(XMVector3Length(Get_State(STATE_UP)));
	vScale.z = XMVectorGetX(XMVector3Length(Get_State(STATE_LOOK)));

	return vScale;
}

_matrix CTransform::Get_WorldMatrix()
{
	return XMLoadFloat4x4(&m_WorldMatrix);
}

_matrix CTransform::Get_PureWorldMatrix()
{
	_matrix MyMatrix = Get_WorldMatrix();
	MyMatrix.r[0] = XMVector3Normalize(MyMatrix.r[0]);
	MyMatrix.r[1] = XMVector3Normalize(MyMatrix.r[1]);
	MyMatrix.r[2] = XMVector3Normalize(MyMatrix.r[2]);
	XMVectorSetW(MyMatrix.r[3], 1.f);

	return MyMatrix;
}

_matrix CTransform::Get_WorldMatrixInverse()
{
	return XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix));
}

_float4x4 CTransform::Get_WorldFloat4x4()
{
	return m_WorldMatrix;
}

_float4x4 CTransform::Get_WorldFloat4x4_TP()
{
	_float4x4	WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_WorldMatrix)));

	return WorldMatrix;
}

void CTransform::Set_State(STATE eState, _fvector vState)
{
	_vector MyState = vState;
	if (eState == STATE_POSITION) {
		MyState = XMVectorSetW(MyState, 1.f);
	}

	XMStoreFloat4((_float4*)&m_WorldMatrix.m[eState][0], MyState);
}

void CTransform::Set_WorldMTX(_float4x4 _WorldMatrix)
{
	_matrix WorldMtx = XMLoadFloat4x4(&_WorldMatrix);

	WorldMtx.r[0] = XMVector3Normalize(WorldMtx.r[0]);
	WorldMtx.r[1] = XMVector3Normalize(WorldMtx.r[1]);
	WorldMtx.r[2] = XMVector3Normalize(WorldMtx.r[2]);
	
	XMStoreFloat4x4(&m_WorldMatrix, WorldMtx);
}

HRESULT CTransform::NativeConstruct_Prototype(void* pArg)
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	if (nullptr != pArg)
		memcpy(&m_TransformDesc, pArg, sizeof(TRANSFORMDESC));

	return S_OK;
}

HRESULT CTransform::Bind_WorldMatrixOnShader(CShader * pShader, const char * pConstantName)
{
	if (nullptr == pShader)
		return E_FAIL;

	_float4x4		WorldMatrixTP;

	XMStoreFloat4x4(&WorldMatrixTP, XMMatrixTranspose(XMLoadFloat4x4(&m_WorldMatrix)));

	pShader->Set_RawValue(pConstantName, &WorldMatrixTP, sizeof(_float4x4));

	return S_OK;
}

HRESULT CTransform::Go_Straight(_double TimeDelta)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vLook = Get_State(CTransform::STATE_LOOK);

	vPosition += XMVector3Normalize(vLook) * (_float)m_TransformDesc.SpeedPerSec * (_float)TimeDelta;

	Set_State(CTransform::STATE_POSITION, vPosition);

	return S_OK;
}

HRESULT CTransform::Go_Left(_double TimeDelta)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vRight = Get_State(CTransform::STATE_RIGHT);

	vPosition -= XMVector3Normalize(vRight) * (_float)m_TransformDesc.SpeedPerSec * (_float)TimeDelta;

	Set_State(CTransform::STATE_POSITION, vPosition);

	return S_OK;
}

HRESULT CTransform::Go_Right(_double TimeDelta)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vRight = Get_State(CTransform::STATE_RIGHT);

	vPosition += XMVector3Normalize(vRight) * (_float)m_TransformDesc.SpeedPerSec * (_float)TimeDelta;

	Set_State(CTransform::STATE_POSITION, vPosition);

	return S_OK;
}

HRESULT CTransform::Go_BackWard(_double TimeDelta)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vLook = Get_State(CTransform::STATE_LOOK);

	vPosition -= XMVector3Normalize(vLook) * (_float)m_TransformDesc.SpeedPerSec * (_float)TimeDelta;

	Set_State(CTransform::STATE_POSITION, vPosition);

	return S_OK;
}

HRESULT CTransform::Go_Up(_double TimeDelta)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vUp = Get_State(CTransform::STATE_UP);

	vPosition += XMVector3Normalize(vUp) * (_float)m_TransformDesc.SpeedPerSec * (_float)TimeDelta;

	Set_State(CTransform::STATE_POSITION, vPosition);

	return S_OK;
}

HRESULT CTransform::Go_Down(_double TimeDelta)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vUp = Get_State(CTransform::STATE_UP);

	vPosition -= XMVector3Normalize(vUp) * (_float)m_TransformDesc.SpeedPerSec * (_float)TimeDelta;

	Set_State(CTransform::STATE_POSITION, vPosition);

	return S_OK;
}

HRESULT CTransform::Go_MyStraight(_double TimeDelta)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vLook = Get_State(CTransform::STATE_LOOK);
	_float3		temp;
	XMStoreFloat3(&temp, vLook);
	temp.y = 0.f;
	vLook = XMLoadFloat3(&temp);

	vPosition += XMVector3Normalize(vLook) * (_float)m_TransformDesc.SpeedPerSec * (_float)TimeDelta;

	Set_State(CTransform::STATE_POSITION, vPosition);

	return S_OK;
}

HRESULT CTransform::Go_MyBackWard(_double TimeDelta)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vLook = Get_State(CTransform::STATE_LOOK);
	_float3		temp;
	XMStoreFloat3(&temp, vLook);
	temp.y = 0.f;
	vLook = XMLoadFloat3(&temp);

	vPosition -= XMVector3Normalize(vLook) * (_float)m_TransformDesc.SpeedPerSec * (_float)TimeDelta;

	Set_State(CTransform::STATE_POSITION, vPosition);

	return S_OK;
}

HRESULT CTransform::Go_MyLeft(_double TimeDelta)
{
	_vector		vLook = Get_State(STATE_LOOK);
	_vector		vPosition = Get_State(STATE_POSITION);
	_float3		Y = _float3(0.f, 1.f, 0.f);
	_vector vLeft = XMVector3Cross(vLook, XMLoadFloat3(&Y));

	vPosition += XMVector3Normalize(vLeft) * (_float)m_TransformDesc.SpeedPerSec * (_float)TimeDelta;

	Set_State(CTransform::STATE_POSITION, vPosition);

	return S_OK;
}

HRESULT CTransform::Go_MyRight(_double TimeDelta)
{
	_vector		vLook = Get_State(STATE_LOOK);
	_vector		vPosition = Get_State(STATE_POSITION);
	_float3		Y = _float3(0.f, 1.f, 0.f);
	_vector vRight = XMVector3Cross(XMLoadFloat3(&Y), vLook);

	vPosition += XMVector3Normalize(vRight) * (_float)m_TransformDesc.SpeedPerSec * (_float)TimeDelta;

	Set_State(CTransform::STATE_POSITION, vPosition);

	return S_OK;
}

HRESULT CTransform::Go_StraightOnNavigation(_double TimeDelta, _uint * pCurrentIndex, _float DefaultY)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vPrevPos = vPosition;
	_vector		vDir;
	_vector		vLook = Get_State(CTransform::STATE_LOOK);
	_float3		fPosition;


	vPosition += XMVector3Normalize(vLook) * (_float)m_TransformDesc.SpeedPerSec * (_float)TimeDelta;
	XMStoreFloat3(&fPosition, vPosition);

	vDir = vPosition - vPrevPos;
	CNavigation*		pNavigation = GET_INSTANCE(CNavigation);

	if(pNavigation->Move_OnNavigation(&fPosition, pCurrentIndex, vDir))
		Set_State(CTransform::STATE_POSITION, XMVectorSet(fPosition.x, fPosition.y+DefaultY, fPosition.z, 1.f));

	RELEASE_INSTANCE(CNavigation)

		return S_OK;
}

HRESULT CTransform::Go_BackOnNavigation(_double TimeDelta, _uint * pCurrentIndex, _float DefaultY)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vLook = Get_State(CTransform::STATE_LOOK);
	_float3		fPosition;
	_vector		vPrevPos = vPosition;
	_vector		vDir;

	vPosition -= XMVector3Normalize(vLook) * (_float)m_TransformDesc.SpeedPerSec * (_float)TimeDelta;
	XMStoreFloat3(&fPosition, vPosition);

	vDir = vPosition - vPrevPos;

	CNavigation*		pNavigation = GET_INSTANCE(CNavigation);

	if(pNavigation->Move_OnNavigation(&fPosition, pCurrentIndex, vDir))
		Set_State(CTransform::STATE_POSITION, XMVectorSet(fPosition.x, fPosition.y + DefaultY, fPosition.z, 1.f));


	RELEASE_INSTANCE(CNavigation)

		return S_OK;
}

HRESULT CTransform::Go_LeftOnNavigation(_double TimeDelta, _uint * pCurrentIndex, _float DefaultY)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vRight = Get_State(CTransform::STATE_RIGHT);
	_float3		fPosition;
	_vector		vPrevPos = vPosition;
	_vector		vDir;

	vPosition -= XMVector3Normalize(vRight) * (_float)m_TransformDesc.SpeedPerSec * (_float)TimeDelta;
	XMStoreFloat3(&fPosition, vPosition);

	vDir = vPosition - vPrevPos;
	CNavigation*		pNavigation = GET_INSTANCE(CNavigation);

	if (true == pNavigation->Move_OnNavigation(&fPosition, pCurrentIndex, vDir)) {
		Set_State(CTransform::STATE_POSITION, XMVectorSet(fPosition.x, fPosition.y + DefaultY, fPosition.z, 1.f));
	}

	RELEASE_INSTANCE(CNavigation)

	return S_OK;
}

HRESULT CTransform::Go_RightOnNavigation(_double TimeDelta, _uint * pCurrentIndex, _float DefaultY)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vRight = Get_State(CTransform::STATE_RIGHT);
	_float3		fPosition;
	_vector		vPrevPos = vPosition;
	_vector		vDir;

	vPosition += XMVector3Normalize(vRight) * (_float)m_TransformDesc.SpeedPerSec * (_float)TimeDelta;
	XMStoreFloat3(&fPosition, vPosition);

	vDir = vPosition - vPrevPos;
	CNavigation*		pNavigation = GET_INSTANCE(CNavigation);

	if (true == pNavigation->Move_OnNavigation(&fPosition, pCurrentIndex, vDir)) {
		Set_State(CTransform::STATE_POSITION, XMVectorSet(fPosition.x, fPosition.y + DefaultY, fPosition.z, 1.f));
	}

	RELEASE_INSTANCE(CNavigation)

		return S_OK;
}

HRESULT CTransform::Set_NaviPosition(_fvector vPos, _uint * pCurrentIndex, _float DefaultY)
{
	CNavigation*		pNavigation = GET_INSTANCE(CNavigation);
	_float3		fPosition;
	XMStoreFloat3(&fPosition, vPos);

	_vector		vDir = vPos - Get_State(CTransform::STATE_POSITION);

	if (true == pNavigation->Move_OnNavigation(&fPosition, pCurrentIndex, vDir)) {
		Set_State(CTransform::STATE_POSITION, XMVectorSet(fPosition.x, fPosition.y + DefaultY, fPosition.z, 1.f));
		return S_OK;
	}

	RELEASE_INSTANCE(CNavigation);

	return E_FAIL;
}


HRESULT CTransform::Go_Jumping(_fvector PrevPos, _float Speed, _double TimeDelta, _uint * pCurrentIndex, _float DefaultY)
{
	if (m_JumpDesc.isDoubleJump == false && m_JumpDesc.fTime > 0.2f) {
		if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
			CSound_Manager::GetInstance()->StopSound(0);
			CSound_Manager::GetInstance()->SoundPlay(L"DoubleJump.ogg", 0, 0.3f);
			m_JumpDesc.isDoubleJump = true;
			m_JumpDesc.fTime = 0.f;
			m_JumpDesc.doubleJumpY = XMVectorGetY(Get_State(STATE_POSITION)) - XMVectorGetY(m_JumpDesc.vOriginPos);
			m_JumpDesc.MaxY = 0.f;
		}
	}

	if (FAILED(Go_JumpStartAnim(TimeDelta)))
		return S_OK;

	if (m_JumpDesc.isJump == false) {
		CSound_Manager::GetInstance()->StopSound(0);
		CSound_Manager::GetInstance()->SoundPlay(L"Jump.ogg", 0, 0.3f);
		m_JumpDesc.fTime = 0.f;
		m_JumpDesc.vOriginPos = Get_State(STATE_POSITION);
		m_JumpDesc.isJump = true;
		m_JumpDesc.vPrevPos = PrevPos; //이전 프레임의 위치
		m_JumpDesc.fSpeed = Speed; //이전프레임의 속도
		m_JumpDesc.isDoubleJump = false;
		m_JumpDesc.doubleJumpY = 0.f;
		m_JumpDesc.MaxY = 0.f;
		m_JumpDesc.fPower = 5.f;
		m_JumpDesc.TimeSpeed = 1.3f;
	}

	// 포물선 공식
	//	y = 파워 * 시간 - (중력 * 시간의 제곱) / 2

	_float fY = (m_JumpDesc.fPower * m_JumpDesc.fTime * (1 + (m_JumpDesc.fSpeed * 0.2f))) - ((9.81f * m_JumpDesc.fTime * m_JumpDesc.fTime) / 2.f) + XMVectorGetY(m_JumpDesc.vOriginPos) + m_JumpDesc.doubleJumpY;
	
	if (m_JumpDesc.MaxY < fY) {
		m_JumpDesc.MaxY = fY;
	}

	m_JumpDesc.fTime += (_float)TimeDelta * m_JumpDesc.TimeSpeed;

	_vector pos = Get_State(STATE_POSITION);
	_vector Dir = XMVector3Normalize(pos - m_JumpDesc.vPrevPos);
	_float3 fPos;
	pos += Dir * (m_JumpDesc.fSpeed * 0.03f);
	pos = XMVectorSetY(pos, fY);

	XMStoreFloat3(&fPos, pos);

	CNavigation*		pNavigation = GET_INSTANCE(CNavigation);

	fPos.y = fY;

	if (true == pNavigation->Move_OnNavigation(&fPos, pCurrentIndex, Dir, true)) {
		Set_State(CTransform::STATE_POSITION, XMVectorSet(fPos.x, fPos.y, fPos.z, 1.f));
	}

	RELEASE_INSTANCE(CNavigation);

	if (fPos.y > fY && m_JumpDesc.fTime > 1.f) {		//임시 y축 높이 나중에 지형타면 지형의 y값을 가져와야함.
		m_JumpDesc.isJump = false;
		m_JumpAnimDesc[JUMP_START].fTimeAcc = 0.f;
		m_JumpAnimDesc[JUMP_START].isStart = false;
		m_JumpAnimDesc[JUMP_UP].isStart = false;
		m_JumpAnimDesc[JUMP_UP].fTimeAcc = 0.f;
		m_JumpAnimDesc[JUMP_DOWN].fTimeAcc = 0.f;
		m_JumpAnimDesc[JUMP_DOWN].isStart = false;
		m_JumpAnimDesc[JUMP_ARRIVE].isStart = false;
		m_JumpAnimDesc[JUMP_ARRIVE].fTimeAcc = 0.f;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CTransform::Go_Slide(_double TimeDelta)
{
	_vector pos = Get_State(STATE_POSITION);
	_vector vDir = XMVector3Normalize(pos - m_SlideDesc.vPrevPos) * m_SlideDesc.fSlideSpeed;
	_vector Dest = XMVectorSet(XMVectorGetX(vDir + pos), XMVectorGetY(pos), XMVectorGetZ(vDir + pos), 1.f);

	if (m_SlideDesc.fTime < m_SlideDesc.fSlideDuration) {
		_float tTime = (_float)TimeDelta * 0.2f;
		m_SlideDesc.fTime += (_float)TimeDelta;

		if (m_SlideDesc.isTurn == true && m_SlideDesc.fMaxTurn > m_SlideDesc.fNowTurn) { // x * PI / 180.f = x도
			if (m_SlideDesc.isLeft == true)
				MyRotation(Get_State(STATE_LOOK), XMConvertToRadians(-m_SlideDesc.fTurnSpeed));
			else
				MyRotation(Get_State(STATE_LOOK), XMConvertToRadians(m_SlideDesc.fTurnSpeed));

			m_SlideDesc.fNowTurn += m_SlideDesc.fTurnSpeed;  //돌아가는 각도크기
		}

	}
	else {
		if (m_SlideDesc.fNowTurn > 0.f) {
			if (m_SlideDesc.isLeft == true)
				MyRotation(Get_State(STATE_LOOK), XMConvertToRadians(m_SlideDesc.fTurnSpeed));
			else
				MyRotation(Get_State(STATE_LOOK), XMConvertToRadians(-m_SlideDesc.fTurnSpeed));

			m_SlideDesc.fNowTurn -= m_SlideDesc.fTurnSpeed; //슬라이드가 끝나고 돌아오는 각도크기
		}
		else {
			m_SlideDesc.isTurn = false;
			m_SlideDesc.isSlide = false;
		}
	}
	Set_State(STATE_POSITION, Dest);

	return S_OK;
}

HRESULT CTransform::Go_SitDown(_double TimeDelta)
{
	_vector pos = Get_State(STATE_POSITION);
	_vector Dest = pos;

	if (m_SitDesc.isSit == false) {
		m_SitDesc.isSit = true;
		m_SitDesc.fPower = 0.2f;
		m_SitDesc.fSize = 0.f;
		m_SitDesc.fTime = 0.f;
		m_SlideDesc.vPrevPos = pos;
		m_SlideDesc.fSlideDuration = 0.5f;
		m_SlideDesc.fSlideSpeed = 0.3f;
		m_SlideDesc.fNowTurn = 0.f;
		m_SlideDesc.fTime = 0.f;
		m_SlideDesc.isSlide = false;
		CSound_Manager::GetInstance()->StopSound(0);
		CSound_Manager::GetInstance()->SoundPlay(L"Sit.ogg", 0, 0.3f);
		if (GetAsyncKeyState('A') & 0x8000) {
			CSound_Manager::GetInstance()->StopSound(0);
			CSound_Manager::GetInstance()->SoundPlay(L"Slide.ogg", 0, 0.3f);
			m_SlideDesc.isSlide = true;
			m_SlideDesc.isLeft = true;
			m_SlideDesc.isTurn = true;
		}
		else if (GetAsyncKeyState('D') & 0x8000) {
			CSound_Manager::GetInstance()->StopSound(0);
			CSound_Manager::GetInstance()->SoundPlay(L"Slide.ogg", 0, 0.3f);
			m_SlideDesc.isSlide = true;
			m_SlideDesc.isTurn = true;
			m_SlideDesc.isLeft = false;
		}
	}

	if (m_SitDesc.isSit == true) {

		if (m_SitDesc.fSize < 1.f) {			//앉기 애님
			m_SitDesc.fSize += m_SitDesc.fPower;
			Dest = XMVectorSetY(Dest, XMVectorGetY(pos) - m_SitDesc.fPower);
		}
		Set_State(STATE_POSITION, Dest);

		if (m_SlideDesc.isSlide == true) {		//슬라이드 애님
			Go_Slide(TimeDelta);
		}
	}

	return S_OK;
}

HRESULT CTransform::Go_SitUp(_double TimeDelta)
{
	if (m_SitDesc.isSit == true) {
		m_SitDesc.isSit = false;
		m_SitDesc.fPower = 0.2f;
		m_SitDesc.fSize = 0.f;
		m_SlideDesc.vPrevPos = Get_State(STATE_POSITION);
		m_SlideDesc.fTime = 0.f;
		m_SlideDesc.isSlide = false;
		CSound_Manager::GetInstance()->StopSound(0);
		CSound_Manager::GetInstance()->SoundPlay(L"UnSit.ogg", 0, 0.3f);
	}

	if (m_SitDesc.isSit == false && m_SitDesc.fSize < 1.f) {

		_vector pos = Get_State(STATE_POSITION);
		pos = XMVectorSetY(pos, XMVectorGetY(pos) + m_SitDesc.fPower);
		Set_State(STATE_POSITION, pos);

		m_SitDesc.fSize += m_SitDesc.fPower;

		if (m_SlideDesc.isTurn == true) {
			if (m_SlideDesc.fNowTurn > 0.f) {
				if (m_SlideDesc.isLeft == true)
					MyRotation(Get_State(STATE_LOOK), XMConvertToRadians(1.f));
				else
					MyRotation(Get_State(STATE_LOOK), XMConvertToRadians(-1.f));

				m_SlideDesc.fNowTurn -= 1.f;
			}
			else {
				MyRotation(Get_State(STATE_LOOK), 0.f);
				m_SlideDesc.isTurn = false;
				m_SlideDesc.isSlide = false;
			}
		}

	}
	else {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CTransform::Go_JumpStartAnim(_double TimeDelta)
{
	if (m_JumpAnimDesc[JUMP_START].isStart == false) {
		m_JumpAnimDesc[JUMP_START].isStart = true;
		m_JumpAnimDesc[JUMP_START].fDuration = 0.05f;
		m_JumpAnimDesc[JUMP_START].fTimeAcc = 0.f;
		m_JumpAnimDesc[JUMP_START].Rotation_Speed = 0.4f;
	}

	if (m_JumpAnimDesc[JUMP_START].isStart == true && m_JumpAnimDesc[JUMP_START].fTimeAcc < m_JumpAnimDesc[JUMP_START].fDuration) {
		Turn(Get_State(STATE_RIGHT), TimeDelta * m_JumpAnimDesc[JUMP_START].Rotation_Speed);
		m_JumpAnimDesc[JUMP_START].fTimeAcc += (_float)TimeDelta;
	}
	else {
		if (m_JumpAnimDesc[JUMP_UP].isStart == false) {
			m_JumpAnimDesc[JUMP_UP].isStart = true;
			m_JumpAnimDesc[JUMP_UP].fDuration = 0.03f;
			m_JumpAnimDesc[JUMP_UP].fTimeAcc = 0.f;
			m_JumpAnimDesc[JUMP_UP].Rotation_Speed = 0.15f;
		}

		if (m_JumpAnimDesc[JUMP_UP].isStart == true && m_JumpAnimDesc[JUMP_UP].fTimeAcc < m_JumpAnimDesc[JUMP_UP].fDuration) {
			Turn(Get_State(STATE_RIGHT), -TimeDelta * m_JumpAnimDesc[JUMP_UP].Rotation_Speed);
			m_JumpAnimDesc[JUMP_UP].fTimeAcc += (_float)TimeDelta;
		}
		else {
			return S_OK;
		}
	}

	return E_FAIL;
}

HRESULT CTransform::Go_JumpEndAnim(_double TimeDelta)
{

	if (m_JumpAnimDesc[JUMP_DOWN].isStart == false) {
		m_JumpAnimDesc[JUMP_DOWN].isStart = true;
		m_JumpAnimDesc[JUMP_DOWN].fDuration = 0.05f;
		m_JumpAnimDesc[JUMP_DOWN].fTimeAcc = 0.f;
		m_JumpAnimDesc[JUMP_DOWN].Rotation_Speed = 0.2f;
	}

	if (m_JumpAnimDesc[JUMP_DOWN].isStart == true && m_JumpAnimDesc[JUMP_DOWN].fTimeAcc < m_JumpAnimDesc[JUMP_DOWN].fDuration) {
		Turn(Get_State(STATE_RIGHT), TimeDelta * m_JumpAnimDesc[JUMP_DOWN].Rotation_Speed);
		m_JumpAnimDesc[JUMP_DOWN].fTimeAcc += (_float)TimeDelta;
	}
	else {
		if (m_JumpAnimDesc[JUMP_ARRIVE].isStart == false) {
			m_JumpAnimDesc[JUMP_ARRIVE].isStart = true;
			m_JumpAnimDesc[JUMP_ARRIVE].fDuration = 0.05f;
			m_JumpAnimDesc[JUMP_ARRIVE].fTimeAcc = 0.f;
			m_JumpAnimDesc[JUMP_ARRIVE].Rotation_Speed = 0.05f;
		}

		if (m_JumpAnimDesc[JUMP_ARRIVE].isStart == true && m_JumpAnimDesc[JUMP_ARRIVE].fTimeAcc < m_JumpAnimDesc[JUMP_ARRIVE].fDuration) {
			Turn(Get_State(STATE_RIGHT), -TimeDelta * m_JumpAnimDesc[JUMP_ARRIVE].Rotation_Speed);
			m_JumpAnimDesc[JUMP_ARRIVE].fTimeAcc += (_float)TimeDelta;
		}
		else {
			return S_OK;
		}
	}

	return E_FAIL;
}

HRESULT CTransform::Go_Shake(_double TimeDelta, _float Power, _float Speed, _int Duration)
{
	_vector vPos = Get_State(STATE_POSITION);
	_vector vRight = XMVector3Normalize(Get_State(STATE_RIGHT));
	_vector vUp = XMVector3Normalize(Get_State(STATE_UP));
	_vector vResult = _vector();

	if (m_ShakeDesc.TimeAcc >= m_ShakeDesc.Duration) { //지속시간초과
		m_ShakeDesc.isStart = false;
		m_ShakeDesc.TimeAcc = 0;
		return E_FAIL;
	}

	if (m_ShakeDesc.isStart == false) { //처음 스타트
		m_ShakeDesc.isStart = true;
		m_ShakeDesc.Duration = Duration;
		m_ShakeDesc.LerpTime = 0.f;
		m_ShakeDesc.Speed = Speed;
		m_ShakeDesc.TimeAcc = 0;
		m_ShakeDesc.flag = false;
		m_ShakeDesc.Power = Power;
		m_ShakeDesc.vLerp = vPos;
	}
	
	if (m_ShakeDesc.flag == false) {
		m_ShakeDesc.flag = true;
		m_ShakeDesc.LerpTime = 0.f;
		m_ShakeDesc.vLerp = (vRight * (_float((rand() % 21 - 10.f) / 10.f)) * m_ShakeDesc.Power) + (vUp * (_float((rand() % 21 - 10.f) / 10.f)) * m_ShakeDesc.Power); //(-1.0f~ +1.f) * 파워;
	}
	
	if (m_ShakeDesc.flag == true) {
		if (m_ShakeDesc.Rotate == false) {
			if (m_ShakeDesc.LerpTime >= 1.f) {
				m_ShakeDesc.LerpTime = 1.f;
				m_ShakeDesc.Rotate = true;
			}
			
			m_ShakeDesc.LerpTime += (_float)TimeDelta * m_ShakeDesc.Speed;
		}
		else {
			if (m_ShakeDesc.LerpTime <= 0.f) {
				m_ShakeDesc.LerpTime = 0.f;
				m_ShakeDesc.Rotate = false;
				m_ShakeDesc.flag = false;
				m_ShakeDesc.TimeAcc++;
			}

			m_ShakeDesc.LerpTime -= (_float)TimeDelta * m_ShakeDesc.Speed;
		}
		vResult = XMVectorLerp(vPos, vPos+m_ShakeDesc.vLerp, m_ShakeDesc.LerpTime);
		Set_State(STATE_POSITION, vResult);
	}
	return S_OK;
}

HRESULT CTransform::Go_X(_double TimeDelta)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vAxisX = XMLoadFloat3(&_float3(1.f, 0.f, 0.f));

	vPosition += XMVector3Normalize(vAxisX) * (_float)m_TransformDesc.SpeedPerSec * (_float)TimeDelta;

	Set_State(CTransform::STATE_POSITION, vPosition);

	return S_OK;
}

HRESULT CTransform::Go_Y(_double TimeDelta)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vAxisY = XMLoadFloat3(&_float3(0.f, 1.f, 0.f));

	vPosition += XMVector3Normalize(vAxisY) * (_float)m_TransformDesc.SpeedPerSec * (_float)TimeDelta;

	Set_State(CTransform::STATE_POSITION, vPosition);

	return S_OK;
}

HRESULT CTransform::Go_Z(_double TimeDelta)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vAxisZ = XMLoadFloat3(&_float3(0.f, 0.f, 1.f));

	vPosition += XMVector3Normalize(vAxisZ) * (_float)m_TransformDesc.SpeedPerSec * (_float)TimeDelta;

	Set_State(CTransform::STATE_POSITION, vPosition);

	return S_OK;
}

HRESULT CTransform::Set_X(_float fX)
{
	_vector vPos = Get_State(STATE_POSITION);
	vPos = XMVectorSetX(vPos, fX);
	Set_State(CTransform::STATE_POSITION, vPos);

	return S_OK;
}

HRESULT CTransform::Set_Y(_float fY)
{
	_vector vPos = Get_State(STATE_POSITION);
	vPos = XMVectorSetY(vPos, fY);
	Set_State(CTransform::STATE_POSITION, vPos);

	return S_OK;
}

HRESULT CTransform::Set_Z(_float fZ)
{
	_vector vPos = Get_State(STATE_POSITION);
	vPos = XMVectorSetZ(vPos, fZ);
	Set_State(CTransform::STATE_POSITION, vPos);

	return S_OK;
}

HRESULT CTransform::Skill_Anim(_double TimeDelta, _bool* isSkill)
{
	if (m_AnimDesc[ANIM_SKILL].isStart == false) {
		m_AnimDesc[ANIM_SKILL].isStart = true;
		m_AnimDesc[ANIM_SKILL].fTimeAcc = 0.0f;
		m_AnimDesc[ANIM_SKILL].fDuration = 0.40f;
		m_AnimDesc[ANIM_SKILL].fSpeed = 2.f;
		m_AnimDesc[ANIM_SKILL].flag = false;
	}
	
	_float Speed = m_AnimDesc[ANIM_SKILL].fSpeed;
	
	if (*isSkill == false) {
		m_AnimDesc[ANIM_SKILL].isStart = false;
		m_AnimDesc[ANIM_SKILL].flag = false;
		return E_FAIL;
	}

	if (m_AnimDesc[ANIM_SKILL].fTimeAcc <= m_AnimDesc[ANIM_SKILL].fDuration) {
		if (m_AnimDesc[ANIM_SKILL].flag == false) {
			Speed = 0.5f;
			Go_MyBackWard(TimeDelta * Speed);
		}
		else 
			Go_MyStraight(TimeDelta * Speed);
	}

	m_AnimDesc[ANIM_SKILL].fTimeAcc += (_float)TimeDelta;
	if (m_AnimDesc[ANIM_SKILL].flag == false && m_AnimDesc[ANIM_SKILL].fTimeAcc > 0.15f) {
		m_AnimDesc[ANIM_SKILL].flag = true;
	}

	return S_OK;
}

HRESULT CTransform::Aim_ReBounding(_double ReboundPer, _bool* isAttack, _double TimeDelta)
{
	if (m_AnimDesc[ANIM_REBOUND].isStart == false) {
		m_AnimDesc[ANIM_REBOUND].isStart = true;
		m_AnimDesc[ANIM_REBOUND].fTimeAcc = 0.0;
		m_AnimDesc[ANIM_REBOUND].fDuration = 0.02f;
		m_AnimDesc[ANIM_REBOUND].isRight = rand() % 2;
	}

	if (m_AnimDesc[ANIM_REBOUND].fTimeAcc <= m_AnimDesc[ANIM_REBOUND].fDuration) {
		Turn(Get_State(STATE_RIGHT), -TimeDelta * ReboundPer);

		if (m_AnimDesc[ANIM_REBOUND].isRight == 0)
			Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), TimeDelta * ReboundPer);
		else
			Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), -TimeDelta * ReboundPer);

	}
	
	if(*isAttack == false){
		m_AnimDesc[ANIM_REBOUND].isStart = false;
		return E_FAIL;
	}

	m_AnimDesc[ANIM_REBOUND].fTimeAcc += (_float)TimeDelta;
	return S_OK;
}



HRESULT CTransform::Scaled(const _float3 & vScale)
{
	_vector		vRight = Get_State(CTransform::STATE_RIGHT);
	_vector		vUp = Get_State(CTransform::STATE_UP);
	_vector		vLook = Get_State(CTransform::STATE_LOOK);

	vRight = XMVector3Normalize(vRight) * vScale.x;
	vUp = XMVector3Normalize(vUp) * vScale.y;
	vLook = XMVector3Normalize(vLook) * vScale.z;

	Set_State(CTransform::STATE_RIGHT, vRight);
	Set_State(CTransform::STATE_UP, vUp);
	Set_State(CTransform::STATE_LOOK, vLook);

	return S_OK;
}

HRESULT CTransform::Scailing(_double TimeDelta)
{
	_float		fScale = 0.f;

	_vector		vRight = Get_State(CTransform::STATE_RIGHT);
	fScale = XMVectorGetX(XMVector3Length(vRight)) + (_float)m_TransformDesc.SpeedPerSec * (_float)TimeDelta;
	Set_State(CTransform::STATE_RIGHT, XMVector3Normalize(vRight) * fScale);

	_vector		vUp = Get_State(CTransform::STATE_UP);
	fScale = (_float)(XMVectorGetX(XMVector3Length(vUp)) + m_TransformDesc.SpeedPerSec * TimeDelta);
	Set_State(CTransform::STATE_UP, XMVector3Normalize(vUp) * fScale);


	_vector		vLook = Get_State(CTransform::STATE_LOOK);
	fScale = (_float)(XMVectorGetX(XMVector3Length(vLook)) + m_TransformDesc.SpeedPerSec * TimeDelta);
	Set_State(CTransform::STATE_LOOK, XMVector3Normalize(vLook) * fScale);

	return S_OK;
}

HRESULT CTransform::AccelTurn(_fvector _vAxis, _double Timedelta, _bool* isRight)
{
	if (m_AccelDesc.isStart == false) {
		m_AccelDesc.isStart = true;
		m_AccelDesc.Speed = 0.0;
		m_AccelDesc.MaxSpeed = 0.03;
		m_AccelDesc.TotalTime = 0.0;
		m_AccelDesc.WhenSlow = 3.0;
		m_AccelDesc.isRight = rand() % 2;
	}


	if(m_AccelDesc.isRight == 1)
		Turn(_vAxis, m_AccelDesc.Speed);
	else
		Turn(_vAxis, -m_AccelDesc.Speed);

	if (m_AccelDesc.Speed < m_AccelDesc.MaxSpeed)
		m_AccelDesc.Speed += Timedelta * 0.1f;

	if (m_AccelDesc.TotalTime >= 5.0) {
		m_AccelDesc.isStart = false;
		return E_FAIL;
	}

	m_AccelDesc.TotalTime += Timedelta;

	*isRight = m_AccelDesc.isRight;

	return S_OK;
}

HRESULT CTransform::Turn(_fvector vAxis, _double TimeDelta)
{
	_vector		vRight = Get_State(CTransform::STATE_RIGHT);
	_vector		vUp = Get_State(CTransform::STATE_UP);
	_vector		vLook = Get_State(CTransform::STATE_LOOK);

	_matrix		RotationMatrix;
	RotationMatrix = XMMatrixRotationAxis(vAxis, _float(m_TransformDesc.RotationPerSec * TimeDelta));

	vRight = XMVector3TransformNormal(vRight, RotationMatrix);
	vUp = XMVector3TransformNormal(vUp, RotationMatrix);
	vLook = XMVector3TransformNormal(vLook, RotationMatrix);

	Set_State(CTransform::STATE_RIGHT, vRight);
	Set_State(CTransform::STATE_UP, vUp);
	Set_State(CTransform::STATE_LOOK, vLook);

	return S_OK;
}

HRESULT CTransform::Rotation(_fvector vAxis, _double Radian)
{
	_float3		vScale = Get_Scale();

	_vector		vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScale.x;
	_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScale.y;
	_vector		vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScale.z;

	_matrix		RotationMatrix;
	RotationMatrix = XMMatrixRotationAxis(vAxis, _float(Radian));

	vRight = XMVector3TransformNormal(vRight, RotationMatrix);
	vUp = XMVector3TransformNormal(vUp, RotationMatrix);
	vLook = XMVector3TransformNormal(vLook, RotationMatrix);

	Set_State(CTransform::STATE_RIGHT, vRight);
	Set_State(CTransform::STATE_UP, vUp);
	Set_State(CTransform::STATE_LOOK, vLook);

	return S_OK;
}

HRESULT CTransform::MyRotation(_fvector vAxis, _double Radian)
{
	_float3		vScale = Get_Scale();

	//_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScale.y;
	//_vector		vLook = Get_State(CTransform::STATE_LOOK);
	//_vector		vRight = XMVector3Cross(vUp, vLook);
	_vector		vUp = Get_State(CTransform::STATE_UP);
	_vector		vLook = Get_State(CTransform::STATE_LOOK);
	_vector		vRight = Get_State(CTransform::STATE_RIGHT);
	
	_vector		vPos = Get_State(CTransform::STATE_POSITION);
	Set_State(STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	_matrix		RotationMatrix;
	RotationMatrix = XMMatrixRotationAxis(vAxis, _float(Radian));

	vRight = XMVector3TransformNormal(vRight, RotationMatrix);
	vUp = XMVector3TransformNormal(vUp, RotationMatrix);
	vLook = XMVector3TransformNormal(vLook, RotationMatrix);

	Set_State(CTransform::STATE_RIGHT, vRight);
	Set_State(CTransform::STATE_UP, vUp);
	Set_State(CTransform::STATE_LOOK, vLook);
	Set_State(CTransform::STATE_POSITION, vPos);

	return S_OK;
}


HRESULT CTransform::Reserve_Turn(_fvector vAxis, _double Reserve, _double TimeDelta)
{
	if (m_ResverDesc.isStart == false) {
		m_ResverDesc.isStart = true;
		m_ResverDesc.TotalTime = 0.0;
		m_ResverDesc.isRight = rand() % 2;
	}
	
	if (m_ResverDesc.TotalTime > Reserve) {
		m_ResverDesc.isStart = false;
		return E_FAIL;
	}

	if(m_ResverDesc.isRight == true)
		Turn(vAxis, TimeDelta);
	else
		Turn(vAxis, -TimeDelta);

	m_ResverDesc.TotalTime += TimeDelta;
	return S_OK;
}

HRESULT CTransform::TurnAndMove(_fvector vAxis, _fvector vDir, _double TimeDelta)
{

	return S_OK;
}

_vector CTransform::Get_IfTurnLook(_fvector vAxis, _double TimeDelta)
{
	_vector		vLook = Get_State(CTransform::STATE_LOOK);

	_matrix		RotationMatrix;
	RotationMatrix = XMMatrixRotationAxis(vAxis, _float(m_TransformDesc.RotationPerSec * TimeDelta));

	vLook = XMVector3TransformNormal(vLook, RotationMatrix);

	return vLook;
}

HRESULT CTransform::MyLookSet(_fvector Look)
{
	_vector vLook = XMVector3Normalize(Look);
	_vector vRight = XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&_float3(0.f, 1.f, 0.f)), vLook));
	_vector vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

	Set_State(STATE_RIGHT, vRight);
	Set_State(STATE_UP, vUp);
	Set_State(STATE_LOOK, vLook);

	return S_OK;
}

HRESULT CTransform::MyLook(_fvector vTargetPos, _float Speed, _double Timedelta)
{
	_vector vPos = Get_State(STATE_POSITION);
	_vector vLook = XMVector3Normalize(Get_State(STATE_LOOK));
	_vector vDir = XMVector3Normalize(vTargetPos - vPos);
	_vector AxisY = XMLoadFloat3(&_float3(0.f, 1.f, 0.f));
	_vector vNormal = XMVector3Normalize(XMVector3Cross(vLook, vDir));

	if (XMVectorGetY(vNormal) > 0.f)

		MyRotation(AxisY, XMConvertToRadians(Speed));
	else if(XMVectorGetY(vNormal) < 0.f)
		MyRotation(AxisY, XMConvertToRadians(-Speed));
	
	_vector Dot = XMVector3Dot(vDir, vLook);
	if (XMVectorGetX(Dot) >= 0.97)
		return E_FAIL;

	return S_OK;
}

HRESULT CTransform::MyAirLook(_fvector vTargetPos, _float Speed, _double Timedelta)
{
	_uint flag = 0;

	_vector vPos = Get_State(STATE_POSITION);
	_vector vLook = XMVector3Normalize(Get_State(STATE_LOOK));
	
	_vector vDir = XMVector3Normalize(vTargetPos - vPos);
	_vector AxisY = XMLoadFloat3(&_float3(0.f, 1.f, 0.f));

	_vector vNormal = XMVector3Normalize(XMVector3Cross(vLook, vDir));

	

	if (XMVectorGetY(vNormal) > 0.05f)
		MyRotation(AxisY, XMConvertToRadians(Speed));
	else if (XMVectorGetY(vNormal) < -0.05f)
		MyRotation(AxisY, XMConvertToRadians(-Speed));
	

	_vector vRight = XMVector3Normalize(Get_State(STATE_RIGHT));
	vLook = XMVector3Normalize(Get_State(STATE_LOOK));
	vDir = XMVector3Normalize(vTargetPos - vPos);

	_vector vGap = XMVector3Normalize(vDir - vLook);

	if (XMVectorGetY(vGap) > 0.05f)
		MyRotation(vRight, XMConvertToRadians(-Speed));
	else if (XMVectorGetY(vGap) < -0.05f)
		MyRotation(vRight, XMConvertToRadians(Speed));
	else
		return E_FAIL;

	return S_OK;
}

HRESULT CTransform::LookAt(_fvector vTargetPos)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_float3 vScale = Get_Scale();

	_vector vLook = vTargetPos - vPosition;
	_vector vAxisY = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	_vector vRight = XMVector3Cross(vAxisY, vLook);
	_vector vUp = XMVector3Cross(vLook, vRight);

	vRight = XMVector3Normalize(vRight) * vScale.x;
	vUp = XMVector3Normalize(vUp) * vScale.y;
	vLook = XMVector3Normalize(vLook) * vScale.z;

	Set_State(STATE_RIGHT, vRight);
	Set_State(STATE_UP, vUp);
	Set_State(STATE_LOOK, vLook);

	return S_OK;
}

HRESULT CTransform::CameraLookAt(_fvector vTargetPos)
{
	_vector vPosition = Get_State(STATE_POSITION);

	_vector vLook = vTargetPos - vPosition;
	_vector vAxisY = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	_vector vRight = XMVector3Cross(vAxisY, vLook);
	_vector vUp = XMVector3Cross(vLook, vRight);

	vRight = XMVector3Normalize(vRight);
	vUp = XMVector3Normalize(vUp);
	vLook = XMVector3Normalize(vLook);

	Set_State(STATE_RIGHT, vRight);
	Set_State(STATE_UP, vUp);
	Set_State(STATE_LOOK, vLook);

	return S_OK;
}

HRESULT CTransform::Reset_AxisY()
{
	_vector vRight = Get_State(STATE_RIGHT);
	_vector vAxisY = XMLoadFloat3(&_float3(0.f, 1.f, 0.f));
	_vector vLook = XMVector3Normalize(XMVector3Cross(vRight, vAxisY));
	
	Set_State(STATE_RIGHT, vRight);
	Set_State(STATE_UP, vAxisY);
	Set_State(STATE_LOOK, vLook);

	return S_OK;
}

HRESULT CTransform::BulletShoot(_fvector _Look, _fvector _Right, _fvector muzzle)
{
	_fvector vUp = XMVector3Normalize(XMVector3Cross(_Look, _Right));
	_fvector vLook = XMVector3Normalize(_Look);
	_fvector vRight = XMVector3Normalize(_Right);

	return S_OK;
}



CTransform * CTransform::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void* pArg)
{
	CTransform*	pInstance = new CTransform(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created CRenderer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CTransform::Clone(void * pArg)
{
	return nullptr;
}

void CTransform::Free()
{
	__super::Free();

}

