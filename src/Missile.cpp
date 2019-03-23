// Missile.cpp: implementation of the CMissile class.
//
//
////////////////////////////////////////////////////////////////////////////// 

//#include "../../Engine/B3X.h"
#include "../../Engine/Ext/B3Yena_Ext.h"	//DLL ���� ���� �ʼ����. 

//#include "Unit.h"
#include "Plane.h"
#include "ShockWave.h"			 

#include "Camera.h"  
#include "Missile.h"
#include "SMoke.h"				//���� ���. 



 
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//
CMissile::CMissile(
		LPDEVICE	pDev,
		CB3Mesh*	pMesh,			//�̻��Ͽ� �޽� ������.

 		CCamera**	ppCam,			//ī�޶� ���� ������. 2004.06.19. ����.: �߻� ȭ�� ������.
  		UINT		Dmg,			//�̻��ϵ�����.
		
		float		fMaxFlyVelo,	//�̻����� �ִ� ���� �ӵ�.
		VECTOR3		vFlyDir,		//�̻����� ���� ����.	
 
		float		fMaxFuel,		//�ִ� ����. (���� �������� ��ȯ. 2004.11.12.) 
		
		CPlane*		pPlane,			//������ ����� ������.
		BOOL		bIsFired,		//�߻� �Ǿ���?

		B3SPRITE*	pSprFire,		//�̻��� ȭ���� ��Ʈ����Ʈ ������.. 
		VECTOR3		vSmokePos,		//'����', '����' ������ġ.(�𵨰��� ��밪)
 		VECTOR3		vSmokeScale,	//'ȭ��', '����' ������.
		COLOR		Color,			//ȭ�� �÷�.
		B3SPRITE*	psprSmoke,		//���� ��������Ʈ..
		
		CArrayList* pList,			//����ũ ����Ʈ..

		float		fRadius,		//�浹�� �ݰ�.
		LPANIINFO	pBoomAniInfo,	//���� �ִϸ��̼� ���� ����ü.04.06.23+ 
									//NULL �̸� ���õȴ�.

		LPANIINFO	pShockWaveAniInfo,		//���� �ִϸ��̼� ���� ����ü.04.06.23+ 
											// : NULL �̸� ���õȴ�.

		VECTOR3 vPos,			//�̻��Ϲ߻� ��ġ : �� ������� 
								//'�ϵ� ����Ʈ Hard Point' ��ġ�̴�.
								//(����� �𵨰��������� '��� ��ġ')

 		VECTOR3 vScale,			//�̻���'ũ��'
		VECTOR3 vRot/* = VECTOR3(0.0f, 0.0f, 0.0f)*/, 
 		VECTOR3 vMoveVelo/* = VECTOR3(0.0f, 0.0f, 0.0f)*/,
 		VECTOR3 vRotVelo/* = VECTOR3(0.0f, 0.0f, 0.0f)*/
 		 )
:CBullet(pDev, pMesh, Dmg, fMaxFlyVelo, vFlyDir, 
		 //0,		//�ִ� ��Ÿ�...-> ���� �������� ��ȯ 2004.11.12.
		 10000,		//�ִ� ��Ÿ�. 'CBullet::Move ������ �Ÿ� ������ �����ϱ� ���� �ִ밪�� �ִ´�.
					//���� �̻�����'��Ÿ�'�� ��������� �ʴ´�. 2004.11.12.
 		 //fMaxRange, 

		 pSprFire,	ppCam, Color,
		 fRadius, pBoomAniInfo,  
		 vPos, vRot, vScale, vMoveVelo, vRotVelo )
, m_pPlane(pPlane), m_bIsFired(bIsFired) 
, m_sprSmoke(*psprSmoke)	//ȭ�� ��������Ʈ.
, m_pSmokeList(pList)		//ȭ�� ����Ʈ..
, m_fMaxFlyVelo(fMaxFlyVelo)	//�ִ� �ӵ�..
, m_fRocketFuel(fMaxFuel)		//���� ����..
{
 	//
	// �߻� ��..
	//
	m_bIsReleased = FALSE;	//�߻� ��ư�� �����, �ϵ�����Ʈ���� �и�����,
							//'��ȭ', '�߻�' �� ������ �����Ѵ�.
	m_bIsReleasedAst = FALSE;
	m_fRelaseLength = 0.0f;
	m_vReleasePos = VECTOR3(0, 0, 0);
	m_vReleaseTimePlaneDirVelo = VECTOR3(0, 0, 0);

	m_fCurrVelo = 0.0f;

	// ���� ����.
	m_vUp = m_vHorz = VECTOR3(0, 0, 0);		//=> ������ �Ŀ� '������ �����Ұ���'
		

	//
	// �߻� ��ġ ���.
	//
	m_vHardPt = vPos;		//m_vHardPt -�߻���ġ : �ϵ� ����Ʈ ����. 
							//			�߻�Ǳ� �������� �� ���� ��� �� ���̴�.
							//m_vPos - �̵��ϸ鼭 �����Ǹ� �߻� ���� �̰��� 
							//			���� ��ǥ�� ����Ѵ�.
	SetHardPointPos();		//�̻����� ��ġ�� ������� �ϵ�����Ʈ ��ġ���� 
							//��밪���� ����Ѵ�.   
							//�߻�� ���Ŀ��� �ڽ��� ��ǥ�� ������..
	
	//
	// ����ũ ��ġ ��� (�𵨰��� �����ġ) : �� ��ǥ(��밪) * ����� ������ 
	// ���߿� ��½� '�����'�� ���� ��ġ�� �����Ͽ� ����Ѵ�. : ::Move ����.
	//
	m_vSmokePos = vSmokePos;
 	m_vSmokePosW = m_vSmokePos;
	m_vSmokeScale = vSmokeScale;
	
 	SetSmokePos(&vSmokePos);


	//
	// ���� ����. 
	//
	//'����' ȭ�� ���� ������� ...�̻��� �ν��� 'ȭ��'�� x 2
	m_fBoomSphereRaidus = m_vSmokeScale.x*2;	
	//m_vBoomShowTime = 3000;		//'3��'.. -> CMissile::SetDead() ���� ����.
	m_bFadeOut = NO_;	
	m_fAlpha = 1.0f;		

	//
	// ����� ����. : �ϴ�, �̻��� �ʿ��� ������.
	//
	m_pShockWave = new CShockWave(
		pDev, 
		m_fBoomSphereRaidus*0.1f, // �Ѱ��� ũ�� / �β��� �̻��� ���Ŀ� ������.
		m_fBoomSphereRaidus*0.1f/2,	
		m_Color,					//'�̻��� ���ı�' ����� �����ϰ�..
		pShockWaveAniInfo,			
		VECTOR3( 0, 0, 0));
	ASSERT(m_pShockWave != NULL);


	// ���� ����. 04.11.11.
	m_pTarget = NULL;
	m_bHommingON = NO_;  

	m_fMaxAccelSpeed = 0.0f;	//�ִ� ���� ���ӵ�.

}//end of CMissile::CMissile()




//////////////////////////////////////////////////////////////////////////////
//
CMissile::~CMissile()
{
	SAFE_DELETE(m_pShockWave);	

}







//////////////////////////////////////////////////////////////////////////////
//
// ���� �޼ҵ�.
//
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//
// void CMissile::SetSmokePos(..)
//
// �̻����� �Ĺ濡 ����� ����ũ - �Ҳ� & ���� ���� ��ġ��ǥ���� ����Ѵ�.
//
void CMissile::SetSmokePos(VECTOR3* pvSmokePos/*=NULL*/)
{
	VECTOR3 vSmokePos(0, 0, 0);
	
	//��ó�� �ϰ��, '���' �ʱ�ȭ.
	if(pvSmokePos != NULL)
	{
		m_vSmokePos = *pvSmokePos;
	}

	//
	// '����'&'�Ҳ�' ���� ��ġ ���. 
	//
	// �켱 -z ���� �����Ѵ�. ���� ������Ʈ ����.�Ѥ�;;
 	// -> �̻����� '�Ĺ�' ���� '����'�� �ʱ� ��ġ��ŭ ������ ���Ÿ� ���. 
 	VECTOR3 vPos = -(m_vSmokePos.z * m_vFireDir);	
 
	//
	// ���� ��ǥ ���.: �̻��Ͽ����� �����ġ-�̻��� �Ĺ�-�� �ǰڴ�.
	//
	m_vSmokePosW = m_vPos + vPos;					 
	 
 	//
	// ȭ�� ��������Ʈ ���. 
	//
	//B3DSprite_RotateXXX() ==> ȸ���� �׸����� ���� �� ����  
	B3DSprite_Scaling(&m_Spr, m_vSmokeScale.x, m_vSmokeScale.y); 
 	B3DSprite_Translation(&m_Spr, m_vSmokePosW.x, m_vSmokePosW.y, m_vSmokePosW.z, ON3D_);		 
	 
}
 



//////////////////////////////////////////////////////////////////////////////
//
// void CMissile::SetFire()
//
// �������� '�߽�'�� ���. �߻� ���⺤���� ������, 
//  �⺻���� �̻��� �����͸� �����Ѵ�.
//
void CMissile::SetFire()
{ 
 	//------------------------------------------------------------
	//������ �߻�� ���̸� ����. 2004.05.21+
	if(m_bIsReleased == TRUE) return;		

	// �ϵ� ����Ʈ���� �и�!!		2004.06.21+
	m_bIsReleased = TRUE;	
 
	//------------------------------------------------------------

	//
	// �̻��� ȸ���� ���� : �߻��� �̻����� �� ȸ���� ���� �����̴�.
	//
	m_mRotation = m_pPlane->GetRotMtx(); 
	
	//
	// �߻� ������ ���� ����ȭ. => �̻����� ���� ����� ����. : 2004.06.22
	//
	m_vFireDir = m_pPlane->GetNView();
	D3DXVec3Normalize(&m_vFireDir, &m_vFireDir);		

	//
	// ���⼺�� ���.	2004.06.21+
	// �߻� ���� - �ü����� �� �̹� �˰� �ִ�.
	//
 	m_vUp = m_pPlane->GetUp();
	m_vHorz = m_pPlane->GetHorz();

	//
	// '������ Ÿ��' ������ ����� ����+�ӵ� ���. 2004.06.21+
	//
	m_vReleaseTimePlaneDirVelo = m_pPlane->GetCurrDirVelo();	//<-����� �̵�����+�ӵ� 

	//
	// źȯ�� �߻���� & �ӵ�  ����...
	//
	// �̻����� '�ν���' ��ȭ�� ���� �ӵ��� �����ϰ� �Ǹ� '���ϰ�'���� 
	// ���ݴ��� �����̴�.
 	// ���� �̻����� �ӵ���  �������� ������� �ӵ��� ����(���� �ν��Ͱ� �۵�
	// ���� �ʰ� �ִ�.)
	//
	m_fCurrVelo = 0.0f; 
 	m_vMoveVelo = m_vReleaseTimePlaneDirVelo;	//�߻� �ӵ� = '����' ������  ����� �ӵ�*���� 
 
	//
	// �ִ� ��Ÿ��� '��밪'���� �Է� ������, 
	// ���ÿ��� ������ '���� ��ǥ'��  ȯ���մϴ�.
	//
 	SetMaxRange(m_fMaxRange);

	//
	// ȭ�� ��ġ ����.
	//
	//SetSmokePos();
	 
}		





//////////////////////////////////////////////////////////////////////////////
//
// �̻��� ����� �����ð����� '����'�� �����Ѵ�.
//
//
void CMissile::PutSmoke(float dTime)		
{
 	//
	// ���� ���� -> ����Ʈ�� ���.
	//
	CSmoke* pSmoke = new CSmoke(	
						m_pDev, 
						NULL, 
			   			&m_sprSmoke, 
						m_ppCamera, 
						1200, //3500, �����ð� ��� 
						0,
 						//m_Color,						 
 						COLOR(0.8f, 0.8f, 0.8f, .5f),	//����, �������...	
						VECTOR3(m_vSmokePosW.x, m_vSmokePosW.y, m_vSmokePosW.z),
						//m_vSmokeScale*0.4f				 //ȭ�� �������� 60% ũ��� ����. 
						m_vSmokeScale*((rand()%40+25)*0.01f) //0.25f~0.64f ���̿��� ũ�⸦ ����... 
  						);
	if(pSmoke == NULL) return;		//�޸� Ȯ�� ���н�..�׳� �Ѿ.
									//GPF �޼��� �ڽ� ���ٴ� �����ʳ�?
	m_pSmokeList->Add(pSmoke);		//���� ����Ʈ�� ���.
	
}







//////////////////////////////////////////////////////////////////////////////
//
// void CMissile::SetHardPointPos()
//
// �̻����� ��ġ - Hard Point �� ����Ѵ�.
// �� ��ġ���� '�߻� ��'������ ����ϰ� �ȴ�.
//
void CMissile::SetHardPointPos()
{
	// ��� ����. 2009.07.03.

	//����� ��ȯ ��� ��� : '���ɸ�' ���� ������� ���� ������� �ʰ�,
	//�̻��� �ڽ��� ���� ����Ѵ�. -> �׷��� �پ��� ũ���� �̻�����  ����� �ִ�. 
	//	 
	//2004.11.15. '�̻��� ���� ī�޶�'�� ���� ��� ����.
	m_mRotation = m_pPlane->GetRotMtx();				
	m_mTranslation = m_pPlane->GetTransMtx();			
	m_mTM = m_mScale * m_mRotation *  m_mTranslation;	//�����ϸ��� �����ϰ� �������� ����⿡�� ����.


	m_vUp = m_pPlane->GetUp();				//���� ������ ���.
	m_vNView = m_pPlane->GetNView();		// ���� ���� ���� ���.

	//�̻��� ��ġ ���.
	//ex) ������ ���� 3.0F(vHorz) �ϴ� -1.0f �Ʒ� (vUp), ���� 2.0f (vView) ������.
	VECTOR3 vPos = m_vHardPt.x*m_pPlane->GetHorz()+ 
				   m_vHardPt.y*m_pPlane->GetUp()+
				   m_vHardPt.z*m_pPlane->GetNView();


	// �̻��� ��ġ ����.
	m_vPos.x = (m_mTM._41 += vPos.x);	
	m_vPos.y = (m_mTM._42 += vPos.y);  
	m_vPos.z = (m_mTM._43 += vPos.z);
 

	
}





//	****  �̻��� �ޱ� �⺻ �ڵ�. ����� ���ƿ�~  *****
//	{
//		MATRIX mTM 
//		mTM = pHero->GetTM();						//����� TM ���.
//		VECTOR3 vUp = pHero->GetUp();			//���� ������ ���.
//		VECTOR3 vView = pHero->GetNView();		//���� ���� ���� ���.
//		VECTOR3 vPos = -1*vUp + 2*vView;			//�ϴ� -1.0f �Ʒ�, ���� 2.0f ������.
//		 
//		mTM._41 += vPos.x;	mTM._42 += vPos.y; mTM._43 += vPos.z;
//		 
//		g_pd3dDevice->SetTransform(D3DTS_WORLD, &mTM);
//		pMissile->Render(g_pd3dDevice);
//
//	}
//






//////////////////////////////////////////////////////////////////////////////
//
// �̻��� �̵� �޼ҵ� 
//
// ũ�� 3�ܰ�� ������ ����.
// 1�ܰ� : ������� '�ϵ� ����Ʈ' �� �����Ǿ� ������ �Բ� �����̴� ��� : "MOVE" 
//			=> CMissile::MoveOnHardPoint()
// 2�ܰ� : �߻��ư�� ���� '�ϵ� ����Ʈ'���� '�������� �ܰ�' : "RELEASE"
//			=> CMissile::MoveReleaseTime()
// 3�ܰ� : ������ ��, '�ν���'��  ��ȭ�� ���� '����' �ϴ� �ܰ� : "FLY"
//          => CMissile::MoveFly()
//
int CMissile::Move(float dTime/* =0 */)
{
	/* �̻��� �̵� �ڵ� �߰� */

 	//------------------------------------------------------------------------
 	// �߻� ���� '����' ��...
	// => ���� - '����'- ������ ������ �� �����Ƿ� ���� ���� �˻��Ѵ�.
	if(m_bIsFired == YES_)		
	{
		return MoveFly(dTime);
	}


	//------------------------------------------------------------------------
 	// �߻� ��ư ������ '�ϵ�����Ʈ'���� �и��Ǿ��� ���...
 	// 
	if(m_bIsReleased == YES_)		//�и��� �Ǿ�����...
	{
		return MoveReleaseTime(dTime);
	}
	 

	//------------------------------------------------------------------------
	// �߻� ���� �ʰ�, ���� �����-Hard Point �� �����Ǿ� ������...
	//
 	return MoveOnHardPoint(dTime);
 	 
}





//////////////////////////////////////////////////////////////////////////////
//
// int CMissile::MoveFly() 
//
int CMissile::MoveFly(float dTime)
{

	// ���� Ÿ�� ������ ���� ���̰ų�, �����Ǿ��ٸ�...
	// ���̻� ���� ���� �ʰ�, ���� �������� ��� ���ư���.
	if(m_pTarget == NULL)
	{
		m_bHommingON = FALSE;
	}
	else
	{
		if(m_pTarget->IsDead()) 
			m_bHommingON = FALSE;
	}




	//------------------------------------------------------------------------ 
	// �̻����� ���� ó�� �ڵ�...
	// �̻����� �ӵ��� 30m/s �̻��϶�, ������ �����Ѵ�.
	// �ٷ� ������ �ϸ�..��� ����. �Ѥ�;;
	// '�ӵ�' �� �־�� ������ Ʋ �� �ִ�! ��� �����Ͽ�.
	//
	if( D3DXVec3Length(&m_vMoveVelo) > 30.0f) 
	{											 
		m_bHommingON = YES_;					 
	}

	MoveHomming(m_bHommingON, dTime);		//���� ��� ON/OFF!


	//------------------------------------------------------------------------ 
	// ���� �ν��� ���� �˻�...���� �ӵ���ŭ �Ҹ�, ���ΰ� ���ٸ� ����..
	// 
	m_fRocketFuel -= m_fMaxFlyVelo*0.1f*dTime; //�ִ�ӵ��� 10%/sec �� ���� �ʸ�.
	if(m_fRocketFuel <= 0.0f)					//�����ϱ� ���� FALSE ����...
	{
#ifdef _LOGFILE_ON_
		B3D_Error(NO_, "[info] CMissile::MoveFly():\t\t�̻���(0x%x) �ִ� ��Ÿ� �Ѿ. ���� ����.: dTime:%f vPos(%.3f %.3f %.3f)", 
			this, dTime, m_vPos.x, m_vPos.y, m_vPos.z);
#endif 

		//�ٷ� �������� �ʰ�..'����' �ִϸ� �����Ѵ�..
		SetDead(); 

		return TRUE; //FALSE; 	
	}
 
	//------------------------------------------------------------------------ 

	//�̻��� ȭ�� ��ġ ����.
	SetSmokePos();
 

 	//�̻��� ���� ����.
	PutSmoke(dTime);

	return TRUE;

}

 
//////////////////////////////////////////////////////////////////////////////
//
// int CMissile::MoveReleaseTime() 
//
//
int CMissile::MoveReleaseTime(float dTime)
{
	//'�Ʒ�' �� '�и�' �ȴ�.
	VECTOR3 vDrop = -m_vUp * 2.0f * dTime;		//2.0m/s �� �ӵ��� ����..	
	m_vReleasePos += vDrop;						//���ϵ� �Ÿ� ����.

	VECTOR3 vView = m_pPlane->GetNView() * 2.0f * dTime;
	m_vRelAstPos += vView;


	//�̻��� ���� 
	m_vPos += vDrop;
	m_vAstPos += vView;

	// ������, ������� ���ӵ��� �̻����� '����'���ӵ��� 
	// ������ ��ģ��.(�߻��, ������� '�ӵ�' �� ����) 
	m_vPos += m_vReleaseTimePlaneDirVelo*dTime;		

	m_vAstPos += m_vReleaseTimePlaneDirVelo * dTime;


	//����� ��۰ 1.5m ��ŭ ��������..�ν��͸� ��ȭ �Ѵ�. => ��������!!
	if( D3DXVec3Length(&m_vReleasePos) > 1.5f)		
	{ 	
 		m_bIsFired = YES_;
 		SetSmokePos();		// ȭ�� ��ġ ����.
	}

	if( D3DXVec3Length(&m_vRelAstPos) > 1.5f)
	{
		m_bIsFired = YES_;
		SetSmokePos();
	}

	//��ȯ ��� ����.
	//D3DXMatrixScaling(&m_mScale, m_vScale.x, m_vScale.y, m_vScale.z);
	D3DXMatrixTranslation(&m_mTranslation, m_vPos.x, m_vPos.y, m_vPos.z);
	D3DXMatrixTranslation(&m_mTranslation, m_vAstPos.x, m_vAstPos.y, m_vAstPos.z);

	MATRIX temp;
	D3DXMatrixMultiply(&temp, &m_mScale, &m_mRotation);
	D3DXMatrixMultiply(&m_mTM, &temp, &m_mTranslation);


	//�̻��� ȭ�� ��ġ ����.
	//SetSmokePos();

	return TRUE;
}



//////////////////////////////////////////////////////////////////////////////
//
// int CMissile::MoveOnHardPoint() 
//
// �߻� ���� �ʰ�, ���� �����-Hard Point �� �����Ǿ� ������...
// ������� '��ȯ��Ŀ�' ���󰣴�. ���ϱ�  �̻����� ��Ÿ��� '����' �ؾ� �մϴ�..
//
int CMissile::MoveOnHardPoint(float dTime)
{
	//�߻� ������ ���� ����ȭ.: �̻����� �������. 2004.06.22.
	m_vFireDir = m_pPlane->GetNView();
 	D3DXVec3Normalize(&m_vFireDir, &m_vFireDir);
	SetMaxRange(m_fMaxRange);

	
	// �ϵ� ����Ʈ ��ġ�� ���� ������ǥ ���.@fix. 2009.07.03.
	//SetHardPointPos();

	 
	
	//�ϵ�����Ʈ �̵����.
	MATRIX mHardPtTrans;
	D3DXMatrixTranslation(&mHardPtTrans, m_vHardPt.x, m_vHardPt.y, m_vHardPt.z);


	// ȸ���� �̵��� '�����'�� �����ϰ� ó���Ѵ�.(�� ȸ����)..
	m_mRotation = m_pPlane->GetRotMtx();
	m_mTranslation = m_pPlane->GetTransMtx();
	 
	//��� ����.
	m_mTM = m_mScale * mHardPtTrans * m_mRotation * m_mTranslation;		

	
	//��ġ ����.
	m_vPos.x = m_mTM._41;	m_vPos.y = m_mTM._42;	m_vPos.z = m_mTM._43;
	 

	return TRUE;	

}











//////////////////////////////////////////////////////////////////////////////
//
//  int CMissile::Draw(float dTime)   
//
//
int CMissile::Draw(float dTime/*=0*/)   
{

	//----------------------------------------------//
	// �ø��� ���� �޼ҵ忡�� �����ϹǷ� �ּ�ó��	//	
	//----------------------------------------------//
	//if(!IsVisible()) return TRUE;	//�ø��� ������ ������..�׸��� �ʴ´�. 2004.06.25. 

 
	/* �̻��� ������ �ڵ� �߰� ... */
	if(m_bIsDead)						//�̻����� �����ϰų� �浹�� '�����ٸ�'....
		return DrawBoom(dTime);			//���� ��Ŵ....���Ĵ� �ø����� �ʴ´�. 

	
	//�̻��� ȭ�� ���. 
	//+ ȭ�� �ø�.: �̻��Ͽ����� 2���̻��� �ø��˻簡 �߻��ϰ� �ȴ�..�ٿ��� �մϴ�.!! 2004.06.25.
	// CUnit::Draw(float dTime)  �� ����� ���� �ϳ��� ����� �ɼ� �ִ�.
	if(m_bIsFired && IsVisible())	
	{
 		//�̻��� ȭ��(Flare) ���. 
		m_Spr.mRot = (*m_ppCamera)->GetBillMtx();				//������ ��� ���. 
  		B3DSprite_Translation(&m_Spr, m_vSmokePosW.x, m_vSmokePosW.y, m_vSmokePosW.z, ON3D_); //ȭ�� �̵�..
 		B3DSprite_DrawFX(&m_Spr,  m_Color); 
	}

	//�̻��� �׸���.
	return CBullet::Draw(dTime);     
}


/* ȭ�� ��¦��¦,...�ڵ�..���� �̻��� ����.
int CMissile::Draw(float dTime)   
{
	// �̻��� ������ �ڵ� �߰� 
	CBullet::Draw(dTime);     
	
	if(m_bIsFired)
	{
 		//�̻��� ȭ�� ���. 
		m_Spr.mRot = (*m_ppCamera)->GetBillMtx();				//������ ��� ���. 
//		B3DSprite_Scaling(&m_Spr, m_vSmokeScale.x, m_vSmokeScale.y);
 		B3DSprite_Translation(&m_Spr, m_vSmokePosW.x, m_vSmokePosW.y, m_vSmokePosW.z, ON3D_); //ȭ�� �̵�..
 		B3DSprite_DrawFX(&m_Spr,  m_Color); 

//		static BOOL bShow = YES_;
//		if(bShow ^= 1)
//		{ 
//			B3DSprite_Scaling(&m_Spr, m_vSmokeScale.x*0.5f, m_vSmokeScale.y*0.5f);
//			B3DSprite_Translation(&m_Spr, m_vSmokePosW.x, m_vSmokePosW.y, m_vSmokePosW.z, ON3D_); //ȭ�� �̵�..
// 			B3DSprite_DrawFX(&m_Spr,  COLOR(1.0f, 1.0f, 0.3f, 1.0f)); 
//		}

	}

	//�̻��� �׸���.
	return true;
}

*/


//////////////////////////////////////////////////////////////////////////////
//
void CMissile::Release()
{
	//N/A
}






//////////////////////////////////////////////////////////////////////////////
//
// BOOL CMissile::DrawBoom() 
// 
// ���� �ִϸ��̼� ������. 
// �̰��� ������ �޸𸮸� �����ϱ� ���� FALSE �� ���ϵȴ�.
//
// CUnit::DrawBoom �������̵�. 
// 
// : �̻��Ͽ����� '����' �����Ӽ��� ���ؼ��� �ƴ϶�, 
// ���� ȭ���� ����� ���� �ִϸ��̼��� ����ȴ�.
//
BOOL CMissile::DrawBoom(float dTime)	
{
	//
	// ���� �� ũ�� �ø���.
	//
	if( m_bFadeOut == NO_)										//'������ ��'...
	{
		if( m_fBoomSphereRaidus >= m_BoomAniInfo.vScale.x/2)	//�ִ� ������� ũ��...'x'�ุ Ȯ���ص� ����.
		{
			m_bFadeOut = YES_;
		}

		m_fBoomSphereRaidus += 100.0f * dTime;				 
		m_vScale = VECTOR3(1,1,1)*m_fBoomSphereRaidus*2;
	}
	else							//'������� ��'...
	{
		//
		// ���İ� ����.-> 2004.06.30++. '��ũ ���̺�'�� ���� �ڵ�� ��ȯ.!!
		//
		//  m_fAlpha -= 0.3f*dTime;
		//
		//	if(m_fAlpha<0.0f)	//���İ� 0 ����->'������' �������..��ü ������ ���� FALSE ����.
		//	{
		//		return FALSE;
		//	}
	}



	//----------------------------------------------//
	// �ø� ���⼭..								//	
	//----------------------------------------------//
	// �ð����� ȿ���� ���� �ø� �Ѿ... 2004.06.25+
	//if(!IsVisible()) return TRUE;	 <-- �ּ�ó�� �մϴ�.(�������� ������ ȿ���� ����..)


	//----------------------------------------------//
	// ��ũ ���̺� 								//
	//----------------------------------------------//

	//1. '�̻��� ���ı�'�� �߽����� �� ���� �ִ� ó�� �ڵ�.
	//   '�����'�� '���ı�'�� ���¿� ���� ���ϰ� �ȴ�. 
	//	m_pShockWave->SetFadeOutState(m_bFadeOut);
	//	m_pShockWave->SetAlpha(m_fAlpha);
	//	m_pShockWave->Draw(dTime);

	//2. '�����'�� �߽����� ���� �ִϸ� ó���� ����� �ڵ�. 
	//    : ����İ� �������, ��ü�� �����ϱ� ���� false ����..
	if(m_pShockWave->Draw(dTime) == FALSE) 
		return FALSE;					//������ ��...

	//��ũ ���̺��� '���İ�'�� ���ı��� �����Ѵ�.
	m_fAlpha = m_pShockWave->GetAlpha();						
	m_bFadeOut = m_pShockWave->GetFadeOutState();


	//----------------------------------------------//
	// ���� ��										//
	//----------------------------------------------//

	// ȸ������ ī�޶��� 'ȸ�����'�� ��´� : �⺻���� '�Ϲ�' ������..
	m_BoomAniInfo.pSpr[m_BoomAniInfo.Frm].mRot = (*m_BoomAniInfo.ppCamera)->GetBillMtx();

	//�⺻ ���� ȭ��...
	B3DSprite_Scaling(&m_BoomAniInfo.pSpr[m_BoomAniInfo.Frm], m_vScale.x, m_vScale.y);
	B3DSprite_Translation(&m_BoomAniInfo.pSpr[m_BoomAniInfo.Frm], m_vPos.x, m_vPos.y, m_vPos.z, ON3D_);

	//B3DSprite_DrwFX2 !! 
	B3DSprite_DrawFX2(&m_BoomAniInfo.pSpr[m_BoomAniInfo.Frm], m_Color, m_bFadeOut, m_fAlpha, NO_);  

	//�߰� ���� ȭ��.
	B3DSprite_Scaling(&m_BoomAniInfo.pSpr[m_BoomAniInfo.Frm], m_vScale.x*0.8f, m_vScale.y*0.8f);
	//B3DSprite_DrwFX2 !! 
	B3DSprite_DrawFX2(&m_BoomAniInfo.pSpr[m_BoomAniInfo.Frm], COLOR(1.0f, 1.0f, 1.0f, 1.0f), m_bFadeOut, m_fAlpha, NO_);


	return TRUE;
}



//////////////////////////////////////////////////////////////////////////////
//
// void CMissile::SetDead() 
//
// CUnit::SetDead �������̵�.
//
void CMissile::SetDead()
{
	CUnit::SetDead();			//�θ�� ����...
 
	//�̻��� �߰� ������ ����.
	m_BoomShowTime = timeGetTime() + 1000;	//�ִϸ��̼� ���� �ð�...


	//����� �ִϸ��̼� ����. 2005.06.29++
	m_pShockWave->SetDead();
	m_pShockWave->SetPos(m_vPos);			//<- CUnit::SetPos()

}


//////////////////////////////////////////////////////////////////////////////
//
// BOOL CMissile::CheckCollision(...) 
//
// �浹 �Լ� �������̵�. 
// �̻����� '���� ��'�� ���� �ð��� �������� Ŀ���� �Ǹ� �̿����� �浹������
// 'Ŀ����' �Ѵ�.
//
BOOL CMissile::CheckCollision(CUnit* pTarget, float fTargetRadius/*=0.0f*/)
{
	if(m_bIsDead)		//���� ���� ���̸�...
	{
		//"���� ���� �浹���� �̹��� ũ��" ��ŭ �� �̻����� �浹 �ݰ��� �ø���....
		m_fCollRadius =  
			m_BoomAniInfo.pSpr[m_BoomAniInfo.Frm].mScale._11/2.5f ;  //x �� ������ 40% ��ŭ.. 
	}

	// �׸��� �Ϲ� �浹�˻� ����...
	return CUnit::CheckCollision(pTarget, fTargetRadius);

}






//////////////////////////////////////////////////////////////////////////////
// 
// void CMissile::LockOn(CUnit* pUnit) 
//
// ��ǥ  Lock-On ��Ű��
// 2004.11.11.
//
void CMissile::LockOn(CUnit* pEmy)	
{
	m_pTarget = dynamic_cast<CPlane*>(pEmy);

#ifdef _LOGFILE_ON_
	B3D_Error(NO_, "[info] CMissile::LockOn : �̻���(0x%08x)  ��ǥ(0x%08x) Lock-On!!",
		this, pEmy);
#endif
}







//////////////////////////////////////////////////////////////////////////////
// 
// void CMissile::Homming()
//
// �̻��� �����ϱ� : ��ǥ�� ���� ȸ�� ��� ���. p
//                   �̻����� '��'�� ������ �ٲٴ� ���� �� �ӹ���.
//

int CMissile::CalHommingRotMtxAst(float dTime)
{
	//1. ���� ��ġ���� '��ǥ' �� ���� ���͸� ���Ѵ�.
	VECTOR3 vNewView =m_vPos - m_pTarget->GetPos();
	//VECTOR3 vNewView = m_pTarget->GetPos() - m_vPos;
	D3DXVec3Normalize(&vNewView, &vNewView);

	//2. �� Up, Horz ���� ���ϱ�.
	VECTOR3 vNewUp = m_vUp;

	VECTOR3 vNewHorz;
	D3DXVec3Cross(&vNewHorz, &vNewUp, &vNewView);	//���� ���� ���.
	D3DXVec3Normalize(&vNewHorz, &vNewHorz);

	D3DXVec3Cross(&vNewUp, &vNewView, &vNewHorz);	//��� ���� �� ���.
	D3DXVec3Normalize(&vNewUp, &vNewUp);			//3���� ���� ����ȭ!!

	//3.������ ���� �� �ະ ���� ������ ��ķ� �����Ѵ�.
	MATRIX mNewTM;
	D3DXMatrixIdentity(&mNewTM);

	mNewTM._11 = vNewHorz.x;	mNewTM._12 = vNewHorz.y;	mNewTM._13 = vNewHorz.z;
	mNewTM._21 = vNewUp.x;		mNewTM._22 = vNewUp.y;		mNewTM._23 = vNewUp.z;
	mNewTM._31 = vNewView.x;	mNewTM._32 = vNewView.y;	mNewTM._33 = vNewView.z;

//	mNewTM._11 = vNewHorz.x;	mNewTM._12 = vNewUp.x;		mNewTM._13 = vNewView.x;	
//	mNewTM._21 = vNewHorz.y;	mNewTM._22 = vNewUp.y;		mNewTM._23 = vNewView.y;	
//	mNewTM._31 = vNewHorz.z;	mNewTM._32 = vNewUp.z;		mNewTM._33 = vNewView.z;	
	
	


	//������ �� ����.
	//mNewTM  = m_mScaling * mNewTM;		

	//�̵��� ����.
	//mNewTM._41 = m_vPos.x;	mNewTM._42 = m_vPos.y;	mNewTM._43 = m_vPos.z;


	//------------------------------------------------------------------//
	//4.�̻����� '�� ��ȯ ���'���� '�̻���'�� ���� ȸ�� ����� ���Ѵ�.	//
	//------------------------------------------------------------------//
	// i)   NewTM = TM * R
	// ii)  TM(-1) * NewTM = R
	// iii) TM(T) * NewTM = R
	//	MATRIX TransTM;  
	//	D3DXMatrixTranspose(&TransTM, &m_mTM);
	//
	//�޽� ������..�� ����� �ǰ���.
	m_mRotation = mNewTM ;		


	//5. �߻� ���⵵ ����..
	m_vFireDir =  m_pTarget->GetPos() - m_vPos;
	D3DXVec3Normalize(&m_vFireDir, &m_vFireDir);


	return TRUE;
}

int CMissile::CalHommingRotMtx(float dTime/*=0*/)
{
	//0. ������ ��ǥ�� ���ٸ�, ���� ����� ���� ������ ������ ����, �����Ѵ�.
	//  ..��� �ؾ� ������...�׳� �Ѿ...
	if(m_pTarget == NULL) return FALSE;

	//1. ���� ��ġ���� '��ǥ' �� ���� ���͸� ���Ѵ�.
	VECTOR3 vNewView =m_vPos - m_pTarget->GetPos();
	//VECTOR3 vNewView = m_pTarget->GetPos() - m_vPos;
	D3DXVec3Normalize(&vNewView, &vNewView);

	//2. �� Up, Horz ���� ���ϱ�.
	VECTOR3 vNewUp = m_vUp;

	VECTOR3 vNewHorz;
	D3DXVec3Cross(&vNewHorz, &vNewUp, &vNewView);	//���� ���� ���.
	D3DXVec3Normalize(&vNewHorz, &vNewHorz);

	D3DXVec3Cross(&vNewUp, &vNewView, &vNewHorz);	//��� ���� �� ���.
	D3DXVec3Normalize(&vNewUp, &vNewUp);			//3���� ���� ����ȭ!!

	//3.������ ���� �� �ະ ���� ������ ��ķ� �����Ѵ�.
	MATRIX mNewTM;
	D3DXMatrixIdentity(&mNewTM);

	mNewTM._11 = vNewHorz.x;	mNewTM._12 = vNewHorz.y;	mNewTM._13 = vNewHorz.z;
	mNewTM._21 = vNewUp.x;		mNewTM._22 = vNewUp.y;		mNewTM._23 = vNewUp.z;
	mNewTM._31 = vNewView.x;	mNewTM._32 = vNewView.y;	mNewTM._33 = vNewView.z;

//	mNewTM._11 = vNewHorz.x;	mNewTM._12 = vNewUp.x;		mNewTM._13 = vNewView.x;	
//	mNewTM._21 = vNewHorz.y;	mNewTM._22 = vNewUp.y;		mNewTM._23 = vNewView.y;	
//	mNewTM._31 = vNewHorz.z;	mNewTM._32 = vNewUp.z;		mNewTM._33 = vNewView.z;	
	
	


	//������ �� ����.
	//mNewTM  = m_mScaling * mNewTM;		

	//�̵��� ����.
	//mNewTM._41 = m_vPos.x;	mNewTM._42 = m_vPos.y;	mNewTM._43 = m_vPos.z;


	//------------------------------------------------------------------//
	//4.�̻����� '�� ��ȯ ���'���� '�̻���'�� ���� ȸ�� ����� ���Ѵ�.	//
	//------------------------------------------------------------------//
	// i)   NewTM = TM * R
	// ii)  TM(-1) * NewTM = R
	// iii) TM(T) * NewTM = R
	//	MATRIX TransTM;  
	//	D3DXMatrixTranspose(&TransTM, &m_mTM);
	//
	//�޽� ������..�� ����� �ǰ���.
	m_mRotation = mNewTM ;		


	//5. �߻� ���⵵ ����..
	m_vFireDir =  m_pTarget->GetPos() - m_vPos;
	D3DXVec3Normalize(&m_vFireDir, &m_vFireDir);


	return TRUE;
}


/*
//0. ������ ��ǥ�� ���ٸ�, ���� ����� ���� ������ ������ ����, �����Ѵ�.
//  ..��� �ؾ� ������...�׳� �Ѿ...
if(m_pTarget == NULL) return FALSE;

//1. ���� ��ġ���� '��ǥ' �� ���� ���͸� ���Ѵ�.
VECTOR3 vNewView = m_vPos - m_pTarget->GetPos();
D3DXVec3Normalize(&vNewView, &vNewView);

//2. �� Up, Horz ���� ���ϱ�.
VECTOR3 vNewUp = m_vUp;

VECTOR3 vNewHorz;
D3DXVec3Cross(&vNewHorz, &vNewUp, &vNewView);	//���� ���� ���.
D3DXVec3Normalize(&vNewHorz, &vNewHorz);

D3DXVec3Cross(&vNewUp, &vNewView, &vNewHorz);	//��� ���� �� ���.
D3DXVec3Normalize(&vNewUp, &vNewUp);			//3���� ���� ����ȭ!!

//3.������ ���� �� �ະ ���� ������ ��ķ� �����Ѵ�.
MATRIX mNewTM;
D3DXMatrixIdentity(&mNewTM);

mNewTM._11 = vNewHorz.x;	mNewTM._12 = vNewHorz.y;	mNewTM._13 = vNewHorz.z;
mNewTM._21 = vNewUp.x;		mNewTM._22 = vNewUp.y;		mNewTM._23 = vNewUp.z;
mNewTM._31 = vNewView.x;	mNewTM._32 = vNewView.y;	mNewTM._33 = vNewView.z;



//������ �� ����.
//mNewTM  = m_mScaling * mNewTM;		

//�̵��� ����.
//mNewTM._41 = m_vPos.x;	mNewTM._42 = m_vPos.y;	mNewTM._43 = m_vPos.z;


//------------------------------------------------------------------//
//4.�̻����� '�� ��ȯ ���'���� '�̻���'�� ���� ȸ�� ����� ���Ѵ�.	//
//------------------------------------------------------------------//
// i)   NewTM = TM * R
// ii)  TM(-1) * NewTM = R
// iii) TM(T) * NewTM = R
//	MATRIX TransTM;  
//	D3DXMatrixTranspose(&TransTM, &m_mTM);
//
m_mRotation = mNewTM ;		


//5. �߻� ���⵵ ����..
m_vFireDir =  m_pTarget->GetPos() - m_vPos;
D3DXVec3Normalize(&m_vFireDir, &m_vFireDir);


*/


//////////////////////////////////////////////////////////////////////////////
//
// ��ǥ�� �̻��� ���� + �̵��ӵ� ���� �ϱ�.
//
int CMissile::MoveHomming(BOOL bHomming, float dTime/*=0*/)
{
	//--------------------------//
	// '�ν��� ���ӵ� ����'		//
	//--------------------------//
	// �ν��� ��ȭ�� ���� �ӵ��� �ø���..�ִ� �ӵ��� �����Ѵ�.
	// �ӵ� ���� : �ʴ� �ְ� �ӵ�(m_fMaxFlyVelo)�� 3%�� ������ŭ...
	m_fCurrVelo += (m_fMaxFlyVelo*0.03f)*(m_fMaxFlyVelo*0.03f)*dTime;		

	//���ӵ� ����. �ְ� �ӵ��̻� �ö󰡴� ���� ���´�.
	if(m_fCurrVelo >= m_fMaxFlyVelo)		
	{
		m_fCurrVelo = m_fMaxFlyVelo;		//���� �ӵ� ����.
		// ...m_vMoveVelo �� ���̻� '�ö���' �ʴ´�...
	}



	//--------------------------//
	// �̻��� ���� �ӵ�  ����.  //
	//--------------------------//
	// �ӵ� ���...
	// �߻� ������ ������(������) * ���װ� ��ŭ �ӵ��� �����Ѵ�.
	// ....���� ...

	//FireDir �� �ٲ���ٸ�..�̰͵� ������ ���ľ� �Ѵ� (���� ����, ���� ����)
	//.... ���� .... 

	//�̻����� '�ӵ�' ����(����)
	m_vMoveVelo = m_fCurrVelo*m_vFireDir + m_vReleaseTimePlaneDirVelo;			


	// 
	// �����ϸ� : �� ��ǥ ��
	//
	D3DXMatrixScaling(&m_mScale, m_vScale.x, m_vScale.y, m_vScale.z);

	// 
	// ȸ�� ó�� : �� ��ǥ ��
	//
	// ��ǥ������ ���� Ʋ�� -> ȸ�� ��� ����.....2004.11.12.
	if(bHomming) CalHommingRotMtx(dTime);


	// 
	// �߷� ����
	//...

	// 
	// �̻��� ��ġ ���. : ��ġ = ���� �ӵ�  * �ð�.
	//
	m_vPos += m_vMoveVelo*dTime;		


	// 
	// �̵���� ����.
	//
	D3DXMatrixTranslation(&m_mTranslation, m_vPos.x, m_vPos.y, m_vPos.z);

	//
	// TM �����.
	//
	MATRIX temp;
	D3DXMatrixMultiply(&temp, &m_mScale, &m_mRotation);
	D3DXMatrixMultiply(&m_mTM, &temp, &m_mTranslation);


	return TRUE;

}






 
/******************  end of file "Missile.cpp" ****************************/