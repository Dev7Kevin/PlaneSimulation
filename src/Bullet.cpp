// Bullet.cpp: implementation of the CBullet class.
//
//
////////////////////////////////////////////////////////////////////////////// 


#include "Bullet.h"

//////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//
CBullet::CBullet(
		LPDEVICE pDev,
		CB3Mesh* pMesh,
		UINT	uDmg,					//������.
		float	fFireVelo,				//�ѱ� �߻� ���ӵ�
		VECTOR3 vFireDir,				//�߻� ����.
		float	fMaxRange,				//�ִ� ��Ÿ�...
		
		SPRDATA*	pSpr,				//�Ѿ˿� ��Ʈ����Ʈ ������..  
		//pMesh, pSpr ���߿� �ϳ��� �־�� �Ѵ�.
		CCamera**	ppCam,				//ī�޶� ���� ������. 2004.06.19. 
		COLOR		Color,				//�Ѿ� �÷�. ��

		float		fRadius,			//�浹�� �ݰ�.
		LPANIINFO	pBoomAniInfo,		//���� �ִϸ��̼� ���� ����ü.04.06.23+ 
										//NULL �̸� ���õȴ�.
 
		VECTOR3 vPos,					//�߻� ��ġ.		
		VECTOR3 vRot/* = VECTOR3(0.0f, 0.0f, 0.0f)*/, 
		VECTOR3 vScale/*= VECTOR3(1.0f, 1.0f, 1.0f)*/,
		VECTOR3 vMoveVelo/* = VECTOR3(0.0f, 0.0f, 0.0f)*/,
		VECTOR3 vRotVelo/* = VECTOR3(0.0f, 0.0f, 0.0f)*/
 		 )
 :CUnit(vPos, vRot, vScale, vMoveVelo, vRotVelo, 
		fRadius, pBoomAniInfo,  
		pMesh, pDev)
 , m_uDmg(uDmg), m_fFireVelo(fFireVelo), m_vFireDir(vFireDir) 
 , m_Spr(*pSpr), m_ppCamera(ppCam), m_Color(Color)	 
{
	InitBullet(fMaxRange);		 
}


//////////////////////////////////////////////////////////////////////////////
//
CBullet::~CBullet()
{
	//N/A
}




//////////////////////////////////////////////////////////////////////////////
//
// �Ѿ� ��� ������ �ʱ�ȭ �޼ҵ�. : 'Missile' �� 'Reset' �� ���� �Լ�ó��.
//
void CBullet::InitBullet(float fMaxRange)
{
	//�߻� ������ ���� ����ȭ.
	D3DXVec3Normalize(&m_vFireDir, &m_vFireDir);

	//�ִ� ��Ÿ� ����.
 	SetMaxRange(fMaxRange);	

 
	//źȯ�� �ʱ� �߻� �ӵ�.. źȯ�� �� �ӵ����� ���� �����ϰ� �ȴ�.
	//�ѱ� �߻� �ӵ� = �߻� ���� * (�߻� �ӵ� + ��ü �ӵ�(*))
	m_vMoveVelo = m_vFireDir * m_fFireVelo;	 
	m_vRotVelo = VECTOR3(0, 0, 0);			//ȸ������ �ʿ� ����.


	//���װ� ��. : �̻��
	m_fResistance = fMaxRange/m_fFireVelo;		  
	m_fGravity = 0.00f; 
;

}




//////////////////////////////////////////////////////////////////////////////
//
// �Ѿ��� �ִ� ��Ÿ� ��� �޼ҵ�.
//
// �ִ� ��Ÿ��� '��밪'���� �Է� ������, 
// ���ÿ��� ���忡���� �̵��� ����� ȯ���մϴ�.
//
void CBullet::SetMaxRange(float fMaxRange)
{
	m_fMaxRange = fMaxRange;					//�ִ� ��Ÿ� (��Į�� ����)
	m_vMaxRange = fMaxRange * m_vFireDir;		//��Ÿ� (����)

	//�Ѿ� �̵� �Ÿ� ���� ����.(�ִ� ��Ÿ� ����)
	m_vMoveAccu = VECTOR3(0, 0, 0);		

}





//////////////////////////////////////////////////////////////////////////////
//
int CBullet::Move(float dTime/* =0 */)
{

	//------------------------------------------------------------------//
	// �����ϸ� : �� ��ǥ ��											//	
	//------------------------------------------------------------------//
	D3DXMatrixScaling(&m_mScale, m_vScale.x, m_vScale.y, m_vScale.z);

	//------------------------------------------------------------------//
	// ȸ�� ó�� : �� ��ǥ ��											//
	//------------------------------------------------------------------//
	//-> �ʿ� ����.


	//------------------------------------------------------------------//
	// �̵� ó��														//
	//------------------------------------------------------------------//
	//�ӵ� ���...
	//1. �߻� �ӵ����� ���� �پ���. (���װ� ����)
	//2. �̿� ���Ͽ�  '�߷�'�� ����ȴ�. (���� -Y ��)

	//�߻� ������ ������(������) * ���װ� ��ŭ �ӵ��� �����Ѵ�.
 	//... 
	
	
	//�߷� ����
	//...
	 	
	// �Ѿ� ���� �̵� �Ÿ� ���.: �Ÿ� = ���� �ӵ�  * �ð�.
	VECTOR3 dist = m_vMoveVelo * dTime;		

 	// �Ѿ� ��ġ (����.����) ���.
 	m_vPos += dist;

	// ��Ÿ� üũ������  �Ѿ� �̵��Ÿ� ����. 
	m_vMoveAccu += m_vMoveVelo*dTime;		 


	// ��Ÿ� ���� �ָ�����...'����'�� �ǹ̷� false �� �����Ѵ�. 
	// ����. 2004.06.21. 
  	if(D3DXVec3Length(&m_vMoveAccu) > D3DXVec3Length(&m_vMaxRange))
	{
 		return FALSE;
	}

	// �̵���� ����.
  	D3DXMatrixTranslation(&m_mTranslation, m_vPos.x, m_vPos.y, m_vPos.z);


	//------------------------------------------------------------------//
	// TM �����.														//
	//------------------------------------------------------------------//
	MATRIX temp;
	D3DXMatrixMultiply(&temp, &m_mScale, &m_mRotation);
	D3DXMatrixMultiply(&m_mTM, &temp, &m_mTranslation);


	return TRUE;
}




//////////////////////////////////////////////////////////////////////////////
//
int CBullet::Draw(float dTime/*=0*/)   
{
	//�ø��� ������ ������..�׸��� �ʴ´�. 2004.06.25.
	if(!IsVisible()) return TRUE;	


	// ��ϵ� �޽��� ������ �׳� 'true' ����. : ���� 2004.10.20.
	if(m_pMesh == NULL) return TRUE;		
	
	m_pDev->SetTransform(D3DTS_WORLD, &m_mTM);
	m_pMesh->Render(m_pDev);

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////////
//
void CBullet::Release()
{
	//N/A
}



/********************  end of file "bullet.cpp" ****************************/
