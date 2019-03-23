// Tornado.h: interface for the CTornado class.
//
//
////////////////////////////////////////////////////////////////////////////// 

#pragma once
 
  
#include "RailGun.h"

///////////////////////////////////////////////////////////////////////////////
//
// class CTornado : 토네이도 총알 클래스 
//
//////////////////////////////////////////////////////////////////////////////
class CTornado : public CRailGun
{ 
protected:

	// 나선형 발사 코드.
	VECTOR3		m_vSpiralPos;
	float		m_fSpiralVal;		
	
	MATRIX		m_mFireRot;		//유닛의 발사시 회전행렬.

public:
 
	//생성자.
	CTornado(
		LPDEVICE pDev,
		CB3Mesh* pMesh,			//총알용 메쉬 포인터.
		SPRDATA* pSpr,			//총알용 스트라이트 포인터.. 둘중에 하나는 있어야 한다.

		CCamera** ppCam,		//카메라 이중 포인터. 2004.06.19. 수정.

		UINT	 Dmg,			//총알 데미지.
		COLOR	 Color,			//총알 컬러.
		float	fFireVelo,		//총구 발사 가속도
		VECTOR3 vFireDir,		//발사 방향. 
		MATRIX	mFireRot,		//발사시, 유닛의회전행렬...
		float	fMaxRange,		//최대 사거리...

		float	 fRadius,		//충돌구 반경.

		VECTOR3 vPos,			//총알 발사 위치.
		VECTOR3 vScale,			//총알 크기. CBullet 과 생성자 파라미터 순서주의
		VECTOR3 vRot = VECTOR3(0.0f, 0.0f, 0.0f),  
		VECTOR3 vMoveVelo = VECTOR3(0.0f, 0.0f, 0.0f),
		VECTOR3 vRotVelo = VECTOR3(0.0f, 0.0f, 0.0f)
		 );

	virtual ~CTornado();

	// 기본 메소드.
	virtual int Move(float dTime=0);
	virtual int Draw(float dTime=0);   
	virtual void Release();

 

};




/********************** end of file "Tornado.h" *************************/ 
