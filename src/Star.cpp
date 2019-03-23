// Star.cpp: implementation of the CStar class.
// 
//
////////////////////////////////////////////////////////////////////////////// 

#include "Star.h"
#include "Plane.h"
#include "Camera.h"



/////////////////////////////////////////////////////////////////////////////
// flot -> DWORD 변환 인라인 함수.
//
inline DWORD FtoDW( FLOAT f ) { return *((DWORD*)&f); }




/////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
/////////////////////////////////////////////////////////////////////////////

CStar::CStar(
	 LPDEVICE	pDev,			//디바이스 포인터.
	 int		Cnt,			//별 개수.
 	 SPRDATA*	pSpr,			//'별' 스프라이트 포인터.
 	 CPlane*	pHero,			//주인공 포인터.
	 CCamera**	ppCamera,		//'현재' 활성화된 카메라의 2중 포인터.
	 int		Width			//'디스플레이' 모드 : '별가루'의 크기에 관여한다.	 
	 )
:CUnit(VECTOR3(0, 0, 0), VECTOR3(0, 0, 0), VECTOR3(0, 0, 0), 
	   VECTOR3(0, 0, 0), VECTOR3(0, 0, 0), 0.0f, NULL, NULL, pDev)	 
,m_Cnt(Cnt), m_Spr(*pSpr), m_pHero(pHero), m_ppCamera(ppCamera)
{
	m_pVBuffer = NULL;			

	//별가루 사이즈 설정.
	m_fSize = 2.5f;
	switch(Width)
	{
	case 1280:	m_fSize += 1.0f;	 
	case 1152:	m_fSize += 1.0f;
	case 1024:	m_fSize += 1.0f;	 
	case 800:	m_fSize += 1.0f;	 
	case 640:	m_fSize += 1.0f;	 
	}


	Init();				

}

/////////////////////////////////////////////////////////////////////////////
//
CStar::~CStar()
{
	Release();
}




/////////////////////////////////////////////////////////////////////////////
//
// 메소드들.
// 
/////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////
//
BOOL CStar::Init()
{
	::srand(::timeGetTime());

	//------------------------------------------------------------------------- 
	//'별' 위치 설정. - 정점 버퍼에 들어갈 데이터 구성...
	//
	VERTEX_STAR* pStar;

	//별가루 : '현재 주인공의 위치를 기준으로 일정 범위 안에 별을 생성한다.
	for(int i=0; i<m_Cnt; i++)		 
	{
		pStar = new VERTEX_STAR;
		ASSERT(pStar != NULL);
		
		pStar->dwColor = COLOR(1,1,1,1); 
 		pStar->vPos =  m_pHero->GetPos() +  
					   VECTOR3((rand()%30)-15.0f, 
					           (rand()%30)-15.0f, 
							   (rand()%30)-15.0f );
		
		m_StarDustList.push_back(pStar);	//리스트에 등록.
	}



	//------------------------------------------------------------------------- 
	// 정점 버퍼 생성  
	//
	HRESULT res = S_OK;
	if( FAILED( res = m_pDev->CreateVertexBuffer(
		m_Cnt * sizeof(VERTEX_STAR), 
 		//D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY | D3DUSAGE_POINTS,	//-> 이 옵션은 당장 필요 없음
		D3DUSAGE_WRITEONLY | D3DUSAGE_POINTS,						//-> 이 옵션 아니면, (최적화)
		//D3DUSAGE_POINTS,											//-> 이 옵션 정도면 충분함.
		FVF_STARS_, 
		D3DPOOL_MANAGED,  
		&m_pVBuffer, 
		NULL)))
	{
		B3D_Error(YES_, "[Error] CStar::Init - 정점 버퍼 생성 실패...:%s", 
						DXGetErrorString9(res));
		return FALSE;	
	}


	//------------------------------------------------------------------------- 
	// 버퍼에 기록...
	ResetVBuffer();
 

	return TRUE;
}





/////////////////////////////////////////////////////////////////////////////
//
// 별가루를 정점 버퍼에 기록한다. 
//
BOOL CStar::ResetVBuffer()
{	
	HRESULT res = S_OK;
	//
	// 정점 버퍼 데이터 셋팅.
	//
	VERTEX_STAR* pVertices;
	if(FAILED(res = m_pVBuffer->Lock(0, 
									 0,		//버퍼 전부 잠그기..
									 (void**)&pVertices,
									 NULL
	)))
	{
		B3D_Error(NO_, "[Error] CStar::Init - 정점 버퍼 잠금 실패..:%s", 
			DXGetErrorString9(res));
		return FALSE;
	}

	//VB 에 데이터 Write.
	for(itor ii=m_StarDustList.begin(); ii != m_StarDustList.end(); ++ii)
	{
		memcpy(pVertices++, *ii, sizeof(VERTEX_STAR));
 	}


	//VB 닫기.
	m_pVBuffer->Unlock();

	return TRUE;
}




/////////////////////////////////////////////////////////////////////////////
//
int CStar::Move(float dTime/*=0*/)
{	 
 	StarDustUpdate(m_pHero->GetPos());		//별가루 생성...

	return TRUE;
}




/////////////////////////////////////////////////////////////////////////////
//
// '별가루' 이동... : '별가루 생성구'를 벗어나면..'새로운 위치로 이동시킨다. 
//                  :  이때, '별가루 생성구'는 주인공을 중심으로 생성된다. 
//
BOOL CStar::StarDustUpdate(VECTOR3 vPos)
{
	// '별가루 생성구'의 크기 :  주인공 충돌구의 3배 크기로 설정. 
	float fStarDustRadius = m_pHero->GetRadius() * 3.0f;	

	//'별가루'의 충돌구 크기 설정.
	m_fCollRadius = 1.0f;								


	//별가루가 '별가루 생성구' 밖에 있게 되면..(즉,충돌되지 않았다면...'밖'에 있은 것임)
	//별가루의 새 '위치'를 계산한다.
	for(itor i=m_StarDustList.begin(); i != m_StarDustList.end(); ++i)
	{
		m_vPos = (*i)->vPos;								//별가루 위치. 

		if(CheckCollision(m_pHero, fStarDustRadius) == COLL_NO_)	
		{ 	
			VECTOR3 vNView = m_pHero->GetNView();		 
			vNView *= 10.0f;								//비행기 전방 10m 앞으로..  		
			 
			VECTOR3 vNewPos = vNView + 
							  //VECTOR3( (rand()%20)-10, (rand()%20)-10, (rand()%20)-10);
							  VECTOR3((rand()%30)-15.0f, 
							  (rand()%30)-15.0f, 
							  (rand()%30)-15.0f );
			
			//별가루의 새 위치 설정.
			(*i)->vPos =  m_pHero->GetPos() + vNewPos; 

#ifdef DEBUG_STARDUST_
			B3D_Error(NO_, "[info] 새 별가루 생성 (0x%x) : vPos(%.3f, %.3f, %.3f)",
				*i, (*i)->vPos.x, (*i)->vPos.y, (*i)->vPos.z );
#endif
 		}

	}

	// 
	//버퍼에 새로 기록하기.
	//
	ResetVBuffer();


	return TRUE;
}








/////////////////////////////////////////////////////////////////////////////
//
int CStar::Draw(float dTime/*=0*/) 
{
	//'주인공이 움직이지 않으면 그리지 않는다.
	if(fabs(m_pHero->GetMovVelo().z) <= 0.9f) 
		return TRUE;


	//
	// '현재' 카메라가..NOFIX, TRACKING 타입이면...'별가루' 를 그리지 않는다.
	//
	if((*m_ppCamera)->IsType(FCT_NOFIX_) 		//'월드 고정 + 비 추적 카메라'
		||(*m_ppCamera)->IsType(FCT_TRACKING_) 	//'월드 고정 + 추적 카메라'
		||(*m_ppCamera)->IsType(FCT_FREE_)		//자유카메라
	  )
	{
		return TRUE;
	}



	//------------------------------------------------------------------------- 
	//기타 디바이스 설정.	 
    m_pDev->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );		//Z-Buffer Write 금지.
	//m_pDev->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE);	//'알파' 테스트 필요 없음... 
    m_pDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    m_pDev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
    m_pDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

 	m_pDev->SetRenderState( D3DRS_LIGHTING, FALSE);				//라이트 oFF!!
	 
	//------------------------------------------------------------------------- 
	//포인트 스프라이트를 위한 옵션 설정.
	m_pDev->SetRenderState(D3DRS_POINTSPRITEENABLE, TRUE);		//Point Sprite ON!
	m_pDev->SetRenderState(D3DRS_POINTSCALEENABLE, FALSE);		//Scale OFF! - 원근감 없음. 
	m_pDev->SetRenderState(D3DRS_POINTSIZE, FtoDW(m_fSize));	//스프라이트 사이즈.
 	//m_pDev->SetRenderState(D3DRS_POINTSIZE_MIN, FtoDW(1.0f));	//최저 사이즈.
	//m_pDev->SetRenderState(D3DRS_POINTSIZE_MAX, FtoDW(3.0f));	//최고 사이즈. 

	// 이하 '스케일 계수'. 그냥 이대로 쓴다.
	m_pDev->SetRenderState(D3DRS_POINTSCALE_A, FtoDW(0.0f));	
	m_pDev->SetRenderState(D3DRS_POINTSCALE_B, FtoDW(0.0f)); 
	m_pDev->SetRenderState(D3DRS_POINTSCALE_C, FtoDW(1.0f));	

	//------------------------------------------------------------------------- 
	//포인트 스프라이트 텍스쳐 설정.
	m_pDev->SetTexture(0, m_Spr.pTexture);

	//------------------------------------------------------------------------- 
	//정점 데이터, 디바이스에 등록.
	m_pDev->SetStreamSource(0, m_pVBuffer, 0, sizeof(VERTEX_STAR));
	m_pDev->SetFVF(FVF_STARS_);



	//------------------------------------------------------------------------- 
	//스프라이트의 행렬 설정. :  '빌보딩' 필요 없음.
	MATRIX mView;
	D3DXMatrixIdentity(&mView);
	m_pDev->SetTransform( D3DTS_WORLD, &mView);		//스프라이트 좌표를 '월드'로 변환..
													//그냥 그자리에..
		 
	//------------------------------------------------------------------------- 
  	//포인트 스프라이트 렌더링.
	m_pDev->DrawPrimitive(D3DPT_POINTLIST, 0, m_Cnt);			//옵션 주의! 

	
	//------------------------------------------------------------------------- 
	//포인트 스프라이트 렌더링 옵션 복구.
	m_pDev->SetRenderState(D3DRS_POINTSPRITEENABLE, FALSE);		 
	m_pDev->SetRenderState(D3DRS_POINTSCALEENABLE, FALSE);		//Scale OFF! - 원근감 있는 스프라이트.


	//------------------------------------------------------------------------- 
	//기타 디바이스 설정 복구. 
	m_pDev->SetRenderState(D3DRS_LIGHTING, TRUE);
 	m_pDev->SetRenderState( D3DRS_ZWRITEENABLE, TRUE);
    m_pDev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE);

	m_pDev->SetRenderState( D3DRS_LIGHTING, TRUE);				//라이트 On!


	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
//
void CStar::Release()
{
	//정점 버퍼 해제.
	SAFE_RELEASE(m_pVBuffer);
 
	//별 리스트 제거.
	for(itor i=m_StarDustList.begin(); i != m_StarDustList.end(); )
	{
		delete (*i);
		m_StarDustList.erase(i++);
	}
}


/*********************** end of file "Star.h" ******************************/
