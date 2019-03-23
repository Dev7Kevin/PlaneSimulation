/////////////////////////////////////////////////////////////////////////////
//
// Unit.h:   최상위 유닛 클래스의 소스 파일.
//
//
/////////////////////////////////////////////////////////////////////////////
 
#pragma once
 
#include "../../Engine/Ext/B3Yena_Ext.h"


class CCamera;	

typedef struct {
	//N/A
}UNITINFO, *LPUNITINFO;


#include "Frustum.h"	


//////////////////////////////////////////////////////////////////////////////
//
// 폭파 등의 애니메이션을 위한 정보 구조체
//
typedef struct {

	VECTOR		vScale;			//폭파 스케일.
	int			OldTime;		//폭파 애니를 위한 시간 변수.
	int			NowTime;

	int			Frm;			//폭파 애니시 현재  프레임 카운터 변수.
	int			TotFrm;			//폭파 애니 총 프레임수.
	int			FPS;			//폭파 애니의 '초당 프레임수' : 애니메이션 속도.

	SPRDATA*	pSpr;			//폭파 애니 용 스프라이트 프레임.

	CCamera**	ppCamera;		//카메라용 이중포인터. : 빌보딩때문에 필요.	

}ANIINFO, *LPANIINFO;






//////////////////////////////////////////////////////////////////////////////
//
// 최상위 클래스 : CUnit
// 
class CUnit  
{
protected:
	//
	// 유닛의 변환 정보
	//
	VECTOR3 m_vPos;			//유닛의 좌표
	VECTOR3	m_vRot;			//유닛의 회전각.( 각 축별 라디안값 )
	VECTOR3 m_vScale;		//유닛의 크기 값. ( 각 축별)


	MATRIX m_mTranslation;
	MATRIX m_mRotation;
	MATRIX m_mScale;
	MATRIX m_mTM;			//유닛의 월드 변환 행렬.


	//
	// 유닛의 일반 정보.
	//
 	VECTOR3	m_vMoveVelo;		//이동 가속도. : 유닛의 이동 거리 = 시간 * 이동가속도 
	VECTOR3	m_vRotVelo;			//회전 가속도. : 유닛의 회전량 = 시간 * 회전가속도 
	//VECTOR3 m_vWorldPosition;	//유닛의 월드 좌표. : 게임상의 '월드'에서의 좌표.
	
	// 유닛의 현재 이동방향.
	// 전방과는 다른의미 : ex: 드리프트..
	VECTOR3 m_vDirection;		
	VECTOR3 m_vOrgDirection;		//초기방향.

	//
	// 유닛의 메쉬 정보.
	//
	CB3Mesh*	m_pMesh;

	//디바이스 포인터.
	LPDEVICE	m_pDev;


	//정적 데이터.
	static UINT m_UnitCnt;

	//
	//  '위치 리셋' 용 백업 변수.
	//
	VECTOR3 m_vOrgPos;
	VECTOR3 m_vOrgRot;
	VECTOR3 m_vOrgScale;


	//
	// '충돌구 반경' 멤버 데이터.
	//
	float	m_fCollRadius;		

	//
	// 폭파 애니를 위한 데이터.: 040623+
	//
	BOOL		m_bIsDead;			//죽었냐...=> 폭파 애니를위한 플래그 이다.
	ANIINFO		m_BoomAniInfo;		//폭파 애니메이션용 정보들...



	//
	// 컬링용 데이터. : 04.06.25.
	//
	//  '출력','이동'에 관련된 모든것을에서 사용될 것이다.
	//
	static CFrustum* m_pFrustum;			
	static BOOL		 m_bUseFrustumCulling;	//컬링 사용하기. 


protected:

	//생성자 
	//CUnit();
	CUnit(VECTOR3 vPos,				//'월드 위치'
		 VECTOR3 vRot,				//'회전'
		 VECTOR3 vScale,			//스케일.
 		 VECTOR3 vMoveVelo,			//이동속도.
		 VECTOR3 vRotVelo,			//회전 속도.
		 
		 float		fRadius,		//충돌구 반경.
		 LPANIINFO	pBoomAniInfo,	//폭파 애니메이션 정보 구조체.04.06.23+

		 CB3Mesh* pMesh,			//메쉬 포인터.
		 LPDEVICE pDev				//디바이스 포인터.
		 );


	// 내부 메소드 :  TM 을 만든다.
	void SetTM(); 

public:

	//소멸자.
	virtual ~CUnit();
	
	//유닛 개수 리턴.
	static UINT& GetUnitCount();
	
	// 기본 메소드.
	virtual int Move(float dTime=0) = 0;
	virtual int Draw(float dTime=0) = 0;		 
	virtual void Release() = 0;

	// 멤버 접근자.
	void SetPos(VECTOR3& rvPos) { m_vPos = rvPos; }			

	const VECTOR3& GetPos() const { return m_vPos; }
	const VECTOR3& GetRot() const { return m_vRot; }			
	const VECTOR3& GetMovVelo() const { return m_vMoveVelo;}	
	const VECTOR3& GetRotVelo() const { return m_vRotVelo;}
	

	const MATRIX& GetTransMtx() const { return m_mTranslation;}	
	const MATRIX& GetRotMtx() const   { return m_mRotation;}	
	const MATRIX& GetScaleMtx() const { return m_mScale;}		
 	const MATRIX& GetTM() const { return m_mTM; }

	//유닛 방향 돌리기.
	//void RotDir(VECTOR3* pvRot = NULL);
	//void RotDir(MATRIX* pmRot = NULL);
	const VECTOR3& GetDir() const { return m_vDirection; };	


	// 유닛을 월드좌표로 직접 이동시키기.
	void WMove(VECTOR3 pos);
	void WRotate(VECTOR3 rot);
	void WScale(VECTOR3 scl);

	// 유닛의 위치를 리셋시킨다.
	void Reset();




	//
	// 충돌 체크 관련
	//
	enum {
		COLL_YES_ = YES_,		// 오케!! 충돌.
		COLL_NO_  =	NO_			// 충돌 않됬음.
	};

	//충돌 검사 하기.
	BOOL CheckCollision(CUnit* pTarget, float fTargetRadius=0.0f);
	//충돌 구의 반경 구하기.
	const float& GetRadius() const { return m_fCollRadius; }			

	//
	// 폭파 애니 관련
	//
	//죽었냐 - 폭파 중인지 여부...?
	const BOOL& IsDead() const { return m_bIsDead; }				
	void SetDead();				//유닛을 '죽게' 합니다..=> 폭파 애니 연출.
 
	//폭파 애니 출력 : 기본스타일이 싫다면, 오버로딩 가능 : 엡데이트.
	virtual BOOL	DrawBoom(float dTime=0);	 
	const ANIINFO&	GetAniInfo() const { return m_BoomAniInfo; }	


	//
	// 컬링 관련.
	//
	//컬링을 통해 화면에 보이는 것으로 판정되면  TRUE 를 리턴한다.
	const BOOL	IsVisible();						

	//컬링 객체 설정하기.
	static void InitFrustum(CFrustum* pFrustum)		
	{ m_pFrustum = pFrustum;  m_bUseFrustumCulling = YES_; }

	//외부에서 컬링 On/Off 하기.
	static const BOOL& SetFrustumCullingState(BOOL bUse){ return (m_bUseFrustumCulling = bUse); }	

	//현재 컬링 상태 얻기.
	static const BOOL& GetFrustumCullingState(){ return m_bUseFrustumCulling; }			


};


/******************* end of file "Unit.h" *******************************/ 
