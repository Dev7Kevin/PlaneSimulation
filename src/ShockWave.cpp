//
// ShockWave.cpp: implementation of the CShockWave class.
//
//
////////////////////////////////////////////////////////////////////////////// 


#include "ShockWave.h"
#include "Camera.h"

//////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////////////

CShockWave::CShockWave(
		 LPDEVICE	pDev,			//디바이스 포인터.
 		 float		fSWRadius,		//충격파 시작 반경. 
 		 float		fSWThick,		//충격파  두께.
		 COLOR		Color,			//연기 색상. 
  		 LPANIINFO	pBoomAniInfo,	//폭파 애니메이션 정보 구조체.	04.06.23+ 
									//: NULL 이면 무시된다.
 		 VECTOR3 vPos				//'월드 위치' -> 충격파의 중심좌표.
    	)
:CUnit( vPos, VECTOR3(0, 0, 0), 
	   VECTOR3(1, 1, 1),		//scale.
	   VECTOR3(0, 0, 0),		//move speed.
	   VECTOR3(0, 0, 0),		//angle speed
	   0.0f,					//충돌구 반경.
	   pBoomAniInfo,			//폭파 애니 정보...-> '충격파' 애니 정보.
	   NULL,					//메쉬포인터.
	   pDev)					//디바이스 포인터.
, m_fSWRadius(fSWRadius), m_fSWThick(fSWThick)
, m_pVB(NULL) , m_Color(Color)
{
 	m_bFadeOut = NO_;
	m_fAlpha = 1.0f;
	m_VertexCnt = 0;

	//충격파 '회전'값 설정. : 각 성분별로. 180도 안에서 설정.
  	m_vRot = VECTOR3( ((float)(rand()%314))/100.0f, 
					  ((float)(rand()%314))/100.0f, 
					  ((float)(rand()%314))/100.0f );	

 	//충격파 데이터 생성... 
	Init();		
 	 
}


/////////////////////////////////////////////////////////////////////////////
//
CShockWave::~CShockWave()
{
	Release();
}




/////////////////////////////////////////////////////////////////////////////
// 메소드 구현.
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
BOOL CShockWave::Init()
{
	//'충돌파' 메쉬를 생성한다.
 	int Divisions = 32;				//원을 32등분한다..	
	int VertexCnt = Divisions * 6;	//정점의 개수. (삼각형 한개당 3개의 정점)
	m_VertexCnt = VertexCnt;


	//정점 버퍼 생성.
	HRESULT res = S_OK;
	if(FAILED(res = m_pDev->CreateVertexBuffer( VertexCnt * sizeof(VERTEX_SHOCKWAVE), 
					D3DUSAGE_WRITEONLY, FVF_SHOCKWAVE_, D3DPOOL_MANAGED, 
					&m_pVB, NULL)))
	{
		B3D_Error(NO_, "[Error] CShockWave(0x%x)::Init - 정점 버퍼 생성 실패...: res = %s", 
					this, DXGetErrorString9(res));
		return FALSE;
	}

	//정점 버퍼를 잠그기. 
	VERTEX_SHOCKWAVE* pVertices = NULL;
	if(FAILED(res = m_pVB->Lock(0, 0, (VOID**)&pVertices, NULL)))
	{
		//정점 버퍼 제거.
		Release(); //SAFE_RELEASE(m_pVB);

		B3D_Error(NO_, "[Error] CShockWave(0x%x)::Init - 정점 버퍼 잠금 실패...: res = %s", 
					this, DXGetErrorString9(res));
		return FALSE;
	}

	//
	// 데이터 입력.
	//
	float step = (D3DX_PI*2)/ Divisions;		//360도를 32등분..한다...

	
	VECTOR2 v[4];
	for(float r = 0; r<D3DX_PI*2; r+=step )	
	{
  		v[0].x = m_fSWRadius * (float)sin(r);			//등분된  사각형의 정점 4개의 위치를 를 계산...
		v[0].y = m_fSWRadius * (float)cos(r);

		v[1].x = m_fSWRadius * (float)sin(r+step);
		v[1].y = m_fSWRadius * (float)cos(r+step);

		v[2].x = (m_fSWRadius - m_fSWThick) * (float)sin(r);
		v[2].y = (m_fSWRadius - m_fSWThick) * (float)cos(r);

		v[3].x = (m_fSWRadius - m_fSWThick) * (float)sin(r+step);
		v[3].y = (m_fSWRadius - m_fSWThick) * (float)cos(r+step);

		//정점 4개를 통해 등분된 사각형 메쉬를 설정 한다. 
		//삼각형 리스트를 사용할 것이므로 총 정점은 6개가 필요.
		pVertices->vPos = VECTOR3(v[0].x, v[0].y, 0.0f);	//0.
		//pVertices->dwColor = m_dwColor;
		pVertices->tu = 0.0f;
		pVertices->tv = 0.0f;
		pVertices++;
		
		pVertices->vPos = VECTOR3(v[1].x, v[1].y, 0.0f);	//1
		//pVertices->dwColor = m_dwColor;
		pVertices->tu = 1.0f;
		pVertices->tv = 0.0f;
		pVertices++;

		pVertices->vPos = VECTOR3(v[2].x, v[2].y, 0.0f);	//2
		//pVertices->dwColor = m_dwColor;
		pVertices->tu = 0.0f;
		pVertices->tv = 1.0f;
		pVertices++;


		pVertices->vPos = VECTOR3(v[2].x, v[2].y, 0.0f);	//3
		//pVertices->dwColor = m_dwColor;
		pVertices->tu = 0.0f;
		pVertices->tv = 1.0f;
 		pVertices++;

		pVertices->vPos = VECTOR3(v[1].x, v[1].y, 0.0f);	//4
		//pVertices->dwColor = m_dwColor;
		pVertices->tu = 1.0f;
		pVertices->tv = 0.0f;
		pVertices++;

		pVertices->vPos = VECTOR3(v[3].x, v[3].y, 0.0f);	//5
		//pVertices->dwColor = m_dwColor;
		pVertices->tu = 1.0f;
		pVertices->tv = 1.0f;
		pVertices++;
 
	}


	//버퍼 닫기.
	m_pVB->Unlock(); 


	return TRUE;
}



 
/////////////////////////////////////////////////////////////////////////////
//
int CShockWave::Move(float dTime/* =0 */)
{
	//쇼크 웨이브 이동 코드 추가...

	return TRUE;

}


/////////////////////////////////////////////////////////////////////////////
//
int CShockWave::Draw(float dTime/*= 0*/)	
{
 	//쇼크 웨이브를 '비스듬하게' 처리한다. -> 생성자에서  처리.
	 

	// 사라질때까지 계속 충격파의 크기를 키운다....
	m_fSWRadius += 20.0f*dTime;							//단위 시간당 20%씩... 
 	
	// '페이드 아웃' 처리.
  	if(m_bFadeOut == NO_)								// 커지는 중이면...
	{
 		if(m_fSWRadius >= m_BoomAniInfo.vScale.x/2)		//최대 반경보다 커지면...'페이드 아웃' 시킨다.
		{
  			m_bFadeOut = YES_;
		}
	}
	else												//페이드 아웃중이면...
	{ 		
		m_fAlpha -= 0.3f*dTime;			//알파값 낮춤.
		
  		if(m_fAlpha<0.0f)		//알파가 0 이하->'완전히' 사라지면..
		{						//객체 삭제를 위해 FALSE 리턴.
  			return FALSE;		//0.0F 보다 작으면 '삭제'의 의미로 FALSE 리턴.
		}
	}

	//
	// 변환 행렬 구성..: '스케일' 값과 '회전'값이 반영되어야 한다.
	//
	// - '회전'값은 생성자에서 구성.
	// - '스케일'은 상단 코드에서 구성.
	// - '위치'는 불변.
	// 
	D3DXMatrixRotationYawPitchRoll(&m_mRotation, m_vRot.y, m_vRot.x, m_vRot.z);
  	D3DXMatrixScaling(&m_mScale, m_fSWRadius*2, m_fSWRadius*2, 1.0f);  
 	D3DXMatrixTranslation(&m_mTranslation, m_vPos.x, m_vPos.y, m_vPos.z);
 	 
	D3DXMatrixMultiply(&m_mTM, &m_mScale, &m_mRotation);
	D3DXMatrixMultiply(&m_mTM, &m_mTM, &m_mTranslation);
 	 
	m_pDev->SetStreamSource(0, m_pVB, 0, sizeof(VERTEX_SHOCKWAVE));
	m_pDev->SetFVF(FVF_SHOCKWAVE_);
	m_pDev->SetTransform(D3DTS_WORLD, &m_mTM);

	//2004.06.29++ 충격파용 효과 함수.
	//외부 써클.
	B3DSprite_DrawFX3(D3DPT_TRIANGLELIST, m_VertexCnt/3, m_BoomAniInfo.pSpr, 
				   	  m_Color, m_bFadeOut, m_fAlpha, NO_);
	 
	//내부 써클.(밝게 빛나기 위함...)
  	B3DSprite_DrawFX3(D3DPT_TRIANGLELIST, m_VertexCnt/3, m_BoomAniInfo.pSpr, 
			  		  D3DCOLOR_ARGB(255, 255, 255,255), m_bFadeOut, m_fAlpha, NO_);
 	 
	return TRUE;
}

 

/////////////////////////////////////////////////////////////////////////////
//
void CShockWave::Release()
{
	SAFE_RELEASE(m_pVB);
}
 



/*********************** end of file "ShockWave.cpp" ***********************/