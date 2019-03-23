// Star.cpp: implementation of the CStar class.
// 
//
////////////////////////////////////////////////////////////////////////////// 

#include "Star.h"
#include "Plane.h"
#include "Camera.h"



/////////////////////////////////////////////////////////////////////////////
// flot -> DWORD ��ȯ �ζ��� �Լ�.
//
inline DWORD FtoDW( FLOAT f ) { return *((DWORD*)&f); }




/////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
/////////////////////////////////////////////////////////////////////////////

CStar::CStar(
	 LPDEVICE	pDev,			//����̽� ������.
	 int		Cnt,			//�� ����.
 	 SPRDATA*	pSpr,			//'��' ��������Ʈ ������.
 	 CPlane*	pHero,			//���ΰ� ������.
	 CCamera**	ppCamera,		//'����' Ȱ��ȭ�� ī�޶��� 2�� ������.
	 int		Width			//'���÷���' ��� : '������'�� ũ�⿡ �����Ѵ�.	 
	 )
:CUnit(VECTOR3(0, 0, 0), VECTOR3(0, 0, 0), VECTOR3(0, 0, 0), 
	   VECTOR3(0, 0, 0), VECTOR3(0, 0, 0), 0.0f, NULL, NULL, pDev)	 
,m_Cnt(Cnt), m_Spr(*pSpr), m_pHero(pHero), m_ppCamera(ppCamera)
{
	m_pVBuffer = NULL;			

	//������ ������ ����.
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
// �޼ҵ��.
// 
/////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////
//
BOOL CStar::Init()
{
	::srand(::timeGetTime());

	//------------------------------------------------------------------------- 
	//'��' ��ġ ����. - ���� ���ۿ� �� ������ ����...
	//
	VERTEX_STAR* pStar;

	//������ : '���� ���ΰ��� ��ġ�� �������� ���� ���� �ȿ� ���� �����Ѵ�.
	for(int i=0; i<m_Cnt; i++)		 
	{
		pStar = new VERTEX_STAR;
		ASSERT(pStar != NULL);
		
		pStar->dwColor = COLOR(1,1,1,1); 
 		pStar->vPos =  m_pHero->GetPos() +  
					   VECTOR3((rand()%30)-15.0f, 
					           (rand()%30)-15.0f, 
							   (rand()%30)-15.0f );
		
		m_StarDustList.push_back(pStar);	//����Ʈ�� ���.
	}



	//------------------------------------------------------------------------- 
	// ���� ���� ����  
	//
	HRESULT res = S_OK;
	if( FAILED( res = m_pDev->CreateVertexBuffer(
		m_Cnt * sizeof(VERTEX_STAR), 
 		//D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY | D3DUSAGE_POINTS,	//-> �� �ɼ��� ���� �ʿ� ����
		D3DUSAGE_WRITEONLY | D3DUSAGE_POINTS,						//-> �� �ɼ� �ƴϸ�, (����ȭ)
		//D3DUSAGE_POINTS,											//-> �� �ɼ� ������ �����.
		FVF_STARS_, 
		D3DPOOL_MANAGED,  
		&m_pVBuffer, 
		NULL)))
	{
		B3D_Error(YES_, "[Error] CStar::Init - ���� ���� ���� ����...:%s", 
						DXGetErrorString9(res));
		return FALSE;	
	}


	//------------------------------------------------------------------------- 
	// ���ۿ� ���...
	ResetVBuffer();
 

	return TRUE;
}





/////////////////////////////////////////////////////////////////////////////
//
// �����縦 ���� ���ۿ� ����Ѵ�. 
//
BOOL CStar::ResetVBuffer()
{	
	HRESULT res = S_OK;
	//
	// ���� ���� ������ ����.
	//
	VERTEX_STAR* pVertices;
	if(FAILED(res = m_pVBuffer->Lock(0, 
									 0,		//���� ���� ��ױ�..
									 (void**)&pVertices,
									 NULL
	)))
	{
		B3D_Error(NO_, "[Error] CStar::Init - ���� ���� ��� ����..:%s", 
			DXGetErrorString9(res));
		return FALSE;
	}

	//VB �� ������ Write.
	for(itor ii=m_StarDustList.begin(); ii != m_StarDustList.end(); ++ii)
	{
		memcpy(pVertices++, *ii, sizeof(VERTEX_STAR));
 	}


	//VB �ݱ�.
	m_pVBuffer->Unlock();

	return TRUE;
}




/////////////////////////////////////////////////////////////////////////////
//
int CStar::Move(float dTime/*=0*/)
{	 
 	StarDustUpdate(m_pHero->GetPos());		//������ ����...

	return TRUE;
}




/////////////////////////////////////////////////////////////////////////////
//
// '������' �̵�... : '������ ������'�� �����..'���ο� ��ġ�� �̵���Ų��. 
//                  :  �̶�, '������ ������'�� ���ΰ��� �߽����� �����ȴ�. 
//
BOOL CStar::StarDustUpdate(VECTOR3 vPos)
{
	// '������ ������'�� ũ�� :  ���ΰ� �浹���� 3�� ũ��� ����. 
	float fStarDustRadius = m_pHero->GetRadius() * 3.0f;	

	//'������'�� �浹�� ũ�� ����.
	m_fCollRadius = 1.0f;								


	//�����簡 '������ ������' �ۿ� �ְ� �Ǹ�..(��,�浹���� �ʾҴٸ�...'��'�� ���� ����)
	//�������� �� '��ġ'�� ����Ѵ�.
	for(itor i=m_StarDustList.begin(); i != m_StarDustList.end(); ++i)
	{
		m_vPos = (*i)->vPos;								//������ ��ġ. 

		if(CheckCollision(m_pHero, fStarDustRadius) == COLL_NO_)	
		{ 	
			VECTOR3 vNView = m_pHero->GetNView();		 
			vNView *= 10.0f;								//����� ���� 10m ������..  		
			 
			VECTOR3 vNewPos = vNView + 
							  //VECTOR3( (rand()%20)-10, (rand()%20)-10, (rand()%20)-10);
							  VECTOR3((rand()%30)-15.0f, 
							  (rand()%30)-15.0f, 
							  (rand()%30)-15.0f );
			
			//�������� �� ��ġ ����.
			(*i)->vPos =  m_pHero->GetPos() + vNewPos; 

#ifdef DEBUG_STARDUST_
			B3D_Error(NO_, "[info] �� ������ ���� (0x%x) : vPos(%.3f, %.3f, %.3f)",
				*i, (*i)->vPos.x, (*i)->vPos.y, (*i)->vPos.z );
#endif
 		}

	}

	// 
	//���ۿ� ���� ����ϱ�.
	//
	ResetVBuffer();


	return TRUE;
}








/////////////////////////////////////////////////////////////////////////////
//
int CStar::Draw(float dTime/*=0*/) 
{
	//'���ΰ��� �������� ������ �׸��� �ʴ´�.
	if(fabs(m_pHero->GetMovVelo().z) <= 0.9f) 
		return TRUE;


	//
	// '����' ī�޶�..NOFIX, TRACKING Ÿ���̸�...'������' �� �׸��� �ʴ´�.
	//
	if((*m_ppCamera)->IsType(FCT_NOFIX_) 		//'���� ���� + �� ���� ī�޶�'
		||(*m_ppCamera)->IsType(FCT_TRACKING_) 	//'���� ���� + ���� ī�޶�'
		||(*m_ppCamera)->IsType(FCT_FREE_)		//����ī�޶�
	  )
	{
		return TRUE;
	}



	//------------------------------------------------------------------------- 
	//��Ÿ ����̽� ����.	 
    m_pDev->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );		//Z-Buffer Write ����.
	//m_pDev->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE);	//'����' �׽�Ʈ �ʿ� ����... 
    m_pDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    m_pDev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
    m_pDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

 	m_pDev->SetRenderState( D3DRS_LIGHTING, FALSE);				//����Ʈ oFF!!
	 
	//------------------------------------------------------------------------- 
	//����Ʈ ��������Ʈ�� ���� �ɼ� ����.
	m_pDev->SetRenderState(D3DRS_POINTSPRITEENABLE, TRUE);		//Point Sprite ON!
	m_pDev->SetRenderState(D3DRS_POINTSCALEENABLE, FALSE);		//Scale OFF! - ���ٰ� ����. 
	m_pDev->SetRenderState(D3DRS_POINTSIZE, FtoDW(m_fSize));	//��������Ʈ ������.
 	//m_pDev->SetRenderState(D3DRS_POINTSIZE_MIN, FtoDW(1.0f));	//���� ������.
	//m_pDev->SetRenderState(D3DRS_POINTSIZE_MAX, FtoDW(3.0f));	//�ְ� ������. 

	// ���� '������ ���'. �׳� �̴�� ����.
	m_pDev->SetRenderState(D3DRS_POINTSCALE_A, FtoDW(0.0f));	
	m_pDev->SetRenderState(D3DRS_POINTSCALE_B, FtoDW(0.0f)); 
	m_pDev->SetRenderState(D3DRS_POINTSCALE_C, FtoDW(1.0f));	

	//------------------------------------------------------------------------- 
	//����Ʈ ��������Ʈ �ؽ��� ����.
	m_pDev->SetTexture(0, m_Spr.pTexture);

	//------------------------------------------------------------------------- 
	//���� ������, ����̽��� ���.
	m_pDev->SetStreamSource(0, m_pVBuffer, 0, sizeof(VERTEX_STAR));
	m_pDev->SetFVF(FVF_STARS_);



	//------------------------------------------------------------------------- 
	//��������Ʈ�� ��� ����. :  '������' �ʿ� ����.
	MATRIX mView;
	D3DXMatrixIdentity(&mView);
	m_pDev->SetTransform( D3DTS_WORLD, &mView);		//��������Ʈ ��ǥ�� '����'�� ��ȯ..
													//�׳� ���ڸ���..
		 
	//------------------------------------------------------------------------- 
  	//����Ʈ ��������Ʈ ������.
	m_pDev->DrawPrimitive(D3DPT_POINTLIST, 0, m_Cnt);			//�ɼ� ����! 

	
	//------------------------------------------------------------------------- 
	//����Ʈ ��������Ʈ ������ �ɼ� ����.
	m_pDev->SetRenderState(D3DRS_POINTSPRITEENABLE, FALSE);		 
	m_pDev->SetRenderState(D3DRS_POINTSCALEENABLE, FALSE);		//Scale OFF! - ���ٰ� �ִ� ��������Ʈ.


	//------------------------------------------------------------------------- 
	//��Ÿ ����̽� ���� ����. 
	m_pDev->SetRenderState(D3DRS_LIGHTING, TRUE);
 	m_pDev->SetRenderState( D3DRS_ZWRITEENABLE, TRUE);
    m_pDev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE);

	m_pDev->SetRenderState( D3DRS_LIGHTING, TRUE);				//����Ʈ On!


	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
//
void CStar::Release()
{
	//���� ���� ����.
	SAFE_RELEASE(m_pVBuffer);
 
	//�� ����Ʈ ����.
	for(itor i=m_StarDustList.begin(); i != m_StarDustList.end(); )
	{
		delete (*i);
		m_StarDustList.erase(i++);
	}
}


/*********************** end of file "Star.h" ******************************/
