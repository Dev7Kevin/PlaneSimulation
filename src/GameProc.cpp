//////////////////////////////////////////////////////////////////////////////
//
// GameProc.cpp : �� ���������� �������� �ҽ�����.
//
//
//////////////////////////////////////////////////////////////////////////////


//#include "../../Engine/B3X.h"
//#include "../../Engine_Ext/Engine.h"
#include "../../Engine/Ext/B3Yena_Ext.h"	//DLL ���� ���� �ʼ����. 


#include "Camera.h" 

#include "Unit.h"
#include "ShockWave.h"			 

#include "Plane.h"
#include "Hero.h" 
#include "Assist.h"
#include "Bullet.h"		
#include "RailGun.h"	
#include "Tornado.h"		
#include "Missile.h"		//�̻��� ����
#include "Smoke.h"			//�̻��� ���� ����.
#include "PlasmaCannon.h"	//�ö��ĳ��. 
#include "Enemy.h"			//����ü.
#include "list"				
#include "Frustum.h"		// ����ü �ø� Ŭ����. 2004.06.25. 
#include "Star.h"			//�� ����.
#include "SkyBox.h"			//��ī�� �ڽ� ����. 2004.06.30. 
#include "Instrument.h"		

#include "Radar.h"			//3D ���̴� ����.


#include "GameProc.h"


#include "list"
using namespace std;

//////////////////////////////////////////////////////////////////////////////
//
namespace GP
{ 
	//------------------------------------
	// ���� 
	//------------------------------------
	BOOL _bShowHelp = FALSE;


	//------------------------------------
	//���ΰ� ����.
	//------------------------------------
	CB3Mesh*	_pPlane = NULL;
	VECTOR3		_HeroPos = VECTOR3(0, 0, 0);
	FLOAT		_HeroRot = 0.0f;
	CHero*		_pHero = NULL;

	//------------------------------------
	//���ΰ� ��������.
	//------------------------------------
	CB3Mesh*	_pAssistMesh = NULL;
	VECTOR3		_vAssistPos = VECTOR3(-3, 3, 2);
	FLOAT		_fAssistoRot = 0.0f;
	CAssist*	_pAssist = NULL;

	//���� �̻��� ����
	//
	#define ASTMISSILE_MAX_ 4
	CMissile* _pAMissile[ASTMISSILE_MAX_] = { NULL,};


	//------------------------------------
	// ī�޶�
	//------------------------------------
	enum {  CAM_FREE_, 
			CAM_PILOT_, 
			CAM_FIX_COMBATVIEW_,	//�����, ������ ����.
			CAM_FIX_FRONTVIEW_,		//���� ����.
			CAM_FIX_LEFTVIEW_,		//���� ����.
			CAM_FIX_RIGHTVIEW_,		//���� ����.
			CAM_FIX_REARVIEW_,		//�Ĺ� ����.
			CAM_TRACKING_, 
			
			CAM_MAX_ 
	};		 
	CCamera*	_pCurrCamSet[CAM_MAX_] = {NULL, };
	CCamera*	_pCurrCam = NULL; 
	UINT		_CurrCamNum =CAM_FREE_;		//���� ī�޶� ��ȣ.

 	BOOL		_bUseFixCamera = FALSE;			

 
	//------------------------------------
	// ���� ��ȯ.
	//------------------------------------
	COLOR ChangeBkColor(BOOL bChange = YES_);



	//------------------------------------
	//��ƼŬ�� ��������Ʈ.
	//------------------------------------
	B3SPRITE	_sprParticle;
	BOOL		_bBillBoardON = TRUE;
	B3SPRITE	_sprMap;



	//------------------------------------
	//����Ʈ on!
	//------------------------------------
	BOOL		_bUseViewport = NO_;


	//------------------------------------
	// ���ϰ� ����.
	//------------------------------------
	//#define _RailGun_Max_  1000
	const UINT _RailGun_Max_ = 500;
	CRailGun* _pRailGun[_RailGun_Max_] = { NULL, };
	CB3Mesh*  _pRailGunMesh = NULL;		//�츮�� 3D ��������Ʈ�� ����� ���̴�.
	B3SPRITE  _sprRailGun;
	B3SPRITE  _sprTornado;		


	//------------------------------------
	//�̻��� ���� ����.
	//------------------------------------
	CArrayList* _pSmokeList = NULL;
	SPRDATA		_sprParticle2;  


	//------------------------------------
	// �̻��� ����. 
	//------------------------------------
	#define MISSILE_NUK_ 3
	#define MISSILE_MAX_ 7
	CMissile* _pMissile[MISSILE_MAX_] = { NULL, };
	CB3Mesh*  _pMeshMissile = NULL;
	BOOL	  _bShowMisInfo = FALSE;

	//��ǥ ���� ����...
	void MissileLockOff(CUnit* pTgt);


	//
	// �̻��� ���� ��ġ - Hard Point ��ġ.
	// => �� ���� '�����'���� ���;� ������, �ϴ� �̷��� ����� ����.
	//
	VECTOR _vHardPoints[MISSILE_MAX_] = { 

		{ 3.5f, -0.8f, -2.0f },			//������ 1��.
		{ 2.5f, -0.8f, -2.0f },			//������ 2��.
		{ 1.5f, -0.8f, -2.0f },			//������ 3��.

		{ 0.0f, -1.5f, -1.5f },			//��� HP

		{ -3.5f, -0.8f, -2.0f },			//���� 1��.
		{ -2.5f, -0.8f, -2.0f },			//���� 2��.
		{ -1.5f, -0.8f, -2.0f },			//���� 3��.

	};

	VECTOR _vAstHardPoint[MISSILE_MAX_] = {
		{-0.5f, -0.6f, 0.0f},
		{-0.5f, 0.6f, 0.0f},
		{0.5f, -0.6f, 0.0f},
		{0.5f, 0.6f, 0.0f},
	};




	//-----------------------------------------------------------------------
	// ���� �ִϸ��̼� ����.
	//
	#define EXPLOSION_MAXFRAM_ 8
	SPRDATA _ExplosionSpr[EXPLOSION_MAXFRAM_];	//���� �ִ� ���� ��������Ʈ.

	void ExplosionSpriteLoad();
	void ExplosionSpriteDraw(float dTime);   
	void ExplosionSpriteRelease();

	//
	// ���� ���� �ִϸ��̼� ����... : �ð����� ������� ���ÿ� ������ ���ֽ��ϴ�.. 
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
	//  �̻��� ���� ��������Ʈ ����.
	//
	// �̻��� ���� ��������Ʈ ����.
	#define EXPLOSION_MISSILE_MAXFRM_ 1						
	SPRDATA _MissileBoomSpr[EXPLOSION_MISSILE_MAXFRM_];	

	// : ������ ũ��/ �ִϸ��̼� �ð� ���/����.(T08 TDBox)
	// 
	ANIINFO _AniInfoMissileBoomSmall = { 
		{15.0f, 15.f, 1.0f}, 0, 0, 0, EXPLOSION_MISSILE_MAXFRM_,  
		1000/35, _MissileBoomSpr, &_pCurrCam						  
	};

	ANIINFO _AniInfoMissileBoomBig = { 
		{25.0f, 25.f, 1.0f}, 0, 0, 0, EXPLOSION_MISSILE_MAXFRM_,  
		1000/35, _MissileBoomSpr, &_pCurrCam
	};

	//����� �ִϸ��̼� ����. 
	// : ������ ũ��/ �ִϸ��̼� �ð� ���/����.(T08 TDBox)
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
	// �浹 ����...
	// 
	void CheckCollisionAll();
	void ChkColl_HBulletVSEnemy();
	void ChkColl_HMissileVSEnemy();	
	void ChkColl_HeroVSEnemy();				//���ΰ� vs ����.


	//-----------------------------------------------------------------------
	//
	// ���� ����.
	//
	#define ENEMY_MAX_ 2  //(2*2*2) : 8�밡 �����.)
	//#define ENEMY_MAX_ 10  //(10*10*2) : 200�밡 �����.)
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
	//  ���� ���� üũ ����. : 2004.06.24. 
	//
	BOOL GameOver(float dTime);
	BOOL _bIsGameOver = FALSE;		//���� ����: ���ΰ��� �װ�..���� �ִϰ� ������..
	//���Ŀ� ���̵� �ƿ�...�̰��� �����ϴ� ���� ����.
	BOOL _bEngineStop = FALSE;



	//
	// ����ü �ø� ����.
	//
	// �ø��� ����� : �� ����� / ���Ѿ� & �̻���
	//               : ���ΰ� �Ѿ� / �̻��� & �̻��� ���� 
	//               : ���������� '��ü' ��.. 
	// => ������ Draw ��ƾ���� �ø�üũ�� �������� �õ��մϴ�.
	//
	CFrustum* _pFrustum = NULL;
	BOOL	  _bUseFrustumCulling = YES_;		//�ø� �õ� ���� �÷���.
	int		  _VisibleEnemyCnt = 0;				//���̴� ���� ���. (�ø��� ���� �����)
	


	//
	//  '��' ����..
	//
	//
	CStar* _pStars = NULL;
	BOOL	_bShowStars = YES_;	


	//
	// ��ī�̹ڽ� ����. 
	//
	BOOL		_bShowSkyBox = YES_;
	CSkyBox*	_pSkyBox = NULL;
	SPRDATA		_SprSkyBox[CSkyBox::SBF_MAX_];

	BOOL SkyBoxInit();			//��ī�� �ڽ��� ������ ����.
	void SkyBoxRelease();		//��ī�� �ڽ��� ������ ����.




	//
	// ���̴� ����
	//
	CRadar*	_pRadar = NULL;
	SPRDATA _sprRadarV;
	BOOL RadarInit();
	void RadarRelease();

	void RadarDrawTDBox(float dTime);	//Lock-On �� ��ǥ���� ���� ���(+TDBox)
	void _RadarDrawTDBoxRect(CEnemy* pEmy, float dTime);	//��
	void _RadarDrawFireQue(CEnemy* pEmy, float dTime);	//��
	//void _RadarDrawTDBoxRect(VECTOR3 vPos);	//��
	//void _RadarDrawFireQuey(VECTOR3 vPos);	//��

	//VECTOR3 Cal2DPos(CEnemy* pEmy);		//3D -> 2D ��ǥ ��ȯ �Լ�.
	VECTOR3 Cal2DPos(VECTOR3 vPos);		//3D -> 2D ��ǥ ��ȯ �Լ�.

	SPRDATA	_sprTDBoxSmall;				//'TD Box Small' : 2004.12.07.��
	SPRDATA	_sprTDBoxBig;				//'TD Box Big' : 2004.12.07.��
	SPRDATA	_sprFireQue;			//'Fire Que' : 2004.12.07.��
	CEnemy* _pLockOnEmy = NULL;		//Lock-On �� ���� ������.
	
	BOOL	_bShowAzimuth = FALSE;	//��

};


using namespace GP;










//////////////////////////////////////////////////////////////////////////////
//
BOOL GP::Loading()
{
	// ������������ �۾� ���� ���� : 
	// �޽� ������ �ؽ��Ĵ� �մϴټ� ���ο���  �ε�Ǳ� ������ 
	// �ؽ��İ� �ִ� path �������� �ܺο��� '�۾�' ������ ������ �־�� �մϴ�.
	//	 
	char currdir[256];
	::GetCurrentDirectory(256, currdir);
	::SetCurrentDirectory("../data");
  

	//���ΰ� �޽� �ε�.	
	_pPlane = new CB3Mesh(g_B3pDevice, "airplane03.x");
	ASSERT(_pPlane != NULL); 

	 
	//���ΰ� Ŭ���� ��ü ����.
	_pHero = new CHero(g_B3pDevice, _pPlane, 
						1,					//Hp. 
						5.0f,				//�浹 ������.
						&_AniInfoBoomSmall,//���� �ִϸ��̼� ����. 
						_HeroPos, 
						VECTOR3(0.0f, 0.0f, 0.0f));
	ASSERT(_pHero != NULL);


	_pAssistMesh = new CB3Mesh(g_B3pDevice, "bigship1.x");
	ASSERT(_pAssistMesh != NULL); 
	 
	//���ΰ� Ŭ���� ��ü ����.
	_pAssist = new CAssist(g_B3pDevice, _pAssistMesh, 
						1,					//Hp. 
						5.0f,				//�浹 ������.
						&_AniInfoBoomSmall,//���� �ִϸ��̼� ����. 
						_vAssistPos, 
						VECTOR3(0.0f, 0.0f, 0.0f));
	ASSERT(_pAssist != NULL);




	//��ƼŬ �ؽ��� �ε�.
	B3DSprite_Load(&_sprParticle, "Particle.bmp");
	B3DSprite_Scaling(&_sprParticle, 10.0f, 10.0f);				 
	B3DSprite_Translation(&_sprParticle, 0.0f, 0.0f, 0.0f, ON3D_);		 
	

	// �ٴ� ���� 2D �޽�.
	B3DSprite_Load(&_sprMap, "earth01.jpg");
	B3DSprite_Scaling(&_sprMap, 320, 240); 
	B3DSprite_Translation(&_sprMap, 0.0f, 0.0f, 300.0f, ON3D_);	

	//���ϰ� �� ��ƼŬ�� ������ ����.
	_sprRailGun = _sprParticle;

	//����̵� ��������Ʈ �ε�.
	B3DSprite_Load(&_sprTornado, "shine0.bmp");
	B3DSprite_Scaling(&_sprTornado, 10, 10); 
	B3DSprite_Translation(&_sprTornado, 0.0f, 0.0f, 0.0f, ON3D_);	



	//�̻��� ����� �ؽ��� 
	B3DSprite_Load(&_sprParticle2, "����2.bmp");
	B3DSprite_Scaling(&_sprParticle2, 1.0f, 1.0f);				 
	B3DSprite_Translation(&_sprParticle2, 0.0f, 0.0f, 0.0f);			//�ϴ� �⺻��. 

	//�̻��� �޽�.
	_pMeshMissile = new CB3Mesh(g_B3pDevice, "��Ŭ��3.x");

 
	//���� �ִϿ� ��������Ʈ�ε�.
	ExplosionSpriteLoad();
 


	// 2004.07.02.
	// �̻��� ������. : '�����'�� ���� ǥ�ñ�� ����� ���̴�. 
	// 2D ���� �������� ����Ұ���.
	WeaponIndicatorLoad();

	//'ADI' �� �޽� �ε�.
	ADILoad();

	//3D Rader ����.
	RadarInit();

	//2���� ���̴� 
	RWRLoad();		



 
	// ���� �ʱ�ȭ.
 	EnemyInit();
	EnemyCreate(_pPlane);		//���� ���� : �ӽ�. 2004.06.22.


 
	//�۾� ���� ����.
	::SetCurrentDirectory(currdir);

 

	//------------------------------------------------------------------------- 
	// ī�޶� �¾� 
	//------------------------------------------------------------------------- 
	CameraSetup();	//������Ʈ.



	//------------------------------------------------------------------------- 
	// ���� �׽�Ʈ. : '�̻���'���� ���� ������ �մϴ�!! 
	//------------------------------------------------------------------------- 
	SmokeInit();


	//------------------------------------------------------------------------- 
	// �̻��� �����.
	//------------------------------------------------------------------------- 
	

	AssistMissileCreate(_pAssist, _pMeshMissile, &_sprParticle, &_sprParticle2, 
				 _vAstHardPoint, &_pCurrCam, 0.0f);

	MissileCreate(_pHero, _pMeshMissile, &_sprParticle, &_sprParticle2, 
				 _vHardPoints, &_pCurrCam, 0.0f);

	//------------------------------------------------------------------------- 
	// �ö�� ĳ�� �ʱ�ȭ. 
	//------------------------------------------------------------------------- 
	PlasmaCannonInit();


	//-------------------------------------------------------------------------
	// �þ� ����ü �ø� ����.
	// -> �������� ����� ������ �ѱ�� : ���� 'ī�޶�'�� ���� ���̴�.
	//-------------------------------------------------------------------------
	_pFrustum = new CFrustum;
	ASSERT(_pFrustum != NULL);

	//����ü �ø��� ��ü �ʱ�ȭ.
	_pFrustum->Init(
 		_pCurrCam->GetFovY(),
		_pCurrCam->GetRatio(),  
		_pCurrCam->GetZnear(),  
		_pCurrCam->GetZfar()    
		);
 
	//'����' Ŭ�����鿡�� �Ҵ��ϱ�. 
	CUnit::InitFrustum(_pFrustum);							//-> �� �޼ҵ� ���ο��� �̹� Culling On! ��Ų��.
	CUnit::SetFrustumCullingState(_bUseFrustumCulling);	//�ø� ���� ����. 


	//-------------------------------------------------------------------------
	// '��' �����...
	//-------------------------------------------------------------------------
	// ��, 2048 �� �����.
	_pStars = new CStar(g_B3pDevice,  
						200,				//
						&_sprParticle,		//
						_pHero, &_pCurrCam,
		                g_Mode.Width		//
						);		
	ASSERT(_pStars != NULL);


	//--------------------------------------------------------------------------
	// '��ī�� �ڽ�' �����.
	//--------------------------------------------------------------------------
	SkyBoxInit();
 
	return TRUE;
}











//////////////////////////////////////////////////////////////////////////////
//
BOOL GP::Run(float dTime)
{  
	
	//------------------------- 
	// �ý���.
	//------------------------- 
	if(IsKeyUp(DIK_F1))	_bShowHelp  ^= TRUE;			//�Ϲ� ����.
	if(IsKeyUp(DIK_F2))	_bShowMisInfo ^= YES_;			//�̻��� ����. 
	if(IsKeyUp(DIK_F4))	_bShowSkyBox ^= YES_;			//�ϴ� ���� on/off. 
	if(IsKeyUp(DIK_F5)) _pHero->Reset();				// ����� ��ġ ����.  
   
	if(IsKeyUp(DIK_F9))									//�ø� On/Off
 	{
		_bUseFrustumCulling = CUnit::GetFrustumCullingState();			
		CUnit::SetFrustumCullingState(_bUseFrustumCulling ^= TRUE);		
 	}

	//'�ػ�' ����ø����� ����ü ������Ʈ.
	/*if(_bUseFrustumCulling)
	{
		_pFrustum->Init(
			_pCurrCam->GetFovY(),
			_pCurrCam->GetRatio(),  
			_pCurrCam->GetZnear(),  
			_pCurrCam->GetZfar()    
			); 
	}*/

	if(IsKeyUp(DIK_F10)) _bShowStars ^= TRUE;			// �� ���� �ֱ�..


	if(IsKeyUp(DIK_V)) _bBillBoardON ^= TRUE;			// ������.-> off.
	if(IsKeyUp(DIK_BACKSPACE))  _bUseViewport ^= TRUE;	// ����Ʈ	

 	if(IsKeyUp(DIK_K)) _bShowAzimuth ^= TRUE;			//�� 


	//------------------------ 
	// ���ΰ� �̵� ó��. 
	//------------------------ 
	_pHero->Move(dTime);

	_pAssist->Move(dTime);
			

	//------------------------ 
	// 'ȸ���� ����' ADI ǥ�ñ� �����̱�.
	//------------------------ 
	ADIMove(_pHero); 


	//------------------------ 
	// ���� ����/ �߻�.
	//------------------------ 
 	if(IsKeyDown(DIK_C)) RailGunCreate(dTime);
	if(IsKeyDown(DIK_N)) TornadoCreate(dTime);		
	if(IsKeyDown(DIK_SPACE)) PlasmaCannonCreate(_pHero, &_pCurrCam);	
 
	// �̻��� �߻�. 
	// 
	if(IsKeyUp(DIK_Z))
		MissileFire(FALSE, FALSE);		//�Ϲ� �̻��� �߻�.

	if(IsKeyUp(DIK_X))
		MissileFire(TRUE, FALSE);		//�ٹ̻��� �߻�!!

	if(IsKeyUp(DIK_Z) && IsKeyDown(DIK_LCONTROL))	//Fire All!!
		MissileFire(TRUE, TRUE);

	if(IsKeyUp(DIK_F7))					//�̻��� ����.
	{
		MissileRelease();
		MissileCreate(_pHero, _pMeshMissile, &_sprParticle, &_sprParticle2,
			_vHardPoints, &_pCurrCam, 0.0f);
	}
  
	if(IsKeyUp(DIK_F8))					//���� ����.
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
	// ���� �̵�.
	//----------------------- 
 	EnemyMove(dTime);


	//------------------------ 
	// ī�޶� ������Ʈ. 	
	//------------------------ 
	CameraUpdate(dTime);	


	//------------------------ 
	// �ϴû��� ���� : ī�޶� ���� ���� ó��.
	//------------------------ 
	_pSkyBox->Move(dTime);

	//------------------------ 
	//�� �̵�..
	//------------------------ 
	if(_bShowStars) _pStars->Move(dTime);


	//------------------------ 
	// ����.�̵�.
	//------------------------ 
	MissileMove(dTime);			// �̻��� & ���� �̵�.
	SmokeMove(dTime); 
	RailGunMove(dTime);			// ���ϰ� �̵�.
 	
	PlasmaCannonMove(dTime);	// �ö��.

	AssistMissileMove(dTime);

 
	//���̴� ������Ʈ.
 	RWRUpdate(dTime);
	_pRadar->Update();			
	

	//------------------------ 
	// �浹 �˻�.
	//------------------------ 
	ChkColl_HBulletVSEnemy();
	ChkColl_HMissileVSEnemy();	
	ChkColl_HeroVSEnemy();		



	//------------------------ 
	// ������.. 
	//------------------------ 
	if(B3X_BeginScene())		
	{  
		COLOR col = ChangeBkColor();	//���� ����
		B3X_BBClear(col);					
 		  

		

		//----------------------------------// 
		// ��ī�� �ڽ� �׸���.			//
		//----------------------------------// 
		if(_bShowSkyBox) _pSkyBox->Draw();
		//B3Spr_Draw(&_sprMap, ON3D_, NOABLEND_, 1.0f, NOZWRITE_);	// ��� 

		
		//----------------------------------// 
		// ���׸���..
		//----------------------------------// 
		if(_bShowStars) _pStars->Draw(dTime);	 

		//----------------------------------// 
		// ���ӿ���							//
		//----------------------------------// 
		if(_bIsGameOver == FALSE)			//���� ���ᰡ �ƴҶ���..���ΰ��� �׸��ϴ�..
		{
			if(_pHero->Draw(dTime) && _pAssist->Draw(dTime) == FALSE)		//���ΰ��� �װ�, '���� �ִ�' ������ FALSE �� ����..
			{					 
				_bIsGameOver = TRUE;		//���� ����... 
			}
		}
 
		//----------------------------------// 
		// ��Ÿ ������ : �����.. �׸���.
		//----------------------------------// 
		
		EnemyDraw(dTime);							 
  		MissileDraw(dTime);				
		SmokeDraw(dTime);				
 		RailGunDraw(dTime);				
		PlasmaCannonDraw(dTime);			

		AssistMissileDraw(dTime);

		//ParticleDraw(dTime);		 		 

 


		//----------------------------------// 
		// ����Ʈ - ����...
		//----------------------------------// 
		if(_bUseViewport) ViewportOn(dTime); 


		//----------------------------------// 
		// �������̽�.						//
		//----------------------------------//  
 		_pRadar->Render(_bShowAzimuth);		//���̴� �׸���.��
		 

		ADIDraw();				//ADI ������.
 		
		//WI ������.(WI �� ������������ ó��)
		B3X_2DOn();
		WeaponIndicatorDraw(_pMissile, MISSILE_MAX_);	
		RWRDraw();				//RWR :2D Radar
 		B3X_2DOff();

		RadarDrawTDBox(dTime);			//LOCK-ON ���� ���� ���.


		//----------------------------------// 
		// GAME OVER - FADE OUT...			//
		//----------------------------------// 
		if(_bIsGameOver)				//���� �����..
		{
			if(GameOver(dTime))				//FADE OUT....�� ������ TRUE ����.
				_bEngineStop = TRUE;						 
		}

 
		//���� �޼���, ����.
 		GP::ShowHelp();
		B3X_StockObjsDraw();			//���� / �⺻ ������ �׸���.


 
		B3X_EndScene();
	}
  
	
	B3X_Flip();			// ȭ�� ����(Present) 
 

	return TRUE;
}




 

//////////////////////////////////////////////////////////////////////////////
//
void GP::Release()
{
	RailGunRelease();					//���ϰ� ����

	B3DSprite_Release(&_sprMap);		//��� ��������Ʈ ����.   
	B3DSprite_Release(&_sprParticle);	//��ƼŬ �� ��������Ʈ ����.
	B3DSprite_Release(&_sprTornado);	//����̵� ��������Ʈ ����.


	SAFE_DELETE(_pPlane);				//����� �޽� ����.
	SAFE_DELETE(_pHero);				//���ΰ� ����.

	SAFE_DELETE(_pAssistMesh);
	SAFE_DELETE(_pAssist);
 
	for(int i=0; i<CAM_MAX_; i++)
	{
		SAFE_DELETE(_pCurrCamSet[i]);	//ī�޶� ����.
	}


	SAFE_DELETE(_pMeshMissile);			//�̻��� �޽� ����.
	MissileRelease();					//�̻��� ����.
	SmokeRelease();						//���� ����.
	PlasmaCannonRelease();				//�ö�� ����.
 	ExplosionSpriteRelease();		//���� �ִϿ� ��������Ʈ ����.

	SAFE_DELETE(_pFrustum);			//����ü �ø� ��ü ����. 2004.06.25.

	SAFE_DELETE(_pStars);			//'��' ����.

	SkyBoxRelease();				//��ī�� �ڽ� ����. 2004.06.30.

	WeaponIndicatorRelease();		
	ADIRelease();			

	RadarRelease();					//���̴� ����.
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
//  ���̴� ����.
//
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// 
BOOL GP::RadarInit()
{
	//------------------------------------------------------------------------
	// ���̴� �ʱ�ȭ
	B3DSprite_Load(&_sprRadarV, "radar_v.png");

	B3DSprite_Scaling(&_sprRadarV, 16.0f, 128.0f);		 
	D3DXMatrixIdentity(&_sprRadarV.mRot); 
	D3DXMatrixIdentity(&_sprRadarV.mTrans); 

	//TD Box 
	B3DSprite_Load(&_sprTDBoxSmall, "TDBox16.bmp");	//��
	B3DSprite_Scaling(&_sprTDBoxSmall, _sprTDBoxSmall.Width, _sprTDBoxSmall.Height);
	B3DSprite_Load(&_sprTDBoxBig, "TDBox32.bmp");	//��
	B3DSprite_Scaling(&_sprTDBoxBig, _sprTDBoxBig.Width, _sprTDBoxBig.Height);
	B3DSprite_Load(&_sprFireQue, "FireQue32.bmp");	//��
	B3DSprite_Scaling(&_sprFireQue, _sprFireQue.Width, _sprFireQue.Height);


	D3DVIEWPORT9 vp = { (DWORD)(g_B3Mode.Width  * 0.5f), 0 ,		//sx, sy
						180, //(DWORD)(g_B3Mode.Width  * 0.25f),		//width.
						138, //(DWORD)(g_B3Mode.Height * 0.25f),		//height.
						0.0f, 0.0f			//�ɼ� ����
					};

	_pRadar = new CRadar(	g_B3pDevice,	// DEVICE ������
							&_pCurrCam,		// ī�޶� ������
							vp,				// ������Ʈ ����
							_EnemyList ,	// ���� list
							_pHero ,		// ���ΰ� ������
							&_sprRadarV,	// ���̴� ��������Ʈ
							&_sprParticle	// ��ƼŬ ��������Ʈ
							);
	ASSERT(_pRadar != NULL);

	return TRUE;
 
}


///////////////////////////////////////////////////////////////////////////
// 
void GP::RadarRelease()
{
	SAFE_DELETE(_pRadar);			// ���̴� ���� 
	B3DSprite_Release(&_sprRadarV);	// ������ �ؽ��� ����

	B3DSprite_Release(&_sprTDBoxSmall); // TD Box ����.
	B3DSprite_Release(&_sprTDBoxBig); // TD Box ����.
	B3DSprite_Release(&_sprFireQue); // TD Box ����.

}







/////////////////////////////////////////////////////////////////////////////
//
// Lock-On �� ��ǥ�� ���� �� TDBox �� ����Ѵ�.
//
void GP::RadarDrawTDBox(float dTime) 
{
	//ī�޶� ��ȣ üũ...: 1,2,3�� ī�޶� �ÿ��� ����Ѵ�.
	if( (   _CurrCamNum == CAM_PILOT_  
		|| _CurrCamNum == CAM_FIX_FRONTVIEW_ 
		|| _CurrCamNum == CAM_FIX_COMBATVIEW_		
		)== FALSE) 
		return;
 	
 	
	//
	// ��ĵ�� ��� ���⿡�� TD Box ����ϱ�.��
	//
	typedef list<CEnemy*>::iterator itor;
	list<CEnemy*>* pScanList = _pRadar->GetScanList(); 
  	for(itor ii = pScanList->begin(); ii != pScanList->end(); ii++) 
	{
		if( (*ii)->IsLockON() ) continue;

		_RadarDrawTDBoxRect( *ii, dTime); 
	}
 
	
	//���̴����� Ÿ������ ������ ���⸦ �����Ѵ�.
	_pLockOnEmy  = const_cast<CEnemy*>(_pRadar->GetTarget());


	//
	// ��ǥ���� ���ų�..'���� ���̶��'....
	//
	if(_pLockOnEmy)
	{ 
 		// Lock-ON �� ���⿡�� Fire Que  ����ϱ�.��
		//
		_RadarDrawFireQue( _pLockOnEmy, dTime);	
 	}
	else
	{
		int x = g_B3Mode.Width/2 - 8*4; 
		int y = g_B3Mode.Height/2 + 50;	 
		B3X_DrawText(FALSE, x, y, COLOR(0.0f, 1.0f, 0.0f, 1.0f), ">> NO TGT");
		//return; @fix. 2009.07.03.��
	}

 
}



 
/////////////////////////////////////////////////////////////////////////////
//
//void GP::_RadarDrawTDBoxRect(VECTOR3 vPos)
void GP::_RadarDrawTDBoxRect(CEnemy* pEmy, float dTime)
{
	// 
	// : bug fix. 
	// : Cal2DPos() �Լ� ���� , �� ������ ���ΰ� ��ü�� ���� ��������, 
	//   TDBox �� ���ΰ� ��ü�� ���鿡 ǥ�õǴ� ���� �������� �ڵ�.
	//
	VECTOR3 vViewPos, vPos = pEmy->GetPos();
	MATRIX mView = _pCurrCam->GetViewMtx();
	D3DXVec3TransformCoord(&vViewPos, &vPos, &mView);		
	if(vViewPos.z < 0) return ;


	//
	//'2D ��ǥ' ��ȯ ��� ���ϱ�.
	//
	VECTOR3 vP = Cal2DPos( vPos );	//�� ��ġ�� 2D ȭ�� ��ǥ�� ��ȯ.��


 	//int x = static_cast<int>(vP.x);
	//int y = static_cast<int>(vP.y);

#if 0
	/*{	//����� ��ġ ���.
		sprintf(msg, "xy(%.3f, %.3f)", vP.x, vP.y);
		int xx = g_B3DisplayMode.Width - strlen(msg) * 16; 
		int yy = 300;
		B3X_DrawText( FALSE, xx, yy, COLOR(0.0f, 1.0f, 0.0f, 1.0f));
	}*/
#endif	


	//TBDox �簢�� �����ǥ ȭ�鿵�� ����.
	float px = vP.x - _sprTDBoxSmall.Width/2;		float py = vP.y - _sprTDBoxSmall.Height/2;
	//if( px < 0 ) px = 0; if( px > (g_B3Mode.Width - _sprTDBoxSmall.Width)) px = (g_B3Mode.Width - _sprTDBoxSmall.Width);
	//if( py < 0 ) py = 0; if( py > (g_B3Mode.Height - _sprTDBoxSmall.Height)) py = (g_B3Mode.Height - _sprTDBoxSmall.Height);

	//TDBox ���. 
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
	// : Cal2DPos() �Լ� ���� , �� ������ ���ΰ� ��ü�� ���� ��������, 
	//   TDBox �� ���ΰ� ��ü�� ���鿡 ǥ�õǴ� ���� �������� �ڵ�.
	//
	VECTOR3 vViewPos, vPos = pEmy->GetPos();
	MATRIX mView = _pCurrCam->GetViewMtx();
	D3DXVec3TransformCoord(&vViewPos, &vPos, &mView);		
	if(vViewPos.z < 0) return ;



	//
	//'2D ��ǥ' ��ȯ ��� ���ϱ�.
	//
	VECTOR3 vP = Cal2DPos( vPos );	//�� ��ġ�� 2D ȭ�� ��ǥ�� ��ȯ.��


	//int x = static_cast<int>(vP.x);
	//int y = static_cast<int>(vP.y);

#if 0
	/*{	//����� ��ġ ���.
		sprintf(msg, "xy(%.3f, %.3f)", vP.x, vP.y);
		int xx = g_B3DisplayMode.Width - strlen(msg) * 16; 
		int yy = 300;
		B3X_DrawText( FALSE, xx, yy, COLOR(0.0f, 1.0f, 0.0f, 1.0f));
	}*/
#endif	




	// ��ǥ���� �ִٸ�..
	static BOOL bShow = YES_;
	static float time = 0.0f;
	if((time += dTime) > .7f)
	{
		time = 0.0f;
		bShow ^= 1;		
	}


	//TBDox �簢�� �����ǥ ȭ�鿵�� ����.
	float px = vP.x - _sprFireQue.Width/2;		float py = vP.y - _sprFireQue.Height/2;
	//if( px < 0 ) px = 0; if( px > (g_B3Mode.Width - _sprTDBoxSmall.Width)) px = (g_B3Mode.Width - _sprTDBoxSmall.Width);
	//if( py < 0 ) py = 0; if( py > (g_B3Mode.Height - _sprTDBoxSmall.Height)) py = (g_B3Mode.Height - _sprTDBoxSmall.Height);

	//TDBox ���. 
	
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

	//��ǥ ���� ��� ��ǥ ����.
 	int x = (int)vP.x + static_cast<int>(_sprTDBoxBig.Width) / 2 + 3;
	int y = (int)vP.y - static_cast<int>(_sprTDBoxBig.Height) / 2 ;

	// ��ǥ�� ���� ���̸�...'���߾��~' �޼��� ���~.

	if(pEmy->IsDead() == YES_)
	{ 
		B3X_DrawText( FALSE, x, y, COLOR(0.0f, 1.0f, 0.0f, 1.0f), "GOOL KILL");
		return;
	}
 
	if(bShow)
	{		 
		B3X_DrawText( FALSE, x, y, COLOR(0.0f, 1.0f, 0.0f, 1.0f), "LOCK-ON");
	}

	// ��ǥ�� ��Ÿ ���� ���.
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
		"DST[%s](%04.01f)", (bCloser)?"��":"��", vec.z);
	//"DST[%c][%04.02f]", (bCloser)?'-':'+', vec.z); 
 
}


/////////////////////////////////////////////////////////////////////////////
//
// 3D (����)��ǥ��   2D Pixel (ȭ��)��ǥ�� ��ȯ�Ѵ�.
//
//VECTOR3 GP::Cal2DPos(CEnemy* pEmy)
VECTOR3 GP::Cal2DPos(VECTOR3 vPos)
{

	//�ش� ������ ���� ��ġ ���.
	//VECTOR3 vPos = pEmy->GetPos();

	//��ȯ ��� ���� ���ϱ�.
	//MATRIX mWorld = pEmy->GetTM();
	MATRIX mView = _pCurrCam->GetViewMtx();
	MATRIX mProj = _pCurrCam->GetProjectMtx();  


	//����Ʈ ��ȯ ��� ���.
	MATRIX mViewPort; 							 
	D3DXMatrixIdentity(&mViewPort);

	mViewPort._11 = g_B3Mode.Width / 2.0f;		//@fix. 2009.07.30.�ػ󵵺���� ���� ����.
	mViewPort._22 = -(g_B3Mode.Height / 2.0f);	
	mViewPort._41 = g_B3Mode.Width / 2.0f;		
	mViewPort._42 = g_B3Mode.Height / 2.0f;		


	//���� '2D ��ǥ' ��ȯ ��� ���ϱ�. 
	MATRIX mTM = mView * mProj * mViewPort;

	//2D ��ǥ ���.(��� ����� Ŭ���̾�Ʈ ��ǥ)
	D3DXVec3TransformCoord(&vPos, &vPos, &mTM);

	//���� ����ϸ� �Ʒ��� ����.
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
// Sky Box ����.
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
// 
// ��ī�� �ڽ��� ������ �ε��ϱ�...
//
BOOL GP::SkyBoxInit()
{

	//���� ����.
	//data ������ ����.
	char OldDir[256];
	::GetCurrentDirectory(256, OldDir);			//���� ���� ����.
	//::SetCurrentDirectory(B3D_GetWorkPath());	//'�ʱ�' �۾� ���� ����.
	::SetCurrentDirectory("../data");			//������ ������ ����.

	for(int i=0; i<CSkyBox::SBF_MAX_; i++)
	{
		char filename[80];
		sprintf(filename, "SkyBox0%d.bmp", i);
		//sprintf(filename, "SkyBox_Name/SkyBox0%d.bmp", i);	//�� �� �׽�Ʈ.
		//sprintf(filename, "skybox/Sky512_0%d.jpg", i);		//Edge Test.
		B3DSprite_Load(&_SprSkyBox[i], filename);
		//�ε� ������ ����ó�� �ڵ� �߰�....
	}

	::SetCurrentDirectory(OldDir);				//���� ���� ����.

	_pSkyBox = new CSkyBox(g_B3pDevice, _pHero, &_pCurrCam, _SprSkyBox);
	if(_pSkyBox == NULL)
	{
		B3D_Error(YES_, "[Error] ��ī�� �ڽ� ��ü ���� ����...");
		return FALSE;
	}

	return TRUE;
}



///////////////////////////////////////////////////////////////////////////
// 
// ��ī�� �ڽ� ������ ����.
//
void GP::SkyBoxRelease()
{
	//��ī�� �ڽ� ��ü ����.
	SAFE_DELETE(_pSkyBox);

	//�ؽ��� ����.
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
// ���ΰ� �׾��� ���...fade out �ϰ� ������ �����մϴ�.
// ������ �����ϱ� ���ؼ�..TRUE ����.
//
BOOL GP::GameOver(float dTime)
{
	//B3DDevice_3DOff(); 			//���� �������� ��ȯ.
	static float exittime = 0;
	exittime += dTime;
 
	_CurrCamNum = CAM_TRACKING_;
	_pCurrCam = _pCurrCamSet[ _CurrCamNum ];		//�⺻ī�޶�.
	
	if(exittime >= 3.0f)
	// '���̵� �ƿ�' ��Ų��...
	//if(B3DSprite_Fade(FADE_OUT_, 0.02f) == TRUE)		//fade out �ܰ�� 50 �ܰ��..
	{
		exittime = 0.0f;
		// ��� ���� �Ǹ�..�극��ũ...
		//B3D_EndRender();		//�� �մϴټ��� ȣ������ �ʰ� �׳� ������ 
		//���� �������� �����ϰ� �˴ϴ�..
		return TRUE;			//���� ���Ḧ ���� TRUE ����.

	}

	//B3D_EngineSync(35);			//���̵� �ӵ��� ���߱� ���ؼ�..


	return FALSE;				// ���� FADE OUT ������ �ʾҾ���.  
}
 



////////////////////////////////////////////////////////////////////////////
//
// ���� ����.
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

void GP::EnemyInit() 
{
	//�ʱ�ȭ... 
}


////////////////////////////////////////////////////////////////////////////

void GP::EnemyCreate(CB3Mesh* pMesh)
{
	CEnemy* pEnemy = NULL;

	//���� ���ΰ� ����.
	float sx = 0.0f, sz = -300.0f;
	for(int j=0; j<ENEMY_MAX_; j++)
	{
		for(int i=0; i<ENEMY_MAX_; i++)
		{
			pEnemy = new CEnemy(g_B3pDevice, pMesh, 1,	5.0f, 
				&_AniInfoBoomSmall,	//���� �ִ� ����. 2004.06.23+
				VECTOR3( sx + i*20.0f, 10.0f, sz + j*30.0f));

			_EnemyList.push_back(pEnemy);		//���̸� ����Ʈ�� ���..

			pEnemy = new CEnemy(g_B3pDevice, pMesh, 1, 5.0f, 
				&_AniInfoBoomSmall,	//���� �ִ� ����. 2004.06.23+
				VECTOR3( sx + i*20.0f, -10.0f, sz + j*30.0f));

			_EnemyList.push_back(pEnemy);		//���̸� ����Ʈ�� ���..
		}
	}

}

 
/*

//���� ���ΰ� ����. '��' �ڷ� ��ġ�ϱ�.
float sx = -50.0f, sz = 0.0f;

for(int i=0; i<ENEMY_MAX_; i++)
{
pEnemy = new CEnemy(g_B3pDevice, pMesh, 1,	5.0f,	
&_AniInfoBoomSmall,	//���� �ִ� ����. 2004.06.23+
VECTOR3( sx + i*100.0f, 30.0f, 10.0f));

_EnemyList.push_back(pEnemy);		//���̸� ����Ʈ�� ���..
}


for(int i=0; i<ENEMY_MAX_; i++)
{
pEnemy = new CEnemy(g_B3pDevice, pMesh, 1,	5.0f, 
&_AniInfoBoomSmall,	//���� �ִ� ����. 2004.06.23+
VECTOR3( 10.0f, -30.0f, sz + i * 100.0f));

_EnemyList.push_back(pEnemy);		//���̸� ����Ʈ�� ���..
}

*/


////////////////////////////////////////////////////////////////////////////
//
// �� ����� �̵�.
// 
// : ����ü �ø��� ������� ��� �������� �մϴ�! �׷��� ������, 
// : ���� �������, ���ڱ� ����Ⱑ ��Ÿ���� �˴ϴ�.....
//
void GP::EnemyMove(float dTime)
{
	for(EmyItor_ i = _EnemyList.begin(); i != _EnemyList.end();)
	{
		//false �� ���ϵǸ� ����. �� ���� ���� �ƴϰ� 		
		//'���� ����'�� ���� ���� ��쿡��  ���������ֽ��ϴ�.
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
// �� ����� �׸���. 
//
// 2004.06.25. ����ü �ø� �߰�: ���̰͸� �׸��ϴ�. 
//
//
void GP::EnemyDraw(float dTime)		
{ 
	for(EmyItor_ i = _EnemyList.begin(); i != _EnemyList.end(); )
	{
		if((*i)->Draw(dTime) == FALSE)	//���� �ִϰ� ������ 'FALSE' �� ���ϵȴ�....
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
		delete (*i);				//�޸� ����.
		_EnemyList.erase(i++);		//������ ����.

	}

}









/////////////////////////////////////////////////////////////////////////////
// 
// �浹 ����...
//

// 
// ���ΰ� �Ѿ˰� ������� �浹�˻�...
// ���ϰ��� ���忡�� �������� �浹�� �˻��մϴ�.
//
void GP::ChkColl_HBulletVSEnemy()
{
	//���ΰ� �Ѿ�...
	for(int j=0; j<_RailGun_Max_; j++)
	{
		if(_pRailGun[j] == NULL) continue;	//�Ѿ��� ������ ����...

		for(EmyItor_ i=_EnemyList.begin(); i != _EnemyList.end(); ++i)			//���� ����..
		{
			// ���Ⱑ '�׾..���� �ִ� ���̸�'...���� ���� �˻�.
			if((*i)->IsDead())		
				continue;

			// �Ѿ˰� ���� �浹�˻�.
			if( _pRailGun[j]->CheckCollision(*i) == CUnit::COLL_YES_)		
			{
				// ������ HP �� �Ѿ��� DMG ��ŭ ���ֽ��ϴ�.....
				// ü���� 0���ϸ�.. ���ο��� m_bIsDead ����.
				// ���� ���� �ִϰ� ������� ��ü�� ���ŵ˴ϴ�. => Draw���� ó���� ���ֽ��ϴ�.. 
				(*i)->TakeDamage(_pRailGun[j]->GetDmg()); 

				//�ش� �� ������ LockOn �ϴ�, �̻��ϵ��� ���� ���⿡ 
				//'Lock-Off' ��Ŵ. 
				if((*i)->IsDead())	MissileLockOff(*i);

				//�Ѿ� ����....
				SAFE_DELETE(_pRailGun[j]);

				//���� �Ѿ� �˻縦 ���� Break!
				break; 
			}
		}//end of for(EmyItor_..)

	}//end of for(int j...)
}





/////////////////////////////////////////////////////////////////////////////
//
// void ChkColl_HMissileVSEnemy()
//
// ���ΰ� �̻��ϰ� �� ����� ������ �浹�˻�..
//
void GP::ChkColl_HMissileVSEnemy()
{
	//���ΰ��� ���  �̻��� �˻�..
	for(int j=0; j<MISSILE_MAX_; j++)	
	{
		if(_pMissile[j] == NULL) continue;	//�̻����� ������ ����...

		if(!_pMissile[j]->IsReleased()) continue;	//�߻���� �ʾ�����, ����...

		// 
		// ���� ���̾ üũ�մϴ� : '���� ����'�� ���� Ŀ���� �ɰ��̴�.
		//if(g_pMissile[j]->IsDead()) continue;		
 

		for(EmyItor_ i=_EnemyList.begin(); i != _EnemyList.end(); ++i)			//���� ����..
		{
			if((*i)->IsDead())		//���Ⱑ '�׾..���� �ִ� ���̸�'...���� ���� �˻�.
				continue;

			if( _pMissile[j]->CheckCollision(*i) == CUnit::COLL_YES_)		//�̻��ϰ� ���� �浹�˻�.
			{
				//������ HP �� �̻����� DMG ��ŭ ���ֽ��ϴ�.....
				//ü���� 0���ϸ�.. ���ο��� m_bIsDead ����.
				// ���� ���� �ִϰ� ������� ��ü�� ���ŵ˴ϴ�. => Draw���� ó���� ���ֽ��ϴ�.. 
				(*i)->TakeDamage(_pMissile[j]->GetDmg()); 

				//�ش� �� ������ LockOn �ϴ�, �̻��ϵ��� ���� ���⿡ 
				//'Lock-Off' ��Ŵ. 
				if((*i)->IsDead())	MissileLockOff(*i);


				// �̻��� ����. 
				// ���ϰǰ��� �ٸ��� �ٷ� �������� �ʰ�..
				// �̻��� �ڽ��� '����' �ִϸ� �����ϰ� �����մϴ�.
				if(!_pMissile[j]->IsDead())			//���� ó�� '�浹'�Ǿ� ���� ���̶��... 
				{ 
					_pMissile[j]->SetDead();		//'�׾���' �̶�� �����ϰ�..'���� �ִ�'�� �����մϴ�.
					break;							//break �� �ؼ�, ó������ 1���� �����ϰ� �浹�˻縦 �����մϴ�.. ->�ڿ������� '���� �ݰ� Ȯ��'�� ������ �� �ֽ��ϴ�..
				}	
				//���� �̻��� �˻縦 ���� Break!  => �ּ�ó��  
				//'���� �Ǵ���' �ֺ��� ��� ������ ��ģ��...
				//�׷��� �ϴ���..�ϴ��� 1�븸 �浹.. ^^;;
				//break;		

			}
		}//end of for(itor..)

	}//end of for(int j...)

	//���ΰ��� ���  �̻��� �˻�..
	for(int j=0; j<ASTMISSILE_MAX_; j++)	
	{
		if(_pAMissile[j] == NULL) continue;	//�̻����� ������ ����...

		if(!_pAMissile[j]->IsReleased()) continue;	//�߻���� �ʾ�����, ����...

		// 
		// ���� ���̾ üũ�մϴ� : '���� ����'�� ���� Ŀ���� �ɰ��̴�.
		//if(g_pMissile[j]->IsDead()) continue;		
 

		for(EmyItor_ i=_EnemyList.begin(); i != _EnemyList.end(); ++i)			//���� ����..
		{
			if((*i)->IsDead())		//���Ⱑ '�׾..���� �ִ� ���̸�'...���� ���� �˻�.
				continue;

			if( _pAMissile[j]->CheckCollision(*i) == CUnit::COLL_YES_)		//�̻��ϰ� ���� �浹�˻�.
			{
				//������ HP �� �̻����� DMG ��ŭ ���ֽ��ϴ�.....
				//ü���� 0���ϸ�.. ���ο��� m_bIsDead ����.
				// ���� ���� �ִϰ� ������� ��ü�� ���ŵ˴ϴ�. => Draw���� ó���� ���ֽ��ϴ�.. 
				(*i)->TakeDamage(_pAMissile[j]->GetDmg()); 

				//�ش� �� ������ LockOn �ϴ�, �̻��ϵ��� ���� ���⿡ 
				//'Lock-Off' ��Ŵ. 
				if((*i)->IsDead())	MissileLockOff(*i);


				// �̻��� ����. 
				// ���ϰǰ��� �ٸ��� �ٷ� �������� �ʰ�..
				// �̻��� �ڽ��� '����' �ִϸ� �����ϰ� �����մϴ�.
				if(!_pAMissile[j]->IsDead())			//���� ó�� '�浹'�Ǿ� ���� ���̶��... 
				{ 
					_pAMissile[j]->SetDead();		//'�׾���' �̶�� �����ϰ�..'���� �ִ�'�� �����մϴ�.
					break;							//break �� �ؼ�, ó������ 1���� �����ϰ� �浹�˻縦 �����մϴ�.. ->�ڿ������� '���� �ݰ� Ȯ��'�� ������ �� �ֽ��ϴ�..
				}	
				//���� �̻��� �˻縦 ���� Break!  => �ּ�ó��  
				//'���� �Ǵ���' �ֺ��� ��� ������ ��ģ��...
				//�׷��� �ϴ���..�ϴ��� 1�븸 �浹.. ^^;;
				//break;		

			}
		}//end of for(itor..)

	}//end of for(int j...)
}







/////////////////////////////////////////////////////////////////////////////
//
// void ChkColl_HeroVSEnemy() 
//
// ���ΰ� �� �� ����� ������ �浹�˻�..
//
void GP::ChkColl_HeroVSEnemy()
{
	//���ΰ� �˻�. 
	if(_pHero->IsDead()) return;			//���ΰ��� '������'�̸� ����...

	//���� ����..
	for(EmyItor_ i=_EnemyList.begin(); i != _EnemyList.end(); ++i)			
	{
		if((*i)->IsDead())		//���Ⱑ '�׾..���� �ִ� ���̸�'...
			continue;			//���� ���� �˻�.

		//�̻��ϰ� ���� �浹�˻�.
		if( _pHero->CheckCollision(*i) == CUnit::COLL_YES_)		
		{
			//������ HP �� DMG ��ŭ ���ֽ��ϴ�.....
			//ü���� 0���ϸ�.. ���ο��� m_bIsDead ����.
			// ���� ���� �ִϰ� ������� ��ü�� ���ŵ˴ϴ�. => Draw���� ó���� ���ֽ��ϴ�.. 
			(*i)->TakeDamage(10);		//�� ������ ���� �ٸ��� ������ 
			//�ϴ� 10���� �ο�. 004.06.24.
			//���ΰ� ������ ������...����� ����. 
			_pHero->TakeDamage(1);		

			//���ΰ��� �׾��ٸ�...üũ ��ƾ���� break; 
			if(_pHero->IsDead())  
			{
				//���ΰ��� ������ �ִ� ��� �ܺ� ����鵵 ���ش�...
				//Ȥ�� ��� '����' ��Ű�� ���� ��������... ^^;;
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
// ���� �ִϸ��̼� ����. 
//
////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////

void GP::ExplosionSpriteLoad()
{
	char filename[80];
	for(int i=0; i<EXPLOSION_MAXFRAM_; i++)
	{
		B3DSprite_Release(&_ExplosionSpr[i]);		//������ �ε�Ȱ��� ������ ����.

		// �� ��������Ʈ �ε�.
		sprintf(filename, "Explosion_Frame0%d.dds", i+1);
		B3DSprite_Load(&_ExplosionSpr[i], filename );

		B3DSprite_Scaling(&_ExplosionSpr[i], 10.0f, 10.0f);		 
		D3DXMatrixIdentity(&(_ExplosionSpr[i].mRot)); 
		D3DXMatrixIdentity(&(_ExplosionSpr[i].mTrans)); 

	}




	//�̻��� ���� ��������Ʈ �ε�.
	for(int i=0; i<EXPLOSION_MISSILE_MAXFRM_; i++)
	{
		B3DSprite_Release(&_MissileBoomSpr[i]);		//������ �ε�Ȱ��� ������ ����.

		// �� ��������Ʈ �ε�.
		sprintf(filename, "Particle.bmp"); //"Explosion_Missile0%d.bmp", i+1);
		B3DSprite_Load(&_MissileBoomSpr[i], filename );

		B3DSprite_Scaling(&_MissileBoomSpr[i], 10.0f, 10.0f);		 
		D3DXMatrixIdentity(&(_MissileBoomSpr[i].mRot)); 
		D3DXMatrixIdentity(&(_MissileBoomSpr[i].mTrans)); 

	}




	//����� �ִ� �ε�.
	for(int i=0; i<SHOCKWAVE_MAXFRM_; i++)
	{
		//������ �ε�Ȱ��� ������ ����.
		B3DSprite_Release(&_ShockWaveSpr[i]);		

		// �� ��������Ʈ �ε�.: ��������Ʈ�� �����̴ϱ�..�̷��� �ܼ��ϰ�. �Ѥ�;
		sprintf(filename, "ShockWave.bmp");  
		B3DSprite_Load(&_ShockWaveSpr[i], filename );

		B3DSprite_Scaling(&_ShockWaveSpr[i], 1.0f, 1.0f);		 //���� ������.
		D3DXMatrixIdentity(&(_ShockWaveSpr[i].mRot)); 
		D3DXMatrixIdentity(&(_ShockWaveSpr[i].mTrans)); 
	}

}


////////////////////////////////////////////////////////////////////////////

void GP::ExplosionSpriteRelease()
{
	for(int i=0; i<EXPLOSION_MAXFRAM_; i++)
	{
		B3DSprite_Release(&_ExplosionSpr[i]);		//��������Ʈ ���� ����.
	}


	//�̻��� ���� ��������Ʈ ����. 
	for(int i=0; i<EXPLOSION_MISSILE_MAXFRM_; i++)
	{
		B3DSprite_Release(&_MissileBoomSpr[i]);	
	}


	//�̻��� ����� ��������Ʈ ���� ����.
	for(int i=0; i<SHOCKWAVE_MAXFRM_; i++)
	{
		B3DSprite_Release(&_ShockWaveSpr[i]);		
	}
}








/////////////////////////////////////////////////////////////////////////////
//
//  �̻��� ����.
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
			g_B3pDevice,						//����̽�.
			pMesh,								//�̻��� �޽�.
			ppCam,								//ī�޶� ���� ������. 2004.06.19.
			100,		//������.

		//	(i==MISSILE_NUK_)?10.f:20.0f,		 
			400.0f,		//�̻��� �ִ� ���� �ӵ�. 2004.11.12.
			pAssist->GetNView(),					//�̻��� �߻� ����. 
			300.0f,	//�̻��� ���� ����. �ð���, 10%�� �Ҹ��. 

			pAssist,								//������ ����� ������.
			NO_,								//�߻� �ʉ���.



			pSprFire,							//ȭ�� ��������Ʈ ������.
			VECTOR3(0, 0, 2.0f),						//ȭ�� ��ġ.(�̻��� �𵨰����� ��ǥ)
			VECTOR3(1.0, 1.0f, 1.0f),			//ȭ�� ũ��.
			COLOR(1.0f, 0.3f, 0.3f, 1.0f),	//ȭ�� ����.
			pSprSmoke,

			_pSmokeList,						//���� ����Ʈ.

			//(i==MISSILE_NUK_)?15.0f:8.0f,		//�浹���� ������.
			4.0f,		//�浹���� ������.
			&_AniInfoMissileBoomSmall, //���� �ִ� 
			&_AniInfoShockWaveSmall,	 //����� �ִ� ����.

			vHardPt[i],						//�̻��� ��ġ: Hard Point - �� ���� �����ġ.
			VECTOR3(0.8f, 0.8f, 0.8f)  	//�̻���  ũ�� 
			);

		ASSERT(_pAMissile[i] != NULL);

	}
}

void GP::AssistMissileFire()
{

	for(int i=0; i<ASTMISSILE_MAX_; i++)
	{

		if(_pAMissile[i] == NULL) continue;		//�̻��� �˻�. 

		if(_pAMissile[i]->IsReleased())			//�߻翩�� �˻��� �߻�!!
			continue;

		//2004.11.11. ���� �ڵ� �߰�.
		//���̴� ���� 'Lock-On' �� ������ ������ ������ 
		//�̻��Ͽ� ������ �Է��Ѵ�. 2004.11.15

		_pRadar->SearchTargets();
		_pRadar->SelectTarget(TRUE);
		_pAMissile[i]->LockOn(const_cast<CEnemy*>(_pRadar->GetTarget()));
		_pAMissile[i]->SetFire();		//�Ϲ� �̻��� �߻�.

		return;							//1�� �߻��� ����. 
	}//end of for()

}

void GP::AssistMissileMove(float dTime)
{
	for(int i=0; i<ASTMISSILE_MAX_; i++)
	{
		if(_pAMissile[i] == NULL) continue;		//�̻��� �˻�. 

		if(_pAMissile[i]->IsDead()) continue;	//'������'�� �̻�����, 
												// �̵����� �ʴ´�.

		//�̻��� �̵� : �ִ� ��Ÿ��� ������..FALSE �� ���ϵ˴ϴ�.. 
		//���� ó���մϴ�...CMIssile ���ο��� ó��������..�ѹ��� Ȯ���� �ش�.
		if(_pAMissile[i]->Move(dTime) == FALSE)		
		{			 
			_pAMissile[i]->SetDead();			//������Ŵ.
		}

	}
}

void GP::AssistMissileDraw(float dTime)
{
	for(int i=0; i<ASTMISSILE_MAX_; i++)
	{
		if(_pAMissile[i] == NULL) continue;		//�̻����� �����Ƿ� �Ѿ.

		if(_pAMissile[i]->Draw(dTime) == FALSE)	// '���� �ִ�' ������ 'FALSE' ����. 
		{
			SAFE_DELETE(_pMissile[i]);		//����.
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
	//�̻��� ����.
	for(int i=0; i<MISSILE_MAX_; i++)
	{
		//_pMissile[i] = NULL;
		_pMissile[i] = new CMissile(
			g_B3pDevice,						//����̽�.
			pMesh,								//�̻��� �޽�.
			ppCam,								//ī�޶� ���� ������. 2004.06.19.
			(i==MISSILE_NUK_)?10000:100,		//������.

		//	(i==MISSILE_NUK_)?10.f:20.0f,		 
			(i==MISSILE_NUK_)?250.f:400.0f,		//�̻��� �ִ� ���� �ӵ�. 2004.11.12.
			pHero->GetNView(),					//�̻��� �߻� ����. 
			(i==MISSILE_NUK_)?450.0f:300.0f,	//�̻��� ���� ����. �ð���, 10%�� �Ҹ��. 

			pHero,								//������ ����� ������.
			NO_,								//�߻� �ʉ���.



			pSprFire,							//ȭ�� ��������Ʈ ������.
			(i==MISSILE_NUK_)?VECTOR3(0, 0, 4.0f):VECTOR3(0, 0, 2.0f),						//ȭ�� ��ġ.(�̻��� �𵨰����� ��ǥ)
			(i==MISSILE_NUK_)?VECTOR3(8.0, 8.0, 1.0f):VECTOR3(4.0, 4.0f, 1.0f),			//ȭ�� ũ��.
			(i==MISSILE_NUK_)?COLOR(.8f, 0.5f, 1.0f, 1.0f):COLOR(1.0f, 0.3f, 0.3f, 1.0f),	//ȭ�� ����.
			pSprSmoke,

			_pSmokeList,						//���� ����Ʈ.

			//(i==MISSILE_NUK_)?15.0f:8.0f,		//�浹���� ������.
			(i==MISSILE_NUK_)?8.0f:4.0f,		//�浹���� ������.
			(i==MISSILE_NUK_)?&_AniInfoMissileBoomBig:&_AniInfoMissileBoomSmall, //���� �ִ� 
			(i==MISSILE_NUK_)?&_AniInfoShockWaveBig:&_AniInfoShockWaveSmall,	 //����� �ִ� ����.

			vHardPt[i],						//�̻��� ��ġ: Hard Point - �� ���� �����ġ.
			(i==MISSILE_NUK_)?VECTOR3(5.0f, 5.0f, 5.0f):VECTOR3(2.0f, 2.0f, 2.0f)  	//�̻���  ũ�� 
			);

		ASSERT(_pMissile[i] != NULL);

	}
}


////////////////////////////////////////////////////////////////////////////
//
// �̻��� �߻�... 2004.06.23+ : ���� �ܼ������� �ְ�. �Ѥ�;; 
//
void GP::MissileFire(BOOL bNuk, BOOL bFireAll)
{
	//
	// ��� �߻��..
	//
	if(bFireAll)
	{
		for(int i=0; i<MISSILE_MAX_; i++)
		{
			if(_pMissile[i] == NULL) continue;		//�̻��� �˻�. 

			if(_pMissile[i]->IsReleased())			//�߻翩�� �˻��� �߻�!!
				continue;

			//2004.11.11. ���� �ڵ� �߰�.
			//���̴� ���� 'Lock-On' �� ������ ������ ������ 
			//�̻��Ͽ� ������ �Է��Ѵ�. 2004.11.15
			_pMissile[i]->LockOn(const_cast<CEnemy*>(_pRadar->GetTarget()));
  			_pMissile[i]->SetFire();				//�̻��� �߻�.

		}//end of for()

		return;										//��� �߻��� ����.
	}

	//----------------------------------------------------------------
	//�ٹ̻���. 
	if(bNuk == TRUE)					
	{
		if(_pMissile[MISSILE_NUK_] == NULL) return;

		if(!_pMissile[MISSILE_NUK_]->IsReleased())		//�߻� �˻�.
		{
			//2004.11.11. ���� �ڵ� �߰�.
			//���̴� ���� 'Lock-On' �� ������ ������ ������ 
			//�̻��Ͽ� ������ �Է��Ѵ�. 2004.11.15
			_pMissile[MISSILE_NUK_]->LockOn(const_cast<CEnemy*>(_pRadar->GetTarget()));
			_pMissile[MISSILE_NUK_]->SetFire();
		}

		return;
	}

	//----------------------------------------------------------------
	// �Ϲ� �̻��� �߻�.
	//
	for(int i=0; i<MISSILE_MAX_; i++)
	{
		if(i == MISSILE_NUK_)  continue;		//�ٹ̻��� ��ȣ�� �׳� ��������..

		if(_pMissile[i] == NULL) continue;		//�̻��� �˻�. 

		if(_pMissile[i]->IsReleased())			//�߻翩�� �˻��� �߻�!!
			continue;

		//2004.11.11. ���� �ڵ� �߰�.
		//���̴� ���� 'Lock-On' �� ������ ������ ������ 
		//�̻��Ͽ� ������ �Է��Ѵ�. 2004.11.15
		_pMissile[i]->LockOn(const_cast<CEnemy*>(_pRadar->GetTarget()));
		_pMissile[i]->SetFire();		//�Ϲ� �̻��� �߻�.

		return;							//1�� �߻��� ����. 
	}//end of for()


}//end of MissileFire()




/////////////////////////////////////////////////////////////////////////////
//
void GP::MissileMove(float dTime)
{

	for(int i=0; i<MISSILE_MAX_; i++)
	{
		if(_pMissile[i] == NULL) continue;		//�̻��� �˻�. 

		if(_pMissile[i]->IsDead()) continue;	//'������'�� �̻�����, 
												// �̵����� �ʴ´�.

		//�̻��� �̵� : �ִ� ��Ÿ��� ������..FALSE �� ���ϵ˴ϴ�.. 
		//���� ó���մϴ�...CMIssile ���ο��� ó��������..�ѹ��� Ȯ���� �ش�.
		if(_pMissile[i]->Move(dTime) == FALSE)		
		{			 
			_pMissile[i]->SetDead();			//������Ŵ.
		}

	}

}

/////////////////////////////////////////////////////////////////////////////
//
void GP::MissileDraw(float dTime)		
{
	for(int i=0; i<MISSILE_MAX_; i++)
	{
		if(_pMissile[i] == NULL) continue;		//�̻����� �����Ƿ� �Ѿ.

		if(_pMissile[i]->Draw(dTime) == FALSE)	// '���� �ִ�' ������ 'FALSE' ����. 
		{
			SAFE_DELETE(_pMissile[i]);		//����.
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
// pTgt �� ���� ���� ������ ���� �Ǿ��� ���, 
// ������ ��ǥ���� ������ �ִ� �̻����� Lock Off ��Ų��.
//
void GP::MissileLockOff(CUnit* pTgt)
{
	for(int i=0; i<MISSILE_MAX_; i++)
	{
		if(_pMissile[i] == NULL) continue;		//�̻��� �˻�. 

		if(_pMissile[i]->IsDead()) continue;	//'������'�� �̻�����, ����.

		if(_pMissile[i]->GetTarget() == pTgt)	//��ǥ�� �����ϸ�..Lock off.
		{
			_pMissile[i]->LockOff();
		}
	}
}








/////////////////////////////////////////////////////////////////////////////
//
//  �̻��� ���� ����.
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
// �Ѿ� ����.
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

	//COMBATVIEW �߰��� ����, ���ϰ� ��ġ����.2009.07.03. 
	VECTOR3 vFirePos = -_pHero->GetHorz() * 1.5f
		               +_pHero->GetNView() * 2.5f;
	vFirePos += _pHero->GetPos();

	for(int i=0; i<_RailGun_Max_; i++)
	{
		if(_pRailGun[i] != NULL) continue;		//�Ѿ��� ������, ��ƾ��... 

		_pRailGun[i] = new CRailGun(
			g_B3pDevice,					//����̽�.
			NULL,							//�޽�.
			&_sprRailGun,					//3D ��������Ʈ ������.
			&_pCurrCam,						//ī�޶� ���� ������. 2004.06.19.
			1,								//������.
			COLOR(1.0f, 0.5f, 0.0f, 1.0f),	//����.
			500.0f + fabs(_pHero->GetMovVelo().z),	//�߻� �ӵ� = �ѱ� �߻� �ӵ� + ����� �ӵ�
			_pHero->GetNView(),				//�߻� ����.  
			1000.0f,						//�ִ� ��Ÿ�. 1000m
			1.0f,							//�Ѿ� �浹���� �ݰ�

			vFirePos,						//�߻� ��ġ(�ѱ� ��ġ)					
 			VECTOR3(1.5f, 1.5f, 1.0f)		//ũ��(��������Ʈ ���� z���� ũ��¹����ص� �˴ϴ�..)
			);

		//ASSERT(_pRailGun[i] != NULL); --> �Ѿ� ������ �����ϴ���..
		//									���ٽ�½ �Ѿ�ϴ�.  �Ѥ�)v

		break;		//1�� �ϼ�. break!!
	}
}







/////////////////////////////////////////////////////////////////////////////
// 
void GP::RailGunMove(float dTime)
{
	for(int i=0; i<_RailGun_Max_; i++)
	{
		if(_pRailGun[i] == NULL) continue;		//�Ѿ� �˻�. 

		//�Ѿ� �̵� : �ִ� ��Ÿ��� ������..FALSE �� ���ϵ˴ϴ�.. 
		if(_pRailGun[i]->Move(dTime) == FALSE)		
		{
			SAFE_DELETE(_pRailGun[i]);			//����.
		}
	}
}



/////////////////////////////////////////////////////////////////////////////
// 
void GP::RailGunDraw(float dTime)	
{
	for(int i=0; i<_RailGun_Max_; i++)
	{
		if(_pRailGun[i] == NULL) continue;		//�Ѿ��� �����Ƿ� �Ѿ.

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
//  ����̵� �����. : ���ϰǰ� ���� �����ϴ�. 
//
void GP::TornadoCreate(float dTime)
{	 
	for(int i=0; i<_RailGun_Max_; i++)
	{
		if(_pRailGun[i] != NULL) continue;		//�Ѿ��� ������, ��ƾ��... 

		// ������ ����̵��� ���ϰ� ����Ʈ���� �����ϰ� �����մϴ�.
		//
		_pRailGun[i] = new CTornado(		
			g_B3pDevice,					//����̽�.
			NULL,							//�޽�.
			&_sprTornado,					// ��������Ʈ ������.
			&_pCurrCam,						//ī�޶� ���� ������. 2004.06.19.
			50,								//������.
			COLOR(0.3f, 0.5f, 1.0f, 0.5f),	//����.
			80.0f + fabs(_pHero->GetMovVelo().z),	//�߻� �ӵ� = �ѱ� �߻� �ӵ� + ����� �ӵ�
			_pHero->GetNView(),				//�߻� ����.
			_pHero->GetRotMtx(),			//�߻��, ������� ȸ�����
			500.0f,							//�ִ� ��Ÿ�. 500m
			10.0f,							//�浹�� �ݰ�
			_pHero->GetPos(),				//�߻� ��ġ(�ѱ� ��ġ)
			VECTOR3(20.0f, 20.0f, 1.0f)		//ũ��(��������Ʈ ���� z���� ũ��¹����ص� �˴ϴ�..)
			);

		//ASSERT(_pRailGun[i] != NULL); --> �Ѿ� ������ �����ϴ���..
		//									���ٽ�½ �Ѿ�ϴ�.  �Ѥ�)v

		break;		//1�� �ϼ�. break!!
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
//  �������� ���� ����ī�޶� ����.
//
void GP::ViewportOn(float dTime)
{
	//----------------- 
	// ī�޶� �缳��. 
	//----------------- 
 	_pCurrCam = _pCurrCamSet[CAM_PILOT_];			//1�� ������ ī�޶� On!
	_pCurrCam->Update(dTime); 


	//----------------- 
	//����Ʈ ����.: ���� ���. 
	//-----------------  
	D3DVIEWPORT9  vp;					
	vp.X = g_B3Mode.Width - 160; 		 
	vp.Y = 200;
	vp.Width = 160;			vp.Height = 120;
	vp.MinZ = 0.0f;			vp.MaxZ = 0.0f;
	g_B3pDevice->SetViewport(&vp);


	//------------------------------------------- 
	//'Z Buffer Clear
	// ����Ʈ ������ Depth ���� Ŭ�����ؾ� �մϴ�.
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
	// Scene ����.
	//----------------------  			
  	
	//----------------------------------// 
	// ��ī�� �ڽ� �׸���.				//
	//----------------------------------// 
	if(_bShowSkyBox) _pSkyBox->Draw();
	//B3DSprite_Draw3D(&_sprMap,NO_, 1.0f, NOZWRITE_);	// ��� 

 	//ParticleDraw(dTime);			// ��ƼŬ �׸���			 
	if(_bShowStars)	_pStars->Draw(dTime);	// �� �׸���.						 


	//----------------------------------// 
	// ���ΰ�.  
	//----------------------------------// 

	_pHero->Draw(dTime);				// ���ΰ�.

	_pAssist->Draw(dTime);
	  
	//----------------------------------// 
	//  ��Ÿ...
	//----------------------------------// 
	EnemyDraw(dTime);		
	MissileDraw(dTime);		// �̻��� �׸���.					 
	SmokeDraw(dTime);		// ���� �׸���. 	
	RailGunDraw(dTime);		// �Ѿ� �׸���.						
 	PlasmaCannonDraw(dTime);		

	AssistMissileDraw(dTime);


	//------------------------ 
	// ī�޶� & ����Ʈ ����..
	//------------------------ 
 	vp.X = 0; 
	vp.Y = 0;
	vp.Width = g_B3Mode.Width;	
	vp.Height = g_B3Mode.Height;
	vp.MinZ = 0.0f;	
	vp.MaxZ = 1.0f;
	g_B3pDevice->SetViewport(&vp);

	// ��, �������ǵ��� �ٽ� ������ �־��, 
	// '������', 'help', ���� ��¿� ������ �����ϴ�.
	// -> ����ī�޶�� _CurrCamNum �̴�. ����
	_pCurrCam = _pCurrCamSet[_CurrCamNum]; 
	_pCurrCam->SetView();		


}











/////////////////////////////////////////////////////////////////////////////
//
// ī�޶� ����.
//
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////// 
//
// ī�޶� ���� :
//
// 2004.07.01. ī�޶� ���� �߰� ex) FCT_FREE_ | FCT_OUTTER_ 
//
void GP::CameraSetup() 
{
  	_pCurrCamSet[CAM_FREE_] = new CCamFree(g_B3pDevice, "����ī�޶�", FCT_FREE_ | FCT_OUTTER_, POS3( 0, 3, -30), DIR3(0, 0, 1));										ASSERT(_pCurrCamSet[CAM_FREE_] != NULL); 
	_pCurrCamSet[CAM_PILOT_] = new CCamPilot(g_B3pDevice, "������ ����", FCT_PILOT_ | FCT_INNER_, _pHero, ANG3(D3DX_PI*0.85f, D3DX_PI*0.45f, 0), POS3(0, 0.65f, -0.85f)); 	ASSERT(_pCurrCamSet[CAM_PILOT_] != NULL); 
	_pCurrCamSet[CAM_FIX_COMBATVIEW_] = new CCamModelFix(g_B3pDevice, "���� ����", FCT_FRONT_ | FCT_OUTTER_, _pHero, POS3(  0,  0,  -5), DIR3(  0, 0, -1));  ASSERT(_pCurrCamSet[CAM_FIX_COMBATVIEW_] != NULL); 
	_pCurrCamSet[CAM_FIX_FRONTVIEW_] = new CCamModelFix(g_B3pDevice, "���� ���� ī�޶�", FCT_FRONT_ | FCT_OUTTER_, _pHero, POS3(  0, 3,  20), DIR3(  0, 0, -1));  ASSERT(_pCurrCamSet[CAM_FIX_FRONTVIEW_] != NULL); 
	_pCurrCamSet[CAM_FIX_LEFTVIEW_]  = new CCamModelFix(g_B3pDevice, "���� ���� ī�޶�", FCT_LEFT_ | FCT_OUTTER_, _pHero, POS3(-12, 1,  -1), DIR3(  1, 0,  0));  ASSERT(_pCurrCamSet[CAM_FIX_LEFTVIEW_] != NULL); 
	_pCurrCamSet[CAM_FIX_RIGHTVIEW_] = new CCamModelFix(g_B3pDevice, "���� ���� ī�޶�", FCT_RIGHT_ | FCT_OUTTER_, _pHero, POS3( 12, 1,  -1), DIR3( -1, 0,  0));  ASSERT(_pCurrCamSet[CAM_FIX_RIGHTVIEW_] != NULL); 
	_pCurrCamSet[CAM_FIX_REARVIEW_]  = new CCamModelFix(g_B3pDevice, "�Ĺ� ���� ī�޶�", FCT_REAR_ | FCT_OUTTER_, _pHero, POS3(  0, 2, -15), DIR3(  0, 0,  1));  ASSERT(_pCurrCamSet[CAM_FIX_REARVIEW_] != NULL); 
	_pCurrCamSet[CAM_TRACKING_] = new CCamTracking(g_B3pDevice, "���� ī�޶�", FCT_TRACKING_ | FCT_OUTTER_, _pHero, POS3(-20, 10, -50));  	ASSERT(_pCurrCamSet[CAM_TRACKING_] != NULL); 

	_CurrCamNum = CAM_FIX_FRONTVIEW_;
	_pCurrCam = _pCurrCamSet[ _CurrCamNum ];		//�⺻ī�޶�.����.
 
}




///////////////////////////////////////////////////////////////////////////////
//
void GP::CameraUpdate(float dTime /* = 0.0f */)
{
 	//ī�޶� ����.
	if(IsKeyUp(DIK_F6))		
	{
		for(int i=0; i<CAM_MAX_; i++)
			_pCurrCamSet[i]->Reset();
	}

  	// ī�޶� ��ȯ. 
	for(int i=DIK_1; i<(DIK_1 + CAM_MAX_); i++)
 		if(IsKeyUp(i))
		{
			_CurrCamNum = i - DIK_1 ;
			_pCurrCam = _pCurrCamSet[ _CurrCamNum ];

			//������ ī�޶�� '����' ��Ų��.
			if(_CurrCamNum == CAM_PILOT_)
				_pCurrCam->Reset();
		}

 
	_pCurrCam->Update(dTime);	//ī�޶� ����. 

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

	if(IsKeyUp(DIK_B) && bChange)		//�մϴټ� 2�� ȣ��� �ߺ� ó���Ǵ� ���� �����մϴ�.
		if(++num >= COLOR_MAX_) num = 0;
	
	return cols[num];

}



//////////////////////////////////////////////////////////////////////////////
//
void GP::ShowHelp()
{ 

	//--------------------------------------------------------------- 
	// �̻��� ����.
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
					"�̻���[%d]:vPos(%.2f, %.2f %.2f) Velo(%.2fm/s) Fuel(%.1f) vSmoke(%.2f %.2f %.2f)",					 
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
	// �Ϲ� ����.
	//--------------------------------------------------------------- 
	if(!_bShowHelp) return;
  
	
	//--------------------------------------------------------------- 
	// ���� ��ǥ ���.
	{ 	
		int posx = g_Mode.Width / 2;
		int posy = 0;
		COLOR col = D3DXCOLOR(1.0f, 0.75f, 1.0f, 1); 
		B3X_DrawText(NO_, posx, 0, col, "�� %s", B3X_GetWindowTitle());  
		char msg[512] = 
"ȭ������ý��� ( Fire Control System ) �� ���� ��ĵ�� ������� ��ġ�� HUD �� �����Ͽ� \
�����簡 ���� ��ǥ���� ���������� Lock-On �� �� �־�� �մϴ�.\n\n\
Lock-On �� ��ǥ���� ���̴��� ��ĵ ������ ��� ��� Lock-Off �Ǿ, �̹� Lock-0n ���¿��� \
�߻�� �̻����� ������ �����Ǵ� ������ Fire and Forget �� �����ؾ� �մϴ�.";

		B3X_DrawTextSV(posx, posy+=14, msg, col, g_dTime);
	}


	
	
	//--------------------------------------------------------------- 
	// ���� / ���� ���.
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
	//B3X_DrawText(NO_,  200, 1, COLOR(1, 1, 0, 1), "��%s", B3X_GetWindowTitle());	 


	B3X_DrawText(NO_, posx, posy+=14, col, "[ī�޶�]");	 
	B3X_DrawText(NO_, posx, posy+=14, col, "����: F6");	 
	B3X_DrawText(NO_, posx, posy+=14, _bUseViewport?col * 1.5f:col, "����ī�޶� : Backspace (%s)", _bUseViewport?"ON":"OFF");	 
	 
	B3X_DrawText(NO_, posx, posy+=14, cols[_CurrCamNum], "��ȯ: 1~7 (%s)", _pCurrCam->GetName());	 
	
	if(_CurrCamNum == CAM_FREE_)
	{
		B3X_DrawText(NO_, posx, posy+=14, cols[_CurrCamNum], "����: Mouse");		 
		B3X_DrawText(NO_, posx, posy+=14, cols[_CurrCamNum], "�̵�: A D W S R F");	 
	  
  	}

	if(_CurrCamNum == CAM_PILOT_)	
	{
		float fovy = _pCurrCam->GetFovY();
		B3X_DrawText(NO_, posx, posy+=14, cols[_CurrCamNum], 
					"Ȯ��: Mouse-wheel : Zoom=x%.1f Fovy=%.1f(%.1f��)", 
					D3DXToRadian(45)/fovy, fovy, D3DXToDegree(fovy));	 
		B3X_DrawText(NO_, posx, posy+=14, cols[_CurrCamNum], "����: Mouse");			  
		B3X_DrawText(NO_, posx, posy+=14, cols[_CurrCamNum], "����: Mouse-MButton");	 
	 
	}
 

	//--------------------------------------------------------------- 
	// ���ΰ�.
	posx = 1;
	posy+=14;
	col = COLOR(.3f, 1.0f, 0.0f, 1.0f);
	B3X_DrawText(NO_, posx, posy+=14, col, "[Hero]");			 
 	B3X_DrawText(NO_, posx, posy+=14, col, "����: F5");		  
	B3X_DrawText(NO_, posx, posy+=14, col, "����: ����Ű");	 
	B3X_DrawText(NO_, posx, posy+=14, col, "����: L-Shift/Ctrl"); 
 	B3X_DrawText(NO_, posx, posy+=14, col, "���ϰ�: C");		 
  	B3X_DrawText(NO_, posx, posy+=14, col, "����̵�: N");		 
  	B3X_DrawText(NO_, posx, posy+=14, col, "�̻���: Z");		 
 	B3X_DrawText(NO_, posx, posy+=14, col, "������: X");		 
	B3X_DrawText(NO_, posx, posy+=14, col, "�̻���-��ι߻�: Z+LCtrl");	 
	B3X_DrawText(NO_, posx, posy+=14, col, "�ö��:` Space");	 
 
		
	//--------------------------------------------------------------------- 
	//
	//--------------------------------------------------------------------- 
	posy+=14; 
	col = COLOR(1, 1, 0, 1);
	B3X_DrawText(NO_, posx, posy+=14, _bShowSkyBox?col:col*0.8f, "�ϴû���: F4 (%s)", _bShowSkyBox?"ON":"OFF");		   
	B3X_DrawText(NO_, posx, posy+=14, col, "�̻��ϸ���: F7");		   
	B3X_DrawText(NO_, posx, posy+=14, col, "���⸮��: F8");			  
	B3X_DrawText(NO_, posx, posy+=14, _bUseFrustumCulling?col:col*0.8f, "F.�ø�: F9 (%s)", _bUseFrustumCulling?"ON":"OFF");	//		  

	B3X_DrawText( FALSE, posx, posy+=14, _bShowStars?col:col*0.8f, "��: F10 (%s)", _bShowStars?"ON":"OFF" );  	

	//--------------------------------------------------------------------- 
	// ȭ�� ���� �޼���
	//--------------------------------------------------------------------- 
	{
		int posy = 300;
		int posx = g_B3Mode.Width - 8 * 12;
 		COLOR col = COLOR(1, 1, 0, 1);

		
		B3X_DrawText(NO_, posx, posy+=14, col, "[���ϰ�]");	 
 		B3X_DrawText(NO_, posx, posy+=14, col, "�߻��: %d", CRailGun::GetRailGunCnt());	 

		posy+=14;  
		col = COLOR(1, 0.7f, 1, 1);
		B3X_DrawText(NO_, posx, posy+=14, col, "[F.Cull]");	 
		B3X_DrawText(NO_, posx, posy+=14, _bUseFrustumCulling?col:col*0.8f, "����: %d", _VisibleEnemyCnt);	 

		posy+=14;  
 		col = COLOR(0, 1, 1, 1);
 		B3X_DrawText(NO_, posx, posy+=14, col, "[���̴�]");	  
		B3X_DrawText( FALSE, posx, posy+=14, _bShowAzimuth?col:col*0.8f, "����: K (%s)", _bShowAzimuth?"ON":"OFF" );  		//��
		B3X_DrawText( FALSE, posx, posy+=14, col, "��ĵ��: %d", _pRadar->GetScanCnt() );  		//��

  	}//end of temp-block



	
	//--------------------------------------------------------------------- 
	// ��ƼŬ.. 
	//--------------------------------------------------------------------- 
 	posy+=14;
	posx = 1; //g_B3Mode.Width - (8 * 10);
	col = COLOR(1.f, 1.f, 0.2f, 1.0f);  
	//B3X_DrawText(NO_, posx, posy+=14, col, "[��ƼŬ]");	 
 	B3X_DrawText(NO_, posx, posy+=14, _bBillBoardON?col:col*0.8f, "��ƼŬ.������: V (%s)", _bBillBoardON?"ON":"OFF");	 
	B3X_DrawText(NO_, posx, posy+=14, col, "����: B");	 
 	 
 	
	
	//--------------------------------------------------------------- 
	//  ȭ�� �ϴ� ����.
	//--------------------------------------------------------------- 
 	//--------------------------------------------------------------- 
	// ���ΰ�.
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

	//���� ��ġ / ���� ȸ�� ��.	
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
	// ī�޶� ����.	 
	//col = COLOR(.5f, 1.f, 0.2f, 1.0f);
	posy+=14;
	col = cols[_CurrCamNum]; 
	
	B3X_DrawText(NO_, posx, posy+=14, col, "[ī�޶�] %s", _pCurrCam->GetName());	 
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


















