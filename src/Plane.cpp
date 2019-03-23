#include "Plane.h"

CPlane::CPlane(
		LPDEVICE pDev,
		CB3Mesh* pMesh,
 		UINT hp,

		float		fRadius,			
		LPANIINFO	pBoomAniInfo,		/
 
		VECTOR3 vPos, 
		VECTOR3 vRot/* = VECTOR3(0.0f, 0.0f, 0.0f)*/, 
		VECTOR3 vScale/*= VECTOR3(1.0f, 1.0f, 1.0f)*/,
		VECTOR3 vMoveVelo/* = VECTOR3(0.0f, 0.0f, 0.0f)*/,
		VECTOR3 vRotVelo/* = VECTOR3(0.0f, 0.0f, 0.0f)*/
 		 )
:CUnit(vPos, vRot, vScale, vMoveVelo, vRotVelo, 
	   fRadius, pBoomAniInfo,	
	   pMesh, pDev)
, m_Hp(hp)
{ 	
	
  	m_vCurrDirVelo = VECTOR3(0, 0, 0);

	
  	m_vOrgNView = m_vNView = VECTOR3(0, 0, -1);		 
	m_vOrgUp = m_vUp = VECTOR3(0, 1, 0);			
	m_vOrgHorz = m_vHorz = VECTOR3(-1, 0, 0);		 

	m_fThrottle = 0.0f;
	m_fPower = 0.0f;
	m_fMaxVelo = 0.0f; 


	m_vShake = VECTOR3(0, 0, 0);

	LRotate(vRot);
	 
	 
}


CPlane::~CPlane()
{
	Release();
}




BOOL CPlane::TakeDamage(int dmg)
{
	m_Hp -= dmg;

	if(m_Hp <=0)					
	{ 	 
		SetDead();					
 
		return EMY_DEAD_;		
	}

	return EMY_ALIVE_;

}

void CPlane::Reset()
{
	CUnit::Reset();

	m_vCurrDirVelo = VECTOR3(0, 0, 0);

	m_fThrottle = 0.0f;
	m_fPower = 0.0f;
	m_fMaxVelo = 0.0f; 


	m_vNView = m_vOrgNView;		
	m_vUp = m_vOrgUp;				 
	m_vHorz = m_vOrgHorz;	


	m_vShake = VECTOR3(0, 0, 0);
 
	LRotate(m_vOrgRot); 

}


MATRIX CPlane::LRotate(VECTOR3 vAngle)	
{	 
	MATRIX mX = LRotateX(vAngle.x);
	MATRIX mY = LRotateY(vAngle.y);
	MATRIX mZ = LRotateZ(vAngle.z);	
	return (mX * mY * mZ);			
}



MATRIX CPlane::LRotateX(float angle)	
{ 
	MATRIX mRot;
	D3DXMatrixRotationAxis(&mRot, &m_vHorz, angle);		 

 	D3DXVec3TransformNormal(&m_vNView, &m_vNView, &mRot);

 	D3DXVec3Cross(&m_vUp, &m_vNView, &m_vHorz);
  
	return mRot;		 
}


MATRIX CPlane::LRotateY(float angle)  
{ 
	MATRIX mRot;
	D3DXMatrixRotationAxis(&mRot, &m_vUp, angle);		 

	D3DXVec3TransformNormal(&m_vNView, &m_vNView, &mRot); 

	D3DXVec3Cross(&m_vHorz, &m_vUp, &m_vNView);
	 
	return mRot; 
}


MATRIX CPlane::LRotateZ(float angle)  
{ 
	MATRIX mRot;
	D3DXMatrixRotationAxis(&mRot, &m_vNView, angle);	

 	D3DXVec3TransformNormal(&m_vUp, &m_vUp, &mRot);
 
	D3DXVec3Cross(&m_vHorz, &m_vUp, &m_vNView);

 	return mRot;		 

}

void CPlane::SetVectors()  
{  
	//D3DXVec3Normalize(&m_vNView, &(m_vLookAt - m_vEye));

	//D3DXVec3Cross(&m_vHorz, &m_vUp, &m_vNView);
	//D3DXVec3Normalize(&m_vHorz, &m_vHorz);

}

int CPlane::Move(float dTime/*=0*/)
{

	D3DXMatrixScaling(&m_mScale, m_vScale.x, m_vScale.y, m_vScale.z);

 	VECTOR3 vRot = (m_vRotVelo * dTime);	
 	m_mRotation *= LRotate(vRot);	

	m_vRot += vRot;	

	D3DXVec3TransformNormal(&m_vDirection, &m_vOrgDirection, &m_mRotation); 

	m_vCurrDirVelo = m_vDirection * m_vMoveVelo.z;					
	m_vPos += m_vCurrDirVelo * dTime;
 
	VECTOR3 vPos = m_vPos + m_vShake;	
	D3DXMatrixTranslation(&m_mTranslation, vPos.x, vPos.y, vPos.z);

	MATRIX temp;
	D3DXMatrixMultiply(&temp, &m_mScale, &m_mRotation);
	D3DXMatrixMultiply(&m_mTM, &temp, &m_mTranslation);


	return TRUE;
}


int CPlane::Draw(float dTime/*=0*/)   
{
	if(!IsVisible()) return TRUE;	


	m_pDev->SetTransform(D3DTS_WORLD, &m_mTM);
	m_pMesh->Render(m_pDev);

	return TRUE;
}

void CPlane::Release()
{
}

float CPlane::GetVelo()
{
	return  m_vMoveVelo.z;  //m/s -> km/h
}

float CPlane::GetSpeed()
{

	return  (m_vMoveVelo.z * 3600 / 1000.0f);  //m/s -> km/h
}

 