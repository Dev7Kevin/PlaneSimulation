// SMoke.h: interface for the CSmoke class.
//
//
////////////////////////////////////////////////////////////////////////////// 
  
#pragma once
 

#include "Unit.h"
#include "Camera.h"

//////////////////////////////////////////////////////////////////////////////
//
// class CSmoke : 미사일의 '연기'등에 사용될 클래스.
//
class CSmoke : public CUnit  
{
	//스프라이트 포인터...
	B3SPRITE m_Spr;

	//'연기' 색상.
	COLOR	m_Color;

	//빌보딩을 위한 카메라 포인터 필요.
	CCamera** m_ppCamera;

	//'유지 시간' : 몇초간 하늘에 떠 있을지...
	int		m_ShowTime;
	//float	m_ElapsedTime;		//경과 시간.(생긴지 몇초 지났는지..)
	int		m_NowTime;
	int		m_OldTime;

	//'페이드 아웃  관련. : 슬슬 사라지기....  
	BOOL		m_bFadeOut;			//페이드 아웃 실행 여부.
	int			m_FadeTime;			//페이드 아웃 실행 시간.
	float		m_fAlpha;			//페이드 아웃을 위한 알파값.
	
	
public:

	CSmoke(
		 LPDEVICE pDev,				//디바이스 포인터.
		 CB3Mesh* pMesh,			//메쉬 포인터.

		 B3SPRITE* pSpr,				//스프라이트 포인터.
		 CCamera** ppCamera,		//카메라 이중포인터. 
		 int		ShowTime,		//유지 시간.
		 int		FadeTime,		//페이드 시간.
		 COLOR		Color,			//연기 색상.
		 
		 VECTOR3 vPos,				//'월드 위치'
		 VECTOR3 vScale = VECTOR3(1, 1, 1),			//스케일.
		 VECTOR3 vRot = VECTOR3(0, 0, 0),			//'회전'
  		 VECTOR3 vMoveVelo = VECTOR3(0, 0, 0),		//이동속도.
		 VECTOR3 vRotVelo = VECTOR3(0, 0, 0)		//회전 속도.
		 );

	virtual ~CSmoke();


	// 기본 메소드.
	virtual int Move(float dTime=0);
	virtual int Draw(float dTime=0);   
	virtual void Release();


};


/****************** end of file "Smoke.h" ********************************/ 
