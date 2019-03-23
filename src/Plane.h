/////////////////////////////////////////////////////////////////////////////
//
// Plane.h:  비행 유닛 관련 헤더 파일. 
//
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
 
#include "Unit.h"

//
//  '비행기' 클래스 : 각종 비행유닛들은 이 클래스에서 파생 된다.
//
class CPlane : public CUnit  
{
protected : 

	int		m_Hp;				//UINT -> INT 로 전환.



	//2004.06.10.
	//'모델'의 중심축을 기준으로 '조종'을 위한 3가지 벡터 추가. 
	VECTOR3	m_vUp,				//상방 벡터.
			m_vHorz,			//수평 벡터.
			m_vNView;			//시선 벡터.
	 

	//비행기 자세 제어를 위한 백업백터들. 		
	VECTOR3 m_vOrgUp, m_vOrgHorz, m_vOrgNView;		


	float m_fMaxVelo;		//최대순간 속도 : m/s.  100km/h = 27.7m/s
	float m_fThrottle;		//쓰로틀 량 (0~110%) : 110% 는 After-Bunner
	float m_fPower;			//Power = N * m/s... 사용하지 않고 있음.

	// 모델의 현재 (방향 + 순간속도) 변수 => 현재 속도 : 미사일 발사에 필요.
	//
	VECTOR3 m_vCurrDirVelo;		//ex) m_vPos += m_vCurrDirVelo * dTime;



	// 2004.07.01.
	//'워프-고속 비행' & '기체 손상 -데미지' 의 표현울 위한 데이터 
	// : '살짝궁 흔들어준다' 이때, '카메라' 의 흔들림에 영향을 미친다.
	// (조정석 카메라 ONLY!)
	VECTOR3 m_vShake;


protected:
	 
	
 	//생성자.: protected 로 바꿈.
	CPlane(
		LPDEVICE pDev,
		 CB3Mesh* pMesh,
 		 UINT hp,

		 float		fRadius,			//충돌구 반경.
		 LPANIINFO	pBoomAniInfo,		//폭파 애니메이션 정보 구조체.	04.06.23+ 
										//NULL 이면 무시된다.
 		 VECTOR3 vPos, 
		 VECTOR3 vRot = VECTOR3(0.0f, 0.0f, 0.0f), 
		 VECTOR3 vScale = VECTOR3(1.0f, 1.0f, 1.0f),
 		 VECTOR3 vMoveVelo = VECTOR3(0.0f, 0.0f, 0.0f),
		 VECTOR3 vRotVelo = VECTOR3(0.0f, 0.0f, 0.0f)
 		 );


	//모델 수직/수평 백터 구하기.
	void	SetVectors();
 

public:

	virtual ~CPlane();

	//리셋 메소드 오버로딩.
	void Reset();

	
	//모델 축 기준 회전 메소드.
	MATRIX	LRotateX(float angle);
	MATRIX	LRotateY(float angle);
	MATRIX	LRotateZ(float angle);
	MATRIX  LRotate(VECTOR3 vAngle);

	//모델 회전 벡터 구하기.
	//const VECTOR3& GetModelRotVec() const { return m_vModelRot; }
	//const MATRIX& GetModelRotMtx() const { return m_mModelRot; }
	//const VECTOR3& GetEye() const { return m_vEye; }
	//const VECTOR3& GetLookAt() const { return m_vLookAt; }
	const VECTOR3& GetUp() const { return m_vUp; }
	const VECTOR3& GetHorz() const { return m_vHorz; }
	const VECTOR3& GetNView() const { return m_vNView; } 

	float GetThrottle() { return m_fThrottle; };	 
	float GetMaxVelo() { return m_fMaxVelo; };		 
	float GetVelo();								 
	float GetSpeed();

	//비행기 현재 방향+순간속도 얻기 2004.06.21+
	const VECTOR3 GetCurrDirVelo() const { return m_vCurrDirVelo; }

	//기체의 '흔들림' 값 얻기.
	const VECTOR3& GetShake() const { return m_vShake; }	


	//공통 기본 메소드.
	virtual int Move(float dTime=0) = 0;
	virtual int Draw(float dTime=0) = 0;   
	virtual void Release() = 0;


	//충돌 관련 추가 메소드.
	BOOL TakeDamage(int dmg);	//갱신.
	enum {
		EMY_DEAD_  = FALSE,
		EMY_ALIVE_ = TRUE
	};

};


/****************** end of file "Plane.h" *******************************/ 
