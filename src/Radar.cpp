//////////////////////////////////////////////////////////////////////////
//
// Radar.cpp
//
// 3D 레이더 구현 
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

	Init(); // 정점 버퍼에 레이더 프레임 입력

	D3DXMatrixInverse(&m_mInverseView, NULL, &m_mView);
		
	//빌보드 '회전 행렬' : 카메라를 향하도록 하는 행렬. 구하기.
	//'이동값'은 무시한다.

	m_mInverseView._41 = 0.0f;
	m_mInverseView._42 = 0.0f;
	m_mInverseView._43 = 0.0f;

	m_mTM *= m_mScale;

	m_pTarget = NULL;	

	//2005.02.25.
	m_pScanArea = NULL; 
 	m_TargetNum = 0;
	D3DXMatrixIdentity(&m_mScanView);

	Init(); // 정점 버퍼에 레이더 프레임 입력
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
	
	float fDivision = 24.0f;					// 레이더 구체의 등분
	float fThickness = 0.04f;					// 프레임의 두께
	float fRadius	=  1.0f;					// 구체의 반경

	float fFrameNumV = 8.0f;					// 구체의 프레임을 세로몇등분 할것인가.
	float fFrameNumH = 9.0f;					// 구체의 프레임을 가로몇등분 할것인가.

	m_VertexCnt = (int)( fDivision * (fFrameNumV / 2) * 6.0f 
		              + (fFrameNumH * fFrameNumV * 6.0f));
	m_PolyCnt =  m_VertexCnt / 3 ;

	HRESULT res = S_OK;
	if(FAILED(res = m_pDev->CreateVertexBuffer( m_VertexCnt * sizeof(VERTEX_RADAR), 
					D3DUSAGE_WRITEONLY, D3DFVF_RADAR_, D3DPOOL_MANAGED, 
					&m_pVB, NULL)))
	{
		B3D_Error(NO_, "[Error] CRader(0x%x)::Init - 정점 버퍼 생성 실패...: res = %s", 
					this, DXGetErrorString9(res));
		return FALSE;
	}

	//정점 버퍼를 잠그기. 
	VERTEX_RADAR *pVertices;
	if(FAILED(res = m_pVB->Lock(0, 0, (VOID**)&pVertices, NULL)))
	{
		B3D_Error(NO_, "[Error] CRader(0x%x)::Init - 정점 버퍼 잠금 실패...: res = %s", 
					this, DXGetErrorString9(res));
		return FALSE;
	}

	//
	// 데이터 입력.
	//


	// 세로 프레임 그리기
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


	// 가로 프레임 그리기

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

	//버퍼 닫기.
	m_pVB->Unlock(); 


	//2005.02.25. : 
	//1. 레이더 스캔 영역 설정하기.
	m_pScanArea = new CFrustum;
	ASSERT(m_pScanArea != NULL);

	m_pScanArea->Init(D3DXToRadian(15),	//스캔 수평 각도 : 25도.
					(float)100/100,	//스캔 가로/세로 비율 : 수평=100m / 수직=100m 
					1.0f,			//스캔 최소 거리 : 1m. 
					10000.0f		//스캔 최대 거리 : 10km.
					);		 


	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
//
int CRadar::Update()
{
	//현재 해상도를 기준으로 뷰포트 영역을 재설정한다.(필요)
	//...


	// 적 탐색  
	SearchTargets();		
	
	// 스캔된 리스트에서 목표고정: Lock-ON  ★
	if(IsKeyUp(DIK_A))
		SelectTarget(SELECT_NEXT_);


	return 0;
}



/////////////////////////////////////////////////////////////////////////////
//
int CRadar::Render(BOOL bShowAzimuth)
{ 
	D3DXMatrixInverse(&m_mTM, NULL, &(m_pHero->GetRotMtx())); 
	
	DrawCircle(); // 레이다 그리기


	// 레이더 스캔 방위각 보여주기.
	// 주인공의 방향화 일치시켜 그려주어야 하겠다...
	// 주인공은  월드 -z 축이 전방.
	// 프러스텀은 월드 +z 축이 전방.
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

	D3DVIEWPORT9 oldVP; // 이전 뷰포트 백업용
	
	m_pDev->GetViewport(&oldVP);
	m_pDev->SetViewport(&m_Viewport);

	m_pDev->SetStreamSource(0, m_pVB, 0, sizeof(VERTEX_RADAR));
	m_pDev->SetFVF(D3DFVF_RADAR_);

	//MATRIX mOldView; 
	//m_pDev->GetTransform(D3DTS_VIEW, &mOldView);
	m_pDev->SetTransform(D3DTS_VIEW, &m_mView);
 	m_pDev->SetTransform(D3DTS_WORLD, &m_mTM );

	// 레이더 프레임 출력
	B3DSprite_DrawFX3(D3DPT_TRIANGLELIST, m_PolyCnt, m_pSprRadarv, 
						D3DXCOLOR(0.5f,0.5f,1.0f,0.4f), FALSE, 1.0, NO_);


	// 주인공 표시 (크기)
	B3DSprite_Scaling(m_pParticle, 0.3f,0.3f);

	// 주인공 표시 (위치) : 레이더의 정 가운데
	B3DSprite_Translation(m_pParticle, 0.0f, 0.0f, 0.0f, ON3D_);
	B3DSprite_DrawFX(m_pParticle,COLOR(0.0, 1.0, 1.0,1.0));	

	// 적 파티클 크기
	B3DSprite_Scaling(m_pParticle,0.15f,0.15f);

	for(itor i = m_pEnemyList.begin(); i != m_pEnemyList.end(); i++)		
	{
		//2004.11.15
		if( (*i)->IsDead())  continue;		//죽은 놈이면..표시할 필요 없다.


		VECTOR3 vEnemyPos;
		m_pParticle->mRot = m_mInverseView;		// 빌보드를 위한 역행렬


		// 충돌처리.. 레이더 범위에 들면 표시하고 아니면 레이더 범위 끝에 표시 레이더 범위 99.0f
		vEnemyPos = CheckCollision((*i)->GetPos(),m_pHero->GetPos(), 99.0f);

		// 주인공을 기준으로 적기 위치를 계산. 
		vEnemyPos -= m_pHero->GetPos();
					
		vEnemyPos = vEnemyPos * ( 1 / 100.0f ) ;  // 적기 위치 크기를 범위만큼 조절

		D3DXVec3TransformCoord(&vEnemyPos, &vEnemyPos, &m_mTM);
		B3DSprite_Translation(m_pParticle, vEnemyPos.x, vEnemyPos.y, vEnemyPos.z, ON3D_);
		//B3DSprite_DrawFX(m_pParticle,COLOR(1, 0, 0, 1));

		
		COLOR color(1.0f, 1.0f, 0.0f, 0.5f); //일반 타겟은 노란색...		

		if( (*i)->IsLockON())				//'락온'된 놈은 붉은색..
		{
			color = COLOR(1.0f, 0.0f, 0.0f, 1.0f);
		}
		B3DSprite_DrawFX(m_pParticle, color);

		//i++; -> 주석처리. 위의 for 문에서 증가함.

	}

	// 뷰포트 복구
	m_pDev->SetViewport(&oldVP);

	//카메라 재 설정.: 다른 오브제의 투영에 여향을 주지 않기위해.
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
	// 두 유닛 사이의 거리를 구한다.
	float fDistance = D3DXVec3Length(&vRadarLength);

	if( fDistance < fRadius)		//  fRadius 레이더의 반경
	{
		return  vTargetPos;			// 충돌하면 적기 위치를 리턴.
	}

	// 아니면 적기를 레이더 끝에 표기
	D3DXVec3Normalize(&vRadarLength,&vRadarLength);
	vRadarLength *= (fDistance - fRadius);

	return vRadarLength + vTargetPos;
}






//////////////////////////////////////////////////////////////////////////////
//
// int CRadar::SelectTarget()
//
// 적기 리스트에서 적기를 Lock-On 한다.. 2004.11.15.
// '발사' 는 다른 클래스 - FCS 등에서 처리 하도록 해야 한다.
//
int CRadar::SelectTarget(BOOL bNext/*=NO_*/)
{
	if(m_pEnemyList.empty()) 
	{
		//적기 리스트가 없으면 리턴...
		//타겟 클리어.
		m_pTarget = NULL;

		return FALSE;	
	} 


	// '다음 적기 선택'...★
	// 리스트의 마지막을 넘어가면, 
	// 리스트의 첫번째로 재 설정.
	//
	if(bNext)										
	{
		if( ++m_TargetNum > (int)m_ScanList.size() - 1 )		//★
			m_TargetNum = 0;						
	}


	//
	// 전체 적기가 아닌, '스캔'된 리스트에서 적 선별, 목표물 고정! (Lock-On)!
	//
	int cnt = 0;
	for(itor ii=m_ScanList.begin(); ii != m_ScanList.end(); ++ii, ++cnt)	
	{
		CEnemy* pEmy = static_cast<CEnemy*>(*ii);

		if(cnt < m_TargetNum)			//'다음 목표 선택' 일 경우,★
		{								//'이전' 목표물 처리...
			pEmy->SetLockON(LOCK_OFF_);	//목표물은, 'Lock-Off' 시킨다.
			m_pTarget = NULL;			//타겟 클리어.

			continue;
		}
  
		if( pEmy->IsDead())				//폭파중인 상태의 적기는...
		{
			pEmy->SetLockON(LOCK_OFF_);	//목표물은, 'Lock-Off' 시킨다.
			m_pTarget = NULL;			//타겟 클리어.

			continue;
		}
 
 		/* Lock-ON!!! */		
		if(m_pTarget) m_pTarget->SetLockON(LOCK_OFF_);	//이전 락온 적기 해제.★

		pEmy->SetLockON();			//적기를 'Lock-ON' 상태로... 	
		m_pTarget = pEmy;			//Lock-On 된 적기 포인터 저장..

		break; 
	}

	return TRUE;
}




/////////////////////////////////////////////////////////////////////////////
//
// 스캐닝 적기 리스트를 리셋한다.
//
void CRadar::ResetTargets(list<CEnemy*>& EnemyList)
{
	m_pEnemyList = EnemyList;		//적기 리스트 재 할당.

	//Lock-On 목표 제거.
	m_pTarget = NULL;

	//스캐닝 리스트 올~ 클리어~♥ 
	m_ScanList.clear(); 
	m_TargetNum = 0;

}




/////////////////////////////////////////////////////////////////////////////
//
// 매 프레임 마다 전체 적기중, 비행기의 시야에 들어온 적 기체들을 
// 선별한다. 
// 2005.02.25.
//
void CRadar::SearchTargets()
{ 

	// 1. 스캐닝 영역 변환 행렬 생성.
	//
	MakeScanViewMatrix();


	// 2. 이전 스캔 데이터 제거.
	//
	m_ScanList.clear();


	// 3. 전체 적기 리스트에서 레이더 스캐닝 영역에 들어온 적기만을 선별.
	//
	VECTOR3 vPos;
	for(itor i = m_pEnemyList.begin(); i != m_pEnemyList.end(); i++)
	{	
		if( (*i)->IsDead()) continue;		//폭파중의 적기는 무시.


		vPos = (*i)->GetPos();
		if(m_pScanArea->IsVisible(&m_mScanView, &vPos, (*i)->GetRadius()))
		{
			m_ScanList.push_back(*i);		//스캐닝 리스트에 등록.
		}
	}

	// 4. 이전에 설정된 목표가 스캐닝 영역을 벗어났다면, 
	//    Lock-On  해제.
	if(m_pTarget)
	{	
		vPos = m_pTarget->GetPos();

		if(!m_pScanArea->IsVisible(&m_mScanView, &vPos, m_pTarget->GetRadius()))
		{
			m_pTarget->SetLockON(LOCK_OFF_);
			m_pTarget = NULL;
		}
	}


	// 5. 스캐닝된 적 유닛중, 0번 유닛을 기본 목표로 선정.. 
	//m_TargetNum = 0;

}




/////////////////////////////////////////////////////////////////////////////
//
// 주인공 비행기의 위치/방향을 기준으로 스캐닝 영역 변환 행렬을 만든다.
// CCamera::SetView 와 유사.
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
// 스캐닝된 적 유닛의 개수를 리턴한다.
//
int CRadar::GetScanCnt()
{ 
	return static_cast<int>(m_ScanList.size()); 
}	





/******************* end of file "Radar.cpp" *******************************/
