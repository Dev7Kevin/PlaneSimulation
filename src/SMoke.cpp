// SMoke.cpp: implementation of the CSmoke class.
//
//
////////////////////////////////////////////////////////////////////////////// 

#include "SMoke.h"

//////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////////////

CSmoke:: CSmoke(
		 LPDEVICE pDev,				//����̽� ������.
		 CB3Mesh* pMesh,			//�޽� ������.

		 SPRDATA*	pSpr,			//�޽� ������.
		 CCamera**	ppCamera,		//ī�޶� ����������. 
		 int		ShowTime,		//���� �ð�.
		 int		FadeTime,		//���̵� �ð�.
		 COLOR		Color,			//���� ����.
		 
		 VECTOR3 vPos,				//'���� ��ġ'
		 VECTOR3 vScale /*= VECTOR3(1, 1, 1)*/,			//������.
		 VECTOR3 vRot /*= VECTOR3(0, 0, 0)*/,			//'ȸ��'
  		 VECTOR3 vMoveVelo/* = VECTOR3(0, 0, 0)*/,		//�̵��ӵ�.
		 VECTOR3 vRotVelo/* = VECTOR3(0, 0, 0)*/		//ȸ�� �ӵ�.
		 )
:CUnit(vPos, vRot, vScale, vMoveVelo, vRotVelo, 
	   0.0f, NULL,	
	   pMesh, pDev)
, m_Spr(*pSpr), m_ppCamera(ppCamera), m_ShowTime(ShowTime), m_FadeTime(FadeTime)
, m_Color(Color)		
{
	m_NowTime = m_OldTime = timeGetTime();		//���� �ð� ���.
 	m_fAlpha = Color.a;

	m_bFadeOut = NO_;				

	//��ġ ���.
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
		//�ϴ� �����Ǹ�..�׸���.�ʰ�...
		//'�����ð�'�� üũ�Ѵ�.
		m_NowTime = timeGetTime();
		if(m_NowTime - m_OldTime > m_ShowTime) 
		{
			//m_OldTime = m_NowTime;				//�� ���� ������..�����Ѵ�.
			//	return FALSE;						//������ �ǹ̷�..FALSE ����.
			m_bFadeOut = YES_;				//�� ���� ������. Fade out ��Ŵ.
		}

	}
	else
	{
		//
		// �󵵸� ���� Ʈ����...
		//
 		float al = m_fAlpha;
		m_fAlpha -= (al*dTime + 0.01f*dTime);		//���� �ܰ�� 100�ܰ��...

		if(m_fAlpha < 0)				//���İ��� 0 ���ϸ�... 
 			return FALSE;				//���⸦ �����ϱ� ���� false ����.
	}

	//
	// ũ�⸦ Ű���..
	//
	VECTOR3 vScale = m_vScale;
	m_vScale += vScale * (0.3f * dTime);			//����� 30%�� Ŀ����.
	B3DSprite_Scaling(&m_Spr, m_vScale.x, m_vScale.y);	 
 
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//
int CSmoke::Draw(float dTime/*=0*/)   
{ 

	//�ø��� ������ ������..�׸��� �ʴ´�. 2004.06.25.
	if(!IsVisible()) return TRUE;			


 	// ��ġ ����.  
 	m_Spr.mRot = (*m_ppCamera)->GetBillMtx();					//������ ��� ���. 
 	B3DSprite_Translation(&m_Spr, m_vPos.x, m_vPos.y, m_vPos.z, ON3D_); //ȭ�� �̵�..
 

	//������.
 	//fade out ���ο� ����..'����'�� ��½�Ÿ���� �����Ѵ�. : 2004.06.24+ ������Ʈ.
  	B3DSprite_DrawFX2(&m_Spr, m_Color, m_bFadeOut, m_fAlpha, NO_);	 //#1 + #2 ����...

	//���� ������ �ڵ�...
	//B3DSprite_Draw3D(&m_Spr, YES_, YES_,  m_fAlpha, NO_);			//#1. '�Ͼ� ����' +  fade out. 
	//B3DSprite_DrawFX(&m_Spr, m_Color);							//#2. '�����ִ� ����' + '����'
 
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
