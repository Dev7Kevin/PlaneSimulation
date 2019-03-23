/////////////////////////////////////////////////////////////////////////////
//
// Unit.cpp:   최상위 유닛 클래스의 소스 파일.
//
//
/////////////////////////////////////////////////////////////////////////////

#include "Unit.h"
#include "Camera.h"	// 폭파 애니 빌보딩을 위해.. 


//////////////////////////////////////////////////////////////////////
//
UINT CUnit::m_UnitCnt = 0;



// 컬링용. 2004.06.25.
CFrustum*	CUnit::m_pFrustum = NULL;
BOOL		CUnit::m_bUseFrustumCulling = YES_;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUnit::CUnit(VECTOR3 vPos, 
			 VECTOR3 vRot, 
			 VECTOR3 vScale,
 			 VECTOR3 vMoveVelo,
			 VECTOR3 vRotVelo,

			 float	 fRadius,			//충돌구 반경.
			 LPANIINFO	pBoomAniInfo,	//폭파 애니메이션 정보 구조체.04.06.23+

			 CB3Mesh* pMesh,
			 LPDEVICE pDev
			 ):
m_vPos(vPos), m_vRot(vRot), m_vScale(vScale),
m_vMoveVelo(vMoveVelo), m_vRotVelo(vRotVelo),
m_fCollRadius(fRadius),  
m_pMesh(pMesh), m_pDev(pDev)
{
	//
	// 충돌 정보..
	//
	m_bIsDead = NO_;

	//
	// 폭파 애니 정보 셋팅 : 외부에서 주어진 데이터가 없다면, 그냥 '클리어'
	//
	ZeroMemory(&m_BoomAniInfo, sizeof(ANIINFO));
	if(pBoomAniInfo != NULL) m_BoomAniInfo = *pBoomAniInfo;


	SetTM();		


	//정적 유닛 카운트 증가.
	++m_UnitCnt;
	

	//데이터 벡업.
	m_vOrgPos = m_vPos;
	m_vOrgRot = m_vRot;
	m_vOrgScale = m_vScale;	

	// 현재 방향. 일반저긍로 -1 을 전방으러 처리한다.
	m_vOrgDirection = m_vDirection = VECTOR3(0, 0, -1);		
 

	// 컬링 데이터.
	m_bUseFrustumCulling = YES_;		//기본...당연!! 
	//m_bIsVisible = YES_;				//일단 무조건 화면에 보이는 것으로!!
}


////////////////////////////////////////////////////////////////////////////
//
CUnit::~CUnit()
{

	Release();

	//정적 유닛 카운트 감소.
	--m_UnitCnt;

}




/////////////////////////////////////////////////////////////////////////////
//
//  const BOOL CUnit::IsVisible() 
//
//  컬링 검사 - 검사후 시야에 물체가 들어와 있으면 TRUE 를 리턴하고..
//  반대의 경우는 FALSE  를 리턴한다. 
//
const BOOL CUnit::IsVisible()
{
	if(m_bUseFrustumCulling == FALSE) return TRUE;		// 컬링을 사용하지 않으면 true 리턴!! => 모두 그린다~!!

	static MATRIX mView;		
	m_pDev->GetTransform(D3DTS_VIEW, &mView);		

	return  m_pFrustum->IsVisible(&mView, &m_vPos, m_fCollRadius);
	//m_bIsVisible = m_pFrustum->IsVisible(&mView, &m_vPos, m_fRadius);
	//return m_bIsVisible;
}





////////////////////////////////////////////////////////////////////////////
//
// void CUnit::SetDead() 
//
// 유닛을 '죽게함' -> 폭파 애니메이션을  수행하게 한다.
//
void CUnit::SetDead()
{
	m_bIsDead = TRUE;

	//폭파 애니 데이터 설정
	m_BoomAniInfo.NowTime = ::timeGetTime();		
	m_BoomAniInfo.OldTime = ::timeGetTime();
	m_BoomAniInfo.Frm = 0;

}




  


////////////////////////////////////////////////////////////////////////////
//
// BOOL CUnit::DrawBoom() - 폭파 애니메이션 렌더링. 
//
// 애니가 종료되면 메모리를 삭제하기 위해 FALSE 가 리턴된다.
//
// 기본 폭파는 유닛의 월드 위치를 중심으로 빌보딩 스타일로 출력된다.
// 물로 하위클래스에서 오버로딩도 충분히 가능하다.
//
BOOL CUnit::DrawBoom(float dTime)	
{
	m_BoomAniInfo.NowTime = timeGetTime();

	if(m_BoomAniInfo.NowTime  - m_BoomAniInfo.OldTime > m_BoomAniInfo.FPS)
	{
		m_BoomAniInfo.OldTime = m_BoomAniInfo.NowTime; 
		if(++m_BoomAniInfo.Frm >=  m_BoomAniInfo.TotFrm ) 
		{
			m_BoomAniInfo.Frm = 0;

			//애니메이션이 끝나면 유닛을 제거하라는 의미로 FALSE 를 리턴한다.
			return FALSE;				
		}
	}



	//컬링을 시도...'시야'에서 벗어나면..렌더링은 하지 않는다.
	//시작적인 효과를 위해 '폭파'는 그대로 남겨둠...2004.06.25+  
	//if(!IsVisible())	return TRUE;		<-- 주석처리 합니다.


	// 회전값은 카메라의 '회전행렬'을 얻는다 : 기본값은 '일반' 빌보딩..
	m_BoomAniInfo.pSpr[m_BoomAniInfo.Frm].mRot = (*m_BoomAniInfo.ppCamera)->GetBillMtx();

	B3DSprite_Scaling(&m_BoomAniInfo.pSpr[m_BoomAniInfo.Frm], m_BoomAniInfo.vScale.x, m_BoomAniInfo.vScale.y);
	B3DSprite_Translation(&m_BoomAniInfo.pSpr[m_BoomAniInfo.Frm], m_vPos.x, m_vPos.y, m_vPos.z, ON3D_);
	B3DSprite_DrawFX(&m_BoomAniInfo.pSpr[m_BoomAniInfo.Frm], COLOR(1.0f, 1.0f, 1.0f, 1.0f));

	//'폭파' 구의 중심 부분을 좀더 밝게 처리학 위해서... 2004.06.24 
	B3DSprite_Draw3D(&m_BoomAniInfo.pSpr[m_BoomAniInfo.Frm], TRUE, 0.2f, NOZWRITE_);	 

	return TRUE;

}//end of BOOL CUnit::DrawBoom()








////////////////////////////////////////////////////////////////////////////
//
// BOOL CUnit::CheckCollision() : 충돌 검사.
//
// 두 유닛의 '월드' 위치의 거리가 
// 목표 유닛의 충돌구의 반경보다 작으면 충돌이다...
//
BOOL CUnit::CheckCollision(CUnit* pTarget,				//목표 유닛 포인터.
						   float fTargetRadius/*=0.0f*/	//폭표 유닛의 충돌 반경.
						   )
{
	// 두 유닛 사이의 거리를 구한다.
	float fDistance = D3DXVec3Length(&(m_vPos - pTarget->GetPos()));

	//주어진 반경이 없다면 목표유닛의 반경을 사용한다.
	if(fTargetRadius == 0.0f) fTargetRadius = pTarget->GetRadius();		


	// 두 유닛의 충돌구의 거리의 합을 구한다.
	float fTotRadius = m_fCollRadius + fTargetRadius;


	// 목표 유닛의 충돌구의 반경보다 작으면 충돌....
	if( fDistance < fTotRadius)
	{
		return  COLL_YES_;			// 충돌의 의미.
	}

	return COLL_NO_;				// 충돌 아님.
}






////////////////////////////////////////////////////////////////////////////
//
UINT& CUnit::GetUnitCount()
{
	return m_UnitCnt;
}


////////////////////////////////////////////////////////////////////////////
//
void CUnit::Release()
{
	//N/A
}




////////////////////////////////////////////////////////////////////////////
//
// 변환 행렬 만들기.
//
void CUnit::SetTM()
{
	D3DXMatrixRotationYawPitchRoll(&m_mRotation, m_vRot.y, m_vRot.x, m_vRot.z);
	D3DXMatrixScaling(&m_mScale, m_vScale.x, m_vScale.y, m_vScale.z);
	D3DXMatrixTranslation(&m_mTranslation, m_vPos.x, m_vPos.y, m_vPos.z);

	MATRIX temp;
	D3DXMatrixMultiply(&temp, &m_mScale, &m_mRotation);
	D3DXMatrixMultiply(&m_mTM, &temp, &m_mTranslation);
 	
}





////////////////////////////////////////////////////////////////////////////
//
void CUnit::WMove(VECTOR3 pos)
{
	m_vPos = pos;

	SetTM();
}

////////////////////////////////////////////////////////////////////////////
//
void CUnit::WRotate(VECTOR3 rot)
{
	m_vRot = rot;

	SetTM();
}


////////////////////////////////////////////////////////////////////////////
//
void CUnit::WScale(VECTOR3 scl)
{
	m_vScale = scl;

	SetTM();
}


////////////////////////////////////////////////////////////////////////////

void CUnit::Reset()
{
	m_vPos = m_vOrgPos;
	m_vRot = m_vOrgRot;
	m_vScale = m_vOrgScale;

	m_vMoveVelo = VECTOR3(0, 0, 0);		//속도 초기화.
	m_vRotVelo = VECTOR3(0, 0, 0);

	m_vDirection = m_vOrgDirection;			


	SetTM();

}







/****************** end of file "Unit.cpp" ********************************/