// Tornado.cpp: implementation of the CTornado class.
//
//
////////////////////////////////////////////////////////////////////////////// 

#include "Tornado.h"

 

//////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//

CTornado::CTornado(
		LPDEVICE pDev,
		CB3Mesh* pMesh,			//�Ѿ˿� �޽� ������.
		B3SPRITE* pSpr,			//�Ѿ˿� ��Ʈ����Ʈ ������.. ���߿� �ϳ��� �־�� �Ѵ�.
		CCamera** ppCam,		//ī�޶� ���� ������. 2004.06.19. ����.

		UINT	 Dmg,			//�Ѿ� ������.
		COLOR	 Color,			//�Ѿ� �÷�.
		float	fFireVelo,		//�ѱ� �߻� ���ӵ�
		VECTOR3 vFireDir,		//�߻� ����.  
		MATRIX	mFireRot,		//�߻��, ������ ȸ�����... 
		float	fMaxRange,		//�ִ� ��Ÿ�...

		float	 fRadius,		//�浹�� �ݰ�.

		VECTOR3 vPos,			//�Ѿ� �߻� ��ġ.		
		VECTOR3 vScale,			//�Ѿ� 'ũ��'
		VECTOR3 vRot/* = VECTOR3(0.0f, 0.0f, 0.0f)*/, 
		VECTOR3 vMoveVelo/* = VECTOR3(0.0f, 0.0f, 0.0f)*/,
		VECTOR3 vRotVelo/* = VECTOR3(0.0f, 0.0f, 0.0f)*/
 		 )
:CRailGun(pDev, pMesh, pSpr, ppCam, 
		  Dmg, Color, fFireVelo, vFireDir, fMaxRange, 
		  fRadius,		
		 vPos, vScale, vRot, vMoveVelo, vRotVelo ) 
{ 

	//������ �ڵ�. �ʱ�ȭ.
	m_vSpiralPos = VECTOR3(0, 0, 0); 
	m_fSpiralVal = 0.0f;
	//m_vMoveVelo = 0.8f;  

	m_mFireRot = mFireRot;		
	 

}//end of CTornado::CTornado()





//////////////////////////////////////////////////////////////////////////////
//
CTornado::~CTornado()
{ 
	Release();	
}




//////////////////////////////////////////////////////////////////////////////
//
// ����̵� �̵�ó�� : ���ÿ��� �����, ����� ��ȯ�Ѵ�.
//
int CTornado::Move(float dTime/* =0 */)
{
	// [���� �̵�] 
 	// ���������� ���� ��� �Ѵ�. (x, y �� �̵��� ��������!) 
	//
	m_fSpiralVal += D3DX_PI * 2 * dTime;  

	POS3 vPos(0, 0, 0);
	vPos.x = (float)sin(m_fSpiralVal) * 3.f;
	vPos.y = (float)cos(m_fSpiralVal) * 3.f;
	vPos.z =  0;  
 
 
	//[�����̵�]
	// �߻�� '�����'��  ȸ�������� ȸ����,
	// �߻�������� �̵�. 
	D3DXVec3TransformCoord(&vPos, &vPos, &m_mFireRot);
	m_vPos += vPos +  m_vFireDir * m_fFireVelo * dTime;

	//------------------------------------------------------------------
	// ��Ÿ� ���.
	// �θ� �޼ҵ带 ������� �ʰ� ���� �̵�ó���Ѵ�. 
	//------------------------------------------------------------------	
	m_vMoveAccu.z += m_fFireVelo * dTime;			//�̵��Ÿ� ����. 
	
	// ��Ÿ� ����� �ӵ������ ���� ��Į��� ����Ѵ�. 
	// CBullet::Move �� ���� ����.
	if( m_vMoveAccu.z > m_fMaxRange)  
 	{
		// �ִ� ��Ÿ��� �����, '����'�� �ǹ̷� false �� �����Ѵ�. 
		return FALSE;
	} 	


	// 
	// TM �����.														
	// 
	D3DXMatrixScaling(&m_mScale, m_vScale.x, m_vScale.y, m_vScale.z);
	D3DXMatrixTranslation(&m_mTranslation, m_vPos.x, m_vPos.y, m_vPos.z);

	MATRIX temp;
	D3DXMatrixMultiply(&temp, &m_mScale, &m_mRotation);
	D3DXMatrixMultiply(&m_mTM, &temp, &m_mTranslation);

	return TRUE;	  
	//return CGun::Move(dTime); -> �����մϴ�.
}




//////////////////////////////////////////////////////////////////////////////
//
int CTornado::Draw(float dTime/*=0*/)   
{
	// ����̵� �ڵ� �߰� 
	// ...
 
	
	// �������� �θ𿡼� ó��.	 
	return CRailGun::Draw(dTime);     
	 
}




//////////////////////////////////////////////////////////////////////////////
//
void CTornado::Release()
{	 
	// ����̵� �ڵ� �߰� 
	// ...	 
	 
}

 

/********************  end of file "Tornado.cpp" ****************************/
