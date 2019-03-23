//
// Instrument.h Header File.
//
// 2005.09. Mad Dog / mad_dog@hanmail.net
//
//////////////////////////////////////////////////////////////////////////////

#pragma once

class CPlane;
class CMissile;


/////////////////////////////////////////////////////////////////////////////
//
//
//
namespace GP
{
	//
	// 자세계 (Altitude Direction Indicator)
	//
	void ADILoad();
	void ADIMove(CPlane* pHero);
	void ADIDraw();
	void ADIRelease();


	//
	// 무장 표시기 - WI (Weapon Indicator) 관련. 2004.07.02._pMeshADI
	//
	void WeaponIndicatorLoad();
 	void WeaponIndicatorDraw(CMissile* pMissile[], DWORD MissileMax);
	void WeaponIndicatorRelease();

	//
	// 2D 레이다.( 레이더 경고 수신기, RWR : Radar Warning Receiver) 
	//
	void RWRLoad();
	void RWRUpdate(float dTime = 0.0f);
	void RWRDraw();
	void RWRRelease();

};

/**************** end of file "Instrument.h " **********************************/
