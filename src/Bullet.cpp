// Bullet.cpp: implementation of the CBullet class.
//
//
////////////////////////////////////////////////////////////////////////////// 


#include "Bullet.h"

//////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//
CBullet::CBullet(
		LPDEVICE pDev,
		CB3Mesh* pMesh,
		UINT	uDmg,					//데미지.
		float	fFireVelo,				//총구 발사 가속도
		VECTOR3 vFireDir,				//발사 방향.
		float	fMaxRange,				//최대 사거리...
		
		SPRDATA*	pSpr,				//총알용 스트라이트 포인터..  
		//pMesh, pSpr 둘중에 하나는 있어야 한다.
		CCamera**	ppCam,				//카메라 이중 포인터. 2004.06.19. 
		COLOR		Color,				//총알 컬러. ㅋ

		float		fRadius,			//충돌구 반경.
		LPANIINFO	pBoomAniInfo,		//폭파 애니메이션 정보 구조체.04.06.23+ 
										//NULL 이면 무시된다.
 
		VECTOR3 vPos,					//발사 위치.		
		VECTOR3 vRot/* = VECTOR3(0.0f, 0.0f, 0.0f)*/, 
		VECTOR3 vScale/*= VECTOR3(1.0f, 1.0f, 1.0f)*/,
		VECTOR3 vMoveVelo/* = VECTOR3(0.0f, 0.0f, 0.0f)*/,
		VECTOR3 vRotVelo/* = VECTOR3(0.0f, 0.0f, 0.0f)*/
 		 )
 :CUnit(vPos, vRot, vScale, vMoveVelo, vRotVelo, 
		fRadius, pBoomAniInfo,  
		pMesh, pDev)
 , m_uDmg(uDmg), m_fFireVelo(fFireVelo), m_vFireDir(vFireDir) 
 , m_Spr(*pSpr), m_ppCamera(ppCam), m_Color(Color)	 
{
	InitBullet(fMaxRange);		 
}


//////////////////////////////////////////////////////////////////////////////
//
CBullet::~CBullet()
{
	//N/A
}




//////////////////////////////////////////////////////////////////////////////
//
// 총알 멤버 데이터 초기화 메소드. : 'Missile' 의 'Reset' 을 위해 함수처리.
//
void CBullet::InitBullet(float fMaxRange)
{
	//발사 방향의 단위 벡터화.
	D3DXVec3Normalize(&m_vFireDir, &m_vFireDir);

	//최대 사거리 산출.
 	SetMaxRange(fMaxRange);	

 
	//탄환의 초기 발사 속도.. 탄환은 이 속도에서 점차 감소하게 된다.
	//총구 발사 속도 = 발사 방향 * (발사 속도 + 기체 속도(*))
	m_vMoveVelo = m_vFireDir * m_fFireVelo;	 
	m_vRotVelo = VECTOR3(0, 0, 0);			//회전값은 필요 없다.


	//저항값 들. : 미사용
	m_fResistance = fMaxRange/m_fFireVelo;		  
	m_fGravity = 0.00f; 
;

}




//////////////////////////////////////////////////////////////////////////////
//
// 총알의 최대 사거리 계산 메소드.
//
// 최대 사거리는 '상대값'으로 입력 받지만, 
// 사용시에는 월드에서의 이동량 누계로 환산합니다.
//
void CBullet::SetMaxRange(float fMaxRange)
{
	m_fMaxRange = fMaxRange;					//최대 사거리 (스칼라 저장)
	m_vMaxRange = fMaxRange * m_vFireDir;		//사거리 (벡터)

	//총알 이동 거리 누계 벡터.(최대 사거리 비교用)
	m_vMoveAccu = VECTOR3(0, 0, 0);		

}





//////////////////////////////////////////////////////////////////////////////
//
int CBullet::Move(float dTime/* =0 */)
{

	//------------------------------------------------------------------//
	// 스케일링 : 모델 좌표 계											//	
	//------------------------------------------------------------------//
	D3DXMatrixScaling(&m_mScale, m_vScale.x, m_vScale.y, m_vScale.z);

	//------------------------------------------------------------------//
	// 회전 처리 : 모델 좌표 계											//
	//------------------------------------------------------------------//
	//-> 필요 없음.


	//------------------------------------------------------------------//
	// 이동 처리														//
	//------------------------------------------------------------------//
	//속도 계산...
	//1. 발사 속도에서 점차 줄어든다. (저항값 적용)
	//2. 이에 더하여  '중력'이 적용된다. (월드 -Y 축)

	//발사 방향의 역방향(역벡터) * 저항값 만큼 속도는 감소한다.
 	//... 
	
	
	//중력 저항
	//...
	 	
	// 총알 순간 이동 거리 계산.: 거리 = 순간 속도  * 시간.
	VECTOR3 dist = m_vMoveVelo * dTime;		

 	// 총알 위치 (월드.절대) 계산.
 	m_vPos += dist;

	// 사거리 체크를위해  총알 이동거리 누계. 
	m_vMoveAccu += m_vMoveVelo*dTime;		 


	// 사거리 보다 멀리가면...'제거'의 의미로 false 를 리턴한다. 
	// 수정. 2004.06.21. 
  	if(D3DXVec3Length(&m_vMoveAccu) > D3DXVec3Length(&m_vMaxRange))
	{
 		return FALSE;
	}

	// 이동행렬 구성.
  	D3DXMatrixTranslation(&m_mTranslation, m_vPos.x, m_vPos.y, m_vPos.z);


	//------------------------------------------------------------------//
	// TM 만들기.														//
	//------------------------------------------------------------------//
	MATRIX temp;
	D3DXMatrixMultiply(&temp, &m_mScale, &m_mRotation);
	D3DXMatrixMultiply(&m_mTM, &temp, &m_mTranslation);


	return TRUE;
}




//////////////////////////////////////////////////////////////////////////////
//
int CBullet::Draw(float dTime/*=0*/)   
{
	//컬링후 보이지 않으면..그리지 않는다. 2004.06.25.
	if(!IsVisible()) return TRUE;	


	// 등록된 메쉬가 없으면 그냥 'true' 리턴. : 수정 2004.10.20.
	if(m_pMesh == NULL) return TRUE;		
	
	m_pDev->SetTransform(D3DTS_WORLD, &m_mTM);
	m_pMesh->Render(m_pDev);

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////////
//
void CBullet::Release()
{
	//N/A
}



/********************  end of file "bullet.cpp" ****************************/
