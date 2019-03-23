//////////////////////////////////////////////////////////////////////////////
//
// GameProc.h : 
//
//
//////////////////////////////////////////////////////////////////////////////

#pragma once

class CCamera;
class CPlane;

namespace GP
{
	BOOL Loading();
	BOOL Run(float dTime);
	void Release();

	void ShowHelp();
	
	//ī�޶�.
	void CameraSetup();						 
 	void CameraUpdate(float dTime = 0.0f);	  

	void ViewportOn(float dTime);		
	void ParticleDraw(float dTime);		
	void DummyDraw(float dTime);		


	//���ϰ� ����.
	void RailGunInit();
	void RailGunCreate(float dTime);	
	void RailGunMove(float dTime);
	void RailGunDraw(float dTime);   
	void RailGunRelease();

	//����̵�.
	void TornadoCreate(float dTime);




	//�̻��� ���� ����.
	void SmokeInit();
	void SmokeMove(float dTime);
	void SmokeDraw(float dTime);   
	void SmokeRelease();


	//�̻��� ����.
 	void MissileCreate(CPlane* pHero, CB3Mesh* pMesh, 
		SPRDATA* pFrameSpr,				//�̻��� �Ҳ�.
		SPRDATA* pSmokeSpr,				//�̻��� ����.
		VECTOR vHardPt[], 
		CCamera** ppCam, float dTime);
	void MissileFire(BOOL bNuk, BOOL bFireAll);
	void MissileMove(float dTime);
	void MissileDraw(float dTime);		
	void MissileRelease();				//�����ִ� �̻��� ����.


	//�̻��� ����.
 	void AssistMissileCreate(CPlane* pHero, CB3Mesh* pMesh, 
		SPRDATA* pFrameSpr,				//�̻��� �Ҳ�.
		SPRDATA* pSmokeSpr,				//�̻��� ����.
		VECTOR vHardPt[], 
		CCamera** ppCam, float dTime);
	void AssistMissileFire();
	void AssistMissileMove(float dTime);
	void AssistMissileDraw(float dTime);		
	void AssistMissileRelease();				//�����ִ� �̻��� ����.

	


	extern BOOL _bEngineStop;		

}



/**************** end of file "GameProc.h" *********************************/