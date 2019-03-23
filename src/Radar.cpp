//////////////////////////////////////////////////////////////////////////
//
// Radar.cpp
//
// 3D ���̴� ���� 
//
//
//////////////////////////////////////////////////////////////////////////

#pragma once
#include "Unit.h"
#include "Plane.h"
#include "Enemy.h"
#include "Camera.h"
#include "Frustum.h"	

#include "Radar.h"




/////////////////////////////////////////////////////////////////////////////
//
CRadar::CRadar(LPDEVICE pDevice, CCamera** ppCamera, D3DVIEWPORT9 viewPort,
			   list<CEnemy*> &EnemyList, CPlane* pHero, SPRDATA* pSprRadarV, 
			   SPRDATA* pParticle)
:m_pDev(pDevice), m_ppCamera(ppCamera), m_Viewport(viewPort), 
m_pEnemyList(EnemyList), m_pHero(pHero), m_pSprRadarv(pSprRadarV), 
m_pParticle(pParticle)
{

	VECTOR3 vEye = VECTOR3(0.0f, 0.0f, 3.0f);
	VECTOR3 vLookAt = VECTOR3(0.0f,0.0f,0.0f);
	VECTOR3 vUp = VECTOR3( 0.0f, 1.0f, 0.0f);

	D3DXMatrixIdentity(&m_mTM);
	D3DXMatrixScaling(&m_mScale, 1.0f,1.0f,1.0f);
	D3DXMatrixLookAtLH(&m_mView, &vEye, &vLookAt, &vUp);

	Init(); // ���� ���ۿ� ���̴� ������ �Է�

	D3DXMatrixInverse(&m_mInverseView, NULL, &m_mView);
		
	//������ 'ȸ�� ���' : ī�޶� ���ϵ��� �ϴ� ���. ���ϱ�.
	//'�̵���'�� �����Ѵ�.

	m_mInverseView._41 = 0.0f;
	m_mInverseView._42 = 0.0f;
	m_mInverseView._43 = 0.0f;

	m_mTM *= m_mScale;

	m_pTarget = NULL;	

	//2005.02.25.
	m_pScanArea = NULL; 
 	m_TargetNum = 0;
	D3DXMatrixIdentity(&m_mScanView);

	Init(); // ���� ���ۿ� ���̴� ������ �Է�
}



/////////////////////////////////////////////////////////////////////////////
//
CRadar::~CRadar()
{
	Release();
}



/////////////////////////////////////////////////////////////////////////////
//
void CRadar::Release()
{
	SAFE_RELEASE(m_pVB);
	SAFE_DELETE(m_pScanArea);		//2005.02.25

}




/////////////////////////////////////////////////////////////////////////////
//
BOOL CRadar::Init()
{
	
	float fDivision = 24.0f;					// ���̴� ��ü�� ���
	float fThickness = 0.04f;					// �������� �β�
	float fRadius	=  1.0f;					// ��ü�� �ݰ�

	float fFrameNumV = 8.0f;					// ��ü�� �������� ���θ��� �Ұ��ΰ�.
	float fFrameNumH = 9.0f;					// ��ü�� �������� ���θ��� �Ұ��ΰ�.

	m_VertexCnt = (int)( fDivision * (fFrameNumV / 2) * 6.0f 
		              + (fFrameNumH * fFrameNumV * 6.0f));
	m_PolyCnt =  m_VertexCnt / 3 ;

	HRESULT res = S_OK;
	if(FAILED(res = m_pDev->CreateVertexBuffer( m_VertexCnt * sizeof(VERTEX_RADAR), 
					D3DUSAGE_WRITEONLY, D3DFVF_RADAR_, D3DPOOL_MANAGED, 
					&m_pVB, NULL)))
	{
		B3D_Error(NO_, "[Error] CRader(0x%x)::Init - ���� ���� ���� ����...: res = %s", 
					this, DXGetErrorString9(res));
		return FALSE;
	}

	//���� ���۸� ��ױ�. 
	VERTEX_RADAR *pVertices;
	if(FAILED(res = m_pVB->Lock(0, 0, (VOID**)&pVertices, NULL)))
	{
		B3D_Error(NO_, "[Error] CRader(0x%x)::Init - ���� ���� ��� ����...: res = %s", 
					this, DXGetErrorString9(res));
		return FALSE;
	}

	//
	// ������ �Է�.
	//


	// ���� ������ �׸���
	int iVertiCnt = 0;

	for(int j = 0; j < fFrameNumV/2 ; j++)
	{
		float fDivAngle = 2 * D3DX_PI / fFrameNumV * j;
		MATRIX mRot;
		D3DXMatrixRotationY(&mRot, fDivAngle);

		for(int i = 0 ; i < fDivision ; i++) 
		{
			float theta = 2 * D3DX_PI / fDivision * i;

			VERTEX_RADAR v[4];

			VECTOR3 vTemp( sinf(theta)*fRadius, cosf(theta)*fRadius, 0.0f);
			VECTOR3 vFirst( 0.0f ,  0.0f , fThickness * -0.5f );
			VECTOR3 vSecond( 0.0f ,  0.0f , fThickness * 0.5f );

			D3DXVec3TransformCoord(&vTemp,&vTemp,&mRot);
			D3DXVec3TransformCoord(&vFirst,&vFirst,&mRot);
			D3DXVec3TransformCoord(&vSecond,&vSecond,&mRot);

			v[0].vPos = vFirst + vTemp;
			v[0].tu   = 0.0f;
			v[0].tv   = 1.0f / fDivision * (float)i;

			v[1].vPos = vSecond + vTemp;
			v[1].tu	  = 0.5f;
			v[1].tv	  = 1.0f / fDivision * (float)i;

			
			theta = 2 * D3DX_PI / fDivision * (i+1);
			vTemp = VECTOR3( sinf(theta)*fRadius, cosf(theta)*fRadius, 0.0f);
			vFirst = VECTOR3( 0.0f ,  0.0f , fThickness * -0.5f );
			vSecond = VECTOR3( 0.0f ,  0.0f , fThickness * 0.5f );

			D3DXVec3TransformCoord(&vTemp,&vTemp,&mRot);
			D3DXVec3TransformCoord(&vFirst,&vFirst,&mRot);
			D3DXVec3TransformCoord(&vSecond,&vSecond,&mRot);

			v[2].vPos = vFirst + vTemp;
			v[2].tu   = 0.0f;
			v[2].tv   = 1.0f / fDivision * ((float)i+1);

			v[3].vPos = vSecond + vTemp;
			v[3].tu	  = 0.5f;
			v[3].tv	  = 1.0f / fDivision * ((float)i+1);

			pVertices[iVertiCnt++] = v[0];
			pVertices[iVertiCnt++] = v[1];
			pVertices[iVertiCnt++] = v[2];
			pVertices[iVertiCnt++] = v[2];
			pVertices[iVertiCnt++] = v[1];
			pVertices[iVertiCnt++] = v[3];
		}
	}


	// ���� ������ �׸���

	for(int j = 0; j < fFrameNumH ; j++)
	{
		for(int i = 0; i < fFrameNumV ; i++)
		{
			MATRIX mRot;
			float theta = 2 * D3DX_PI / fFrameNumV * i;
			
			VERTEX_RADAR v[4];
			
			VECTOR3 vFirst( fRadius  , fThickness * -0.5f, 0.0f  );
			VECTOR3 vSecond( fRadius , fThickness * 0.5f, 0.0f  );
			
			D3DXMatrixRotationZ(&mRot,  (D3DX_PI / fFrameNumH * j) + D3DX_PI * 0.5f);	
			D3DXVec3TransformCoord(&vFirst,&vFirst,&mRot);
			D3DXVec3TransformCoord(&vSecond,&vSecond,&mRot);
			D3DXMatrixRotationY(&mRot,  theta);	
			D3DXVec3TransformCoord(&vFirst,&vFirst,&mRot);
			D3DXVec3TransformCoord(&vSecond,&vSecond,&mRot);
			
			v[0].vPos = vFirst;
			v[0].tu   = 0.5f;
			v[0].tv   = 1.0f / fFrameNumV * (float)i;
			
			v[1].vPos = vSecond;
			v[1].tu	  = 1.0f;
			v[1].tv	  = 1.0f / fFrameNumV * (float)i;

			theta = 2 * D3DX_PI / fFrameNumV * (i+1);
			
			vFirst = VECTOR3( fRadius  , fThickness * -0.5f,  0.0f  );
			vSecond = VECTOR3( fRadius , fThickness * 0.5f,  0.0f  );

			D3DXMatrixRotationZ(&mRot,  (D3DX_PI / fFrameNumH * j) + D3DX_PI * 0.5f);	
			D3DXVec3TransformCoord(&vFirst,&vFirst,&mRot);
			D3DXVec3TransformCoord(&vSecond,&vSecond,&mRot);
			D3DXMatrixRotationY(&mRot,  theta);	
			D3DXVec3TransformCoord(&vFirst,&vFirst,&mRot);
			D3DXVec3TransformCoord(&vSecond,&vSecond,&mRot);

			v[2].vPos = vFirst;
			v[2].tu   = 0.5f;
			v[2].tv   = 1.0f / fFrameNumV * ((float)i+1);

			v[3].vPos = vSecond;
			v[3].tu	  = 1.0f;
			v[3].tv	  = 1.0f / fFrameNumV * ((float)i+1);

			pVertices[iVertiCnt++] = v[0];
			pVertices[iVertiCnt++] = v[1];
			pVertices[iVertiCnt++] = v[2];
			pVertices[iVertiCnt++] = v[2];
			pVertices[iVertiCnt++] = v[1];
			pVertices[iVertiCnt++] = v[3];

		
		}
	}

	//���� �ݱ�.
	m_pVB->Unlock(); 


	//2005.02.25. : 
	//1. ���̴� ��ĵ ���� �����ϱ�.
	m_pScanArea = new CFrustum;
	ASSERT(m_pScanArea != NULL);

	m_pScanArea->Init(D3DXToRadian(15),	//��ĵ ���� ���� : 25��.
					(float)100/100,	//��ĵ ����/���� ���� : ����=100m / ����=100m 
					1.0f,			//��ĵ �ּ� �Ÿ� : 1m. 
					10000.0f		//��ĵ �ִ� �Ÿ� : 10km.
					);		 


	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
//
int CRadar::Update()
{
	//���� �ػ󵵸� �������� ����Ʈ ������ �缳���Ѵ�.(�ʿ�)
	//...


	// �� Ž��  
	SearchTargets();		
	
	// ��ĵ�� ����Ʈ���� ��ǥ����: Lock-ON  ��
	if(IsKeyUp(DIK_A))
		SelectTarget(SELECT_NEXT_);


	return 0;
}



/////////////////////////////////////////////////////////////////////////////
//
int CRadar::Render(BOOL bShowAzimuth)
{ 
	D3DXMatrixInverse(&m_mTM, NULL, &(m_pHero->GetRotMtx())); 
	
	DrawCircle(); // ���̴� �׸���


	// ���̴� ��ĵ ������ �����ֱ�.
	// ���ΰ��� ����ȭ ��ġ���� �׷��־�� �ϰڴ�...
	// ���ΰ���  ���� -z ���� ����.
	// ���������� ���� +z ���� ����.
	if(bShowAzimuth)
	{
		MATRIX mRot;
		D3DXMatrixRotationY(&mRot, D3DX_PI);
		MATRIX mTM = mRot * m_pHero->GetTM();
		m_pScanArea->Draw(m_pDev, COLOR(0, 1, 0, 0.1f), &mTM, FALSE);
	}

	return 0;
}



/////////////////////////////////////////////////////////////////////////////
//
void CRadar::DrawCircle()
{

	D3DVIEWPORT9 oldVP; // ���� ����Ʈ �����
	
	m_pDev->GetViewport(&oldVP);
	m_pDev->SetViewport(&m_Viewport);

	m_pDev->SetStreamSource(0, m_pVB, 0, sizeof(VERTEX_RADAR));
	m_pDev->SetFVF(D3DFVF_RADAR_);

	//MATRIX mOldView; 
	//m_pDev->GetTransform(D3DTS_VIEW, &mOldView);
	m_pDev->SetTransform(D3DTS_VIEW, &m_mView);
 	m_pDev->SetTransform(D3DTS_WORLD, &m_mTM );

	// ���̴� ������ ���
	B3DSprite_DrawFX3(D3DPT_TRIANGLELIST, m_PolyCnt, m_pSprRadarv, 
						D3DXCOLOR(0.5f,0.5f,1.0f,0.4f), FALSE, 1.0, NO_);


	// ���ΰ� ǥ�� (ũ��)
	B3DSprite_Scaling(m_pParticle, 0.3f,0.3f);

	// ���ΰ� ǥ�� (��ġ) : ���̴��� �� ���
	B3DSprite_Translation(m_pParticle, 0.0f, 0.0f, 0.0f, ON3D_);
	B3DSprite_DrawFX(m_pParticle,COLOR(0.0, 1.0, 1.0,1.0));	

	// �� ��ƼŬ ũ��
	B3DSprite_Scaling(m_pParticle,0.15f,0.15f);

	for(itor i = m_pEnemyList.begin(); i != m_pEnemyList.end(); i++)		
	{
		//2004.11.15
		if( (*i)->IsDead())  continue;		//���� ���̸�..ǥ���� �ʿ� ����.


		VECTOR3 vEnemyPos;
		m_pParticle->mRot = m_mInverseView;		// �����带 ���� �����


		// �浹ó��.. ���̴� ������ ��� ǥ���ϰ� �ƴϸ� ���̴� ���� ���� ǥ�� ���̴� ���� 99.0f
		vEnemyPos = CheckCollision((*i)->GetPos(),m_pHero->GetPos(), 99.0f);

		// ���ΰ��� �������� ���� ��ġ�� ���. 
		vEnemyPos -= m_pHero->GetPos();
					
		vEnemyPos = vEnemyPos * ( 1 / 100.0f ) ;  // ���� ��ġ ũ�⸦ ������ŭ ����

		D3DXVec3TransformCoord(&vEnemyPos, &vEnemyPos, &m_mTM);
		B3DSprite_Translation(m_pParticle, vEnemyPos.x, vEnemyPos.y, vEnemyPos.z, ON3D_);
		//B3DSprite_DrawFX(m_pParticle,COLOR(1, 0, 0, 1));

		
		COLOR color(1.0f, 1.0f, 0.0f, 0.5f); //�Ϲ� Ÿ���� �����...		

		if( (*i)->IsLockON())				//'����'�� ���� ������..
		{
			color = COLOR(1.0f, 0.0f, 0.0f, 1.0f);
		}
		B3DSprite_DrawFX(m_pParticle, color);

		//i++; -> �ּ�ó��. ���� for ������ ������.

	}

	// ����Ʈ ����
	m_pDev->SetViewport(&oldVP);

	//ī�޶� �� ����.: �ٸ� �������� ������ ������ ���� �ʱ�����.
	(*m_ppCamera)->SetView();
	//m_pDev->SetTransform(D3DTS_VIEW, &mOldView);
	//MATRIX mTM; D3DXMatrixIdentity(&mTM);
	//m_pDev->SetTransform(D3DTS_WORLD, &mTM);

	
}



/////////////////////////////////////////////////////////////////////////////
//
VECTOR3 CRadar::CheckCollision(VECTOR3 vTargetPos,VECTOR3 vPos, float fRadius)
{
	VECTOR3 vRadarLength = vPos - vTargetPos;
	// �� ���� ������ �Ÿ��� ���Ѵ�.
	float fDistance = D3DXVec3Length(&vRadarLength);

	if( fDistance < fRadius)		//  fRadius ���̴��� �ݰ�
	{
		return  vTargetPos;			// �浹�ϸ� ���� ��ġ�� ����.
	}

	// �ƴϸ� ���⸦ ���̴� ���� ǥ��
	D3DXVec3Normalize(&vRadarLength,&vRadarLength);
	vRadarLength *= (fDistance - fRadius);

	return vRadarLength + vTargetPos;
}






//////////////////////////////////////////////////////////////////////////////
//
// int CRadar::SelectTarget()
//
// ���� ����Ʈ���� ���⸦ Lock-On �Ѵ�.. 2004.11.15.
// '�߻�' �� �ٸ� Ŭ���� - FCS ��� ó�� �ϵ��� �ؾ� �Ѵ�.
//
int CRadar::SelectTarget(BOOL bNext/*=NO_*/)
{
	if(m_pEnemyList.empty()) 
	{
		//���� ����Ʈ�� ������ ����...
		//Ÿ�� Ŭ����.
		m_pTarget = NULL;

		return FALSE;	
	} 


	// '���� ���� ����'...��
	// ����Ʈ�� �������� �Ѿ��, 
	// ����Ʈ�� ù��°�� �� ����.
	//
	if(bNext)										
	{
		if( ++m_TargetNum > (int)m_ScanList.size() - 1 )		//��
			m_TargetNum = 0;						
	}


	//
	// ��ü ���Ⱑ �ƴ�, '��ĵ'�� ����Ʈ���� �� ����, ��ǥ�� ����! (Lock-On)!
	//
	int cnt = 0;
	for(itor ii=m_ScanList.begin(); ii != m_ScanList.end(); ++ii, ++cnt)	
	{
		CEnemy* pEmy = static_cast<CEnemy*>(*ii);

		if(cnt < m_TargetNum)			//'���� ��ǥ ����' �� ���,��
		{								//'����' ��ǥ�� ó��...
			pEmy->SetLockON(LOCK_OFF_);	//��ǥ����, 'Lock-Off' ��Ų��.
			m_pTarget = NULL;			//Ÿ�� Ŭ����.

			continue;
		}
  
		if( pEmy->IsDead())				//�������� ������ �����...
		{
			pEmy->SetLockON(LOCK_OFF_);	//��ǥ����, 'Lock-Off' ��Ų��.
			m_pTarget = NULL;			//Ÿ�� Ŭ����.

			continue;
		}
 
 		/* Lock-ON!!! */		
		if(m_pTarget) m_pTarget->SetLockON(LOCK_OFF_);	//���� ���� ���� ����.��

		pEmy->SetLockON();			//���⸦ 'Lock-ON' ���·�... 	
		m_pTarget = pEmy;			//Lock-On �� ���� ������ ����..

		break; 
	}

	return TRUE;
}




/////////////////////////////////////////////////////////////////////////////
//
// ��ĳ�� ���� ����Ʈ�� �����Ѵ�.
//
void CRadar::ResetTargets(list<CEnemy*>& EnemyList)
{
	m_pEnemyList = EnemyList;		//���� ����Ʈ �� �Ҵ�.

	//Lock-On ��ǥ ����.
	m_pTarget = NULL;

	//��ĳ�� ����Ʈ ��~ Ŭ����~�� 
	m_ScanList.clear(); 
	m_TargetNum = 0;

}




/////////////////////////////////////////////////////////////////////////////
//
// �� ������ ���� ��ü ������, ������� �þ߿� ���� �� ��ü���� 
// �����Ѵ�. 
// 2005.02.25.
//
void CRadar::SearchTargets()
{ 

	// 1. ��ĳ�� ���� ��ȯ ��� ����.
	//
	MakeScanViewMatrix();


	// 2. ���� ��ĵ ������ ����.
	//
	m_ScanList.clear();


	// 3. ��ü ���� ����Ʈ���� ���̴� ��ĳ�� ������ ���� ���⸸�� ����.
	//
	VECTOR3 vPos;
	for(itor i = m_pEnemyList.begin(); i != m_pEnemyList.end(); i++)
	{	
		if( (*i)->IsDead()) continue;		//�������� ����� ����.


		vPos = (*i)->GetPos();
		if(m_pScanArea->IsVisible(&m_mScanView, &vPos, (*i)->GetRadius()))
		{
			m_ScanList.push_back(*i);		//��ĳ�� ����Ʈ�� ���.
		}
	}

	// 4. ������ ������ ��ǥ�� ��ĳ�� ������ ����ٸ�, 
	//    Lock-On  ����.
	if(m_pTarget)
	{	
		vPos = m_pTarget->GetPos();

		if(!m_pScanArea->IsVisible(&m_mScanView, &vPos, m_pTarget->GetRadius()))
		{
			m_pTarget->SetLockON(LOCK_OFF_);
			m_pTarget = NULL;
		}
	}


	// 5. ��ĳ�׵� �� ������, 0�� ������ �⺻ ��ǥ�� ����.. 
	//m_TargetNum = 0;

}




/////////////////////////////////////////////////////////////////////////////
//
// ���ΰ� ������� ��ġ/������ �������� ��ĳ�� ���� ��ȯ ����� �����.
// CCamera::SetView �� ����.
// 2005.02.25.
void CRadar::MakeScanViewMatrix()
{
	VECTOR3 vPos = m_pHero->GetPos();
	VECTOR3 vLookAt = m_pHero->GetNView() + vPos;
	VECTOR3 vUp = m_pHero->GetUp() + vPos;

	D3DXMatrixLookAtLH(&m_mScanView, &vPos, &vLookAt, &vUp);

}



/////////////////////////////////////////////////////////////////////////////
//
// ��ĳ�׵� �� ������ ������ �����Ѵ�.
//
int CRadar::GetScanCnt()
{ 
	return static_cast<int>(m_ScanList.size()); 
}	





/******************* end of file "Radar.cpp" *******************************/
