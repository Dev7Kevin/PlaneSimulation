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
		 LPDEVICE	pDev,			//����̽� ������.
 		 float		fSWRadius,		//����� ���� �ݰ�. 
 		 float		fSWThick,		//�����  �β�.
		 COLOR		Color,			//���� ����. 
  		 LPANIINFO	pBoomAniInfo,	//���� �ִϸ��̼� ���� ����ü.	04.06.23+ 
									//: NULL �̸� ���õȴ�.
 		 VECTOR3 vPos				//'���� ��ġ' -> ������� �߽���ǥ.
    	)
:CUnit( vPos, VECTOR3(0, 0, 0), 
	   VECTOR3(1, 1, 1),		//scale.
	   VECTOR3(0, 0, 0),		//move speed.
	   VECTOR3(0, 0, 0),		//angle speed
	   0.0f,					//�浹�� �ݰ�.
	   pBoomAniInfo,			//���� �ִ� ����...-> '�����' �ִ� ����.
	   NULL,					//�޽�������.
	   pDev)					//����̽� ������.
, m_fSWRadius(fSWRadius), m_fSWThick(fSWThick)
, m_pVB(NULL) , m_Color(Color)
{
 	m_bFadeOut = NO_;
	m_fAlpha = 1.0f;
	m_VertexCnt = 0;

	//����� 'ȸ��'�� ����. : �� ���к���. 180�� �ȿ��� ����.
  	m_vRot = VECTOR3( ((float)(rand()%314))/100.0f, 
					  ((float)(rand()%314))/100.0f, 
					  ((float)(rand()%314))/100.0f );	

 	//����� ������ ����... 
	Init();		
 	 
}


/////////////////////////////////////////////////////////////////////////////
//
CShockWave::~CShockWave()
{
	Release();
}




/////////////////////////////////////////////////////////////////////////////
// �޼ҵ� ����.
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
BOOL CShockWave::Init()
{
	//'�浹��' �޽��� �����Ѵ�.
 	int Divisions = 32;				//���� 32����Ѵ�..	
	int VertexCnt = Divisions * 6;	//������ ����. (�ﰢ�� �Ѱ��� 3���� ����)
	m_VertexCnt = VertexCnt;


	//���� ���� ����.
	HRESULT res = S_OK;
	if(FAILED(res = m_pDev->CreateVertexBuffer( VertexCnt * sizeof(VERTEX_SHOCKWAVE), 
					D3DUSAGE_WRITEONLY, FVF_SHOCKWAVE_, D3DPOOL_MANAGED, 
					&m_pVB, NULL)))
	{
		B3D_Error(NO_, "[Error] CShockWave(0x%x)::Init - ���� ���� ���� ����...: res = %s", 
					this, DXGetErrorString9(res));
		return FALSE;
	}

	//���� ���۸� ��ױ�. 
	VERTEX_SHOCKWAVE* pVertices = NULL;
	if(FAILED(res = m_pVB->Lock(0, 0, (VOID**)&pVertices, NULL)))
	{
		//���� ���� ����.
		Release(); //SAFE_RELEASE(m_pVB);

		B3D_Error(NO_, "[Error] CShockWave(0x%x)::Init - ���� ���� ��� ����...: res = %s", 
					this, DXGetErrorString9(res));
		return FALSE;
	}

	//
	// ������ �Է�.
	//
	float step = (D3DX_PI*2)/ Divisions;		//360���� 32���..�Ѵ�...

	
	VECTOR2 v[4];
	for(float r = 0; r<D3DX_PI*2; r+=step )	
	{
  		v[0].x = m_fSWRadius * (float)sin(r);			//��е�  �簢���� ���� 4���� ��ġ�� �� ���...
		v[0].y = m_fSWRadius * (float)cos(r);

		v[1].x = m_fSWRadius * (float)sin(r+step);
		v[1].y = m_fSWRadius * (float)cos(r+step);

		v[2].x = (m_fSWRadius - m_fSWThick) * (float)sin(r);
		v[2].y = (m_fSWRadius - m_fSWThick) * (float)cos(r);

		v[3].x = (m_fSWRadius - m_fSWThick) * (float)sin(r+step);
		v[3].y = (m_fSWRadius - m_fSWThick) * (float)cos(r+step);

		//���� 4���� ���� ��е� �簢�� �޽��� ���� �Ѵ�. 
		//�ﰢ�� ����Ʈ�� ����� ���̹Ƿ� �� ������ 6���� �ʿ�.
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


	//���� �ݱ�.
	m_pVB->Unlock(); 


	return TRUE;
}



 
/////////////////////////////////////////////////////////////////////////////
//
int CShockWave::Move(float dTime/* =0 */)
{
	//��ũ ���̺� �̵� �ڵ� �߰�...

	return TRUE;

}


/////////////////////////////////////////////////////////////////////////////
//
int CShockWave::Draw(float dTime/*= 0*/)	
{
 	//��ũ ���̺긦 '�񽺵��ϰ�' ó���Ѵ�. -> �����ڿ���  ó��.
	 

	// ����������� ��� ������� ũ�⸦ Ű���....
	m_fSWRadius += 20.0f*dTime;							//���� �ð��� 20%��... 
 	
	// '���̵� �ƿ�' ó��.
  	if(m_bFadeOut == NO_)								// Ŀ���� ���̸�...
	{
 		if(m_fSWRadius >= m_BoomAniInfo.vScale.x/2)		//�ִ� �ݰ溸�� Ŀ����...'���̵� �ƿ�' ��Ų��.
		{
  			m_bFadeOut = YES_;
		}
	}
	else												//���̵� �ƿ����̸�...
	{ 		
		m_fAlpha -= 0.3f*dTime;			//���İ� ����.
		
  		if(m_fAlpha<0.0f)		//���İ� 0 ����->'������' �������..
		{						//��ü ������ ���� FALSE ����.
  			return FALSE;		//0.0F ���� ������ '����'�� �ǹ̷� FALSE ����.
		}
	}

	//
	// ��ȯ ��� ����..: '������' ���� 'ȸ��'���� �ݿ��Ǿ�� �Ѵ�.
	//
	// - 'ȸ��'���� �����ڿ��� ����.
	// - '������'�� ��� �ڵ忡�� ����.
	// - '��ġ'�� �Һ�.
	// 
	D3DXMatrixRotationYawPitchRoll(&m_mRotation, m_vRot.y, m_vRot.x, m_vRot.z);
  	D3DXMatrixScaling(&m_mScale, m_fSWRadius*2, m_fSWRadius*2, 1.0f);  
 	D3DXMatrixTranslation(&m_mTranslation, m_vPos.x, m_vPos.y, m_vPos.z);
 	 
	D3DXMatrixMultiply(&m_mTM, &m_mScale, &m_mRotation);
	D3DXMatrixMultiply(&m_mTM, &m_mTM, &m_mTranslation);
 	 
	m_pDev->SetStreamSource(0, m_pVB, 0, sizeof(VERTEX_SHOCKWAVE));
	m_pDev->SetFVF(FVF_SHOCKWAVE_);
	m_pDev->SetTransform(D3DTS_WORLD, &m_mTM);

	//2004.06.29++ ����Ŀ� ȿ�� �Լ�.
	//�ܺ� ��Ŭ.
	B3DSprite_DrawFX3(D3DPT_TRIANGLELIST, m_VertexCnt/3, m_BoomAniInfo.pSpr, 
				   	  m_Color, m_bFadeOut, m_fAlpha, NO_);
	 
	//���� ��Ŭ.(��� ������ ����...)
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