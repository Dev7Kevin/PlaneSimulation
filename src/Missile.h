// Missile.h: interface for the CMissile class.
//
//
////////////////////////////////////////////////////////////////////////////// 

#pragma once
 
//#include "Plane.h"
#include "Bullet.h"
#include "ArrayList.h"				//'연기' 관리를 위해 ArrayList 사용.
 



//////////////////////////////////////////////////////////////////////////////
//
// class CMissile : 미사일 클래스 ( 일단 비유도. None-Guided) 
//
// 1. 주인공 기체에서 부착되어 움직인다.
// 2. 발사 버튼이 눌리면 파일런에서 'Release' 된다.
// 3. 이후, 로켓부스터 점화.  '비행' 시작. 
// 2. 발사 된후 자신의 'Seeker'를 통해 목표를 추적한다.(Fire & Forget) : => 보류.
// 3. 비행중에는 '연기'를 후방으로 '뿜어낸다' 
// 4. 최대 사거리를 넘어서면 '자폭' 한다.
//
//////////////////////////////////////////////////////////////////////////////

class CMissile : public CBullet  
{
protected:	
	//
	// 미사일 정보..
	//
	CPlane*		m_pPlane;			//미사일이 부착될 '비행기' 포인터. 
									//여기에서 변환 행렬값을 얻어온다.: ex) 주인공.

	//하드포인트 위치.( 발사되기전의 미사일의 위치값을 계산하는데 사용한다)
	//m_vPos - 이동하면서 누적되며 발사 이후 이것을 월드 좌표로 사용한다.
	VECTOR3		m_vHardPt;			

 	BOOL		m_bIsFired;			//발사 여부...발사 되었다면..연기/이동 등의 연산이 필요. 

	VECTOR3		m_vAstPos;


	// 
	// 미사일 화염 관련.
	//
	VECTOR3		m_vSmokeScale;		//'화염', '연기' 스케일.
	VECTOR3		m_vSmokePos;		//'화염', '연기' 위치(미사일 후방:부스터 위치)
									//입력은 모델공간의 상대값 :  출력은 월드 공간의 위치값.
	VECTOR3		m_vSmokePosW;		//'연기', '불촉' 생성위치.(월드 공간 절대값)
	
	//B3SPRITE*	m_pSprSmoke;		//'연기'를 위한 3D 스프라이트 포인터.

	//
	// '연기' 관련 데이터들.
	//
	CArrayList* m_pSmokeList;		//연기 리스트...
	int			m_OldTime;			//연기 출력 시간 변수들.
	int			m_NowTime;
	B3SPRITE	m_sprSmoke;			//연기 스프라이트..


	//
	// 미사일 발사 관련 : 2004.06.21+
	//
	BOOL	m_bIsReleased;		//'폭탄', '미사일'등이 하드포인트에 분리 되었는가...여부.
	BOOL	m_bIsReleasedAst;
	float	m_fRelaseLength;	//'릴리즈' 길이. 
	VECTOR3 m_vReleasePos;		//릴리즈 좌표.
	VECTOR3 m_vRelAstPos;
	VECTOR3 m_vReleaseTimePlaneDirVelo;		//릴리즈 시 비행기 속도 : 릴리즈 될때의 비행기 속도는 
											//미사일의 '릴리즈' 속도가 된다.
 	
	//
	// 미사일 추가 데이터 : 
	//
	VECTOR3 m_vUp;				//미사일의 상방 벡터.
	VECTOR3 m_vNView;			//미사일의 시선 벡터.
	VECTOR3 m_vHorz;			//미사일의 수평 벡터.

	float	m_fCurrVelo;		//미사일의 현재 가속도 (누적) 
 	

	//
	// 폭파 애니관련... 2004.06.24.
	//
	//VECTOR3 m_vBoomSize;			//-> 필요 없음. 
	float	m_fBoomSphereRaidus;	//폭파 구 반경. 
	int	    m_BoomShowTime;			//폭파 유지 시간...

	float	m_fBoomSphereMaxRadius;	//폭파시  '화염구' 의 반경. 
									// : 폭사시..이것의 사이즈가 커지면서..
									//   주변의 유닛까지 데미지를 입힌다.

	BOOL	m_bFadeOut;				//폭파구..사라지기.
	float	m_fAlpha;				//폭파구 알파값.

	// 충격파 객체 
	CShockWave*	m_pShockWave;	


	//미사일 연료 관련.
	float	m_fRocketFuel;				//로켓 연료... 다떨어 지면..자폭한다.
	float	m_fMaxFlyVelo;				//미사일 최대 비행 속도. : m_fMaxRange 대신에사용. 2004.11.12.
	float	m_fMaxAccelSpeed;			//미사일 최대 가속도.

	//유도 관련.
	CPlane*	m_pTarget;					//목표 유닛.
	BOOL	m_bHommingON;				//유도 작동시...ON!



protected:

	void PutSmoke(float dTime);				//스모크 출력 : 미사일이 이동시 
											//일정시간마다 연기를 배출한다.

	void SetHardPointPos();						//미사일 위치 설정.
	void SetSmokePos(VECTOR3* pvPos = NULL);	//미사일 화연& 연기 좌표 산출.


public:

	CMissile(
		LPDEVICE	pDev,
		CB3Mesh*	pMesh,			//미사일용 메쉬 포인터.

 		CCamera**	ppCam,			//카메라 이중 포인터. 2004.06.19. 수정.: 발사 화면 때문에.
  		UINT		Dmg,			//미사일데미지.
		
		float		fMaxFlyVelo,	//미사일의 최대 비행 속도.
		VECTOR3		vFlyDir,		//미사일의 비행 방향.	
 
		float		fMaxFuel,		//최대 연료. (사거리->연료 개념으로 전환. 2004.11.12.)

 		CPlane*		pPlane,			//부착될 비행기 포인터.
		BOOL		bIsFired,		//발사 되었나?

		B3SPRITE*	pSprFire,		//미사일 화염용 스트라이트 포인터.. 
		VECTOR3		vSmokePos,		//'연기', '불촉' 생성위치.(모델공간 상대값)
 		VECTOR3		vSmokeScale,	//'화염', '연기' 스케일.
		COLOR		Color,			//화염 컬러.
		B3SPRITE*	psprSmoke,		//연기 스프라이트..
		
		CArrayList* pList,			//스모크 리스트..(생성된 연기를 등록. 외부에서 관리하게 된다)

		float		fRadius,		//충돌구 반경.
		LPANIINFO	pBoomAniInfo,	//폭파 애니메이션 정보 구조체.04.06.23+ 
									//NULL 이면 무시된다.

		LPANIINFO	pShockWaveAniInfo,	//폭파 애니메이션 정보 구조체. 
										//: NULL 이면 무시된다.

		VECTOR3 vPos,			//미사일발사 위치 : 즉 비행기의 
								//'하드 포인트 Hard Point' 위치이다.
								//(비행기 모델공간에서의 '상대 위치')

		VECTOR3 vScale,			//미사일'크기'
		VECTOR3 vRot = VECTOR3(0.0f, 0.0f, 0.0f), 
 		VECTOR3 vMoveVelo = VECTOR3(0.0f, 0.0f, 0.0f),
 		VECTOR3 vRotVelo = VECTOR3(0.0f, 0.0f, 0.0f)


 		 );

	virtual ~CMissile();

protected:
	//----------------------------------------------------------------------------------//
	// 미사일의 '이동'은  크게 3단계로 나누어 진다.	  									//
	//																					//		
	// 1단계 : 비행기의 '하드 포인트' 에 장착되어 비행기와 함께 움직이는 경우 : "On HP" //
	//			=> CMissile::MoveOnHardPoint()											//
	// 2단계 : 발사버튼이 눌려 '하드 포인트'에서 '떨이지는 단계' : "RELEASE"			//
	//			=> CMissile::MoveReleaseTime()											//	
	// 3단계 : 릴리즈 후, '부스터'의  점화에 의해 '비행' 하는 단계 : "FLY"				//	
	//          => CMissile::MoveFly()													//
	//----------------------------------------------------------------------------------//
	int MoveOnHardPoint(float dTime = 0);
	int MoveReleaseTime(float dTime = 0);
	int AstMoveReleaseTime(float dTime = 0);
	int MoveFly(float dTime = 0);

	//미사일 유도 관련. 2004.11.11. 
	int CalHommingRotMtxAst(float dTime = 0.0f);
	int CalHommingRotMtx(float dTime = 0.0f);			//목표에 대한 회전 행렬 계산.
	int MoveHomming(BOOL bHomming, float dTime = 0.0f);	//목표에 미사일 유도 하기.

public:

	// 기본 메소드.
	virtual int Move(float dTime=0);
	virtual int Draw(float dTime=0);   
	virtual void Release();

	// 추가 메소드.
	const BOOL& IsReleased() const { return m_bIsReleased; }	//'릴리즈' 되었냐? 
	const BOOL& IsReleasedAst() const { return m_bIsReleasedAst; }	//'릴리즈' 되었냐? 
 	const BOOL& IsFired() const  { return m_bIsFired; }		//'로켓 점화' 되었냐? 
	
	//미사일 발사-시킴.
  	void SetFire();		

	const VECTOR3& GetSmokePos() const { return m_vSmokePosW; }
	float GetCurrVelo() const { return m_fCurrVelo; }		  
	//const float& GetCurrentSpeed() const { return m_fCurrVelo; }	
	const float& GetRocketFuel() const { return m_fRocketFuel; }		


	//미사일 '폭파' 애니 오버라이딩. 2004.06.24.
	virtual BOOL DrawBoom(float dTime = 0);	

	//폭파 판정.: 오버라이딩. 2004.06.24+
	virtual void SetDead();			

	//충돌함수 오버라이딩. 2004.06.24+.
	virtual BOOL CheckCollision(CUnit* pTarget, float fTargetRadius=0.0f);	

	//미사일 유도 관련. 2004.11.11. 
	void LockOn(CUnit* pEmy);						//목표를 Lock-On 한다. (목표 설정) 
	void LockOff(){ m_pTarget = NULL; }				//목표 해제.
	CUnit* GetTarget() const { return m_pTarget; }	



 };




/********************* end of file "Missile.h" *****************************/ 
