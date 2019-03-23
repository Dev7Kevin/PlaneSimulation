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
	// �ڼ��� (Altitude Direction Indicator)
	//
	void ADILoad();
	void ADIMove(CPlane* pHero);
	void ADIDraw();
	void ADIRelease();


	//
	// ���� ǥ�ñ� - WI (Weapon Indicator) ����. 2004.07.02._pMeshADI
	//
	void WeaponIndicatorLoad();
 	void WeaponIndicatorDraw(CMissile* pMissile[], DWORD MissileMax);
	void WeaponIndicatorRelease();

	//
	// 2D ���̴�.( ���̴� ��� ���ű�, RWR : Radar Warning Receiver) 
	//
	void RWRLoad();
	void RWRUpdate(float dTime = 0.0f);
	void RWRDraw();
	void RWRRelease();

};

/**************** end of file "Instrument.h " **********************************/
