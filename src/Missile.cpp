// Missile.cpp: implementation of the CMissile class.
//
//
////////////////////////////////////////////////////////////////////////////// 

//#include "../../Engine/B3X.h"
#include "../../Engine/Ext/B3Yena_Ext.h"	//DLL 버전 엔진 필수헤더. 

//#include "Unit.h"
#include "Plane.h"
#include "ShockWave.h"			 

#include "Camera.h"  
#include "Missile.h"
#include "SMoke.h"				//연기 헤더. 



 
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//
CMissile::CMissile(
		LPDEVICE	pDev,
		CB3Mesh*	pMesh,			//미사일용 메쉬 포인터.

 		CCamera**	ppCam,			//카메라 이중 포인터. 2004.06.19. 수정.: 발사 화면 때문에.
  		UINT		Dmg,			//미사일데미지.
		
		float		fMaxFlyVelo,	//미사일의 최대 비행 속도.
		VECTOR3		vFlyDir,		//미사일의 비행 방향.	
 
		float		fMaxFuel,		//최대 연료. (연료 개념으로 전환. 2004.11.12.) 
		
		CPlane*		pPlane,			//부착될 비행기 포인터.
		BOOL		bIsFired,		//발사 되었나?

		B3SPRITE*	pSprFire,		//미사일 화염용 스트라이트 포인터.. 
		VECTOR3		vSmokePos,		//'연기', '불촉' 생성위치.(모델공간 상대값)
 		VECTOR3		vSmokeScale,	//'화염', '연기' 스케일.
		COLOR		Color,			//화염 컬러.
		B3SPRITE*	psprSmoke,		//연기 스프라이트..
		
		CArrayList* pList,			//스모크 리스트..

		float		fRadius,		//충돌구 반경.
		LPANIINFO	pBoomAniInfo,	//폭파 애니메이션 정보 구조체.04.06.23+ 
									//NULL 이면 무시된다.

		LPANIINFO	pShockWaveAniInfo,		//폭파 애니메이션 정보 구조체.04.06.23+ 
											// : NULL 이면 무시된다.

		VECTOR3 vPos,			//미사일발사 위치 : 즉 비행기의 
								//'하드 포인트 Hard Point' 위치이다.
								//(비행기 모델공간에서의 '상대 위치')

 		VECTOR3 vScale,			//미사일'크기'
		VECTOR3 vRot/* = VECTOR3(0.0f, 0.0f, 0.0f)*/, 
 		VECTOR3 vMoveVelo/* = VECTOR3(0.0f, 0.0f, 0.0f)*/,
 		VECTOR3 vRotVelo/* = VECTOR3(0.0f, 0.0f, 0.0f)*/
 		 )
:CBullet(pDev, pMesh, Dmg, fMaxFlyVelo, vFlyDir, 
		 //0,		//최대 사거리...-> 연로 개념으로 전환 2004.11.12.
		 10000,		//최대 사거리. 'CBullet::Move 에서의 거리 측정을 무시하기 위해 최대값을 넣는다.
					//실제 미사일의'사거리'에 적용되지는 않는다. 2004.11.12.
 		 //fMaxRange, 

		 pSprFire,	ppCam, Color,
		 fRadius, pBoomAniInfo,  
		 vPos, vRot, vScale, vMoveVelo, vRotVelo )
, m_pPlane(pPlane), m_bIsFired(bIsFired) 
, m_sprSmoke(*psprSmoke)	//화염 스프라이트.
, m_pSmokeList(pList)		//화염 리스트..
, m_fMaxFlyVelo(fMaxFlyVelo)	//최대 속도..
, m_fRocketFuel(fMaxFuel)		//로켓 연료..
{
 	//
	// 발사 전..
	//
	m_bIsReleased = FALSE;	//발사 버튼을 누루면, 하드포인트에서 분리된후,
							//'점화', '발사' 의 순으로 동작한다.
	m_bIsReleasedAst = FALSE;
	m_fRelaseLength = 0.0f;
	m_vReleasePos = VECTOR3(0, 0, 0);
	m_vReleaseTimePlaneDirVelo = VECTOR3(0, 0, 0);

	m_fCurrVelo = 0.0f;

	// 방향 벡터.
	m_vUp = m_vHorz = VECTOR3(0, 0, 0);		//=> 릴리즈 후에 '정보를 설정할것임'
		

	//
	// 발사 위치 계산.
	//
	m_vHardPt = vPos;		//m_vHardPt -발사위치 : 하드 포인트 저장. 
							//			발사되기 전까지는 이 값을 사용 할 것이다.
							//m_vPos - 이동하면서 누적되며 발사 이후 이것을 
							//			월드 좌표로 사용한다.
	SetHardPointPos();		//미사일의 위치는 비행기의 하드포인트 위치에서 
							//상대값으로 계산한다.   
							//발사된 이후에는 자신의 자표로 재조정..
	
	//
	// 스모크 위치 계산 (모델공간 상대위치) : 모델 좌표(상대값) * 비행기 스케일 
	// 나중에 출력시 '비행기'의 월드 위치와 결합하여 출력한다. : ::Move 참고.
	//
	m_vSmokePos = vSmokePos;
 	m_vSmokePosW = m_vSmokePos;
	m_vSmokeScale = vSmokeScale;
	
 	SetSmokePos(&vSmokePos);


	//
	// 폭파 관련. 
	//
	//'폭파' 화염 시작 사이즈는 ...미사일 부스터 '화염'의 x 2
	m_fBoomSphereRaidus = m_vSmokeScale.x*2;	
	//m_vBoomShowTime = 3000;		//'3초'.. -> CMissile::SetDead() 에서 결정.
	m_bFadeOut = NO_;	
	m_fAlpha = 1.0f;		

	//
	// 충격파 관련. : 일단, 미사일 쪽에서 관리함.
	//
	m_pShockWave = new CShockWave(
		pDev, 
		m_fBoomSphereRaidus*0.1f, // 총격파 크기 / 두께는 미사일 폭파에 따른다.
		m_fBoomSphereRaidus*0.1f/2,	
		m_Color,					//'미사일 폭파구' 색상과 동일하게..
		pShockWaveAniInfo,			
		VECTOR3( 0, 0, 0));
	ASSERT(m_pShockWave != NULL);


	// 유도 관련. 04.11.11.
	m_pTarget = NULL;
	m_bHommingON = NO_;  

	m_fMaxAccelSpeed = 0.0f;	//최대 순간 가속도.

}//end of CMissile::CMissile()




//////////////////////////////////////////////////////////////////////////////
//
CMissile::~CMissile()
{
	SAFE_DELETE(m_pShockWave);	

}







//////////////////////////////////////////////////////////////////////////////
//
// 내부 메소드.
//
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//
// void CMissile::SetSmokePos(..)
//
// 미사일의 후방에 출력할 스모크 - 불꽃 & 연기 등의 위치좌표등을 계산한다.
//
void CMissile::SetSmokePos(VECTOR3* pvSmokePos/*=NULL*/)
{
	VECTOR3 vSmokePos(0, 0, 0);
	
	//맨처음 일경우, '멤버' 초기화.
	if(pvSmokePos != NULL)
	{
		m_vSmokePos = *pvSmokePos;
	}

	//
	// '연기'&'불꽃' 로컬 위치 계산. 
	//
	// 우선 -z 값만 적용한다. 추후 업데이트 예정.ㅡㅡ;;
 	// -> 미사일의 '후방' 으로 '연기'의 초기 위치만큼 떨어진 상대거리 계산. 
 	VECTOR3 vPos = -(m_vSmokePos.z * m_vFireDir);	
 
	//
	// 월드 좌표 계산.: 미사일에서의 상대위치-미사일 후방-가 되겠다.
	//
	m_vSmokePosW = m_vPos + vPos;					 
	 
 	//
	// 화염 스프라이트 출력. 
	//
	//B3DSprite_RotateXXX() ==> 회전은 그릴때만 적용 할 것임  
	B3DSprite_Scaling(&m_Spr, m_vSmokeScale.x, m_vSmokeScale.y); 
 	B3DSprite_Translation(&m_Spr, m_vSmokePosW.x, m_vSmokePosW.y, m_vSmokePosW.z, ON3D_);		 
	 
}
 



//////////////////////////////////////////////////////////////////////////////
//
// void CMissile::SetFire()
//
// 마사일의 '발시'에 사용. 발사 방향벡터의 설정등, 
//  기본적인 미사일 데이터를 셋팅한다.
//
void CMissile::SetFire()
{ 
 	//------------------------------------------------------------
	//이전에 발사된 놈이면 리턴. 2004.05.21+
	if(m_bIsReleased == TRUE) return;		

	// 하드 포인트에서 분리!!		2004.06.21+
	m_bIsReleased = TRUE;	
 
	//------------------------------------------------------------

	//
	// 미사일 회전값 설정 : 발사후 미사일의 모델 회전에 쓰기 위함이다.
	//
	m_mRotation = m_pPlane->GetRotMtx(); 
	
	//
	// 발사 방향의 단위 벡터화. => 미사일의 비행 방향과 동일. : 2004.06.22
	//
	m_vFireDir = m_pPlane->GetNView();
	D3DXVec3Normalize(&m_vFireDir, &m_vFireDir);		

	//
	// 방향성분 얻기.	2004.06.21+
	// 발사 방향 - 시선벡터 는 이미 알고 있다.
	//
 	m_vUp = m_pPlane->GetUp();
	m_vHorz = m_pPlane->GetHorz();

	//
	// '릴리즈 타임' 에서의 비행기 방향+속도 얻기. 2004.06.21+
	//
	m_vReleaseTimePlaneDirVelo = m_pPlane->GetCurrDirVelo();	//<-비행기 이동방향+속도 

	//
	// 탄환의 발사방향 & 속도  설정...
	//
	// 미사일은 '부스터' 점화후 점차 속도가 증가하게 되며 '레일건'과는 
	// 정반대의 성격이다.
 	// 현재 미사일의 속도는  릴리즈時 비행기의 속도와 동일(아직 부스터가 작동
	// 하지 않고 있다.)
	//
	m_fCurrVelo = 0.0f; 
 	m_vMoveVelo = m_vReleaseTimePlaneDirVelo;	//발사 속도 = '투하' 시점의  비행기 속도*방향 
 
	//
	// 최대 사거리는 '상대값'으로 입력 받지만, 
	// 사용시에는 월드의 '절대 좌표'로  환산합니다.
	//
 	SetMaxRange(m_fMaxRange);

	//
	// 화염 위치 산출.
	//
	//SetSmokePos();
	 
}		





//////////////////////////////////////////////////////////////////////////////
//
// 미사일 비행시 일정시간마다 '연기'를 배출한다.
//
//
void CMissile::PutSmoke(float dTime)		
{
 	//
	// 연기 생성 -> 리스트에 등록.
	//
	CSmoke* pSmoke = new CSmoke(	
						m_pDev, 
						NULL, 
			   			&m_sprSmoke, 
						m_ppCamera, 
						1200, //3500, 유지시간 축소 
						0,
 						//m_Color,						 
 						COLOR(0.8f, 0.8f, 0.8f, .5f),	//색상, 흰색으로...	
						VECTOR3(m_vSmokePosW.x, m_vSmokePosW.y, m_vSmokePosW.z),
						//m_vSmokeScale*0.4f				 //화염 스케일의 60% 크기로 결정. 
						m_vSmokeScale*((rand()%40+25)*0.01f) //0.25f~0.64f 사이에서 크기를 결정... 
  						);
	if(pSmoke == NULL) return;		//메모리 확보 실패시..그냥 넘어감.
									//GPF 메세지 박스 보다는 좋지않나?
	m_pSmokeList->Add(pSmoke);		//연기 리스트에 등록.
	
}







//////////////////////////////////////////////////////////////////////////////
//
// void CMissile::SetHardPointPos()
//
// 미사일의 위치 - Hard Point 를 계산한다.
// 이 위치값은 '발사 전'까지만 사용하게 된다.
//
void CMissile::SetHardPointPos()
{
	// 사용 않함. 2009.07.03.

	//비행기 변환 행렬 얻기 : '스케링' 값은 비행기의 것을 사용하지 않고,
	//미사일 자신의 것을 사용한다. -> 그래야 다양한 크기의 미사일을  만들수 있다. 
	//	 
	//2004.11.15. '미사일 시점 카메라'를 위한 행렬 셋팅.
	m_mRotation = m_pPlane->GetRotMtx();				
	m_mTranslation = m_pPlane->GetTransMtx();			
	m_mTM = m_mScale * m_mRotation *  m_mTranslation;	//스케일링만 제외하고 나머지는 비행기에서 얻어옴.


	m_vUp = m_pPlane->GetUp();				//모델의 업벡터 얻기.
	m_vNView = m_pPlane->GetNView();		// 모델의 전방 벡터 얻기.

	//미사일 위치 계산.
	//ex) 오른쪽 날개 3.0F(vHorz) 하단 -1.0f 아래 (vUp), 전방 2.0f (vView) 앞으로.
	VECTOR3 vPos = m_vHardPt.x*m_pPlane->GetHorz()+ 
				   m_vHardPt.y*m_pPlane->GetUp()+
				   m_vHardPt.z*m_pPlane->GetNView();


	// 미사일 위치 보정.
	m_vPos.x = (m_mTM._41 += vPos.x);	
	m_vPos.y = (m_mTM._42 += vPos.y);  
	m_vPos.z = (m_mTM._43 += vPos.z);
 

	
}





//	****  미사일 달기 기본 코드. 상상해 보아요~  *****
//	{
//		MATRIX mTM 
//		mTM = pHero->GetTM();						//비행기 TM 얻기.
//		VECTOR3 vUp = pHero->GetUp();			//모델의 업벡터 얻기.
//		VECTOR3 vView = pHero->GetNView();		//모델의 전방 벡터 얻기.
//		VECTOR3 vPos = -1*vUp + 2*vView;			//하단 -1.0f 아래, 전방 2.0f 앞으로.
//		 
//		mTM._41 += vPos.x;	mTM._42 += vPos.y; mTM._43 += vPos.z;
//		 
//		g_pd3dDevice->SetTransform(D3DTS_WORLD, &mTM);
//		pMissile->Render(g_pd3dDevice);
//
//	}
//






//////////////////////////////////////////////////////////////////////////////
//
// 미사일 이동 메소드 
//
// 크게 3단계로 나누어 진다.
// 1단계 : 비행기의 '하드 포인트' 에 장착되어 비행기와 함께 움직이는 경우 : "MOVE" 
//			=> CMissile::MoveOnHardPoint()
// 2단계 : 발사버튼이 눌려 '하드 포인트'에서 '떨이지는 단계' : "RELEASE"
//			=> CMissile::MoveReleaseTime()
// 3단계 : 릴리즈 후, '부스터'의  점화에 의해 '비행' 하는 단계 : "FLY"
//          => CMissile::MoveFly()
//
int CMissile::Move(float dTime/* =0 */)
{
	/* 미사일 이동 코드 추가 */

 	//------------------------------------------------------------------------
 	// 발사 된후 '비행' 중...
	// => 이쪽 - '비행'- 이후의 동작이 더 많으므로 먼저 조건 검색한다.
	if(m_bIsFired == YES_)		
	{
		return MoveFly(dTime);
	}


	//------------------------------------------------------------------------
 	// 발사 버튼 조작후 '하드포인트'에서 분리되었을 경우...
 	// 
	if(m_bIsReleased == YES_)		//분리가 되었으면...
	{
		return MoveReleaseTime(dTime);
	}
	 

	//------------------------------------------------------------------------
	// 발사 되지 않고, 아직 비행기-Hard Point 에 장착되어 있으면...
	//
 	return MoveOnHardPoint(dTime);
 	 
}





//////////////////////////////////////////////////////////////////////////////
//
// int CMissile::MoveFly() 
//
int CMissile::MoveFly(float dTime)
{

	// 만약 타겟 유닛이 폭파 중이거나, 삭제되었다면...
	// 더이상 유도 하지 않고, 현재 방향으로 계속 날아간다.
	if(m_pTarget == NULL)
	{
		m_bHommingON = FALSE;
	}
	else
	{
		if(m_pTarget->IsDead()) 
			m_bHommingON = FALSE;
	}




	//------------------------------------------------------------------------ 
	// 미사일의 유도 처리 코드...
	// 미사일의 속도가 30m/s 이상일때, 유도를 시작한다.
	// 바로 유도를 하면..재미 없다. ㅡㅡ;;
	// '속도' 가 있어야 방향을 틀 수 있다! 라는 가정하에.
	//
	if( D3DXVec3Length(&m_vMoveVelo) > 30.0f) 
	{											 
		m_bHommingON = YES_;					 
	}

	MoveHomming(m_bHommingON, dTime);		//유도 기능 ON/OFF!


	//------------------------------------------------------------------------ 
	// 로켓 부스터 연료 검사...현재 속도만큼 소모, 연로가 없다면 자폭..
	// 
	m_fRocketFuel -= m_fMaxFlyVelo*0.1f*dTime; //최대속도의 10%/sec 씩 연료 초모.
	if(m_fRocketFuel <= 0.0f)					//자폭하기 위해 FALSE 리턴...
	{
#ifdef _LOGFILE_ON_
		B3D_Error(NO_, "[info] CMissile::MoveFly():\t\t미사일(0x%x) 최대 사거리 넘어섬. 자폭 셋팅.: dTime:%f vPos(%.3f %.3f %.3f)", 
			this, dTime, m_vPos.x, m_vPos.y, m_vPos.z);
#endif 

		//바로 삭제하지 않고..'폭파' 애니를 수행한다..
		SetDead(); 

		return TRUE; //FALSE; 	
	}
 
	//------------------------------------------------------------------------ 

	//미사일 화염 위치 조종.
	SetSmokePos();
 

 	//미사일 연기 배출.
	PutSmoke(dTime);

	return TRUE;

}

 
//////////////////////////////////////////////////////////////////////////////
//
// int CMissile::MoveReleaseTime() 
//
//
int CMissile::MoveReleaseTime(float dTime)
{
	//'아래' 로 '분리' 된다.
	VECTOR3 vDrop = -m_vUp * 2.0f * dTime;		//2.0m/s 의 속도록 낙하..	
	m_vReleasePos += vDrop;						//낙하된 거리 누계.

	VECTOR3 vView = m_pPlane->GetNView() * 2.0f * dTime;
	m_vRelAstPos += vView;


	//미사일 낙하 
	m_vPos += vDrop;
	m_vAstPos += vView;

	// 낙하중, 비행기의 가속도가 미사일의 '전방'가속도에 
	// 영향을 미친다.(발사시, 비행기의 '속도' 와 동일) 
	m_vPos += m_vReleaseTimePlaneDirVelo*dTime;		

	m_vAstPos += m_vReleaseTimePlaneDirVelo * dTime;


	//비행기 下方 1.5m 만큼 떨어진후..부스터를 점화 한다. => 자유비행!!
	if( D3DXVec3Length(&m_vReleasePos) > 1.5f)		
	{ 	
 		m_bIsFired = YES_;
 		SetSmokePos();		// 화염 위치 산출.
	}

	if( D3DXVec3Length(&m_vRelAstPos) > 1.5f)
	{
		m_bIsFired = YES_;
		SetSmokePos();
	}

	//변환 행렬 생성.
	//D3DXMatrixScaling(&m_mScale, m_vScale.x, m_vScale.y, m_vScale.z);
	D3DXMatrixTranslation(&m_mTranslation, m_vPos.x, m_vPos.y, m_vPos.z);
	D3DXMatrixTranslation(&m_mTranslation, m_vAstPos.x, m_vAstPos.y, m_vAstPos.z);

	MATRIX temp;
	D3DXMatrixMultiply(&temp, &m_mScale, &m_mRotation);
	D3DXMatrixMultiply(&m_mTM, &temp, &m_mTranslation);


	//미사일 화염 위치 조종.
	//SetSmokePos();

	return TRUE;
}



//////////////////////////////////////////////////////////////////////////////
//
// int CMissile::MoveOnHardPoint() 
//
// 발사 되지 않고, 아직 비행기-Hard Point 에 장착되어 있으면...
// 비행기의 '변환행렬에' 따라간다. 더하기  미사일의 사거리도 '조정' 해야 합니다..
//
int CMissile::MoveOnHardPoint(float dTime)
{
	//발사 방향의 단위 벡터화.: 미사일의 비행방향. 2004.06.22.
	m_vFireDir = m_pPlane->GetNView();
 	D3DXVec3Normalize(&m_vFireDir, &m_vFireDir);
	SetMaxRange(m_fMaxRange);

	
	// 하드 포인트 위치를 통한 월드좌표 계산.@fix. 2009.07.03.
	//SetHardPointPos();

	 
	
	//하드포인트 이동행렬.
	MATRIX mHardPtTrans;
	D3DXMatrixTranslation(&mHardPtTrans, m_vHardPt.x, m_vHardPt.y, m_vHardPt.z);


	// 회전과 이동은 '비행기'와 동일하게 처리한다.(모델 회전용)..
	m_mRotation = m_pPlane->GetRotMtx();
	m_mTranslation = m_pPlane->GetTransMtx();
	 
	//행렬 결합.
	m_mTM = m_mScale * mHardPtTrans * m_mRotation * m_mTranslation;		

	
	//위치 저장.
	m_vPos.x = m_mTM._41;	m_vPos.y = m_mTM._42;	m_vPos.z = m_mTM._43;
	 

	return TRUE;	

}











//////////////////////////////////////////////////////////////////////////////
//
//  int CMissile::Draw(float dTime)   
//
//
int CMissile::Draw(float dTime/*=0*/)   
{

	//----------------------------------------------//
	// 컬링은 상위 메소드에서 수행하므로 주석처리	//	
	//----------------------------------------------//
	//if(!IsVisible()) return TRUE;	//컬링후 보이지 않으면..그리지 않는다. 2004.06.25. 

 
	/* 미사일 렌더링 코드 추가 ... */
	if(m_bIsDead)						//미사일이 자폭하거나 충돌후 '터졌다면'....
		return DrawBoom(dTime);			//폭파 시킴....폭파는 컬링하지 않는다. 

	
	//미사일 화염 출력. 
	//+ 화염 컬링.: 미사일에서만 2번이상의 컬링검사가 발생하게 된다..줄여야 합니다.!! 2004.06.25.
	// CUnit::Draw(float dTime)  를 만드는 것이 하나의 대안이 될수 있다.
	if(m_bIsFired && IsVisible())	
	{
 		//미사일 화염(Flare) 출력. 
		m_Spr.mRot = (*m_ppCamera)->GetBillMtx();				//빌보딩 행렬 얻기. 
  		B3DSprite_Translation(&m_Spr, m_vSmokePosW.x, m_vSmokePosW.y, m_vSmokePosW.z, ON3D_); //화염 이동..
 		B3DSprite_DrawFX(&m_Spr,  m_Color); 
	}

	//미사일 그리기.
	return CBullet::Draw(dTime);     
}


/* 화염 반짝반짝,...코드..별로 이쁘지 않음.
int CMissile::Draw(float dTime)   
{
	// 미사일 렌더링 코드 추가 
	CBullet::Draw(dTime);     
	
	if(m_bIsFired)
	{
 		//미사일 화염 출력. 
		m_Spr.mRot = (*m_ppCamera)->GetBillMtx();				//빌보딩 행렬 얻기. 
//		B3DSprite_Scaling(&m_Spr, m_vSmokeScale.x, m_vSmokeScale.y);
 		B3DSprite_Translation(&m_Spr, m_vSmokePosW.x, m_vSmokePosW.y, m_vSmokePosW.z, ON3D_); //화염 이동..
 		B3DSprite_DrawFX(&m_Spr,  m_Color); 

//		static BOOL bShow = YES_;
//		if(bShow ^= 1)
//		{ 
//			B3DSprite_Scaling(&m_Spr, m_vSmokeScale.x*0.5f, m_vSmokeScale.y*0.5f);
//			B3DSprite_Translation(&m_Spr, m_vSmokePosW.x, m_vSmokePosW.y, m_vSmokePosW.z, ON3D_); //화염 이동..
// 			B3DSprite_DrawFX(&m_Spr,  COLOR(1.0f, 1.0f, 0.3f, 1.0f)); 
//		}

	}

	//미사일 그리기.
	return true;
}

*/


//////////////////////////////////////////////////////////////////////////////
//
void CMissile::Release()
{
	//N/A
}






//////////////////////////////////////////////////////////////////////////////
//
// BOOL CMissile::DrawBoom() 
// 
// 폭파 애니메이션 렌더링. 
// 이것이 끝나면 메모리를 삭제하기 위해 FALSE 가 리턴된다.
//
// CUnit::DrawBoom 오버라이딩. 
// 
// : 미사일에서는 '폭파' 프레임수에 의해서가 아니라, 
// 폭파 화염의 사이즈에 의해 애니메이션이 수행된다.
//
BOOL CMissile::DrawBoom(float dTime)	
{
	//
	// 폭파 구 크기 늘리기.
	//
	if( m_bFadeOut == NO_)										//'터지는 중'...
	{
		if( m_fBoomSphereRaidus >= m_BoomAniInfo.vScale.x/2)	//최대 사이즈보다 크면...'x'축만 확인해도 무방.
		{
			m_bFadeOut = YES_;
		}

		m_fBoomSphereRaidus += 100.0f * dTime;				 
		m_vScale = VECTOR3(1,1,1)*m_fBoomSphereRaidus*2;
	}
	else							//'사라지는 중'...
	{
		//
		// 알파값 낮춤.-> 2004.06.30++. '쇼크 웨이브'에 의존 코드로 전환.!!
		//
		//  m_fAlpha -= 0.3f*dTime;
		//
		//	if(m_fAlpha<0.0f)	//알파가 0 이하->'완전히' 사라지면..객체 삭제를 위해 FALSE 리턴.
		//	{
		//		return FALSE;
		//	}
	}



	//----------------------------------------------//
	// 컬링 여기서..								//	
	//----------------------------------------------//
	// 시각적인 효과를 위해 컬링 넘어감... 2004.06.25+
	//if(!IsVisible()) return TRUE;	 <-- 주석처리 합니다.(스프레드 데미지 효과를 위해..)


	//----------------------------------------------//
	// 쇼크 웨이브 								//
	//----------------------------------------------//

	//1. '미사일 폭파구'를 중심으로 한 폭파 애니 처리 코드.
	//   '충격파'는 '폭파구'의 상태에 따라 변하게 된다. 
	//	m_pShockWave->SetFadeOutState(m_bFadeOut);
	//	m_pShockWave->SetAlpha(m_fAlpha);
	//	m_pShockWave->Draw(dTime);

	//2. '충격파'를 중심으로 폭파 애니를 처리할 경우의 코드. 
	//    : 충격파가 사라지면, 객체를 제거하기 위해 false 리턴..
	if(m_pShockWave->Draw(dTime) == FALSE) 
		return FALSE;					//랜더링 끝...

	//쇼크 웨이브의 '알파값'을 폭파구에 적용한다.
	m_fAlpha = m_pShockWave->GetAlpha();						
	m_bFadeOut = m_pShockWave->GetFadeOutState();


	//----------------------------------------------//
	// 폭파 구										//
	//----------------------------------------------//

	// 회전값은 카메라의 '회전행렬'을 얻는다 : 기본값은 '일반' 빌보딩..
	m_BoomAniInfo.pSpr[m_BoomAniInfo.Frm].mRot = (*m_BoomAniInfo.ppCamera)->GetBillMtx();

	//기본 폭파 화염...
	B3DSprite_Scaling(&m_BoomAniInfo.pSpr[m_BoomAniInfo.Frm], m_vScale.x, m_vScale.y);
	B3DSprite_Translation(&m_BoomAniInfo.pSpr[m_BoomAniInfo.Frm], m_vPos.x, m_vPos.y, m_vPos.z, ON3D_);

	//B3DSprite_DrwFX2 !! 
	B3DSprite_DrawFX2(&m_BoomAniInfo.pSpr[m_BoomAniInfo.Frm], m_Color, m_bFadeOut, m_fAlpha, NO_);  

	//추가 폭파 화염.
	B3DSprite_Scaling(&m_BoomAniInfo.pSpr[m_BoomAniInfo.Frm], m_vScale.x*0.8f, m_vScale.y*0.8f);
	//B3DSprite_DrwFX2 !! 
	B3DSprite_DrawFX2(&m_BoomAniInfo.pSpr[m_BoomAniInfo.Frm], COLOR(1.0f, 1.0f, 1.0f, 1.0f), m_bFadeOut, m_fAlpha, NO_);


	return TRUE;
}



//////////////////////////////////////////////////////////////////////////////
//
// void CMissile::SetDead() 
//
// CUnit::SetDead 오버라이드.
//
void CMissile::SetDead()
{
	CUnit::SetDead();			//부모것 실행...
 
	//미사일 추가 데이터 실행.
	m_BoomShowTime = timeGetTime() + 1000;	//애니메이션 수행 시간...


	//충격파 애니메이션 셋팅. 2005.06.29++
	m_pShockWave->SetDead();
	m_pShockWave->SetPos(m_vPos);			//<- CUnit::SetPos()

}


//////////////////////////////////////////////////////////////////////////////
//
// BOOL CMissile::CheckCollision(...) 
//
// 충돌 함수 오버라이딩. 
// 미사일의 '폭파 구'는 일정 시간을 지날수록 커지게 되며 이에따라 충돌영역도
// '커져야' 한다.
//
BOOL CMissile::CheckCollision(CUnit* pTarget, float fTargetRadius/*=0.0f*/)
{
	if(m_bIsDead)		//현재 폭파 중이면...
	{
		//"현재 폭파 충돌구의 이미지 크기" 만큼 의 미사일의 충돌 반경을 늘린다....
		m_fCollRadius =  
			m_BoomAniInfo.pSpr[m_BoomAniInfo.Frm].mScale._11/2.5f ;  //x 축 길이의 40% 만큼.. 
	}

	// 그리고 일반 충돌검사 수행...
	return CUnit::CheckCollision(pTarget, fTargetRadius);

}






//////////////////////////////////////////////////////////////////////////////
// 
// void CMissile::LockOn(CUnit* pUnit) 
//
// 목표  Lock-On 시키기
// 2004.11.11.
//
void CMissile::LockOn(CUnit* pEmy)	
{
	m_pTarget = dynamic_cast<CPlane*>(pEmy);

#ifdef _LOGFILE_ON_
	B3D_Error(NO_, "[info] CMissile::LockOn : 미사일(0x%08x)  목표(0x%08x) Lock-On!!",
		this, pEmy);
#endif
}







//////////////////////////////////////////////////////////////////////////////
// 
// void CMissile::Homming()
//
// 미사일 유도하기 : 목표에 대한 회전 행렬 계산. p
//                   미사일의 '모델'의 방향을 바꾸는 것이 주 임무다.
//

int CMissile::CalHommingRotMtxAst(float dTime)
{
	//1. 현재 위치에서 '목표' 의 방향 벡터를 구한다.
	VECTOR3 vNewView =m_vPos - m_pTarget->GetPos();
	//VECTOR3 vNewView = m_pTarget->GetPos() - m_vPos;
	D3DXVec3Normalize(&vNewView, &vNewView);

	//2. 새 Up, Horz 벡터 구하기.
	VECTOR3 vNewUp = m_vUp;

	VECTOR3 vNewHorz;
	D3DXVec3Cross(&vNewHorz, &vNewUp, &vNewView);	//수평 벡터 계산.
	D3DXVec3Normalize(&vNewHorz, &vNewHorz);

	D3DXVec3Cross(&vNewUp, &vNewView, &vNewHorz);	//상방 벡터 재 계산.
	D3DXVec3Normalize(&vNewUp, &vNewUp);			//3축의 정규 직교화!!

	//3.새로이 계산된 각 축별 방향 성분을 행렬로 구성한다.
	MATRIX mNewTM;
	D3DXMatrixIdentity(&mNewTM);

	mNewTM._11 = vNewHorz.x;	mNewTM._12 = vNewHorz.y;	mNewTM._13 = vNewHorz.z;
	mNewTM._21 = vNewUp.x;		mNewTM._22 = vNewUp.y;		mNewTM._23 = vNewUp.z;
	mNewTM._31 = vNewView.x;	mNewTM._32 = vNewView.y;	mNewTM._33 = vNewView.z;

//	mNewTM._11 = vNewHorz.x;	mNewTM._12 = vNewUp.x;		mNewTM._13 = vNewView.x;	
//	mNewTM._21 = vNewHorz.y;	mNewTM._22 = vNewUp.y;		mNewTM._23 = vNewView.y;	
//	mNewTM._31 = vNewHorz.z;	mNewTM._32 = vNewUp.z;		mNewTM._33 = vNewView.z;	
	
	


	//스케일 값 보정.
	//mNewTM  = m_mScaling * mNewTM;		

	//이동값 보정.
	//mNewTM._41 = m_vPos.x;	mNewTM._42 = m_vPos.y;	mNewTM._43 = m_vPos.z;


	//------------------------------------------------------------------//
	//4.미사일의 '새 변환 행렬'에서 '미사일'의 방향 회전 행렬을 구한다.	//
	//------------------------------------------------------------------//
	// i)   NewTM = TM * R
	// ii)  TM(-1) * NewTM = R
	// iii) TM(T) * NewTM = R
	//	MATRIX TransTM;  
	//	D3DXMatrixTranspose(&TransTM, &m_mTM);
	//
	//메쉬 돌리기..용 행렬이 되겠음.
	m_mRotation = mNewTM ;		


	//5. 발사 방향도 설정..
	m_vFireDir =  m_pTarget->GetPos() - m_vPos;
	D3DXVec3Normalize(&m_vFireDir, &m_vFireDir);


	return TRUE;
}

int CMissile::CalHommingRotMtx(float dTime/*=0*/)
{
	//0. 설정된 목표가 없다면, 가장 가까운 비행 유닛을 적으로 설정, 추적한다.
	//  ..라고 해야 하지만...그냥 넘어감...
	if(m_pTarget == NULL) return FALSE;

	//1. 현재 위치에서 '목표' 의 방향 벡터를 구한다.
	VECTOR3 vNewView =m_vPos - m_pTarget->GetPos();
	//VECTOR3 vNewView = m_pTarget->GetPos() - m_vPos;
	D3DXVec3Normalize(&vNewView, &vNewView);

	//2. 새 Up, Horz 벡터 구하기.
	VECTOR3 vNewUp = m_vUp;

	VECTOR3 vNewHorz;
	D3DXVec3Cross(&vNewHorz, &vNewUp, &vNewView);	//수평 벡터 계산.
	D3DXVec3Normalize(&vNewHorz, &vNewHorz);

	D3DXVec3Cross(&vNewUp, &vNewView, &vNewHorz);	//상방 벡터 재 계산.
	D3DXVec3Normalize(&vNewUp, &vNewUp);			//3축의 정규 직교화!!

	//3.새로이 계산된 각 축별 방향 성분을 행렬로 구성한다.
	MATRIX mNewTM;
	D3DXMatrixIdentity(&mNewTM);

	mNewTM._11 = vNewHorz.x;	mNewTM._12 = vNewHorz.y;	mNewTM._13 = vNewHorz.z;
	mNewTM._21 = vNewUp.x;		mNewTM._22 = vNewUp.y;		mNewTM._23 = vNewUp.z;
	mNewTM._31 = vNewView.x;	mNewTM._32 = vNewView.y;	mNewTM._33 = vNewView.z;

//	mNewTM._11 = vNewHorz.x;	mNewTM._12 = vNewUp.x;		mNewTM._13 = vNewView.x;	
//	mNewTM._21 = vNewHorz.y;	mNewTM._22 = vNewUp.y;		mNewTM._23 = vNewView.y;	
//	mNewTM._31 = vNewHorz.z;	mNewTM._32 = vNewUp.z;		mNewTM._33 = vNewView.z;	
	
	


	//스케일 값 보정.
	//mNewTM  = m_mScaling * mNewTM;		

	//이동값 보정.
	//mNewTM._41 = m_vPos.x;	mNewTM._42 = m_vPos.y;	mNewTM._43 = m_vPos.z;


	//------------------------------------------------------------------//
	//4.미사일의 '새 변환 행렬'에서 '미사일'의 방향 회전 행렬을 구한다.	//
	//------------------------------------------------------------------//
	// i)   NewTM = TM * R
	// ii)  TM(-1) * NewTM = R
	// iii) TM(T) * NewTM = R
	//	MATRIX TransTM;  
	//	D3DXMatrixTranspose(&TransTM, &m_mTM);
	//
	//메쉬 돌리기..용 행렬이 되겠음.
	m_mRotation = mNewTM ;		


	//5. 발사 방향도 설정..
	m_vFireDir =  m_pTarget->GetPos() - m_vPos;
	D3DXVec3Normalize(&m_vFireDir, &m_vFireDir);


	return TRUE;
}


/*
//0. 설정된 목표가 없다면, 가장 가까운 비행 유닛을 적으로 설정, 추적한다.
//  ..라고 해야 하지만...그냥 넘어감...
if(m_pTarget == NULL) return FALSE;

//1. 현재 위치에서 '목표' 의 방향 벡터를 구한다.
VECTOR3 vNewView = m_vPos - m_pTarget->GetPos();
D3DXVec3Normalize(&vNewView, &vNewView);

//2. 새 Up, Horz 벡터 구하기.
VECTOR3 vNewUp = m_vUp;

VECTOR3 vNewHorz;
D3DXVec3Cross(&vNewHorz, &vNewUp, &vNewView);	//수평 벡터 계산.
D3DXVec3Normalize(&vNewHorz, &vNewHorz);

D3DXVec3Cross(&vNewUp, &vNewView, &vNewHorz);	//상방 벡터 재 계산.
D3DXVec3Normalize(&vNewUp, &vNewUp);			//3축의 정규 직교화!!

//3.새로이 계산된 각 축별 방향 성분을 행렬로 구성한다.
MATRIX mNewTM;
D3DXMatrixIdentity(&mNewTM);

mNewTM._11 = vNewHorz.x;	mNewTM._12 = vNewHorz.y;	mNewTM._13 = vNewHorz.z;
mNewTM._21 = vNewUp.x;		mNewTM._22 = vNewUp.y;		mNewTM._23 = vNewUp.z;
mNewTM._31 = vNewView.x;	mNewTM._32 = vNewView.y;	mNewTM._33 = vNewView.z;



//스케일 값 보정.
//mNewTM  = m_mScaling * mNewTM;		

//이동값 보정.
//mNewTM._41 = m_vPos.x;	mNewTM._42 = m_vPos.y;	mNewTM._43 = m_vPos.z;


//------------------------------------------------------------------//
//4.미사일의 '새 변환 행렬'에서 '미사일'의 방향 회전 행렬을 구한다.	//
//------------------------------------------------------------------//
// i)   NewTM = TM * R
// ii)  TM(-1) * NewTM = R
// iii) TM(T) * NewTM = R
//	MATRIX TransTM;  
//	D3DXMatrixTranspose(&TransTM, &m_mTM);
//
m_mRotation = mNewTM ;		


//5. 발사 방향도 설정..
m_vFireDir =  m_pTarget->GetPos() - m_vPos;
D3DXVec3Normalize(&m_vFireDir, &m_vFireDir);


*/


//////////////////////////////////////////////////////////////////////////////
//
// 목표에 미사일 유도 + 이동속도 결정 하기.
//
int CMissile::MoveHomming(BOOL bHomming, float dTime/*=0*/)
{
	//--------------------------//
	// '부스터 가속도 결정'		//
	//--------------------------//
	// 부스터 점화시 점차 속도를 올리고..최대 속도를 유지한다.
	// 속도 증가 : 초당 최고 속도(m_fMaxFlyVelo)의 3%의 제곱만큼...
	m_fCurrVelo += (m_fMaxFlyVelo*0.03f)*(m_fMaxFlyVelo*0.03f)*dTime;		

	//가속도 보정. 최고 속도이상 올라가는 것을 막는다.
	if(m_fCurrVelo >= m_fMaxFlyVelo)		
	{
		m_fCurrVelo = m_fMaxFlyVelo;		//현재 속도 고정.
		// ...m_vMoveVelo 는 더이상 '올라가지' 않는다...
	}



	//--------------------------//
	// 미사일 비행 속도  결정.  //
	//--------------------------//
	// 속도 계산...
	// 발사 방향의 역방향(역벡터) * 저항값 만큼 속도는 감소한다.
	// ....생략 ...

	//FireDir 이 바뀌었다면..이것도 영향을 미쳐야 한다 (공기 저항, 감속 등의)
	//.... 생략 .... 

	//미사일의 '속도' 결정(누계)
	m_vMoveVelo = m_fCurrVelo*m_vFireDir + m_vReleaseTimePlaneDirVelo;			


	// 
	// 스케일링 : 모델 좌표 계
	//
	D3DXMatrixScaling(&m_mScale, m_vScale.x, m_vScale.y, m_vScale.z);

	// 
	// 회전 처리 : 모델 좌표 계
	//
	// 목표쪽으로 방향 틀기 -> 회전 행렬 구성.....2004.11.12.
	if(bHomming) CalHommingRotMtx(dTime);


	// 
	// 중력 저항
	//...

	// 
	// 미사일 위치 계산. : 위치 = 순간 속도  * 시간.
	//
	m_vPos += m_vMoveVelo*dTime;		


	// 
	// 이동행렬 구성.
	//
	D3DXMatrixTranslation(&m_mTranslation, m_vPos.x, m_vPos.y, m_vPos.z);

	//
	// TM 만들기.
	//
	MATRIX temp;
	D3DXMatrixMultiply(&temp, &m_mScale, &m_mRotation);
	D3DXMatrixMultiply(&m_mTM, &temp, &m_mTranslation);


	return TRUE;

}






 
/******************  end of file "Missile.cpp" ****************************/