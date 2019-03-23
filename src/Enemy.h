// Enemy.h: interface for the CEnemy class.
//
//
////////////////////////////////////////////////////////////////////////////// 
 
#pragma once
 
#include "Plane.h"

//
// class CEnemy : 적 비행기 클래스 
//
class CEnemy : public CPlane  
{
protected:
	
	static int m_EnemyCnt;	//적기 카운터.

	//추가 데이터....
	int	m_SNum;				//적기 시리얼 넘버.


	// 적기 기동 관련 추가 멤버.
	int		m_ThrottleState;		//적기의 엔진 추력 상태..
	int		m_ThrottleStateOldTime;

	int		m_PitchRollState;		//핏치, 롤 상태..
	int		m_PitchRollStateOldTime;



	//'Lock-On' 체크 변수. 2004.11.15.
	BOOL	m_bLockON;							


public:
	CEnemy ( 
		 LPDEVICE	pDev,			//디바이스 포인터.
		 CB3Mesh*	pMesh,			//메쉬 포인터.
 		 UINT		hp,				//적기 체력.
		 float		fRadius,		//충돌구 반경.
		 LPANIINFO	pBoomAniInfo,	//폭파 애니메이션 정보 구조체.04.06.23+ 
									//NULL 이면 무시된다.
		 VECTOR3 vPos,				//적기 위치.
		 VECTOR3 vRot = VECTOR3(0.0f, 0.0f, 0.0f), 
		 VECTOR3 vScale = VECTOR3(1.0f, 1.0f, 1.0f),
 		 VECTOR3 vMoveVelo = VECTOR3(0.0f, 0.0f, 0.0f),
		 VECTOR3 vRotVelo = VECTOR3(0.0f, 0.0f, 0.0f)
		 );

	virtual ~CEnemy();

	//기본 메소드.
	virtual int Move(float dTime=0);
	virtual int Draw(float dTime=0);   
	virtual void Release();

	//추가 메소드.
	const int& GetEnemyCnt() const { return m_EnemyCnt; }
	 

	//2004.11.15.
	const BOOL& IsLockON() const { return m_bLockON; }				
	void SetLockON(BOOL bLockOn = YES_) { m_bLockON = bLockOn; }		

};


/******************** end of file "Enemy.h" ***************************/
 
