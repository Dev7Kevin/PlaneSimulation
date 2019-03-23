// SMoke.cpp: implementation of the CSmoke class.
//
//
////////////////////////////////////////////////////////////////////////////// 

#include "SMoke.h"

//////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////////////

CSmoke:: CSmoke(
		 LPDEVICE pDev,				//디바이스 포인터.
		 CB3Mesh* pMesh,			//메쉬 포인터.

		 SPRDATA*	pSpr,			//메쉬 포인터.
		 CCamera**	ppCamera,		//카메라 이중포인터. 
		 int		ShowTime,		//유지 시간.
		 int		FadeTime,		//페이드 시간.
		 COLOR		Color,			//연기 색상.
		 
		 VECTOR3 vPos,				//'월드 위치'
		 VECTOR3 vScale /*= VECTOR3(1, 1, 1)*/,			//스케일.
		 VECTOR3 vRot /*= VECTOR3(0, 0, 0)*/,			//'회전'
  		 VECTOR3 vMoveVelo/* = VECTOR3(0, 0, 0)*/,		//이동속도.
		 VECTOR3 vRotVelo/* = VECTOR3(0, 0, 0)*/		//회전 속도.
		 )
:CUnit(vPos, vRot, vScale, vMoveVelo, vRotVelo, 
	   0.0f, NULL,	
	   pMesh, pDev)
, m_Spr(*pSpr), m_ppCamera(ppCamera), m_ShowTime(ShowTime), m_FadeTime(FadeTime)
, m_Color(Color)		
{
	m_NowTime = m_OldTime = timeGetTime();		//생성 시간 백업.
 	m_fAlpha = Color.a;

	m_bFadeOut = NO_;				

	//위치 잡기.
	B3DSprite_Scaling(&m_Spr, m_vScale.x, m_vScale.y); 
 	B3DSprite_Translation(&m_Spr, m_vPos.x, m_vPos.y, m_vPos.z);		 
}

/////////////////////////////////////////////////////////////////////////////
//
CSmoke::~CSmoke()
{
	//N/A
}




/////////////////////////////////////////////////////////////////////////////
//
int CSmoke::Move(float dTime/* =0 */)
{

	if(m_bFadeOut == NO_)
	{
		//일단 생성되면..그리지.않고...
		//'유지시간'을 체크한다.
		m_NowTime = timeGetTime();
		if(m_NowTime - m_OldTime > m_ShowTime) 
		{
			//m_OldTime = m_NowTime;				//다 보여 줬으면..삭제한다.
			//	return FALSE;						//제거의 의미로..FALSE 리턴.
			m_bFadeOut = YES_;				//다 보여 줬으면. Fade out 시킴.
		}

	}
	else
	{
		//
		// 농도를 떨어 트린다...
		//
 		float al = m_fAlpha;
		m_fAlpha -= (al*dTime + 0.01f*dTime);		//알파 단계는 100단계로...

		if(m_fAlpha < 0)				//알파값이 0 이하면... 
 			return FALSE;				//연기를 삭제하기 위해 false 리턴.
	}

	//
	// 크기를 키운다..
	//
	VECTOR3 vScale = m_vScale;
	m_vScale += vScale * (0.3f * dTime);			//연기는 30%씩 커진다.
	B3DSprite_Scaling(&m_Spr, m_vScale.x, m_vScale.y);	 
 
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//
int CSmoke::Draw(float dTime/*=0*/)   
{ 

	//컬링후 보이지 않으면..그리지 않는다. 2004.06.25.
	if(!IsVisible()) return TRUE;			


 	// 위치 조절.  
 	m_Spr.mRot = (*m_ppCamera)->GetBillMtx();					//빌보딩 행렬 얻기. 
 	B3DSprite_Translation(&m_Spr, m_vPos.x, m_vPos.y, m_vPos.z, ON3D_); //화염 이동..
 

	//렌더링.
 	//fade out 여부에 따라..'연기'의 출력스타일을 결정한다. : 2004.06.24+ 업데이트.
  	B3DSprite_DrawFX2(&m_Spr, m_Color, m_bFadeOut, m_fAlpha, NO_);	 //#1 + #2 결합...

	//기존 렌더링 코드...
	//B3DSprite_Draw3D(&m_Spr, YES_, YES_,  m_fAlpha, NO_);			//#1. '하얀 구름' +  fade out. 
	//B3DSprite_DrawFX(&m_Spr, m_Color);							//#2. '색깔있는 연기' + '빛남'
 
	return TRUE;
}


 
////////////////////////////////////////////////////////////////////////////
//
void CSmoke::Release()
{
	//N/A
}

/////////////////////////////////////////////////////////////////////////////
//


/////////////////////////////////////////////////////////////////////////////
//

/////////////////////////////////////////////////////////////////////////////
//


/********************* end of file "Smoke.cpp" ****************************/
