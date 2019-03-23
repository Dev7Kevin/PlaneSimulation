// RailGun.cpp: implementation of the CRailGun class.
//
//
////////////////////////////////////////////////////////////////////////////// 

//#include "../../Engine/B3X.h"
#include "../../Engine/Ext/B3Yena_Ext.h"	//DLL 버전 엔진 필수헤더. 

#include "RailGun.h"




//////////////////////////////////////////////////////////////////////////////
//Test.only. 2009.06.15
namespace GP
{
	extern BOOL _bBillBoardON; 
}
using namespace GP;


//////////////////////////////////////////////////////////////////////////////
// static data. 
int CRailGun::m_RailGunCnt = 0;

#ifdef RAILGUN_TEST_
int CRailGun::m_RailGunSNCnt = 0;
#endif


//////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//

CRailGun::CRailGun(
		LPDEVICE pDev,
		CB3Mesh* pMesh,			//총알용 메쉬 포인터.
		SPRDATA* pSpr,			//총알용 스트라이트 포인터.. 둘중에 하나는 있어야 한다.

		CCamera** ppCam,		//카메라 이중 포인터. 2004.06.19. 수정.

  		UINT	 Dmg,			//총알 데미지.
		COLOR	 Color,			//총알 컬러.
		float	fFireVelo,		//총구 발사 가속도
		VECTOR3 vFireDir,		//발사 방향.
		float	fMaxRange,		//최대 사거리...

		float	 fRadius,		//충돌구 반경.
		
		VECTOR3 vPos,			//총알 발사 위치.		
		VECTOR3 vScale,			//총알 '크기'
		VECTOR3 vRot/* = VECTOR3(0.0f, 0.0f, 0.0f)*/, 
 		VECTOR3 vMoveVelo/* = VECTOR3(0.0f, 0.0f, 0.0f)*/,
 		VECTOR3 vRotVelo/* = VECTOR3(0.0f, 0.0f, 0.0f)*/
 		 )
:CBullet(pDev, pMesh, Dmg, 
		 fFireVelo, vFireDir, fMaxRange,	  
		 pSpr, ppCam, Color,	
		 fRadius, NULL,	 
 		 vPos, vRot, vScale, vMoveVelo, vRotVelo 
		 )	
//, m_pSpr(pSpr), m_ppCamera(ppCam), m_Color(Color) -> CBullet 으로 옮김 : 
{
		 
	//B3DSprite_RotateXXX() ==> 회전은 필요 없다.
	B3DSprite_Scaling(&m_Spr, m_vScale.x, m_vScale.y);						
 	B3DSprite_Translation(&m_Spr, m_vPos.x, m_vPos.y, m_vPos.z);  

	//레일건 카운트 증가. 
	++m_RailGunCnt;


//-----------------------------------------------------------------------------
#ifdef RAILGUN_TEST_ 

	//레일건 번호 부여.
	m_RailGunSN = ++m_RailGunSNCnt;
 
	B3D_Error(NO_, "[info] 레일건 생성[tot:%d SN:%d now:%d] : \
				   vPos(%.3f %.3f %.3f) vFireDir(%.3f %.3f %.3f)\n\
				   최대 사거리 : fMax: %.3f vMaxRange(%.3f %.3f %.3f)",
			 m_RailGunSNCnt, m_RailGunSN, m_RailGunCnt,
 			 m_vPos.x, m_vPos.y, m_vPos.z, m_vFireDir.x, m_vFireDir.y, m_vFireDir.z,
			 fMaxRange, m_vMaxRange.x, m_vMaxRange.y, m_vMaxRange.z);
#endif
//-----------------------------------------------------------------------------

}//end of CRailGun::CRailGun()


//////////////////////////////////////////////////////////////////////////////
//
CRailGun::~CRailGun()
{

//-----------------------------------------------------------------------------
#ifdef RAILGUN_TEST_
	B3D_Error(NO_, "[info] 레일건 제거... [tot:%d SN:%d now:%d] : \
				   vPos(%.3f %.3f %.3f) vFireDir(%.3f %.3f %.3f)\n\
				   최대 사거리 : vMaxRange(%.3f %.3f %.3f)",
			m_RailGunSNCnt, m_RailGunSN, m_RailGunCnt,
 			m_vPos.x, m_vPos.y, m_vPos.z, m_vFireDir.x, m_vFireDir.y, m_vFireDir.z,
			m_vMaxRange.x, m_vMaxRange.y, m_vMaxRange.z);
#endif
//-----------------------------------------------------------------------------

	Release();
	
}




//////////////////////////////////////////////////////////////////////////////
//
int CRailGun::Move(float dTime/* =0 */)
{
	/* 레일건 이동 코드 추가 */
	 
	//** 스케일링 / 회전  처리 불필요.
	
	// '이동' 은 부모 메소드에서 처리.
	return CBullet::Move(dTime);
}



 

//////////////////////////////////////////////////////////////////////////////
//
int CRailGun::Draw(float dTime/*=0*/)   
{
	/* 레일건 렌더링 코드 추가 */

	//컬링후 보이지 않으면..그리지 않는다. 2004.06.25.
	if(!IsVisible()) return TRUE;	

 
	if(m_pMesh == NULL)				//메쉬가 없으면 스프라이트를 출력한다.
	{
		//카메라 역행렬 얻기 
		//이중포인터로써 원본인 '카메라'를 직접 접근한다. 2004.06.19 
		//빌보딩 업데이트...
		if(_bBillBoardON)  m_Spr.mRot = (*m_ppCamera)->GetBillMtx();			
		else D3DXMatrixIdentity(&m_Spr.mRot);										

			
		B3DSprite_Translation(&m_Spr, m_vPos.x, m_vPos.y, m_vPos.z, ON3D_); 		
 		B3DSprite_DrawFX(&m_Spr,  m_Color);												
 
		return TRUE;
	}
	
	// 메쉬가 있으면 그것도 그린다. : 재 수정 2004.10.20. 
	return CBullet::Draw(dTime);     
	 
}




//////////////////////////////////////////////////////////////////////////////
//
void CRailGun::Release()
{	 
	//레일건 카운트 감소. 
	--m_RailGunCnt;
}

 

/********************  end of file "RailGun.cpp" ****************************/
