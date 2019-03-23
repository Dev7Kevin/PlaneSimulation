// RailGun.h: interface for the CRailGun class.
//
//
////////////////////////////////////////////////////////////////////////////// 

#pragma once
 

#include "Bullet.h"
#include "Camera.h"				//'빌보딩'을 위해 카메라 정보가 필요.


///////////////////////////////////////////////////////////////////////////////
//
// class CRailGun : 레일건 총알 클래스  
//
//////////////////////////////////////////////////////////////////////////////
class CRailGun : public CBullet
{
//------------------------------------------------------------------------	
//#define RAILGUN_TEST_			//총알 테스트용.
#ifdef RAILGUN_TEST_		 
	static int	m_RailGunSNCnt;	//총 발사수 카운트 변수. (발사된후 제거 된것 포함)
	int			m_RailGunSN;	//발사된 레일건 번호. : 완전 개발용.
#endif
//------------------------------------------------------------------------	

	//레일건 '생성'수 : 발사된 레일건 수.
	static int m_RailGunCnt;	 

 
	//
	// 레일건 관련 변수들  
	//
	// * 이전에 사용되던 m_pSpr / m_Color / m_ppCamera 는 CMissile 의 처리와 
	// * 공유하기 위해 상위 클래스인 CBullet 으로 올림. 
	// * 생성자는 변경 없음.
	//   

	
public:
 
	//생성자.
	CRailGun(
		LPDEVICE pDev,
		CB3Mesh* pMesh,			//총알용 메쉬 포인터.
		SPRDATA* pSpr,			//총알용 스트라이트 포인터.. 둘중에 하나는 있어야 한다.

		CCamera** ppCam,		//카메라 이중 포인터. 2004.06.19. 수정.

		UINT	 Dmg,			//총알 데미지.
		COLOR	 Color,			//총알 컬러.
		float	fFireVelo,		//총구 발사 가속도
		VECTOR3 vFireDir,		//발사 방향.
		float	fMaxRange,		//최대 사거리...

		float	 fRadius,			//충돌구 반경.

		VECTOR3 vPos,			//총알 발사 위치.
		VECTOR3 vScale,			//총알 크기. CBullet 과 생성자 파라미터 순서주의
		VECTOR3 vRot = VECTOR3(0.0f, 0.0f, 0.0f), 
		VECTOR3 vMoveVelo = VECTOR3(0.0f, 0.0f, 0.0f),
		VECTOR3 vRotVelo = VECTOR3(0.0f, 0.0f, 0.0f)
		 );

	virtual ~CRailGun();

	// 기본 메소드.
	virtual int Move(float dTime=0);
	virtual int Draw(float dTime=0);   
	virtual void Release();

	//레일건 발사된 개수 얻기.
	static int& GetRailGunCnt() { return m_RailGunCnt; }

};




/********************** end of file "RailGun.h" *************************/ 
