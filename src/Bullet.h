// Bullet.h: interface for the CBullet class.
//
//
////////////////////////////////////////////////////////////////////////////// 
 
#pragma once

#include "Unit.h"

class CCamera;

//
// class CBullet : 총알을 관장하는 클래스 (ADT)
//
class CBullet : public CUnit  
{	 
protected:
	//
	// 총알 관련 변수들.
	//
	UINT m_uDmg;				//총알 데미지.

	//사격시 발사 속도.: 이 속도에서 점차 감소한다..(*)
	float m_fFireVelo;		
	float m_fResistance;		//속도 저항값.(*) 
	float m_fGravity;			//중력 저항값.(*)

	VECTOR3 m_vFireDir;			//사격 방향.	
	 
	VECTOR3 m_vMaxRange;		//최대 사거리(벡터)
	float	m_fMaxRange;		//최대 사거리.(스칼라)	2004.06.21.
	VECTOR3 m_vMoveAccu;		//총알 이동 거리 누계 벡터.(최대 사거리 비교用)
	 

	//
	// '레일건', 미사일 등의 '불꽃' 효과를 위한 스프라이트 정보 : 2004.06.21.  
	//
	SPRDATA		m_Spr;			//총알 메쉬용 3D 스프라이트 개체. 외부에서 설정되는 것을 복사해 사용한다. 2004.06.21+
	COLOR		m_Color;		//총알 색상.

	// - 미사일의 '불꽃'을 처리하기 위해 CRailGun 에서 상위클래스로 올림. 
	// 카메라 2중 포인터. :  pCamera = CameraSet[x] 에서 사용되는 pCamera 는 현재 할당된 
	// 카메라의 포인터의 복사본이므로  다른 카메라로 변경되었을 경우 
	// 날아가고 있는 총알은 카메라의 빌보드 행렬을 얻을 수가 없다.   
	// 따라서 '원본'-CameraSet[x]- 을 레일건 클래스에서 알아야하므로 이중포인가 필요하다. 
	CCamera**	m_ppCamera;		



protected:
	
	CBullet(
		 LPDEVICE	pDev,
		 CB3Mesh*	pMesh,
 		 UINT		Dmg,				//총알 데미지.
 		 float		fFireVelo,			//총구 발사 가속도
		 VECTOR3	vFireDir,			//발사 방향.
		 float		fMaxRange,			//최대 사거리...(발사 위치에서의 '상대값')
		 
		 SPRDATA*	pSpr,				//총알용 스트라이트 포인터..  
		 //pMesh, pSpr 둘중에 하나는 있어야 한다.
		 CCamera**	ppCam,				//카메라 이중 포인터. 2004.06.19. 
		 COLOR		Color,				//총알 컬러. 
 
		 float		fRadius,			//충돌구 반경.
		 LPANIINFO	pBoomAniInfo,		//폭파 애니메이션 정보 구조체.04.06.23+ 
										//NULL 이면 무시된다.
		 
		 VECTOR3	vPos,				//총알 발사 위치.
		 VECTOR3	vRot = VECTOR3(0.0f, 0.0f, 0.0f),	 
		 VECTOR3	vScale = VECTOR3(1.0f, 1.0f, 1.0f),	 
 		 VECTOR3	vMoveVelo = VECTOR3(0.0f, 0.0f, 0.0f),
		 VECTOR3	vRotVelo = VECTOR3(0.0f, 0.0f, 0.0f)
 		 );
public:

	//총알 추가 메소드.
 	const VECTOR3& GetFirDir() const { return m_vFireDir; }
	const VECTOR3& GetMaxRange() const { return m_vMaxRange; }
	float GetMaxRangeF() const { return m_fMaxRange; }				
	float GetMoveAccu() { return D3DXVec3Length(&m_vMoveAccu); }		
 	
	const UINT&	GetDmg() const { return m_uDmg; }	

	//'Bullet'의 최대 사거리 산출 메소드. 
	void SetMaxRange(float fMaxRange);

	//'Bullet'초기화 메소드. 
	void InitBullet(float fMaxRange);


	virtual ~CBullet();

	// 기본 메소드.
	virtual int Move(float dTime=0) = 0;
	virtual int Draw(float dTime=0) = 0;   
	virtual void Release() = 0;

};



/************** "end of file "Bullet.h" ***************************/ 
