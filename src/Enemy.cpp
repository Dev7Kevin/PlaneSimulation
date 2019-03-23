// Enemy.cpp: implementation of the CEnemy class.
//
//
////////////////////////////////////////////////////////////////////////////// 


#include "Enemy.h"


/////////////////////////////////////////////////////////////////////////////
//
int CEnemy::m_EnemyCnt = 0;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEnemy::CEnemy(
		LPDEVICE	pDev,			//����̽� ������.
		CB3Mesh*	pMesh,			//�޽� ������.
 		UINT		hp,				//���� ü��.
		float		fRadius,		//�浹�� �ݰ�.
		LPANIINFO	pBoomAniInfo,	//���� �ִϸ��̼� ���� ����ü.04.06.23+ 
									//NULL �̸� ���õȴ�.
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
	 
	m_SNum = m_EnemyCnt++;

#ifdef _DEBUG
	B3D_Error(NO_, "[info] No.%d ����(0x%x) ���� /  ���� ���� �� : %d.",
				m_SNum, this, m_EnemyCnt);
#endif

	//2004.11.11  ���� �⵿����.
	m_ThrottleState = 0;
	m_ThrottleStateOldTime = ::GetTickCount();
	m_PitchRollState = 0;
	m_PitchRollStateOldTime = ::GetTickCount();

	//2004.11.15.
	m_bLockON = NO_;
}



////////////////////////////////////////////////////////////////////////////
//
CEnemy::~CEnemy()
{
	--m_EnemyCnt;

#ifdef _DEBUG
	B3D_Error(NO_, "[info] No.%d ����(0x%x) ����. /  ���� ���� �� : %d. ",
				m_SNum, this, m_EnemyCnt);
#endif

	Release();
}






/////////////////////////////////////////////////////////////////////////////
// �⺻ �޼ҵ�.
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
//
// int CEnemy::Move(float dTime/* =0 */) - ���� �̵�. 
// 
//
int CEnemy::Move(float dTime/* =0 */)		
{	
	//-------------------------------------------------------------------
	// �̵� ó��.
	//-------------------------------------------------------------------
	static const float fac1 = 100.0f;	//<- �ְ� �ӷ��̶�� �����ϸ� �ȴ�.

	{
		int nowtime = GetTickCount();
		if(nowtime - m_ThrottleStateOldTime > (1000/(rand()%3+1)))
		{
			m_ThrottleStateOldTime = nowtime;
			m_ThrottleState = rand()%5;
		}
	}

	switch(m_ThrottleState)
	{
	case 0: 
	case 1:
	case 2:
	case 3:
		// ���� ���.. Throttle 
		m_vMoveVelo.z += (fac1 * dTime);		
		break;
	case 4:
		break;

	}


	//-------------------------------------------------------------------
	// ����� ������-Stick- ó��. 
	//-------------------------------------------------------------------
	static float fac2 = D3DX_PI;	//<- �ְ� ȸ����.

	{
		int nowtime = GetTickCount();
		if(nowtime - m_PitchRollStateOldTime > (1000/(rand()%3+1)))
		{
			m_PitchRollStateOldTime = nowtime; 
			m_PitchRollState = rand()%5;
		}
	}

	switch(m_PitchRollState)			
	{
	case 0:
		// Stick �� �������� ����..: ���� ��ũ.
		m_vRotVelo.z += fac2*dTime; 
		break;
	case 1:
		// Stick �� ���������� ����..: ������ ��ũ.
		m_vRotVelo.z -= fac2*dTime; 
		break;
	case 2:
		// Stick ��  �δ�..'�ϰ�' 
		m_vRotVelo.x += fac2/2*dTime;		 
		break;
	case 3:
		// Stick ��  ��ƴ���..'���' 
		m_vRotVelo.x -= fac2/2*dTime;      
		break;
	default:
		break;
	}


	//-------------------------------------------------------------------
	// Ű ������ ������ '����' : ������ �����Ѵ�.
	// 1�ʰ� Ű������ ������ �ӵ��� 0 �� �ȴ�. 
	//-------------------------------------------------------------------
	m_vMoveVelo *= (1.0f - dTime);
	m_vRotVelo *= (1.0f - dTime);	


	return CPlane::Move(dTime);			
}



/////////////////////////////////////////////////////////////////////////////
//
// int CEnemy::Draw(float dTime/*=0*/)   - ���� ��� 
//
int CEnemy::Draw(float dTime/*=0*/)   
{
	//
	// ���� �׾��ٸ�..���� �ִ� ���.
	//
	if(m_bIsDead) 
		return DrawBoom(dTime);				//Ư���� �����ε� ���� �ʰ�, �׳ɻ��.

	//
	// ���� ��� �ִٸ�...���� ���.
	//
	return CPlane::Draw(dTime);      
}



/////////////////////////////////////////////////////////////////////////////
//
void CEnemy::Release()
{
	//N/A
}

 

/****************** end of file "Enemy.h" **********************************/

