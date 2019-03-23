// Enemy.cpp: implementation of the CEnemy class.
//
//
////////////////////////////////////////////////////////////////////////////// 


#include "Enemy.h"


/////////////////////////////////////////////////////////////////////////////
//
int CEnemy::m_EnemyCnt = 0;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEnemy::CEnemy(
		LPDEVICE	pDev,			//디바이스 포인터.
		CB3Mesh*	pMesh,			//메쉬 포인터.
 		UINT		hp,				//적기 체력.
		float		fRadius,		//충돌구 반경.
		LPANIINFO	pBoomAniInfo,	//폭파 애니메이션 정보 구조체.04.06.23+ 
									//NULL 이면 무시된다.
		VECTOR3 vPos, 
		VECTOR3 vRot/* = VECTOR3(0.0f, 0.0f, 0.0f)*/, 
		VECTOR3 vScale/*= VECTOR3(1.0f, 1.0f, 1.0f)*/,
		VECTOR3 vMoveVelo/* = VECTOR3(0.0f, 0.0f, 0.0f)*/,
		VECTOR3 vRotVelo/* = VECTOR3(0.0f, 0.0f, 0.0f)*/
 		 )
:CPlane(pDev, pMesh, hp, 
		fRadius, pBoomAniInfo, 
		vPos, vRot, vScale, vMoveVelo, vRotVelo)
{
	 
	m_SNum = m_EnemyCnt++;

#ifdef _DEBUG
	B3D_Error(NO_, "[info] No.%d 적기(0x%x) 생성 /  현재 적기 수 : %d.",
				m_SNum, this, m_EnemyCnt);
#endif

	//2004.11.11  적기 기동관련.
	m_ThrottleState = 0;
	m_ThrottleStateOldTime = ::GetTickCount();
	m_PitchRollState = 0;
	m_PitchRollStateOldTime = ::GetTickCount();

	//2004.11.15.
	m_bLockON = NO_;
}



////////////////////////////////////////////////////////////////////////////
//
CEnemy::~CEnemy()
{
	--m_EnemyCnt;

#ifdef _DEBUG
	B3D_Error(NO_, "[info] No.%d 적기(0x%x) 제거. /  현재 적기 수 : %d. ",
				m_SNum, this, m_EnemyCnt);
#endif

	Release();
}






/////////////////////////////////////////////////////////////////////////////
// 기본 메소드.
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
//
// int CEnemy::Move(float dTime/* =0 */) - 적기 이동. 
// 
//
int CEnemy::Move(float dTime/* =0 */)		
{	
	//-------------------------------------------------------------------
	// 이동 처리.
	//-------------------------------------------------------------------
	static const float fac1 = 100.0f;	//<- 최고 속력이라고 생각하면 된다.

	{
		int nowtime = GetTickCount();
		if(nowtime - m_ThrottleStateOldTime > (1000/(rand()%3+1)))
		{
			m_ThrottleStateOldTime = nowtime;
			m_ThrottleState = rand()%5;
		}
	}

	switch(m_ThrottleState)
	{
	case 0: 
	case 1:
	case 2:
	case 3:
		// 가속 페달.. Throttle 
		m_vMoveVelo.z += (fac1 * dTime);		
		break;
	case 4:
		break;

	}


	//-------------------------------------------------------------------
	// 비행기 조종간-Stick- 처리. 
	//-------------------------------------------------------------------
	static float fac2 = D3DX_PI;	//<- 최고 회전값.

	{
		int nowtime = GetTickCount();
		if(nowtime - m_PitchRollStateOldTime > (1000/(rand()%3+1)))
		{
			m_PitchRollStateOldTime = nowtime; 
			m_PitchRollState = rand()%5;
		}
	}

	switch(m_PitchRollState)			
	{
	case 0:
		// Stick 을 왼쪽으로 기울기..: 왼쪽 뱅크.
		m_vRotVelo.z += fac2*dTime; 
		break;
	case 1:
		// Stick 을 오른쪽으로 기울기..: 오른쪽 뱅크.
		m_vRotVelo.z -= fac2*dTime; 
		break;
	case 2:
		// Stick 을  민다..'하강' 
		m_vRotVelo.x += fac2/2*dTime;		 
		break;
	case 3:
		// Stick 을  잡아당긴다..'상승' 
		m_vRotVelo.x -= fac2/2*dTime;      
		break;
	default:
		break;
	}


	//-------------------------------------------------------------------
	// 키 반응이 없으면 '감속' : 저항을 적용한다.
	// 1초간 키반응이 없으면 속도는 0 이 된다. 
	//-------------------------------------------------------------------
	m_vMoveVelo *= (1.0f - dTime);
	m_vRotVelo *= (1.0f - dTime);	


	return CPlane::Move(dTime);			
}



/////////////////////////////////////////////////////////////////////////////
//
// int CEnemy::Draw(float dTime/*=0*/)   - 적기 출력 
//
int CEnemy::Draw(float dTime/*=0*/)   
{
	//
	// 만약 죽었다면..폭파 애니 출력.
	//
	if(m_bIsDead) 
		return DrawBoom(dTime);				//특별히 오버로딩 하지 않고, 그냥사용.

	//
	// 아직 살아 있다면...정상 출력.
	//
	return CPlane::Draw(dTime);      
}



/////////////////////////////////////////////////////////////////////////////
//
void CEnemy::Release()
{
	//N/A
}

 

/****************** end of file "Enemy.h" **********************************/

