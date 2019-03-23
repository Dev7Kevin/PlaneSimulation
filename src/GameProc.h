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
	
	//카메라.
	void CameraSetup();						 
 	void CameraUpdate(float dTime = 0.0f);	  

	void ViewportOn(float dTime);		
	void ParticleDraw(float dTime);		
	void DummyDraw(float dTime);		


	//레일건 관련.
	void RailGunInit();
	void RailGunCreate(float dTime);	
	void RailGunMove(float dTime);
	void RailGunDraw(float dTime);   
	void RailGunRelease();

	//토네이도.
	void TornadoCreate(float dTime);




	//미사일 연기 관련.
	void SmokeInit();
	void SmokeMove(float dTime);
	void SmokeDraw(float dTime);   
	void SmokeRelease();


	//미사일 관련.
 	void MissileCreate(CPlane* pHero, CB3Mesh* pMesh, 
		SPRDATA* pFrameSpr,				//미사일 불꽃.
		SPRDATA* pSmokeSpr,				//미사일 연기.
		VECTOR vHardPt[], 
		CCamera** ppCam, float dTime);
	void MissileFire(BOOL bNuk, BOOL bFireAll);
	void MissileMove(float dTime);
	void MissileDraw(float dTime);		
	void MissileRelease();				//남아있는 미사일 제거.


	//미사일 관련.
 	void AssistMissileCreate(CPlane* pHero, CB3Mesh* pMesh, 
		SPRDATA* pFrameSpr,				//미사일 불꽃.
		SPRDATA* pSmokeSpr,				//미사일 연기.
		VECTOR vHardPt[], 
		CCamera** ppCam, float dTime);
	void AssistMissileFire();
	void AssistMissileMove(float dTime);
	void AssistMissileDraw(float dTime);		
	void AssistMissileRelease();				//남아있는 미사일 제거.

	


	extern BOOL _bEngineStop;		

}



/**************** end of file "GameProc.h" *********************************/