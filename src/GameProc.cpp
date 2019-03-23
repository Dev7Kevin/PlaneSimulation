//////////////////////////////////////////////////////////////////////////////
//
// GameProc.cpp : 각 스테이지별 관리모쥴 소스파일.
//
//
//////////////////////////////////////////////////////////////////////////////


//#include "../../Engine/B3X.h"
//#include "../../Engine_Ext/Engine.h"
#include "../../Engine/Ext/B3Yena_Ext.h"	//DLL 버전 엔진 필수헤더. 


#include "Camera.h" 

#include "Unit.h"
#include "ShockWave.h"			 

#include "Plane.h"
#include "Hero.h" 
#include "Assist.h"
#include "Bullet.h"		
#include "RailGun.h"	
#include "Tornado.h"		
#include "Missile.h"		//미사일 관련
#include "Smoke.h"			//미사일 연기 관련.
#include "PlasmaCannon.h"	//플라즈마캐논. 
#include "Enemy.h"			//적기체.
#include "list"				
#include "Frustum.h"		// 절두체 컬링 클래스. 2004.06.25. 
#include "Star.h"			//별 관련.
#include "SkyBox.h"			//스카이 박스 관련. 2004.06.30. 
#include "Instrument.h"		

#include "Radar.h"			//3D 레이더 관련.


#include "GameProc.h"


#include "list"
using namespace std;

//////////////////////////////////////////////////////////////////////////////
//
namespace GP
{ 
	//------------------------------------
	// 도움말 
	//------------------------------------
	BOOL _bShowHelp = FALSE;


	//------------------------------------
	//주인공 관련.
	//------------------------------------
	CB3Mesh*	_pPlane = NULL;
	VECTOR3		_HeroPos = VECTOR3(0, 0, 0);
	FLOAT		_HeroRot = 0.0f;
	CHero*		_pHero = NULL;

	//------------------------------------
	//주인공 보조관련.
	//------------------------------------
	CB3Mesh*	_pAssistMesh = NULL;
	VECTOR3		_vAssistPos = VECTOR3(-3, 3, 2);
	FLOAT		_fAssistoRot = 0.0f;
	CAssist*	_pAssist = NULL;

	//보조 미사일 관련
	//
	#define ASTMISSILE_MAX_ 4
	CMissile* _pAMissile[ASTMISSILE_MAX_] = { NULL,};


	//------------------------------------
	// 카메라
	//------------------------------------
	enum {  CAM_FREE_, 
			CAM_PILOT_, 
			CAM_FIX_COMBATVIEW_,	//기수앞, 전투용 조망.
			CAM_FIX_FRONTVIEW_,		//전방 조망.
			CAM_FIX_LEFTVIEW_,		//좌측 조망.
			CAM_FIX_RIGHTVIEW_,		//우측 조망.
			CAM_FIX_REARVIEW_,		//후방 조망.
			CAM_TRACKING_, 
			
			CAM_MAX_ 
	};		 
	CCamera*	_pCurrCamSet[CAM_MAX_] = {NULL, };
	CCamera*	_pCurrCam = NULL; 
	UINT		_CurrCamNum =CAM_FREE_;		//현재 카메라 번호.

 	BOOL		_bUseFixCamera = FALSE;			

 
	//------------------------------------
	// 배경색 전환.
	//------------------------------------
	COLOR ChangeBkColor(BOOL bChange = YES_);



	//------------------------------------
	//파티클용 스프라이트.
	//------------------------------------
	B3SPRITE	_sprParticle;
	BOOL		_bBillBoardON = TRUE;
	B3SPRITE	_sprMap;



	//------------------------------------
	//뷰포트 on!
	//------------------------------------
	BOOL		_bUseViewport = NO_;


	//------------------------------------
	// 레일건 관련.
	//------------------------------------
	//#define _RailGun_Max_  1000
	const UINT _RailGun_Max_ = 500;
	CRailGun* _pRailGun[_RailGun_Max_] = { NULL, };
	CB3Mesh*  _pRailGunMesh = NULL;		//우리는 3D 스프라이트를 사용할 것이다.
	B3SPRITE  _sprRailGun;
	B3SPRITE  _sprTornado;		


	//------------------------------------
	//미사일 연기 관련.
	//------------------------------------
	CArrayList* _pSmokeList = NULL;
	SPRDATA		_sprParticle2;  


	//------------------------------------
	// 미사일 관련. 
	//------------------------------------
	#define MISSILE_NUK_ 3
	#define MISSILE_MAX_ 7
	CMissile* _pMissile[MISSILE_MAX_] = { NULL, };
	CB3Mesh*  _pMeshMissile = NULL;
	BOOL	  _bShowMisInfo = FALSE;

	//목표 해제 관련...
	void MissileLockOff(CUnit* pTgt);


	//
	// 미사일 장착 위치 - Hard Point 위치.
	// => 이 값은 '비행기'에서 얻어와야 하지만, 일단 이렇게 사용해 본다.
	//
	VECTOR _vHardPoints[MISSILE_MAX_] = { 

		{ 3.5f, -0.8f, -2.0f },			//오른쪽 1번.
		{ 2.5f, -0.8f, -2.0f },			//오른쪽 2번.
		{ 1.5f, -0.8f, -2.0f },			//오른쪽 3번.

		{ 0.0f, -1.5f, -1.5f },			//가운데 HP

		{ -3.5f, -0.8f, -2.0f },			//왼쪽 1번.
		{ -2.5f, -0.8f, -2.0f },			//왼쪽 2번.
		{ -1.5f, -0.8f, -2.0f },			//왼쪽 3번.

	};

	VECTOR _vAstHardPoint[MISSILE_MAX_] = {
		{-0.5f, -0.6f, 0.0f},
		{-0.5f, 0.6f, 0.0f},
		{0.5f, -0.6f, 0.0f},
		{0.5f, 0.6f, 0.0f},
	};




	//-----------------------------------------------------------------------
	// 폭파 애니메이션 관련.
	//
	#define EXPLOSION_MAXFRAM_ 8
	SPRDATA _ExplosionSpr[EXPLOSION_MAXFRAM_];	//폭파 애니 전역 스프라이트.

	void ExplosionSpriteLoad();
	void ExplosionSpriteDraw(float dTime);   
	void ExplosionSpriteRelease();

	//
	// 작은 폭파 애니메이션 정보... : 시간관련 멤버등은 사용시에 설정할 것있습니다.. 
	// 
	ANIINFO _AniInfoBoomSmall = { 
		{20.0f, 20.f, 20.0f}, 0, 0, 0, 
		EXPLOSION_MAXFRAM_,  1000/10, _ExplosionSpr, &_pCurrCam
	};

	ANIINFO _AniInfoBoomBig = { 
		{100.0f, 100.f, 1.0f}, 0, 0, 0, 
		EXPLOSION_MAXFRAM_,  1000/10, _ExplosionSpr, &_pCurrCam
	};
 

	//
	//  미사일 폭파 스프라이트 관련.
	//
	// 미사일 폭파 스프라이트 관련.
	#define EXPLOSION_MISSILE_MAXFRM_ 1						
	SPRDATA _MissileBoomSpr[EXPLOSION_MISSILE_MAXFRM_];	

	// : 각각의 크기/ 애니메이션 시간 축소/변경.(T08 TDBox)
	// 
	ANIINFO _AniInfoMissileBoomSmall = { 
		{15.0f, 15.f, 1.0f}, 0, 0, 0, EXPLOSION_MISSILE_MAXFRM_,  
		1000/35, _MissileBoomSpr, &_pCurrCam						  
	};

	ANIINFO _AniInfoMissileBoomBig = { 
		{25.0f, 25.f, 1.0f}, 0, 0, 0, EXPLOSION_MISSILE_MAXFRM_,  
		1000/35, _MissileBoomSpr, &_pCurrCam
	};

	//충격파 애니메이션 정보. 
	// : 각각의 크기/ 애니메이션 시간 축소/변경.(T08 TDBox)
	#define SHOCKWAVE_MAXFRM_ 1								
	SPRDATA _ShockWaveSpr[SHOCKWAVE_MAXFRM_];				

	ANIINFO _AniInfoShockWaveBig = { 
		{15.0f, 15.f, 1.0f}, 0, 0, 0, SHOCKWAVE_MAXFRM_,  
		1000/35, _ShockWaveSpr, &_pCurrCam
	};
	ANIINFO _AniInfoShockWaveSmall = { 
		{25.0f, 25.f, 1.0f}, 0, 0, 0, SHOCKWAVE_MAXFRM_,  
		1000/35, _ShockWaveSpr, &_pCurrCam
	};




	//-----------------------------------------------------------------------
	// 
	// 충돌 관련...
	// 
	void CheckCollisionAll();
	void ChkColl_HBulletVSEnemy();
	void ChkColl_HMissileVSEnemy();	
	void ChkColl_HeroVSEnemy();				//주인공 vs 적기.


	//-----------------------------------------------------------------------
	//
	// 적기 관련.
	//
	#define ENEMY_MAX_ 2  //(2*2*2) : 8대가 생긴다.)
	//#define ENEMY_MAX_ 10  //(10*10*2) : 200대가 생긴다.)
	using namespace std;
	typedef list<CEnemy*>::iterator EmyItor_;
	list<CEnemy*> _EnemyList;

	void EnemyInit();
	void EnemyCreate(CB3Mesh* pMesh);
	void EnemyMove(float dTime);
	void EnemyDraw(float dTime);		
	void EnemyRelease();



	//-----------------------------------------------------------------------
	//
	//  게임 상태 체크 관련. : 2004.06.24. 
	//
	BOOL GameOver(float dTime);
	BOOL _bIsGameOver = FALSE;		//게임 오버: 주인공이 죽고..폭파 애니가 끝나고..
	//그후에 페이디 아웃...이것을 관리하는 전역 변수.
	BOOL _bEngineStop = FALSE;



	//
	// 절두체 컬링 관련.
	//
	// 컬링의 대상은 : 적 비행기 / 적총알 & 미사일
	//               : 주인공 총알 / 미사일 & 미사일 연기 
	//               : 전투공간의 '물체' 들.. 
	// => 각각의 Draw 루틴에서 컬링체크후 렌더링을 시도합니다.
	//
	CFrustum* _pFrustum = NULL;
	BOOL	  _bUseFrustumCulling = YES_;		//컬링 시도 전역 플래그.
	int		  _VisibleEnemyCnt = 0;				//보이는 적기 대수. (컬링후 남은 적기수)
	


	//
	//  '별' 관련..
	//
	//
	CStar* _pStars = NULL;
	BOOL	_bShowStars = YES_;	


	//
	// 스카이박스 관련. 
	//
	BOOL		_bShowSkyBox = YES_;
	CSkyBox*	_pSkyBox = NULL;
	SPRDATA		_SprSkyBox[CSkyBox::SBF_MAX_];

	BOOL SkyBoxInit();			//스카이 박스용 데이터 설정.
	void SkyBoxRelease();		//스카이 박스용 데이터 제거.




	//
	// 레이다 관련
	//
	CRadar*	_pRadar = NULL;
	SPRDATA _sprRadarV;
	BOOL RadarInit();
	void RadarRelease();

	void RadarDrawTDBox(float dTime);	//Lock-On 된 목표물의 정보 출력(+TDBox)
	void _RadarDrawTDBoxRect(CEnemy* pEmy, float dTime);	//★
	void _RadarDrawFireQue(CEnemy* pEmy, float dTime);	//★
	//void _RadarDrawTDBoxRect(VECTOR3 vPos);	//★
	//void _RadarDrawFireQuey(VECTOR3 vPos);	//★

	//VECTOR3 Cal2DPos(CEnemy* pEmy);		//3D -> 2D 좌표 변환 함수.
	VECTOR3 Cal2DPos(VECTOR3 vPos);		//3D -> 2D 좌표 변환 함수.

	SPRDATA	_sprTDBoxSmall;				//'TD Box Small' : 2004.12.07.★
	SPRDATA	_sprTDBoxBig;				//'TD Box Big' : 2004.12.07.★
	SPRDATA	_sprFireQue;			//'Fire Que' : 2004.12.07.★
	CEnemy* _pLockOnEmy = NULL;		//Lock-On 된 적기 포인터.
	
	BOOL	_bShowAzimuth = FALSE;	//★

};


using namespace GP;










//////////////////////////////////////////////////////////////////////////////
//
BOOL GP::Loading()
{
	// 데이터폴더로 작업 폴더 변경 : 
	// 메쉬 파일의 텍스쳐는 합니다수 내부에서  로드되기 때문에 
	// 텍스쳐가 있는 path 설정으로 외부에서 '작업' 폴더를 변경해 주어야 합니다.
	//	 
	char currdir[256];
	::GetCurrentDirectory(256, currdir);
	::SetCurrentDirectory("../data");
  

	//주인공 메쉬 로드.	
	_pPlane = new CB3Mesh(g_B3pDevice, "airplane03.x");
	ASSERT(_pPlane != NULL); 

	 
	//주인공 클래스 객체 생성.
	_pHero = new CHero(g_B3pDevice, _pPlane, 
						1,					//Hp. 
						5.0f,				//충돌 반지름.
						&_AniInfoBoomSmall,//폭파 애니메이션 정보. 
						_HeroPos, 
						VECTOR3(0.0f, 0.0f, 0.0f));
	ASSERT(_pHero != NULL);


	_pAssistMesh = new CB3Mesh(g_B3pDevice, "bigship1.x");
	ASSERT(_pAssistMesh != NULL); 
	 
	//주인공 클래스 객체 생성.
	_pAssist = new CAssist(g_B3pDevice, _pAssistMesh, 
						1,					//Hp. 
						5.0f,				//충돌 반지름.
						&_AniInfoBoomSmall,//폭파 애니메이션 정보. 
						_vAssistPos, 
						VECTOR3(0.0f, 0.0f, 0.0f));
	ASSERT(_pAssist != NULL);




	//파티클 텍스쳐 로드.
	B3DSprite_Load(&_sprParticle, "Particle.bmp");
	B3DSprite_Scaling(&_sprParticle, 10.0f, 10.0f);				 
	B3DSprite_Translation(&_sprParticle, 0.0f, 0.0f, 0.0f, ON3D_);		 
	

	// 바닥 배경용 2D 메쉬.
	B3DSprite_Load(&_sprMap, "earth01.jpg");
	B3DSprite_Scaling(&_sprMap, 320, 240); 
	B3DSprite_Translation(&_sprMap, 0.0f, 0.0f, 300.0f, ON3D_);	

	//레일건 용 파티클로 데이터 공유.
	_sprRailGun = _sprParticle;

	//토네이도 스프라이트 로드.
	B3DSprite_Load(&_sprTornado, "shine0.bmp");
	B3DSprite_Scaling(&_sprTornado, 10, 10); 
	B3DSprite_Translation(&_sprTornado, 0.0f, 0.0f, 0.0f, ON3D_);	



	//미사일 연기용 텍스쳐 
	B3DSprite_Load(&_sprParticle2, "연기2.bmp");
	B3DSprite_Scaling(&_sprParticle2, 1.0f, 1.0f);				 
	B3DSprite_Translation(&_sprParticle2, 0.0f, 0.0f, 0.0f);			//일단 기본값. 

	//미사일 메쉬.
	_pMeshMissile = new CB3Mesh(g_B3pDevice, "너클러3.x");

 
	//폭파 애니용 스프라이트로드.
	ExplosionSpriteLoad();
 


	// 2004.07.02.
	// 미사일 아이콘. : '계기판'의 무장 표시기로 사용할 것이다. 
	// 2D 직교 투영으로 사용할것임.
	WeaponIndicatorLoad();

	//'ADI' 용 메쉬 로드.
	ADILoad();

	//3D Rader 생성.
	RadarInit();

	//2차원 레이더 
	RWRLoad();		



 
	// 적기 초기화.
 	EnemyInit();
	EnemyCreate(_pPlane);		//적기 생성 : 임시. 2004.06.22.


 
	//작업 폴더 복구.
	::SetCurrentDirectory(currdir);

 

	//------------------------------------------------------------------------- 
	// 카메라 셋업 
	//------------------------------------------------------------------------- 
	CameraSetup();	//업데이트.



	//------------------------------------------------------------------------- 
	// 연기 테스트. : '미사일'보다 먼저 만들어야 합니다!! 
	//------------------------------------------------------------------------- 
	SmokeInit();


	//------------------------------------------------------------------------- 
	// 미사일 만들기.
	//------------------------------------------------------------------------- 
	

	AssistMissileCreate(_pAssist, _pMeshMissile, &_sprParticle, &_sprParticle2, 
				 _vAstHardPoint, &_pCurrCam, 0.0f);

	MissileCreate(_pHero, _pMeshMissile, &_sprParticle, &_sprParticle2, 
				 _vHardPoints, &_pCurrCam, 0.0f);

	//------------------------------------------------------------------------- 
	// 플라즈마 캐논 초기화. 
	//------------------------------------------------------------------------- 
	PlasmaCannonInit();


	//-------------------------------------------------------------------------
	// 시야 절두체 컬링 셋팅.
	// -> 프로젝션 행렬의 정보를 넘긴다 : 현재 '카메라'의 렌즈 값이다.
	//-------------------------------------------------------------------------
	_pFrustum = new CFrustum;
	ASSERT(_pFrustum != NULL);

	//절두체 컬링용 객체 초기화.
	_pFrustum->Init(
 		_pCurrCam->GetFovY(),
		_pCurrCam->GetRatio(),  
		_pCurrCam->GetZnear(),  
		_pCurrCam->GetZfar()    
		);
 
	//'유닛' 클래스들에게 할당하기. 
	CUnit::InitFrustum(_pFrustum);							//-> 이 메소드 내부에서 이미 Culling On! 시킨다.
	CUnit::SetFrustumCullingState(_bUseFrustumCulling);	//컬링 상태 설정. 


	//-------------------------------------------------------------------------
	// '별' 만들기...
	//-------------------------------------------------------------------------
	// 별, 2048 개 만들기.
	_pStars = new CStar(g_B3pDevice,  
						200,				//
						&_sprParticle,		//
						_pHero, &_pCurrCam,
		                g_Mode.Width		//
						);		
	ASSERT(_pStars != NULL);


	//--------------------------------------------------------------------------
	// '스카이 박스' 만들기.
	//--------------------------------------------------------------------------
	SkyBoxInit();
 
	return TRUE;
}











//////////////////////////////////////////////////////////////////////////////
//
BOOL GP::Run(float dTime)
{  
	
	//------------------------- 
	// 시스템.
	//------------------------- 
	if(IsKeyUp(DIK_F1))	_bShowHelp  ^= TRUE;			//일반 도움말.
	if(IsKeyUp(DIK_F2))	_bShowMisInfo ^= YES_;			//미사일 도움말. 
	if(IsKeyUp(DIK_F4))	_bShowSkyBox ^= YES_;			//하늘 상자 on/off. 
	if(IsKeyUp(DIK_F5)) _pHero->Reset();				// 비행기 위치 리셋.  
   
	if(IsKeyUp(DIK_F9))									//컬링 On/Off
 	{
		_bUseFrustumCulling = CUnit::GetFrustumCullingState();			
		CUnit::SetFrustumCullingState(_bUseFrustumCulling ^= TRUE);		
 	}

	//'해상도' 변경시를위한 절두체 업데이트.
	/*if(_bUseFrustumCulling)
	{
		_pFrustum->Init(
			_pCurrCam->GetFovY(),
			_pCurrCam->GetRatio(),  
			_pCurrCam->GetZnear(),  
			_pCurrCam->GetZfar()    
			); 
	}*/

	if(IsKeyUp(DIK_F10)) _bShowStars ^= TRUE;			// 별 보여 주기..


	if(IsKeyUp(DIK_V)) _bBillBoardON ^= TRUE;			// 빌보드.-> off.
	if(IsKeyUp(DIK_BACKSPACE))  _bUseViewport ^= TRUE;	// 뷰포트	

 	if(IsKeyUp(DIK_K)) _bShowAzimuth ^= TRUE;			//★ 


	//------------------------ 
	// 주인공 이동 처리. 
	//------------------------ 
	_pHero->Move(dTime);

	_pAssist->Move(dTime);
			

	//------------------------ 
	// '회전에 따른' ADI 표시기 움직이기.
	//------------------------ 
	ADIMove(_pHero); 


	//------------------------ 
	// 무기 생성/ 발사.
	//------------------------ 
 	if(IsKeyDown(DIK_C)) RailGunCreate(dTime);
	if(IsKeyDown(DIK_N)) TornadoCreate(dTime);		
	if(IsKeyDown(DIK_SPACE)) PlasmaCannonCreate(_pHero, &_pCurrCam);	
 
	// 미사일 발사. 
	// 
	if(IsKeyUp(DIK_Z))
		MissileFire(FALSE, FALSE);		//일반 미사일 발사.

	if(IsKeyUp(DIK_X))
		MissileFire(TRUE, FALSE);		//핵미사일 발사!!

	if(IsKeyUp(DIK_Z) && IsKeyDown(DIK_LCONTROL))	//Fire All!!
		MissileFire(TRUE, TRUE);

	if(IsKeyUp(DIK_F7))					//미사일 리셋.
	{
		MissileRelease();
		MissileCreate(_pHero, _pMeshMissile, &_sprParticle, &_sprParticle2,
			_vHardPoints, &_pCurrCam, 0.0f);
	}
  
	if(IsKeyUp(DIK_F8))					//적기 리셋.
	{
		EnemyRelease();
		EnemyCreate(_pPlane);
	}

	if(IsKeyUp(DIK_S))	
		AssistMissileFire();

	if(IsKeyUp(DIK_D))
	{
		AssistMissileRelease();
		AssistMissileCreate(_pAssist, _pMeshMissile, &_sprParticle, &_sprParticle2, 
				 _vAstHardPoint, &_pCurrCam, 0.0f);
	}
	

 

	//----------------------- 
	// 적기 이동.
	//----------------------- 
 	EnemyMove(dTime);


	//------------------------ 
	// 카메라 업데이트. 	
	//------------------------ 
	CameraUpdate(dTime);	


	//------------------------ 
	// 하늘상자 갱신 : 카메라 갱신 이후 처리.
	//------------------------ 
	_pSkyBox->Move(dTime);

	//------------------------ 
	//별 이동..
	//------------------------ 
	if(_bShowStars) _pStars->Move(dTime);


	//------------------------ 
	// 무기.이동.
	//------------------------ 
	MissileMove(dTime);			// 미사일 & 연기 이동.
	SmokeMove(dTime); 
	RailGunMove(dTime);			// 레일건 이동.
 	
	PlasmaCannonMove(dTime);	// 플라즈마.

	AssistMissileMove(dTime);

 
	//레이더 업데이트.
 	RWRUpdate(dTime);
	_pRadar->Update();			
	

	//------------------------ 
	// 충돌 검사.
	//------------------------ 
	ChkColl_HBulletVSEnemy();
	ChkColl_HMissileVSEnemy();	
	ChkColl_HeroVSEnemy();		



	//------------------------ 
	// 렌더링.. 
	//------------------------ 
	if(B3X_BeginScene())		
	{  
		COLOR col = ChangeBkColor();	//배경색 변경
		B3X_BBClear(col);					
 		  

		

		//----------------------------------// 
		// 스카이 박스 그리기.			//
		//----------------------------------// 
		if(_bShowSkyBox) _pSkyBox->Draw();
		//B3Spr_Draw(&_sprMap, ON3D_, NOABLEND_, 1.0f, NOZWRITE_);	// 배경 

		
		//----------------------------------// 
		// 별그리기..
		//----------------------------------// 
		if(_bShowStars) _pStars->Draw(dTime);	 

		//----------------------------------// 
		// 게임오버							//
		//----------------------------------// 
		if(_bIsGameOver == FALSE)			//게임 종료가 아닐때만..주인공을 그립니다..
		{
			if(_pHero->Draw(dTime) && _pAssist->Draw(dTime) == FALSE)		//주인공이 죽고, '폭파 애니' 끝나면 FALSE 가 리턴..
			{					 
				_bIsGameOver = TRUE;		//게임 종료... 
			}
		}
 
		//----------------------------------// 
		// 기타 오브제 : 적기등.. 그리기.
		//----------------------------------// 
		
		EnemyDraw(dTime);							 
  		MissileDraw(dTime);				
		SmokeDraw(dTime);				
 		RailGunDraw(dTime);				
		PlasmaCannonDraw(dTime);			

		AssistMissileDraw(dTime);

		//ParticleDraw(dTime);		 		 

 


		//----------------------------------// 
		// 뷰포트 - 적용...
		//----------------------------------// 
		if(_bUseViewport) ViewportOn(dTime); 


		//----------------------------------// 
		// 인테페이스.						//
		//----------------------------------//  
 		_pRadar->Render(_bShowAzimuth);		//레이더 그리기.★
		 

		ADIDraw();				//ADI 렌더링.
 		
		//WI 렌더링.(WI 는 직교투영으로 처리)
		B3X_2DOn();
		WeaponIndicatorDraw(_pMissile, MISSILE_MAX_);	
		RWRDraw();				//RWR :2D Radar
 		B3X_2DOff();

		RadarDrawTDBox(dTime);			//LOCK-ON 적기 정보 출력.


		//----------------------------------// 
		// GAME OVER - FADE OUT...			//
		//----------------------------------// 
		if(_bIsGameOver)				//게임 종료시..
		{
			if(GameOver(dTime))				//FADE OUT....이 끝나면 TRUE 리턴.
				_bEngineStop = TRUE;						 
		}

 
		//각종 메세지, 도움말.
 		GP::ShowHelp();
		B3X_StockObjsDraw();			//엔진 / 기본 오브제 그리기.


 
		B3X_EndScene();
	}
  
	
	B3X_Flip();			// 화면 전송(Present) 
 

	return TRUE;
}




 

//////////////////////////////////////////////////////////////////////////////
//
void GP::Release()
{
	RailGunRelease();					//레일건 제거

	B3DSprite_Release(&_sprMap);		//배경 스프라이트 제거.   
	B3DSprite_Release(&_sprParticle);	//파티클 用 스프라이트 제거.
	B3DSprite_Release(&_sprTornado);	//토네이도 스프라이트 제거.


	SAFE_DELETE(_pPlane);				//비행기 메쉬 제거.
	SAFE_DELETE(_pHero);				//주인공 제거.

	SAFE_DELETE(_pAssistMesh);
	SAFE_DELETE(_pAssist);
 
	for(int i=0; i<CAM_MAX_; i++)
	{
		SAFE_DELETE(_pCurrCamSet[i]);	//카메라 제거.
	}


	SAFE_DELETE(_pMeshMissile);			//미사일 메쉬 제거.
	MissileRelease();					//미사일 제거.
	SmokeRelease();						//연기 제거.
	PlasmaCannonRelease();				//플라즈마 제거.
 	ExplosionSpriteRelease();		//폭파 애니용 스프라이트 제거.

	SAFE_DELETE(_pFrustum);			//절두체 컬링 객체 제거. 2004.06.25.

	SAFE_DELETE(_pStars);			//'별' 제거.

	SkyBoxRelease();				//스카이 박스 제거. 2004.06.30.

	WeaponIndicatorRelease();		
	ADIRelease();			

	RadarRelease();					//레이더 제거.
	RWRRelease();					

	AssistMissileRelease();

}









////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////
// 
//  레이더 관련.
//
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// 
BOOL GP::RadarInit()
{
	//------------------------------------------------------------------------
	// 레이더 초기화
	B3DSprite_Load(&_sprRadarV, "radar_v.png");

	B3DSprite_Scaling(&_sprRadarV, 16.0f, 128.0f);		 
	D3DXMatrixIdentity(&_sprRadarV.mRot); 
	D3DXMatrixIdentity(&_sprRadarV.mTrans); 

	//TD Box 
	B3DSprite_Load(&_sprTDBoxSmall, "TDBox16.bmp");	//★
	B3DSprite_Scaling(&_sprTDBoxSmall, _sprTDBoxSmall.Width, _sprTDBoxSmall.Height);
	B3DSprite_Load(&_sprTDBoxBig, "TDBox32.bmp");	//★
	B3DSprite_Scaling(&_sprTDBoxBig, _sprTDBoxBig.Width, _sprTDBoxBig.Height);
	B3DSprite_Load(&_sprFireQue, "FireQue32.bmp");	//★
	B3DSprite_Scaling(&_sprFireQue, _sprFireQue.Width, _sprFireQue.Height);


	D3DVIEWPORT9 vp = { (DWORD)(g_B3Mode.Width  * 0.5f), 0 ,		//sx, sy
						180, //(DWORD)(g_B3Mode.Width  * 0.25f),		//width.
						138, //(DWORD)(g_B3Mode.Height * 0.25f),		//height.
						0.0f, 0.0f			//옵션 주의
					};

	_pRadar = new CRadar(	g_B3pDevice,	// DEVICE 포인터
							&_pCurrCam,		// 카메라 포인터
							vp,				// 뷰포인트 정보
							_EnemyList ,	// 적기 list
							_pHero ,		// 주인공 포인터
							&_sprRadarV,	// 레이더 스프라이트
							&_sprParticle	// 파티클 스프라이트
							);
	ASSERT(_pRadar != NULL);

	return TRUE;
 
}


///////////////////////////////////////////////////////////////////////////
// 
void GP::RadarRelease()
{
	SAFE_DELETE(_pRadar);			// 레이더 제거 
	B3DSprite_Release(&_sprRadarV);	// 레이터 텍스쳐 제거

	B3DSprite_Release(&_sprTDBoxSmall); // TD Box 제거.
	B3DSprite_Release(&_sprTDBoxBig); // TD Box 제거.
	B3DSprite_Release(&_sprFireQue); // TD Box 제거.

}







/////////////////////////////////////////////////////////////////////////////
//
// Lock-On 된 목표의 정보 및 TDBox 를 출력한다.
//
void GP::RadarDrawTDBox(float dTime) 
{
	//카메라 번호 체크...: 1,2,3번 카메라 시에만 출력한다.
	if( (   _CurrCamNum == CAM_PILOT_  
		|| _CurrCamNum == CAM_FIX_FRONTVIEW_ 
		|| _CurrCamNum == CAM_FIX_COMBATVIEW_		
		)== FALSE) 
		return;
 	
 	
	//
	// 스캔된 모든 적기에게 TD Box 출력하기.★
	//
	typedef list<CEnemy*>::iterator itor;
	list<CEnemy*>* pScanList = _pRadar->GetScanList(); 
  	for(itor ii = pScanList->begin(); ii != pScanList->end(); ii++) 
	{
		if( (*ii)->IsLockON() ) continue;

		_RadarDrawTDBoxRect( *ii, dTime); 
	}
 
	
	//레이더에서 타겟으로 선정된 적기를 선택한다.
	_pLockOnEmy  = const_cast<CEnemy*>(_pRadar->GetTarget());


	//
	// 목표물이 없거나..'폭파 중이라면'....
	//
	if(_pLockOnEmy)
	{ 
 		// Lock-ON 된 적기에게 Fire Que  출력하기.★
		//
		_RadarDrawFireQue( _pLockOnEmy, dTime);	
 	}
	else
	{
		int x = g_B3Mode.Width/2 - 8*4; 
		int y = g_B3Mode.Height/2 + 50;	 
		B3X_DrawText(FALSE, x, y, COLOR(0.0f, 1.0f, 0.0f, 1.0f), ">> NO TGT");
		//return; @fix. 2009.07.03.★
	}

 
}



 
/////////////////////////////////////////////////////////////////////////////
//
//void GP::_RadarDrawTDBoxRect(VECTOR3 vPos)
void GP::_RadarDrawTDBoxRect(CEnemy* pEmy, float dTime)
{
	// 
	// : bug fix. 
	// : Cal2DPos() 함수 사용시 , 적 유닛이 주인공 기체의 뒷편에 있을때도, 
	//   TDBox 가 주인공 기체의 정면에 표시되는 것을 막기위한 코드.
	//
	VECTOR3 vViewPos, vPos = pEmy->GetPos();
	MATRIX mView = _pCurrCam->GetViewMtx();
	D3DXVec3TransformCoord(&vViewPos, &vPos, &mView);		
	if(vViewPos.z < 0) return ;


	//
	//'2D 좌표' 변환 행렬 구하기.
	//
	VECTOR3 vP = Cal2DPos( vPos );	//적 위치를 2D 화면 좌표로 변환.★


 	//int x = static_cast<int>(vP.x);
	//int y = static_cast<int>(vP.y);

#if 0
	/*{	//비행기 위치 출력.
		sprintf(msg, "xy(%.3f, %.3f)", vP.x, vP.y);
		int xx = g_B3DisplayMode.Width - strlen(msg) * 16; 
		int yy = 300;
		B3X_DrawText( FALSE, xx, yy, COLOR(0.0f, 1.0f, 0.0f, 1.0f));
	}*/
#endif	


	//TBDox 사각형 출력좌표 화면영역 보정.
	float px = vP.x - _sprTDBoxSmall.Width/2;		float py = vP.y - _sprTDBoxSmall.Height/2;
	//if( px < 0 ) px = 0; if( px > (g_B3Mode.Width - _sprTDBoxSmall.Width)) px = (g_B3Mode.Width - _sprTDBoxSmall.Width);
	//if( py < 0 ) py = 0; if( py > (g_B3Mode.Height - _sprTDBoxSmall.Height)) py = (g_B3Mode.Height - _sprTDBoxSmall.Height);

	//TDBox 출력. 
	B3X_2DOn(); 
	B3DSprite_Translation(&_sprTDBoxSmall, px, py, 1.0f);
	B3DSprite_DrawFX(&_sprTDBoxSmall, COLOR(0.0f, 1.0f, 0.0f, 1.0f));
	B3X_2DOff();
 
}





/////////////////////////////////////////////////////////////////////////////
//
//void GP::_RadarDrawFireQue(VECTOR3 vPos)
void GP::_RadarDrawFireQue(CEnemy* pEmy, float dTime)
{

	// 
	// : bug fix. 
	// : Cal2DPos() 함수 사용시 , 적 유닛이 주인공 기체의 뒷편에 있을때도, 
	//   TDBox 가 주인공 기체의 정면에 표시되는 것을 막기위한 코드.
	//
	VECTOR3 vViewPos, vPos = pEmy->GetPos();
	MATRIX mView = _pCurrCam->GetViewMtx();
	D3DXVec3TransformCoord(&vViewPos, &vPos, &mView);		
	if(vViewPos.z < 0) return ;



	//
	//'2D 좌표' 변환 행렬 구하기.
	//
	VECTOR3 vP = Cal2DPos( vPos );	//적 위치를 2D 화면 좌표로 변환.★


	//int x = static_cast<int>(vP.x);
	//int y = static_cast<int>(vP.y);

#if 0
	/*{	//비행기 위치 출력.
		sprintf(msg, "xy(%.3f, %.3f)", vP.x, vP.y);
		int xx = g_B3DisplayMode.Width - strlen(msg) * 16; 
		int yy = 300;
		B3X_DrawText( FALSE, xx, yy, COLOR(0.0f, 1.0f, 0.0f, 1.0f));
	}*/
#endif	




	// 목표물이 있다면..
	static BOOL bShow = YES_;
	static float time = 0.0f;
	if((time += dTime) > .7f)
	{
		time = 0.0f;
		bShow ^= 1;		
	}


	//TBDox 사각형 출력좌표 화면영역 보정.
	float px = vP.x - _sprFireQue.Width/2;		float py = vP.y - _sprFireQue.Height/2;
	//if( px < 0 ) px = 0; if( px > (g_B3Mode.Width - _sprTDBoxSmall.Width)) px = (g_B3Mode.Width - _sprTDBoxSmall.Width);
	//if( py < 0 ) py = 0; if( py > (g_B3Mode.Height - _sprTDBoxSmall.Height)) py = (g_B3Mode.Height - _sprTDBoxSmall.Height);

	//TDBox 출력. 
	
	{
		B3X_2DOn(); 
		
		if(bShow)
		{
			B3DSprite_Translation(&_sprFireQue, px, py, 1.0f);
			B3DSprite_DrawFX(&_sprFireQue, COLOR(0.0f, 1.0f, 0.0f, 1.0f));
		}
		B3DSprite_Translation(&_sprTDBoxBig, px, py, 1.0f);
		B3DSprite_DrawFX(&_sprTDBoxBig, COLOR(0.0f, 1.0f, 0.0f, 1.0f));

		B3X_2DOff();
	}

	//목표 정보 출력 좌표 보정.
 	int x = (int)vP.x + static_cast<int>(_sprTDBoxBig.Width) / 2 + 3;
	int y = (int)vP.y - static_cast<int>(_sprTDBoxBig.Height) / 2 ;

	// 목표가 폭파 중이면...'잘했어요~' 메세지 출력~.

	if(pEmy->IsDead() == YES_)
	{ 
		B3X_DrawText( FALSE, x, y, COLOR(0.0f, 1.0f, 0.0f, 1.0f), "GOOL KILL");
		return;
	}
 
	if(bShow)
	{		 
		B3X_DrawText( FALSE, x, y, COLOR(0.0f, 1.0f, 0.0f, 1.0f), "LOCK-ON");
	}

	// 목표의 기타 정보 출력.
	VECTOR3 vec = pEmy->GetPos(); 
	B3X_DrawText( FALSE, x, y+=20, COLOR(0.0f, 1.0f, 0.0f, 1.0f), 
		"POS[%04.01f %04.01f %04.01f]", vec.x, vec.y, vec.z);


	vec = pEmy->GetNView(); 
	B3X_DrawText( FALSE, x, y+=20, COLOR(0.0f, 1.0f, 0.0f, 1.0f),
		"DIR[%04.01f %04.01f %04.01f]", vec.x, vec.y, vec.z);


	vec.z = D3DXVec3Length(&pEmy->GetCurrDirVelo()); 
	B3X_DrawText( FALSE, x, y+=20, COLOR(0.0f, 1.0f, 0.0f, 1.0f),
		"SPD[%04.01f]", vec.z);

	vec.z = D3DXVec3Length(&VECTOR3(pEmy->GetPos() - _pHero->GetPos()));
	static float oldz = vec.z;
	static BOOL bCloser = YES_;
	bCloser = (oldz > vec.z)?YES_:NO_;
	oldz = vec.z;

	B3X_DrawText( FALSE, x, y+=20, COLOR(0.0f, 1.0f, 0.0f, 1.0f), 
		"DST[%s](%04.01f)", (bCloser)?"▼":"▲", vec.z);
	//"DST[%c][%04.02f]", (bCloser)?'-':'+', vec.z); 
 
}


/////////////////////////////////////////////////////////////////////////////
//
// 3D (월드)좌표를   2D Pixel (화면)좌표로 변환한다.
//
//VECTOR3 GP::Cal2DPos(CEnemy* pEmy)
VECTOR3 GP::Cal2DPos(VECTOR3 vPos)
{

	//해당 유닛의 월드 위치 얻기.
	//VECTOR3 vPos = pEmy->GetPos();

	//변환 행렬 성분 구하기.
	//MATRIX mWorld = pEmy->GetTM();
	MATRIX mView = _pCurrCam->GetViewMtx();
	MATRIX mProj = _pCurrCam->GetProjectMtx();  


	//뷰포트 변환 행렬 계산.
	MATRIX mViewPort; 							 
	D3DXMatrixIdentity(&mViewPort);

	mViewPort._11 = g_B3Mode.Width / 2.0f;		//@fix. 2009.07.30.해상도변경시 오류 수정.
	mViewPort._22 = -(g_B3Mode.Height / 2.0f);	
	mViewPort._41 = g_B3Mode.Width / 2.0f;		
	mViewPort._42 = g_B3Mode.Height / 2.0f;		


	//최종 '2D 좌표' 변환 행렬 구하기. 
	MATRIX mTM = mView * mProj * mViewPort;

	//2D 좌표 계산.(계산 결과는 클라이언트 좌표)
	D3DXVec3TransformCoord(&vPos, &vPos, &mTM);

	//직접 계산하면 아래와 같다.
	//	VECTOR3 vP;
	//	vP.x = vPos.x * mTM._11 + vPos.y * mTM._21 + vPos.z * mTM._31;
	//	vP.y = vPos.x * mTM._12 + vPos.y * mTM._22 + vPos.z * mTM._32;
	//	vP.z = vPos.x * mTM._13 + vPos.y * mTM._23 + vPos.z * mTM._33;

 

	return vPos;

}











////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////




///////////////////////////////////////////////////////////////////////////
// 
// Sky Box 관련.
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
// 
// 스카이 박스용 데이터 로드하기...
//
BOOL GP::SkyBoxInit()
{

	//폴더 변경.
	//data 폴더로 변경.
	char OldDir[256];
	::GetCurrentDirectory(256, OldDir);			//현재 폴더 저장.
	//::SetCurrentDirectory(B3D_GetWorkPath());	//'초기' 작업 폴더 설정.
	::SetCurrentDirectory("../data");			//데이터 폴더로 변경.

	for(int i=0; i<CSkyBox::SBF_MAX_; i++)
	{
		char filename[80];
		sprintf(filename, "SkyBox0%d.bmp", i);
		//sprintf(filename, "SkyBox_Name/SkyBox0%d.bmp", i);	//각 면 테스트.
		//sprintf(filename, "skybox/Sky512_0%d.jpg", i);		//Edge Test.
		B3DSprite_Load(&_SprSkyBox[i], filename);
		//로드 에러時 에러처리 코드 추가....
	}

	::SetCurrentDirectory(OldDir);				//원래 폴더 변경.

	_pSkyBox = new CSkyBox(g_B3pDevice, _pHero, &_pCurrCam, _SprSkyBox);
	if(_pSkyBox == NULL)
	{
		B3D_Error(YES_, "[Error] 스카이 박스 객체 생성 실패...");
		return FALSE;
	}

	return TRUE;
}



///////////////////////////////////////////////////////////////////////////
// 
// 스카이 박스 데이터 삭제.
//
void GP::SkyBoxRelease()
{
	//스카이 박스 객체 제거.
	SAFE_DELETE(_pSkyBox);

	//텍스쳐 제거.
	for(int i=0; i<CSkyBox::SBF_MAX_; i++)
		B3DSprite_Release(&_SprSkyBox[i]);

}




/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////// 
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
//
// 주인공 죽었을 경우...fade out 하고 엔진을 종료합니다.
// 엔진을 종료하기 위해서..TRUE 리턴.
//
BOOL GP::GameOver(float dTime)
{
	//B3DDevice_3DOff(); 			//직교 투영으로 전환.
	static float exittime = 0;
	exittime += dTime;
 
	_CurrCamNum = CAM_TRACKING_;
	_pCurrCam = _pCurrCamSet[ _CurrCamNum ];		//기본카메라.
	
	if(exittime >= 3.0f)
	// '페이드 아웃' 시킨다...
	//if(B3DSprite_Fade(FADE_OUT_, 0.02f) == TRUE)		//fade out 단계는 50 단계로..
	{
		exittime = 0.0f;
		// 모두 종료 되면..브레이크...
		//B3D_EndRender();		//이 합니다수를 호출하지 않고 그냥 나가면 
		//다음 렌더링시 실패하게 됩니다..
		return TRUE;			//엔진 종료를 위해 TRUE 리턴.

	}

	//B3D_EngineSync(35);			//페이드 속도를 맞추기 위해서..


	return FALSE;				// 아직 FADE OUT 끝나지 않았았음.  
}
 



////////////////////////////////////////////////////////////////////////////
//
// 적기 관련.
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

void GP::EnemyInit() 
{
	//초기화... 
}


////////////////////////////////////////////////////////////////////////////

void GP::EnemyCreate(CB3Mesh* pMesh)
{
	CEnemy* pEnemy = NULL;

	//더미 주인공 띄우기.
	float sx = 0.0f, sz = -300.0f;
	for(int j=0; j<ENEMY_MAX_; j++)
	{
		for(int i=0; i<ENEMY_MAX_; i++)
		{
			pEnemy = new CEnemy(g_B3pDevice, pMesh, 1,	5.0f, 
				&_AniInfoBoomSmall,	//폭파 애니 정보. 2004.06.23+
				VECTOR3( sx + i*20.0f, 10.0f, sz + j*30.0f));

			_EnemyList.push_back(pEnemy);		//더미를 리스트에 등록..

			pEnemy = new CEnemy(g_B3pDevice, pMesh, 1, 5.0f, 
				&_AniInfoBoomSmall,	//폭파 애니 정보. 2004.06.23+
				VECTOR3( sx + i*20.0f, -10.0f, sz + j*30.0f));

			_EnemyList.push_back(pEnemy);		//더미를 리스트에 등록..
		}
	}

}

 
/*

//더미 주인공 띄우기. 'ㄱ' 자로 배치하기.
float sx = -50.0f, sz = 0.0f;

for(int i=0; i<ENEMY_MAX_; i++)
{
pEnemy = new CEnemy(g_B3pDevice, pMesh, 1,	5.0f,	
&_AniInfoBoomSmall,	//폭파 애니 정보. 2004.06.23+
VECTOR3( sx + i*100.0f, 30.0f, 10.0f));

_EnemyList.push_back(pEnemy);		//더미를 리스트에 등록..
}


for(int i=0; i<ENEMY_MAX_; i++)
{
pEnemy = new CEnemy(g_B3pDevice, pMesh, 1,	5.0f, 
&_AniInfoBoomSmall,	//폭파 애니 정보. 2004.06.23+
VECTOR3( 10.0f, -30.0f, sz + i * 100.0f));

_EnemyList.push_back(pEnemy);		//더미를 리스트에 등록..
}

*/


////////////////////////////////////////////////////////////////////////////
//
// 적 비행기 이동.
// 
// : 절두체 컬링과 상관없이 계속 움직여야 합니다! 그렇지 않으면, 
// : 고개를 돌릴경우, 갑자기 비행기가 나타나게 됩니다.....
//
void GP::EnemyMove(float dTime)
{
	for(EmyItor_ i = _EnemyList.begin(); i != _EnemyList.end();)
	{
		//false 가 리턴되면 삭제. 꼭 폭파 등이 아니고 		
		//'전투 영역'을 벗어 났을 경우에도  마찬가지있습니다.
		if((*i)->Move(dTime) == FALSE)	
		{								
			delete (*i);				 
			_EnemyList.erase(i++);		 

		}
		else ++i;
	}
}




////////////////////////////////////////////////////////////////////////////
//
// 적 비행기 그리기. 
//
// 2004.06.25. 절두체 컬링 추가: 보이것만 그립니다. 
//
//
void GP::EnemyDraw(float dTime)		
{ 
	for(EmyItor_ i = _EnemyList.begin(); i != _EnemyList.end(); )
	{
		if((*i)->Draw(dTime) == FALSE)	//폭파 애니가 끝나면 'FALSE' 가 리턴된다....
		{
			delete (*i);
			_EnemyList.erase(i++);
		}
		else ++i;
	}
}


 



////////////////////////////////////////////////////////////////////////////

void GP::EnemyRelease()
{
	for(EmyItor_ i = _EnemyList.begin(); i != _EnemyList.end();)
	{
		delete (*i);				//메모리 제거.
		_EnemyList.erase(i++);		//포인터 제거.

	}

}









/////////////////////////////////////////////////////////////////////////////
// 
// 충돌 관련...
//

// 
// 주인공 총알과 적기와의 충돌검사...
// 레일건의 입장에서 적비행기와 충돌을 검사합니다.
//
void GP::ChkColl_HBulletVSEnemy()
{
	//주인공 총알...
	for(int j=0; j<_RailGun_Max_; j++)
	{
		if(_pRailGun[j] == NULL) continue;	//총알이 없으면 다음...

		for(EmyItor_ i=_EnemyList.begin(); i != _EnemyList.end(); ++i)			//적기 루프..
		{
			// 적기가 '죽어서..폭파 애니 중이면'...다음 적기 검사.
			if((*i)->IsDead())		
				continue;

			// 총알과 적기 충돌검사.
			if( _pRailGun[j]->CheckCollision(*i) == CUnit::COLL_YES_)		
			{
				// 적기의 HP 를 총알의 DMG 만큼 줄있습니다.....
				// 체력이 0이하면.. 내부에서 m_bIsDead 셋팅.
				// 이후 폭파 애니가 연출된후 객체가 제거됩니다. => Draw에서 처리할 것있습니다.. 
				(*i)->TakeDamage(_pRailGun[j]->GetDmg()); 

				//해당 적 유닛을 LockOn 하던, 미사일등의 유도 무기에 
				//'Lock-Off' 시킴. 
				if((*i)->IsDead())	MissileLockOff(*i);

				//총알 제거....
				SAFE_DELETE(_pRailGun[j]);

				//다음 총알 검사를 위해 Break!
				break; 
			}
		}//end of for(EmyItor_..)

	}//end of for(int j...)
}





/////////////////////////////////////////////////////////////////////////////
//
// void ChkColl_HMissileVSEnemy()
//
// 주인공 미사일과 적 비행기 사이의 충돌검사..
//
void GP::ChkColl_HMissileVSEnemy()
{
	//주인공의 모든  미사일 검사..
	for(int j=0; j<MISSILE_MAX_; j++)	
	{
		if(_pMissile[j] == NULL) continue;	//미사일이 없으면 다음...

		if(!_pMissile[j]->IsReleased()) continue;	//발사되지 않았으면, 다음...

		// 
		// 폭파 중이어도 체크합니다 : '폭파 범위'는 점점 커지게 될것이다.
		//if(g_pMissile[j]->IsDead()) continue;		
 

		for(EmyItor_ i=_EnemyList.begin(); i != _EnemyList.end(); ++i)			//적기 루프..
		{
			if((*i)->IsDead())		//적기가 '죽어서..폭파 애니 중이면'...다음 적기 검사.
				continue;

			if( _pMissile[j]->CheckCollision(*i) == CUnit::COLL_YES_)		//미사일과 적기 충돌검사.
			{
				//적기의 HP 를 미사일의 DMG 만큼 줄있습니다.....
				//체력이 0이하면.. 내부에서 m_bIsDead 셋팅.
				// 이후 폭파 애니가 연출된후 객체가 제거됩니다. => Draw에서 처리할 것있습니다.. 
				(*i)->TakeDamage(_pMissile[j]->GetDmg()); 

				//해당 적 유닛을 LockOn 하던, 미사일등의 유도 무기에 
				//'Lock-Off' 시킴. 
				if((*i)->IsDead())	MissileLockOff(*i);


				// 미사일 제거. 
				// 레일건과는 다르게 바로 제거하지 않고..
				// 미사일 자신의 '폭파' 애니를 연출하고 제거합니다.
				if(!_pMissile[j]->IsDead())			//지금 처음 '충돌'되어 터진 놈이라면... 
				{ 
					_pMissile[j]->SetDead();		//'죽었음' 이라고 셋팅하고..'폭파 애니'를 시작합니다.
					break;							//break 를 해서, 처음에는 1대의 적기하고만 충돌검사를 수행합니다.. ->자연스러운 '폭파 반경 확대'를 적용할 수 있습니다..
				}	
				//다음 미사일 검사를 위해 Break!  => 주석처리  
				//'폭파 되더라도' 주변에 모두 영향을 미친다...
				//그렇다 하더라도..일단은 1대만 충돌.. ^^;;
				//break;		

			}
		}//end of for(itor..)

	}//end of for(int j...)

	//주인공의 모든  미사일 검사..
	for(int j=0; j<ASTMISSILE_MAX_; j++)	
	{
		if(_pAMissile[j] == NULL) continue;	//미사일이 없으면 다음...

		if(!_pAMissile[j]->IsReleased()) continue;	//발사되지 않았으면, 다음...

		// 
		// 폭파 중이어도 체크합니다 : '폭파 범위'는 점점 커지게 될것이다.
		//if(g_pMissile[j]->IsDead()) continue;		
 

		for(EmyItor_ i=_EnemyList.begin(); i != _EnemyList.end(); ++i)			//적기 루프..
		{
			if((*i)->IsDead())		//적기가 '죽어서..폭파 애니 중이면'...다음 적기 검사.
				continue;

			if( _pAMissile[j]->CheckCollision(*i) == CUnit::COLL_YES_)		//미사일과 적기 충돌검사.
			{
				//적기의 HP 를 미사일의 DMG 만큼 줄있습니다.....
				//체력이 0이하면.. 내부에서 m_bIsDead 셋팅.
				// 이후 폭파 애니가 연출된후 객체가 제거됩니다. => Draw에서 처리할 것있습니다.. 
				(*i)->TakeDamage(_pAMissile[j]->GetDmg()); 

				//해당 적 유닛을 LockOn 하던, 미사일등의 유도 무기에 
				//'Lock-Off' 시킴. 
				if((*i)->IsDead())	MissileLockOff(*i);


				// 미사일 제거. 
				// 레일건과는 다르게 바로 제거하지 않고..
				// 미사일 자신의 '폭파' 애니를 연출하고 제거합니다.
				if(!_pAMissile[j]->IsDead())			//지금 처음 '충돌'되어 터진 놈이라면... 
				{ 
					_pAMissile[j]->SetDead();		//'죽었음' 이라고 셋팅하고..'폭파 애니'를 시작합니다.
					break;							//break 를 해서, 처음에는 1대의 적기하고만 충돌검사를 수행합니다.. ->자연스러운 '폭파 반경 확대'를 적용할 수 있습니다..
				}	
				//다음 미사일 검사를 위해 Break!  => 주석처리  
				//'폭파 되더라도' 주변에 모두 영향을 미친다...
				//그렇다 하더라도..일단은 1대만 충돌.. ^^;;
				//break;		

			}
		}//end of for(itor..)

	}//end of for(int j...)
}







/////////////////////////////////////////////////////////////////////////////
//
// void ChkColl_HeroVSEnemy() 
//
// 주인공 과 적 비행기 사이의 충돌검사..
//
void GP::ChkColl_HeroVSEnemy()
{
	//주인공 검사. 
	if(_pHero->IsDead()) return;			//죽인공이 '폭파중'이면 리턴...

	//적기 루프..
	for(EmyItor_ i=_EnemyList.begin(); i != _EnemyList.end(); ++i)			
	{
		if((*i)->IsDead())		//적기가 '죽어서..폭파 애니 중이면'...
			continue;			//다음 적기 검사.

		//미사일과 적기 충돌검사.
		if( _pHero->CheckCollision(*i) == CUnit::COLL_YES_)		
		{
			//적기의 HP 를 DMG 만큼 줄있습니다.....
			//체력이 0이하면.. 내부에서 m_bIsDead 셋팅.
			// 이후 폭파 애니가 연출된후 객체가 제거됩니다. => Draw에서 처리할 것있습니다.. 
			(*i)->TakeDamage(10);		//적 보스일 경우는 다르기 때문에 
			//일단 10정도 부여. 004.06.24.
			//주인공 데미지 입히기...적기와 동일. 
			_pHero->TakeDamage(1);		

			//주인공이 죽었다면...체크 루틴에서 break; 
			if(_pHero->IsDead())  
			{
				//주인공이 가지고 있던 모든 외부 무기들도 없앤다...
				//혹은 모두 '폭파' 시키는 것이 나을지도... ^^;;
				MissileRelease();
				break;
			}
		}

	}//end of for(itor..)

}//end of void ChkColl_HeroVSEnemy()



////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////
//
// 폭파 애니메이션 관련. 
//
////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////

void GP::ExplosionSpriteLoad()
{
	char filename[80];
	for(int i=0; i<EXPLOSION_MAXFRAM_; i++)
	{
		B3DSprite_Release(&_ExplosionSpr[i]);		//이전에 로드된것이 있으면 삭제.

		// 새 스프라이트 로드.
		sprintf(filename, "Explosion_Frame0%d.dds", i+1);
		B3DSprite_Load(&_ExplosionSpr[i], filename );

		B3DSprite_Scaling(&_ExplosionSpr[i], 10.0f, 10.0f);		 
		D3DXMatrixIdentity(&(_ExplosionSpr[i].mRot)); 
		D3DXMatrixIdentity(&(_ExplosionSpr[i].mTrans)); 

	}




	//미사일 폭파 스프라이트 로드.
	for(int i=0; i<EXPLOSION_MISSILE_MAXFRM_; i++)
	{
		B3DSprite_Release(&_MissileBoomSpr[i]);		//이전에 로드된것이 있으면 삭제.

		// 새 스프라이트 로드.
		sprintf(filename, "Particle.bmp"); //"Explosion_Missile0%d.bmp", i+1);
		B3DSprite_Load(&_MissileBoomSpr[i], filename );

		B3DSprite_Scaling(&_MissileBoomSpr[i], 10.0f, 10.0f);		 
		D3DXMatrixIdentity(&(_MissileBoomSpr[i].mRot)); 
		D3DXMatrixIdentity(&(_MissileBoomSpr[i].mTrans)); 

	}




	//충격파 애니 로드.
	for(int i=0; i<SHOCKWAVE_MAXFRM_; i++)
	{
		//이전에 로드된것이 있으면 삭제.
		B3DSprite_Release(&_ShockWaveSpr[i]);		

		// 새 스프라이트 로드.: 스프라이트는 한장이니까..이렇게 단순하게. ㅡㅡ;
		sprintf(filename, "ShockWave.bmp");  
		B3DSprite_Load(&_ShockWaveSpr[i], filename );

		B3DSprite_Scaling(&_ShockWaveSpr[i], 1.0f, 1.0f);		 //시작 스케일.
		D3DXMatrixIdentity(&(_ShockWaveSpr[i].mRot)); 
		D3DXMatrixIdentity(&(_ShockWaveSpr[i].mTrans)); 
	}

}


////////////////////////////////////////////////////////////////////////////

void GP::ExplosionSpriteRelease()
{
	for(int i=0; i<EXPLOSION_MAXFRAM_; i++)
	{
		B3DSprite_Release(&_ExplosionSpr[i]);		//스프라이트 정보 삭제.
	}


	//미사일 폭파 스프라이트 삭제. 
	for(int i=0; i<EXPLOSION_MISSILE_MAXFRM_; i++)
	{
		B3DSprite_Release(&_MissileBoomSpr[i]);	
	}


	//미사일 충격파 스프라이트 정보 삭제.
	for(int i=0; i<SHOCKWAVE_MAXFRM_; i++)
	{
		B3DSprite_Release(&_ShockWaveSpr[i]);		
	}
}








/////////////////////////////////////////////////////////////////////////////
//
//  미사일 관련.
//
/////////////////////////////////////////////////////////////////////////////

void GP::AssistMissileCreate(CPlane* pAssist, CB3Mesh* pMesh, 
					   SPRDATA* pSprFire, 
					   SPRDATA* pSprSmoke,
					   VECTOR vHardPt[], 
					   CCamera** ppCam, float dTime)
{
	for(int i=0; i< ASTMISSILE_MAX_; i++)
	{
		//_pMissile[i] = NULL;
		_pAMissile[i] = new CMissile(
			g_B3pDevice,						//디바이스.
			pMesh,								//미사일 메쉬.
			ppCam,								//카메라 이중 포인터. 2004.06.19.
			100,		//데미지.

		//	(i==MISSILE_NUK_)?10.f:20.0f,		 
			400.0f,		//미사일 최대 비행 속도. 2004.11.12.
			pAssist->GetNView(),					//미사일 발사 방향. 
			300.0f,	//미사일 로켓 연료. 시간당, 10%씩 소모됨. 

			pAssist,								//부착될 비행기 포인터.
			NO_,								//발사 않됬음.



			pSprFire,							//화염 스프라이트 포인터.
			VECTOR3(0, 0, 2.0f),						//화염 위치.(미사일 모델공간의 좌표)
			VECTOR3(1.0, 1.0f, 1.0f),			//화염 크기.
			COLOR(1.0f, 0.3f, 0.3f, 1.0f),	//화염 색상.
			pSprSmoke,

			_pSmokeList,						//연기 리스트.

			//(i==MISSILE_NUK_)?15.0f:8.0f,		//충돌구의 반지름.
			4.0f,		//충돌구의 반지름.
			&_AniInfoMissileBoomSmall, //폭파 애니 
			&_AniInfoShockWaveSmall,	 //충격파 애니 정보.

			vHardPt[i],						//미사일 위치: Hard Point - 모델 공간 상대위치.
			VECTOR3(0.8f, 0.8f, 0.8f)  	//미사일  크기 
			);

		ASSERT(_pAMissile[i] != NULL);

	}
}

void GP::AssistMissileFire()
{

	for(int i=0; i<ASTMISSILE_MAX_; i++)
	{

		if(_pAMissile[i] == NULL) continue;		//미사일 검사. 

		if(_pAMissile[i]->IsReleased())			//발사여부 검사후 발사!!
			continue;

		//2004.11.11. 유도 코드 추가.
		//레이더 에서 'Lock-On' 된 적기의 정보를 얻은후 
		//미사일에 정보를 입력한다. 2004.11.15

		_pRadar->SearchTargets();
		_pRadar->SelectTarget(TRUE);
		_pAMissile[i]->LockOn(const_cast<CEnemy*>(_pRadar->GetTarget()));
		_pAMissile[i]->SetFire();		//일반 미사일 발사.

		return;							//1발 발사후 리턴. 
	}//end of for()

}

void GP::AssistMissileMove(float dTime)
{
	for(int i=0; i<ASTMISSILE_MAX_; i++)
	{
		if(_pAMissile[i] == NULL) continue;		//미사일 검사. 

		if(_pAMissile[i]->IsDead()) continue;	//'폭파중'인 미사일은, 
												// 이동하지 않는다.

		//미사일 이동 : 최대 사거리를 넘으면..FALSE 가 리턴됩니다.. 
		//자폭 처리합니다...CMIssile 내부에서 처리되지만..한번더 확인해 준다.
		if(_pAMissile[i]->Move(dTime) == FALSE)		
		{			 
			_pAMissile[i]->SetDead();			//자폭시킴.
		}

	}
}

void GP::AssistMissileDraw(float dTime)
{
	for(int i=0; i<ASTMISSILE_MAX_; i++)
	{
		if(_pAMissile[i] == NULL) continue;		//미사일이 있으므로 넘어감.

		if(_pAMissile[i]->Draw(dTime) == FALSE)	// '폭파 애니' 끝나면 'FALSE' 리턴. 
		{
			SAFE_DELETE(_pMissile[i]);		//삭제.
		}
	}
}

void GP::AssistMissileRelease()
{
	for(int i=0; i<ASTMISSILE_MAX_; i++)
	{
		SAFE_DELETE(_pAMissile[i]);
	}
}

///////////////////////////////////////////////////////////////////////////

void GP::MissileCreate(CPlane* pHero, CB3Mesh* pMesh, 
					   SPRDATA* pSprFire, 
					   SPRDATA* pSprSmoke,
					   VECTOR vHardPt[], 
					   CCamera** ppCam, float dTime)
{
	//미사일 생성.
	for(int i=0; i<MISSILE_MAX_; i++)
	{
		//_pMissile[i] = NULL;
		_pMissile[i] = new CMissile(
			g_B3pDevice,						//디바이스.
			pMesh,								//미사일 메쉬.
			ppCam,								//카메라 이중 포인터. 2004.06.19.
			(i==MISSILE_NUK_)?10000:100,		//데미지.

		//	(i==MISSILE_NUK_)?10.f:20.0f,		 
			(i==MISSILE_NUK_)?250.f:400.0f,		//미사일 최대 비행 속도. 2004.11.12.
			pHero->GetNView(),					//미사일 발사 방향. 
			(i==MISSILE_NUK_)?450.0f:300.0f,	//미사일 로켓 연료. 시간당, 10%씩 소모됨. 

			pHero,								//부착될 비행기 포인터.
			NO_,								//발사 않됬음.



			pSprFire,							//화염 스프라이트 포인터.
			(i==MISSILE_NUK_)?VECTOR3(0, 0, 4.0f):VECTOR3(0, 0, 2.0f),						//화염 위치.(미사일 모델공간의 좌표)
			(i==MISSILE_NUK_)?VECTOR3(8.0, 8.0, 1.0f):VECTOR3(4.0, 4.0f, 1.0f),			//화염 크기.
			(i==MISSILE_NUK_)?COLOR(.8f, 0.5f, 1.0f, 1.0f):COLOR(1.0f, 0.3f, 0.3f, 1.0f),	//화염 색상.
			pSprSmoke,

			_pSmokeList,						//연기 리스트.

			//(i==MISSILE_NUK_)?15.0f:8.0f,		//충돌구의 반지름.
			(i==MISSILE_NUK_)?8.0f:4.0f,		//충돌구의 반지름.
			(i==MISSILE_NUK_)?&_AniInfoMissileBoomBig:&_AniInfoMissileBoomSmall, //폭파 애니 
			(i==MISSILE_NUK_)?&_AniInfoShockWaveBig:&_AniInfoShockWaveSmall,	 //충격파 애니 정보.

			vHardPt[i],						//미사일 위치: Hard Point - 모델 공간 상대위치.
			(i==MISSILE_NUK_)?VECTOR3(5.0f, 5.0f, 5.0f):VECTOR3(2.0f, 2.0f, 2.0f)  	//미사일  크기 
			);

		ASSERT(_pMissile[i] != NULL);

	}
}


////////////////////////////////////////////////////////////////////////////
//
// 미사일 발사... 2004.06.23+ : 역신 단순무식이 최고. ㅡㅡ;; 
//
void GP::MissileFire(BOOL bNuk, BOOL bFireAll)
{
	//
	// 모두 발사시..
	//
	if(bFireAll)
	{
		for(int i=0; i<MISSILE_MAX_; i++)
		{
			if(_pMissile[i] == NULL) continue;		//미사일 검사. 

			if(_pMissile[i]->IsReleased())			//발사여부 검사후 발사!!
				continue;

			//2004.11.11. 유도 코드 추가.
			//레이더 에서 'Lock-On' 된 적기의 정보를 얻은후 
			//미사일에 정보를 입력한다. 2004.11.15
			_pMissile[i]->LockOn(const_cast<CEnemy*>(_pRadar->GetTarget()));
  			_pMissile[i]->SetFire();				//미사일 발사.

		}//end of for()

		return;										//모두 발사후 리턴.
	}

	//----------------------------------------------------------------
	//핵미사일. 
	if(bNuk == TRUE)					
	{
		if(_pMissile[MISSILE_NUK_] == NULL) return;

		if(!_pMissile[MISSILE_NUK_]->IsReleased())		//발사 검사.
		{
			//2004.11.11. 유도 코드 추가.
			//레이더 에서 'Lock-On' 된 적기의 정보를 얻은후 
			//미사일에 정보를 입력한다. 2004.11.15
			_pMissile[MISSILE_NUK_]->LockOn(const_cast<CEnemy*>(_pRadar->GetTarget()));
			_pMissile[MISSILE_NUK_]->SetFire();
		}

		return;
	}

	//----------------------------------------------------------------
	// 일반 미사일 발사.
	//
	for(int i=0; i<MISSILE_MAX_; i++)
	{
		if(i == MISSILE_NUK_)  continue;		//핵미사일 번호면 그냥 다음으로..

		if(_pMissile[i] == NULL) continue;		//미사일 검사. 

		if(_pMissile[i]->IsReleased())			//발사여부 검사후 발사!!
			continue;

		//2004.11.11. 유도 코드 추가.
		//레이더 에서 'Lock-On' 된 적기의 정보를 얻은후 
		//미사일에 정보를 입력한다. 2004.11.15
		_pMissile[i]->LockOn(const_cast<CEnemy*>(_pRadar->GetTarget()));
		_pMissile[i]->SetFire();		//일반 미사일 발사.

		return;							//1발 발사후 리턴. 
	}//end of for()


}//end of MissileFire()




/////////////////////////////////////////////////////////////////////////////
//
void GP::MissileMove(float dTime)
{

	for(int i=0; i<MISSILE_MAX_; i++)
	{
		if(_pMissile[i] == NULL) continue;		//미사일 검사. 

		if(_pMissile[i]->IsDead()) continue;	//'폭파중'인 미사일은, 
												// 이동하지 않는다.

		//미사일 이동 : 최대 사거리를 넘으면..FALSE 가 리턴됩니다.. 
		//자폭 처리합니다...CMIssile 내부에서 처리되지만..한번더 확인해 준다.
		if(_pMissile[i]->Move(dTime) == FALSE)		
		{			 
			_pMissile[i]->SetDead();			//자폭시킴.
		}

	}

}

/////////////////////////////////////////////////////////////////////////////
//
void GP::MissileDraw(float dTime)		
{
	for(int i=0; i<MISSILE_MAX_; i++)
	{
		if(_pMissile[i] == NULL) continue;		//미사일이 있으므로 넘어감.

		if(_pMissile[i]->Draw(dTime) == FALSE)	// '폭파 애니' 끝나면 'FALSE' 리턴. 
		{
			SAFE_DELETE(_pMissile[i]);		//삭제.
		}
	}

}

/////////////////////////////////////////////////////////////////////////////
//
void GP::MissileRelease()
{
	for(int i=0; i<MISSILE_MAX_; i++)
	{
		SAFE_DELETE(_pMissile[i]);
	}

}





/////////////////////////////////////////////////////////////////////////////
//
// pTgt 이 폭파 등의 이유로 제거 되었을 경우, 
// 동일한 목표물을 가지고 있는 미사일을 Lock Off 시킨다.
//
void GP::MissileLockOff(CUnit* pTgt)
{
	for(int i=0; i<MISSILE_MAX_; i++)
	{
		if(_pMissile[i] == NULL) continue;		//미사일 검사. 

		if(_pMissile[i]->IsDead()) continue;	//'폭파중'인 미사일은, 제외.

		if(_pMissile[i]->GetTarget() == pTgt)	//목표와 동일하면..Lock off.
		{
			_pMissile[i]->LockOff();
		}
	}
}








/////////////////////////////////////////////////////////////////////////////
//
//  미사일 연기 관련.
//
/////////////////////////////////////////////////////////////////////////////

void GP::SmokeInit()
{
	_pSmokeList = new CArrayList(AL_REFERENCE);
	ASSERT(_pSmokeList != NULL);
}


void GP::SmokeMove(float dTime)
{
	for(UINT i = 0 ; i<_pSmokeList->Count(); i++)   
	{
		CSmoke* pSmoke = (CSmoke*)_pSmokeList->GetPtr(i);
		if(pSmoke == NULL) continue;

		if(pSmoke->Move(dTime) == FALSE)
		{
			delete pSmoke;
			_pSmokeList->Remove(i);	
		}
	}		
}


void GP::SmokeDraw(float dTime)   
{
	for(UINT i = 0 ; i<_pSmokeList->Count(); i++)  
	{
		CSmoke* pSmoke = (CSmoke*)_pSmokeList->GetPtr(i);
		if(pSmoke == NULL) continue;

		pSmoke->Draw(dTime);		
	}		
}


void GP::SmokeRelease()
{
	SAFE_DELETE(_pSmokeList);
}









/////////////////////////////////////////////////////////////////////////////
//
// 총알 관련.
//
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// 
void GP::RailGunInit()
{
	for(int i=0; i<_RailGun_Max_; i++)
	{
		_pRailGun[i] = NULL;
	}
}





/////////////////////////////////////////////////////////////////////////////
// 
void GP::RailGunCreate(float dTime)
{	 

	//COMBATVIEW 추가로 인한, 레일건 위치변경.2009.07.03. 
	VECTOR3 vFirePos = -_pHero->GetHorz() * 1.5f
		               +_pHero->GetNView() * 2.5f;
	vFirePos += _pHero->GetPos();

	for(int i=0; i<_RailGun_Max_; i++)
	{
		if(_pRailGun[i] != NULL) continue;		//총알이 있으면, 컨틴유... 

		_pRailGun[i] = new CRailGun(
			g_B3pDevice,					//디바이스.
			NULL,							//메쉬.
			&_sprRailGun,					//3D 스프라이트 포인터.
			&_pCurrCam,						//카메라 이중 포인터. 2004.06.19.
			1,								//데미지.
			COLOR(1.0f, 0.5f, 0.0f, 1.0f),	//색상.
			500.0f + fabs(_pHero->GetMovVelo().z),	//발사 속도 = 총구 발사 속도 + 비행기 속도
			_pHero->GetNView(),				//발사 방향.  
			1000.0f,						//최대 사거리. 1000m
			1.0f,							//총알 충돌구의 반경

			vFirePos,						//발사 위치(총구 위치)					
 			VECTOR3(1.5f, 1.5f, 1.0f)		//크기(스프라이트 사용시 z축의 크기는무시해도 됩니다..)
			);

		//ASSERT(_pRailGun[i] != NULL); --> 총알 생성에 실패하더라도..
		//									은근슬쩍 넘어갑니다.  ㅡㅡ)v

		break;		//1발 완성. break!!
	}
}







/////////////////////////////////////////////////////////////////////////////
// 
void GP::RailGunMove(float dTime)
{
	for(int i=0; i<_RailGun_Max_; i++)
	{
		if(_pRailGun[i] == NULL) continue;		//총알 검사. 

		//총알 이동 : 최대 사거리를 넘으면..FALSE 가 리턴됩니다.. 
		if(_pRailGun[i]->Move(dTime) == FALSE)		
		{
			SAFE_DELETE(_pRailGun[i]);			//삭제.
		}
	}
}



/////////////////////////////////////////////////////////////////////////////
// 
void GP::RailGunDraw(float dTime)	
{
	for(int i=0; i<_RailGun_Max_; i++)
	{
		if(_pRailGun[i] == NULL) continue;		//총알이 있으므로 넘어감.

		_pRailGun[i]->Draw(dTime);	
	}

}



/////////////////////////////////////////////////////////////////////////////
// 
void GP::RailGunRelease()
{
	for(int i=0; i<_RailGun_Max_; i++)
	{
		SAFE_DELETE(_pRailGun[i]);
	}

}







/////////////////////////////////////////////////////////////////////////////
// 
//  토네이도 만들기. : 레일건과 거의 동일하다. 
//
void GP::TornadoCreate(float dTime)
{	 
	for(int i=0; i<_RailGun_Max_; i++)
	{
		if(_pRailGun[i] != NULL) continue;		//총알이 있으면, 컨틴유... 

		// 생성된 토네이도는 레일건 리스트에서 동일하게 관리합니다.
		//
		_pRailGun[i] = new CTornado(		
			g_B3pDevice,					//디바이스.
			NULL,							//메쉬.
			&_sprTornado,					// 스프라이트 포인터.
			&_pCurrCam,						//카메라 이중 포인터. 2004.06.19.
			50,								//데미지.
			COLOR(0.3f, 0.5f, 1.0f, 0.5f),	//색상.
			80.0f + fabs(_pHero->GetMovVelo().z),	//발사 속도 = 총구 발사 속도 + 비행기 속도
			_pHero->GetNView(),				//발사 방향.
			_pHero->GetRotMtx(),			//발사시, 비행기의 회전행렬
			500.0f,							//최대 사거리. 500m
			10.0f,							//충돌구 반경
			_pHero->GetPos(),				//발사 위치(총구 위치)
			VECTOR3(20.0f, 20.0f, 1.0f)		//크기(스프라이트 사용시 z축의 크기는무시해도 됩니다..)
			);

		//ASSERT(_pRailGun[i] != NULL); --> 총알 생성에 실패하더라도..
		//									은근슬쩍 넘어갑니다.  ㅡㅡ)v

		break;		//1발 완성. break!!
	}
}









//////////////////////////////////////////////////////////////////////////////
//
void GP::DummyDraw(float dTime)  
{ 
	static CHero Dummy1 = *_pHero, Dummy2 = *_pHero; 
	static const float sx = -50.0f, sz = -50.0f;
	for(int j=0; j<10; j++)
	{
		for(int i=0; i<10; i++)
		{
			Dummy1.WMove(VECTOR3( sx + i*20.0f, 10.0f, sz + j*30.0f));
			Dummy1.Draw(dTime);	

			Dummy2.WMove(VECTOR3( sx + i*20.0f, -10.0f, sz + j*30.0f));
			Dummy2.Draw(dTime);  
		}
	}
 
}



//////////////////////////////////////////////////////////////////////////////
//
void GP::ParticleDraw(float dTime)
{
	if(_bBillBoardON) _sprParticle.mRot = _pCurrCam->GetBillMtx();
	else D3DXMatrixIdentity(&_sprParticle.mRot);

	B3Spr_Trans(&_sprParticle, 12.0f, 0.0f, 1.0f, ON3D_); 
	B3Spr_DrawFX(&_sprParticle, COLOR(1.0f, 0.0f, 0.0f, 1.0f));  
	B3Spr_Trans(&_sprParticle, 0.0f,  6.0f, 0.0f, ON3D_); 
	B3Spr_DrawFX(&_sprParticle, COLOR(0.0f, 1.0f, 0.0f, 1.0f));  
	B3Spr_Trans(&_sprParticle, -12.0f, 0.0f, 1.0f, ON3D_); 
	B3Spr_DrawFX(&_sprParticle, COLOR(0.0f, 0.0f, 1.0f, 1.0f));  
}



//////////////////////////////////////////////////////////////////////////////
//
//  뷰포팅을 통한 보조카메라 구현.
//
void GP::ViewportOn(float dTime)
{
	//----------------- 
	// 카메라 재설정. 
	//----------------- 
 	_pCurrCam = _pCurrCamSet[CAM_PILOT_];			//1번 조종석 카메라 On!
	_pCurrCam->Update(dTime); 


	//----------------- 
	//뷰포트 설정.: 우측 상단. 
	//-----------------  
	D3DVIEWPORT9  vp;					
	vp.X = g_B3Mode.Width - 160; 		 
	vp.Y = 200;
	vp.Width = 160;			vp.Height = 120;
	vp.MinZ = 0.0f;			vp.MaxZ = 0.0f;
	g_B3pDevice->SetViewport(&vp);


	//------------------------------------------- 
	//'Z Buffer Clear
	// 뷰포트 영역의 Depth 값을 클리어해야 합니다.
	//------------------------------------------- 
 	D3DRECT rc;
	rc.x1 = vp.X;
	rc.y1 = vp.Y;
	rc.x2 = rc.x1 + vp.Width;
	rc.y2 = rc.y1 + vp.Height;

	COLOR col = ChangeBkColor(0);	 
 	g_B3pDevice->Clear(1, &rc, D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, 
					   col, 1.0f, 0);
		 

	//---------------------- 
	// Scene 구성.
	//----------------------  			
  	
	//----------------------------------// 
	// 스카이 박스 그리기.				//
	//----------------------------------// 
	if(_bShowSkyBox) _pSkyBox->Draw();
	//B3DSprite_Draw3D(&_sprMap,NO_, 1.0f, NOZWRITE_);	// 배경 

 	//ParticleDraw(dTime);			// 파티클 그리기			 
	if(_bShowStars)	_pStars->Draw(dTime);	// 별 그리기.						 


	//----------------------------------// 
	// 주인공.  
	//----------------------------------// 

	_pHero->Draw(dTime);				// 주인공.

	_pAssist->Draw(dTime);
	  
	//----------------------------------// 
	//  기타...
	//----------------------------------// 
	EnemyDraw(dTime);		
	MissileDraw(dTime);		// 미사일 그리기.					 
	SmokeDraw(dTime);		// 연기 그리기. 	
	RailGunDraw(dTime);		// 총알 그리기.						
 	PlasmaCannonDraw(dTime);		

	AssistMissileDraw(dTime);


	//------------------------ 
	// 카메라 & 뷰포트 복구..
	//------------------------ 
 	vp.X = 0; 
	vp.Y = 0;
	vp.Width = g_B3Mode.Width;	
	vp.Height = g_B3Mode.Height;
	vp.MinZ = 0.0f;	
	vp.MaxZ = 1.0f;
	g_B3pDevice->SetViewport(&vp);

	// 뷰, 프로젝션등을 다시 설정해 주어야, 
	// '오브제', 'help', 등의 출력에 영향이 없습니다.
	// -> 메인카메라는 _CurrCamNum 이다. 주의
	_pCurrCam = _pCurrCamSet[_CurrCamNum]; 
	_pCurrCam->SetView();		


}











/////////////////////////////////////////////////////////////////////////////
//
// 카메라 관련.
//
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////// 
//
// 카메라 설정 :
//
// 2004.07.01. 카메라 종류 추가 ex) FCT_FREE_ | FCT_OUTTER_ 
//
void GP::CameraSetup() 
{
  	_pCurrCamSet[CAM_FREE_] = new CCamFree(g_B3pDevice, "자유카메라", FCT_FREE_ | FCT_OUTTER_, POS3( 0, 3, -30), DIR3(0, 0, 1));										ASSERT(_pCurrCamSet[CAM_FREE_] != NULL); 
	_pCurrCamSet[CAM_PILOT_] = new CCamPilot(g_B3pDevice, "조종사 시점", FCT_PILOT_ | FCT_INNER_, _pHero, ANG3(D3DX_PI*0.85f, D3DX_PI*0.45f, 0), POS3(0, 0.65f, -0.85f)); 	ASSERT(_pCurrCamSet[CAM_PILOT_] != NULL); 
	_pCurrCamSet[CAM_FIX_COMBATVIEW_] = new CCamModelFix(g_B3pDevice, "전투 시점", FCT_FRONT_ | FCT_OUTTER_, _pHero, POS3(  0,  0,  -5), DIR3(  0, 0, -1));  ASSERT(_pCurrCamSet[CAM_FIX_COMBATVIEW_] != NULL); 
	_pCurrCamSet[CAM_FIX_FRONTVIEW_] = new CCamModelFix(g_B3pDevice, "전방 조망 카메라", FCT_FRONT_ | FCT_OUTTER_, _pHero, POS3(  0, 3,  20), DIR3(  0, 0, -1));  ASSERT(_pCurrCamSet[CAM_FIX_FRONTVIEW_] != NULL); 
	_pCurrCamSet[CAM_FIX_LEFTVIEW_]  = new CCamModelFix(g_B3pDevice, "좌측 조망 카메라", FCT_LEFT_ | FCT_OUTTER_, _pHero, POS3(-12, 1,  -1), DIR3(  1, 0,  0));  ASSERT(_pCurrCamSet[CAM_FIX_LEFTVIEW_] != NULL); 
	_pCurrCamSet[CAM_FIX_RIGHTVIEW_] = new CCamModelFix(g_B3pDevice, "우측 조망 카메라", FCT_RIGHT_ | FCT_OUTTER_, _pHero, POS3( 12, 1,  -1), DIR3( -1, 0,  0));  ASSERT(_pCurrCamSet[CAM_FIX_RIGHTVIEW_] != NULL); 
	_pCurrCamSet[CAM_FIX_REARVIEW_]  = new CCamModelFix(g_B3pDevice, "후방 조망 카메라", FCT_REAR_ | FCT_OUTTER_, _pHero, POS3(  0, 2, -15), DIR3(  0, 0,  1));  ASSERT(_pCurrCamSet[CAM_FIX_REARVIEW_] != NULL); 
	_pCurrCamSet[CAM_TRACKING_] = new CCamTracking(g_B3pDevice, "추적 카메라", FCT_TRACKING_ | FCT_OUTTER_, _pHero, POS3(-20, 10, -50));  	ASSERT(_pCurrCamSet[CAM_TRACKING_] != NULL); 

	_CurrCamNum = CAM_FIX_FRONTVIEW_;
	_pCurrCam = _pCurrCamSet[ _CurrCamNum ];		//기본카메라.결정.
 
}




///////////////////////////////////////////////////////////////////////////////
//
void GP::CameraUpdate(float dTime /* = 0.0f */)
{
 	//카메라 리셋.
	if(IsKeyUp(DIK_F6))		
	{
		for(int i=0; i<CAM_MAX_; i++)
			_pCurrCamSet[i]->Reset();
	}

  	// 카메라 전환. 
	for(int i=DIK_1; i<(DIK_1 + CAM_MAX_); i++)
 		if(IsKeyUp(i))
		{
			_CurrCamNum = i - DIK_1 ;
			_pCurrCam = _pCurrCamSet[ _CurrCamNum ];

			//조종사 카메라는 '리셋' 시킨다.
			if(_CurrCamNum == CAM_PILOT_)
				_pCurrCam->Reset();
		}

 
	_pCurrCam->Update(dTime);	//카메라 갱신. 

}

 




//////////////////////////////////////////////////////////////////////////////
//
COLOR GP::ChangeBkColor(BOOL bChange/*=YES*/)
{
	#define COLOR_MAX_ 6
	static COLOR cols[] = 
	{
		COLOR(0.0f, 0.0f, 1.0f, 1.0f),
		COLOR(0.9f, 0.9f, 0.9f, 1.0f),
		COLOR(0.7f, 0.7f, 0.7f, 1.0f),
		COLOR(0.4f, 0.4f, 0.4f, 1.0f),
		COLOR(0.15f, 0.15f, 0.15f, 1.0f),
		COLOR(0.0f, 0.0f, 0.05f, 1.0f),
	};

	static int num = COLOR_MAX_ - 3;

	if(IsKeyUp(DIK_B) && bChange)		//합니다수 2번 호출시 중복 처리되는 것을 방지합니다.
		if(++num >= COLOR_MAX_) num = 0;
	
	return cols[num];

}



//////////////////////////////////////////////////////////////////////////////
//
void GP::ShowHelp()
{ 

	//--------------------------------------------------------------- 
	// 미사일 정보.
	//--------------------------------------------------------------- 
	if(_bShowMisInfo && !_bShowHelp)
	{		 
		COLOR col(0.1f, 1.0f, 0.3f, 1.0f); 
		int posx = 300;
		int posy = 50;

		
		B3X_DrawText(NO_, posx, posy+=14, col, "[Missile]");	 
	 
		for(int i=0; i<MISSILE_MAX_; i++)
		{
			if(_pMissile[i]) 
			{

				B3X_DrawText(NO_, posx, posy+=14, col, 
					"미사일[%d]:vPos(%.2f, %.2f %.2f) Velo(%.2fm/s) Fuel(%.1f) vSmoke(%.2f %.2f %.2f)",					 
					i,
					_pMissile[i]->GetPos().x, _pMissile[i]->GetPos().y, _pMissile[i]->GetPos().z,
					_pMissile[i]->GetCurrVelo(),
					_pMissile[i]->GetRocketFuel(),	
					//_pMissile[i]->GetRocketFuel()/_pMissile[i]->GetMaxRangeF() * 100.0f,					 
					_pMissile[i]->GetSmokePos().x, _pMissile[i]->GetSmokePos().y, _pMissile[i]->GetSmokePos().z 
					);	 
					 
  			}
			else
			{
				B3X_DrawText(NO_, posx, posy+=14, col*0.75f, "Missile[%d] : Empty", i);				 
 			}
 		}
	}

 
 

	//--------------------------------------------------------------- 
	// 일반 도움말.
	//--------------------------------------------------------------- 
	if(!_bShowHelp) return;
  
	
	//--------------------------------------------------------------- 
	// 강의 목표 출력.
	{ 	
		int posx = g_Mode.Width / 2;
		int posy = 0;
		COLOR col = D3DXCOLOR(1.0f, 0.75f, 1.0f, 1); 
		B3X_DrawText(NO_, posx, 0, col, "■ %s", B3X_GetWindowTitle());  
		char msg[512] = 
"화기관제시스템 ( Fire Control System ) 을 통해 스캔된 적기들의 위치를 HUD 에 시현하여 \
조종사가 다중 목표물을 선택적으로 Lock-On 할 수 있어야 합니다.\n\n\
Lock-On 된 목표물이 레이더의 스캔 범위를 벗어날 경우 Lock-Off 되어도, 이미 Lock-0n 상태에서 \
발사된 미사일은 스스로 유도되는 완전한 Fire and Forget 을 구현해야 합니다.";

		B3X_DrawTextSV(posx, posy+=14, msg, col, g_dTime);
	}


	
	
	//--------------------------------------------------------------- 
	// 도움말 / 정보 출력.
	static COLOR cols[CAM_MAX_] = {
		COLOR(1.0f, 0.4f, 0, 1),
		COLOR(1.0f, 1.0f, 0, 1),		
		COLOR(0.4f, 1.0f, 0, 1),
		COLOR(0.4f, 1.0f, 0, 1),
		COLOR(0.4f, 1.0f, 0, 1),
		COLOR(0.4f, 1.0f, 0, 1),
		COLOR(0.8f, 0.3f, 1, 1),
	};

	COLOR col = COLOR(1.0f, 1.0f, 0.3f, 1.0f); 
	int posy = 200;
	int posx = 1;
	static char msg[256]; 

	//title
	//B3X_DrawText(NO_,  200, 1, COLOR(1, 1, 0, 1), "■%s", B3X_GetWindowTitle());	 


	B3X_DrawText(NO_, posx, posy+=14, col, "[카메라]");	 
	B3X_DrawText(NO_, posx, posy+=14, col, "리셋: F6");	 
	B3X_DrawText(NO_, posx, posy+=14, _bUseViewport?col * 1.5f:col, "보조카메라 : Backspace (%s)", _bUseViewport?"ON":"OFF");	 
	 
	B3X_DrawText(NO_, posx, posy+=14, cols[_CurrCamNum], "전환: 1~7 (%s)", _pCurrCam->GetName());	 
	
	if(_CurrCamNum == CAM_FREE_)
	{
		B3X_DrawText(NO_, posx, posy+=14, cols[_CurrCamNum], "시점: Mouse");		 
		B3X_DrawText(NO_, posx, posy+=14, cols[_CurrCamNum], "이동: A D W S R F");	 
	  
  	}

	if(_CurrCamNum == CAM_PILOT_)	
	{
		float fovy = _pCurrCam->GetFovY();
		B3X_DrawText(NO_, posx, posy+=14, cols[_CurrCamNum], 
					"확대: Mouse-wheel : Zoom=x%.1f Fovy=%.1f(%.1fº)", 
					D3DXToRadian(45)/fovy, fovy, D3DXToDegree(fovy));	 
		B3X_DrawText(NO_, posx, posy+=14, cols[_CurrCamNum], "시점: Mouse");			  
		B3X_DrawText(NO_, posx, posy+=14, cols[_CurrCamNum], "리셋: Mouse-MButton");	 
	 
	}
 

	//--------------------------------------------------------------- 
	// 주인공.
	posx = 1;
	posy+=14;
	col = COLOR(.3f, 1.0f, 0.0f, 1.0f);
	B3X_DrawText(NO_, posx, posy+=14, col, "[Hero]");			 
 	B3X_DrawText(NO_, posx, posy+=14, col, "리셋: F5");		  
	B3X_DrawText(NO_, posx, posy+=14, col, "조종: 방향키");	 
	B3X_DrawText(NO_, posx, posy+=14, col, "엔진: L-Shift/Ctrl"); 
 	B3X_DrawText(NO_, posx, posy+=14, col, "레일건: C");		 
  	B3X_DrawText(NO_, posx, posy+=14, col, "토네이도: N");		 
  	B3X_DrawText(NO_, posx, posy+=14, col, "미사일: Z");		 
 	B3X_DrawText(NO_, posx, posy+=14, col, "대포통: X");		 
	B3X_DrawText(NO_, posx, posy+=14, col, "미사일-모두발사: Z+LCtrl");	 
	B3X_DrawText(NO_, posx, posy+=14, col, "플라즈마:` Space");	 
 
		
	//--------------------------------------------------------------------- 
	//
	//--------------------------------------------------------------------- 
	posy+=14; 
	col = COLOR(1, 1, 0, 1);
	B3X_DrawText(NO_, posx, posy+=14, _bShowSkyBox?col:col*0.8f, "하늘상자: F4 (%s)", _bShowSkyBox?"ON":"OFF");		   
	B3X_DrawText(NO_, posx, posy+=14, col, "미사일리셋: F7");		   
	B3X_DrawText(NO_, posx, posy+=14, col, "적기리셋: F8");			  
	B3X_DrawText(NO_, posx, posy+=14, _bUseFrustumCulling?col:col*0.8f, "F.컬링: F9 (%s)", _bUseFrustumCulling?"ON":"OFF");	//		  

	B3X_DrawText( FALSE, posx, posy+=14, _bShowStars?col:col*0.8f, "별: F10 (%s)", _bShowStars?"ON":"OFF" );  	

	//--------------------------------------------------------------------- 
	// 화면 우측 메세지
	//--------------------------------------------------------------------- 
	{
		int posy = 300;
		int posx = g_B3Mode.Width - 8 * 12;
 		COLOR col = COLOR(1, 1, 0, 1);

		
		B3X_DrawText(NO_, posx, posy+=14, col, "[레일건]");	 
 		B3X_DrawText(NO_, posx, posy+=14, col, "발사수: %d", CRailGun::GetRailGunCnt());	 

		posy+=14;  
		col = COLOR(1, 0.7f, 1, 1);
		B3X_DrawText(NO_, posx, posy+=14, col, "[F.Cull]");	 
		B3X_DrawText(NO_, posx, posy+=14, _bUseFrustumCulling?col:col*0.8f, "적기: %d", _VisibleEnemyCnt);	 

		posy+=14;  
 		col = COLOR(0, 1, 1, 1);
 		B3X_DrawText(NO_, posx, posy+=14, col, "[레이더]");	  
		B3X_DrawText( FALSE, posx, posy+=14, _bShowAzimuth?col:col*0.8f, "보기: K (%s)", _bShowAzimuth?"ON":"OFF" );  		//★
		B3X_DrawText( FALSE, posx, posy+=14, col, "스캔수: %d", _pRadar->GetScanCnt() );  		//★

  	}//end of temp-block



	
	//--------------------------------------------------------------------- 
	// 파티클.. 
	//--------------------------------------------------------------------- 
 	posy+=14;
	posx = 1; //g_B3Mode.Width - (8 * 10);
	col = COLOR(1.f, 1.f, 0.2f, 1.0f);  
	//B3X_DrawText(NO_, posx, posy+=14, col, "[파티클]");	 
 	B3X_DrawText(NO_, posx, posy+=14, _bBillBoardON?col:col*0.8f, "파티클.빌보드: V (%s)", _bBillBoardON?"ON":"OFF");	 
	B3X_DrawText(NO_, posx, posy+=14, col, "배경색: B");	 
 	 
 	
	
	//--------------------------------------------------------------- 
	//  화면 하단 정보.
	//--------------------------------------------------------------- 
 	//--------------------------------------------------------------- 
	// 주인공.
	posy = g_B3Mode.Height - (14 * 13);
	posx = 1;
	col = COLOR(.3f, 0.8f, 1.0f, 1.0f); 
	
	B3X_DrawText(NO_, posx, posy+=14, col, "[Hero]");	  	
	B3X_DrawText(NO_, posx, posy+=14, col, "Throttle=%.2f%%", _pHero->GetThrottle()); 
	B3X_DrawText(NO_, posx, posy+=14, col, "Velo=%.2fm/s", _pHero->GetVelo()); 
	B3X_DrawText(NO_, posx, posy+=14, col, "Speed=%.2fkm/h", _pHero->GetSpeed()); 
	B3X_DrawText(NO_, posx, posy+=14, col, "MovVelo(%.3f %.3f %.3f) RotVelo(%.3f %.3f %.3f)",
		_pHero->GetMovVelo().x, _pHero->GetMovVelo().y, _pHero->GetMovVelo().z,
		_pHero->GetRotVelo().x, _pHero->GetRotVelo().y, _pHero->GetRotVelo().z
		); 

	//월드 위치 / 월드 회전 값.	
	B3X_DrawText(NO_, posx, posy+=14, col, "Pos(%.3f %.3f %.3f) Rot(%.3f %.3f %.3f) Dir(%.3f, %.3f, %.3f)",
		_pHero->GetPos().x, _pHero->GetPos().y, _pHero->GetPos().z,
		_pHero->GetRot().x, _pHero->GetRot().y, _pHero->GetRot().z,
		_pHero->GetDir().x, _pHero->GetDir().y, _pHero->GetDir().z
 		);	 

	B3X_DrawText(NO_, posx, posy+=14, col, "vNView(%.3f %.3f %.3f) vHorz(%.3f %.3f %.3f) vUp(%.3f %.3f %.3f)",
		_pHero->GetNView().x, _pHero->GetNView().y, _pHero->GetNView().z,
		_pHero->GetHorz().x, _pHero->GetHorz().y, _pHero->GetHorz().z,
		_pHero->GetUp().x, _pHero->GetUp().y, _pHero->GetUp().z
		);
	 

 



	//--------------------------------------------------------------- 
	// 카메라 정보.	 
	//col = COLOR(.5f, 1.f, 0.2f, 1.0f);
	posy+=14;
	col = cols[_CurrCamNum]; 
	
	B3X_DrawText(NO_, posx, posy+=14, col, "[카메라] %s", _pCurrCam->GetName());	 
	B3X_DrawText(NO_, posx, posy+=14, col, "FOV=%.1f", D3DXToDegree(_pCurrCam->GetFovY()));	 
 	B3X_DrawText(NO_, posx, posy+=14, col, "vEye(%.3f %.3f %.3f) vLookAt(%.3f %.3f %.3f)",
		_pCurrCam->GetEye().x, _pCurrCam->GetEye().y, _pCurrCam->GetEye().z,
		_pCurrCam->GetLookAt().x, _pCurrCam->GetLookAt().y, _pCurrCam->GetLookAt().z
		); 
	B3X_DrawText(NO_, posx, posy+=14, col, "vNView(%.3f %.3f %.3f) vHorz(%.3f %.3f %.3f) vUp(%.3f %.3f %.3f)",
		_pCurrCam->GetNView().x, _pCurrCam->GetNView().y, _pCurrCam->GetNView().z,
		_pCurrCam->GetHorz().x, _pCurrCam->GetHorz().y, _pCurrCam->GetHorz().z,
		_pCurrCam->GetUp().x, _pCurrCam->GetUp().y, _pCurrCam->GetUp().z
		);	 
 
}




/**************** end of file "GameProc.cpp" *********************************/


















