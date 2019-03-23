/////////////////////////////////////////////////////////////////////////////
//
// Hero.cpp:  ���ΰ� ����� ���� �ҽ� ����
//
//
/////////////////////////////////////////////////////////////////////////////

#include "Hero.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHero::CHero(
			 LPDEVICE pDev,
			 CB3Mesh* pMesh,
			 UINT hp,
			 
			 float		fRadius,			//�浹�� �ݰ�.
			 LPANIINFO	pBoomAniInfo,		//���� �ִϸ��̼� ���� ����ü.04.06.23+ 
											// NULL �̸� ���õȴ�.


			 VECTOR3 vPos, 
			 VECTOR3 vRot/* = VECTOR3(0.0f, 0.0f, 0.0f)*/, 
			 VECTOR3 vScale/*= VECTOR3(1.0f, 1.0f, 1.0f)*/,
			 VECTOR3 vMoveVelo/* = VECTOR3(0.0f, 0.0f, 0.0f)*/,
			 VECTOR3 vRotVelo/* = VECTOR3(0.0f, 0.0f, 0.0f)*/
			 )
:CPlane(pDev, pMesh, hp, 
		fRadius, pBoomAniInfo,	
		vPos, vRot, vScale, vMoveVelo, vRotVelo)
{
	//n/a	
}

////////////////////////////////////////////////////////////////////////////
//
CHero::~CHero()
{
	Release();
}




////////////////////////////////////////////////////////////////////////////
//
// ���ΰ� �̵� ó��. 
//
// ����Ű�� ������ ���ӵ��� ������Ų��.
//
int CHero::Move(float dTime/*=0*/)
{ 		 
	//-------------------------------------------------------------------
	// �̵� ó��.
	//-------------------------------------------------------------------
	//static const float fac1 = 100.0;	//���� �ִ� �߷� (100m/s) 

	//������ �߷�(Throttle) �� ����. 
	//if(IsKeyDown(DIK_LSHIFT))			//�߷� ����.
	//	m_vMoveVelo.z += (fac1 * dTime);		
	//if(IsKeyDown(DIK_LCONTROL))			 
	//	m_vMoveVelo.z -= (fac1 * dTime);
	m_fMaxVelo = 27.7f * 3.0f;	//�ü� 300km/h max. 

	if(IsKeyDown(DIK_LSHIFT)) 	 
	{
		m_fThrottle += 30.0f * dTime;  // Ʈ��Ʋ ������ : 30%/sec  
		if(m_fThrottle >= 110.0f)		//100 % �� �ְ�...
			m_fThrottle = 110.0f;		// ����-�õ� off ����..�׳� '����' �� ó��... 
	}

	if(IsKeyDown(DIK_LCONTROL))
	{
		m_fThrottle -= 30.0f * dTime;  // Ʈ��Ʋ ������ : 30%/sec  
		if(m_fThrottle <= 0.001f)
			m_fThrottle = 0.0f;
	}

	//�̵� �ӵ� ����.. : ����� ����Ʋ�� �ܼ� ������Ų��. 
	m_vMoveVelo.z = m_fMaxVelo * m_fThrottle * 0.01f;	//Max.m/s * (0.0 ~ 1.1) : 0~110%	



	//-------------------------------------------------------------------
	// ����� ������-Stick- ó��. 
	//-------------------------------------------------------------------
	static float fac2 = D3DX_PI;	//���� �ִ� ��ȸ��(180��/s)

	// Stick �� �������� ����..: ���� ��ũ.
	if(IsKeyDown(DIK_LEFT) && !IsKeyDown(DIK_RIGHT)) //���ÿ� ���� ���� ����!! 
	{
		m_vRotVelo.z += fac2*dTime; 
	}

	// Stick �� ���������� ����..: ������ ��ũ.
	if(IsKeyDown(DIK_RIGHT) && !IsKeyDown(DIK_LEFT))  
	{
		m_vRotVelo.z -= fac2*dTime; 
	}

	// Stick ��  �δ�..'�ϰ�' 
	if(IsKeyDown(DIK_UP) && !IsKeyDown(DIK_DOWN))  
	{	
		m_vRotVelo.x += fac2 * dTime;		 
	}

	// Stick ��  ��ƴ���..'���' 
	if(IsKeyDown(DIK_DOWN) && !IsKeyDown(DIK_UP))  
	{
		m_vRotVelo.x -= fac2 * 0.7f *dTime;      
	} 



	//-------------------------------------------------------------------
	// ���� : ���� ������Ʈ.
	//-------------------------------------------------------------------	 
	/*if(IsKeyDown(DIK_COMMA) && !IsKeyDown(DIK_PERIOD))  
	{
	m_vRotVelo.y += fac2 * 0.3f * dTime;		 
	}	 
	if(IsKeyDown(DIK_PERIOD) && !IsKeyDown(DIK_COMMA))  
	{
	m_vRotVelo.y -= fac2 * 0.3f * dTime;      
	}*/


	//-------------------------------------------------------------------
	// Ű ������ ������ '����' : ������ �����Ѵ�.
	// 1�ʰ� Ű������ ������ �ӵ��� 0 �� �ȴ�. 
	//-------------------------------------------------------------------
	//m_vMoveVelo *= (1.f - dTime); 
	m_vRotVelo *= (1.f - dTime); 


	//-------------------------------------------------------------------
	//'�ӵ�'�� ��������..��ü�� ��¦ '����' �ش�. : 2004.07.01.
	// �ְ�ӵ����� 85% �̻��..
	//-------------------------------------------------------------------
	if(fabs(m_vMoveVelo.z) >= (m_fMaxVelo * 0.85f)) 
	{
		m_vShake.x = (rand()%2)?1.0f:-1.0f;		//������...
		m_vShake.y = (rand()%2)?1.0f:-1.0f; 
		m_vShake.z = (rand()%2)?1.0f:-1.0f;

		m_vShake *=  (1.0f - dTime)* 0.01f;

	}
	else 
		m_vShake = VECTOR3(0, 0, 0);



	//�������� �θ𿡼� ó��. 
	return CPlane::Move(dTime);
}




////////////////////////////////////////////////////////////////////////////
//
int CHero::Draw(float dTime/*=0*/)   
{

	//----------------------------------------------//
	// ���ΰ��� �ø��� �ʿ䰡 ������?				//	
	//----------------------------------------------//
	//if(!IsVisible()) return TRUE;	



	// ���ϴ� �ڵ带 �߰� �Ұ�.
	//���ΰ��� �׾��ٸ�...
	if(m_bIsDead)
		return DrawBoom(dTime);	//���� �ִ� ���. : ���߿� ���ΰ����� 
								//���� �ִϸ� ���� �����ε� �� ���̴�.



	//�������� �θ𿡼� ó��. 
	return CPlane::Draw(dTime);      
}



////////////////////////////////////////////////////////////////////////////
//
void CHero::Release()
{
	// ���ϴ� �ڵ带 �߰� �Ұ�.
	//N/A
}


/************************* end of file "Hero.cpp" **************************/