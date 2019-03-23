// RailGun.cpp: implementation of the CRailGun class.
//
//
////////////////////////////////////////////////////////////////////////////// 

//#include "../../Engine/B3X.h"
#include "../../Engine/Ext/B3Yena_Ext.h"	//DLL ���� ���� �ʼ����. 

#include "RailGun.h"




//////////////////////////////////////////////////////////////////////////////
//Test.only. 2009.06.15
namespace GP
{
	extern BOOL _bBillBoardON; 
}
using namespace GP;


//////////////////////////////////////////////////////////////////////////////
// static data. 
int CRailGun::m_RailGunCnt = 0;

#ifdef RAILGUN_TEST_
int CRailGun::m_RailGunSNCnt = 0;
#endif


//////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//

CRailGun::CRailGun(
		LPDEVICE pDev,
		CB3Mesh* pMesh,			//�Ѿ˿� �޽� ������.
		SPRDATA* pSpr,			//�Ѿ˿� ��Ʈ����Ʈ ������.. ���߿� �ϳ��� �־�� �Ѵ�.

		CCamera** ppCam,		//ī�޶� ���� ������. 2004.06.19. ����.

  		UINT	 Dmg,			//�Ѿ� ������.
		COLOR	 Color,			//�Ѿ� �÷�.
		float	fFireVelo,		//�ѱ� �߻� ���ӵ�
		VECTOR3 vFireDir,		//�߻� ����.
		float	fMaxRange,		//�ִ� ��Ÿ�...

		float	 fRadius,		//�浹�� �ݰ�.
		
		VECTOR3 vPos,			//�Ѿ� �߻� ��ġ.		
		VECTOR3 vScale,			//�Ѿ� 'ũ��'
		VECTOR3 vRot/* = VECTOR3(0.0f, 0.0f, 0.0f)*/, 
 		VECTOR3 vMoveVelo/* = VECTOR3(0.0f, 0.0f, 0.0f)*/,
 		VECTOR3 vRotVelo/* = VECTOR3(0.0f, 0.0f, 0.0f)*/
 		 )
:CBullet(pDev, pMesh, Dmg, 
		 fFireVelo, vFireDir, fMaxRange,	  
		 pSpr, ppCam, Color,	
		 fRadius, NULL,	 
 		 vPos, vRot, vScale, vMoveVelo, vRotVelo 
		 )	
//, m_pSpr(pSpr), m_ppCamera(ppCam), m_Color(Color) -> CBullet ���� �ű� : 
{
		 
	//B3DSprite_RotateXXX() ==> ȸ���� �ʿ� ����.
	B3DSprite_Scaling(&m_Spr, m_vScale.x, m_vScale.y);						
 	B3DSprite_Translation(&m_Spr, m_vPos.x, m_vPos.y, m_vPos.z);  

	//���ϰ� ī��Ʈ ����. 
	++m_RailGunCnt;


//-----------------------------------------------------------------------------
#ifdef RAILGUN_TEST_ 

	//���ϰ� ��ȣ �ο�.
	m_RailGunSN = ++m_RailGunSNCnt;
 
	B3D_Error(NO_, "[info] ���ϰ� ����[tot:%d SN:%d now:%d] : \
				   vPos(%.3f %.3f %.3f) vFireDir(%.3f %.3f %.3f)\n\
				   �ִ� ��Ÿ� : fMax: %.3f vMaxRange(%.3f %.3f %.3f)",
			 m_RailGunSNCnt, m_RailGunSN, m_RailGunCnt,
 			 m_vPos.x, m_vPos.y, m_vPos.z, m_vFireDir.x, m_vFireDir.y, m_vFireDir.z,
			 fMaxRange, m_vMaxRange.x, m_vMaxRange.y, m_vMaxRange.z);
#endif
//-----------------------------------------------------------------------------

}//end of CRailGun::CRailGun()


//////////////////////////////////////////////////////////////////////////////
//
CRailGun::~CRailGun()
{

//-----------------------------------------------------------------------------
#ifdef RAILGUN_TEST_
	B3D_Error(NO_, "[info] ���ϰ� ����... [tot:%d SN:%d now:%d] : \
				   vPos(%.3f %.3f %.3f) vFireDir(%.3f %.3f %.3f)\n\
				   �ִ� ��Ÿ� : vMaxRange(%.3f %.3f %.3f)",
			m_RailGunSNCnt, m_RailGunSN, m_RailGunCnt,
 			m_vPos.x, m_vPos.y, m_vPos.z, m_vFireDir.x, m_vFireDir.y, m_vFireDir.z,
			m_vMaxRange.x, m_vMaxRange.y, m_vMaxRange.z);
#endif
//-----------------------------------------------------------------------------

	Release();
	
}




//////////////////////////////////////////////////////////////////////////////
//
int CRailGun::Move(float dTime/* =0 */)
{
	/* ���ϰ� �̵� �ڵ� �߰� */
	 
	//** �����ϸ� / ȸ��  ó�� ���ʿ�.
	
	// '�̵�' �� �θ� �޼ҵ忡�� ó��.
	return CBullet::Move(dTime);
}



 

//////////////////////////////////////////////////////////////////////////////
//
int CRailGun::Draw(float dTime/*=0*/)   
{
	/* ���ϰ� ������ �ڵ� �߰� */

	//�ø��� ������ ������..�׸��� �ʴ´�. 2004.06.25.
	if(!IsVisible()) return TRUE;	

 
	if(m_pMesh == NULL)				//�޽��� ������ ��������Ʈ�� ����Ѵ�.
	{
		//ī�޶� ����� ��� 
		//���������ͷν� ������ 'ī�޶�'�� ���� �����Ѵ�. 2004.06.19 
		//������ ������Ʈ...
		if(_bBillBoardON)  m_Spr.mRot = (*m_ppCamera)->GetBillMtx();			
		else D3DXMatrixIdentity(&m_Spr.mRot);										

			
		B3DSprite_Translation(&m_Spr, m_vPos.x, m_vPos.y, m_vPos.z, ON3D_); 		
 		B3DSprite_DrawFX(&m_Spr,  m_Color);												
 
		return TRUE;
	}
	
	// �޽��� ������ �װ͵� �׸���. : �� ���� 2004.10.20. 
	return CBullet::Draw(dTime);     
	 
}




//////////////////////////////////////////////////////////////////////////////
//
void CRailGun::Release()
{	 
	//���ϰ� ī��Ʈ ����. 
	--m_RailGunCnt;
}

 

/********************  end of file "RailGun.cpp" ****************************/
