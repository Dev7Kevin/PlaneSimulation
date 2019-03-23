/////////////////////////////////////////////////////////////////////////////
//
// Hero.cpp:  주인공 비행기 관련 소스 파일
//
//
/////////////////////////////////////////////////////////////////////////////

#include "Hero.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHero::CHero(
			 LPDEVICE pDev,
			 CB3Mesh* pMesh,
			 UINT hp,
			 
			 float		fRadius,			//충돌구 반경.
			 LPANIINFO	pBoomAniInfo,		//폭파 애니메이션 정보 구조체.04.06.23+ 
											// NULL 이면 무시된다.


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
	//n/a	
}

////////////////////////////////////////////////////////////////////////////
//
CHero::~CHero()
{
	Release();
}




////////////////////////////////////////////////////////////////////////////
//
// 주인공 이동 처리. 
//
// 엑셀키를 누르면 가속도를 증가시킨다.
//
int CHero::Move(float dTime/*=0*/)
{ 		 
	//-------------------------------------------------------------------
	// 이동 처리.
	//-------------------------------------------------------------------
	//static const float fac1 = 100.0;	//순간 최대 추력 (100m/s) 

	//엔진의 추력(Throttle) 의 증감. 
	//if(IsKeyDown(DIK_LSHIFT))			//추력 증가.
	//	m_vMoveVelo.z += (fac1 * dTime);		
	//if(IsKeyDown(DIK_LCONTROL))			 
	//	m_vMoveVelo.z -= (fac1 * dTime);
	m_fMaxVelo = 27.7f * 3.0f;	//시속 300km/h max. 

	if(IsKeyDown(DIK_LSHIFT)) 	 
	{
		m_fThrottle += 30.0f * dTime;  // 트로틀 증가율 : 30%/sec  
		if(m_fThrottle >= 110.0f)		//100 % 가 최고...
			m_fThrottle = 110.0f;		// 엔진-시동 off 없이..그냥 '정지' 로 처리... 
	}

	if(IsKeyDown(DIK_LCONTROL))
	{
		m_fThrottle -= 30.0f * dTime;  // 트로틀 감소율 : 30%/sec  
		if(m_fThrottle <= 0.001f)
			m_fThrottle = 0.0f;
	}

	//이동 속도 결정.. : 현재는 쓰로틀에 단순 대응시킨다. 
	m_vMoveVelo.z = m_fMaxVelo * m_fThrottle * 0.01f;	//Max.m/s * (0.0 ~ 1.1) : 0~110%	



	//-------------------------------------------------------------------
	// 비행기 조종간-Stick- 처리. 
	//-------------------------------------------------------------------
	static float fac2 = D3DX_PI;	//순간 최대 선회율(180도/s)

	// Stick 을 왼쪽으로 기울기..: 왼쪽 뱅크.
	if(IsKeyDown(DIK_LEFT) && !IsKeyDown(DIK_RIGHT)) //동시에 누를 수는 없다!! 
	{
		m_vRotVelo.z += fac2*dTime; 
	}

	// Stick 을 오른쪽으로 기울기..: 오른쪽 뱅크.
	if(IsKeyDown(DIK_RIGHT) && !IsKeyDown(DIK_LEFT))  
	{
		m_vRotVelo.z -= fac2*dTime; 
	}

	// Stick 을  민다..'하강' 
	if(IsKeyDown(DIK_UP) && !IsKeyDown(DIK_DOWN))  
	{	
		m_vRotVelo.x += fac2 * dTime;		 
	}

	// Stick 을  잡아당긴다..'상승' 
	if(IsKeyDown(DIK_DOWN) && !IsKeyDown(DIK_UP))  
	{
		m_vRotVelo.x -= fac2 * 0.7f *dTime;      
	} 



	//-------------------------------------------------------------------
	// 러더 : 추후 업데이트.
	//-------------------------------------------------------------------	 
	/*if(IsKeyDown(DIK_COMMA) && !IsKeyDown(DIK_PERIOD))  
	{
	m_vRotVelo.y += fac2 * 0.3f * dTime;		 
	}	 
	if(IsKeyDown(DIK_PERIOD) && !IsKeyDown(DIK_COMMA))  
	{
	m_vRotVelo.y -= fac2 * 0.3f * dTime;      
	}*/


	//-------------------------------------------------------------------
	// 키 반응이 없으면 '감속' : 저항을 적용한다.
	// 1초간 키반응이 없으면 속도는 0 이 된다. 
	//-------------------------------------------------------------------
	//m_vMoveVelo *= (1.f - dTime); 
	m_vRotVelo *= (1.f - dTime); 


	//-------------------------------------------------------------------
	//'속도'가 높아지면..기체를 살짝 '흔들어' 준다. : 2004.07.01.
	// 최고속도에서 85% 이상시..
	//-------------------------------------------------------------------
	if(fabs(m_vMoveVelo.z) >= (m_fMaxVelo * 0.85f)) 
	{
		m_vShake.x = (rand()%2)?1.0f:-1.0f;		//적당히...
		m_vShake.y = (rand()%2)?1.0f:-1.0f; 
		m_vShake.z = (rand()%2)?1.0f:-1.0f;

		m_vShake *=  (1.0f - dTime)* 0.01f;

	}
	else 
		m_vShake = VECTOR3(0, 0, 0);



	//나머지는 부모에서 처리. 
	return CPlane::Move(dTime);
}




////////////////////////////////////////////////////////////////////////////
//
int CHero::Draw(float dTime/*=0*/)   
{

	//----------------------------------------------//
	// 주인공은 컬링할 필요가 있을까?				//	
	//----------------------------------------------//
	//if(!IsVisible()) return TRUE;	



	// 원하는 코드를 추가 할것.
	//주인공이 죽었다면...
	if(m_bIsDead)
		return DrawBoom(dTime);	//폭파 애니 출력. : 나중에 주인공만의 
								//폭파 애니를 위해 오버로딩 할 것이다.



	//나머지는 부모에서 처리. 
	return CPlane::Draw(dTime);      
}



////////////////////////////////////////////////////////////////////////////
//
void CHero::Release()
{
	// 원하는 코드를 추가 할것.
	//N/A
}


/************************* end of file "Hero.cpp" **************************/