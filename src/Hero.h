/////////////////////////////////////////////////////////////////////////////
//
// Hero.h:  주인공 클래스 비행기 관련 헤더 파일.
//
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
 
#include "Plane.h"

//
// 주인공 클래스 
//
class CHero : public CPlane  
{
protected:

	
public:
	//CHero();				 
	CHero(
		 LPDEVICE pDev,
		 CB3Mesh* pMesh,
 		 UINT hp,
		 
		 float		fRadius,			//충돌구 반경.
		 LPANIINFO	pBoomAniInfo,		//폭파 애니메이션 정보 구조체.04.06.23+ 
										// NULL 이면 무시된다. 
		 VECTOR3 vPos, 
		 VECTOR3 vRot = VECTOR3(0.0f, 0.0f, 0.0f), 
		 VECTOR3 vScale = VECTOR3(1.0f, 1.0f, 1.0f),
 		 VECTOR3 vMoveVelo = VECTOR3(0.0f, 0.0f, 0.0f),
		 VECTOR3 vRotVelo = VECTOR3(0.0f, 0.0f, 0.0f)
		 );

	virtual ~CHero();

 	virtual int Move(float dTime=0);
	virtual int Draw(float dTime=0);   
	virtual void Release();

};

//class Assist : public CP

/***************** end of file "Hero.h" *******************************/ 
