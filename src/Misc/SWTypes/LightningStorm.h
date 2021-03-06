#ifndef SUPERTYPE_EXT_LSTORM_H
#define SUPERTYPE_EXT_LSTORM_H

#include <xcompile.h>

#include "../SWTypes.h"

class SW_LightningStorm : NewSWType
{
	public:
		SW_LightningStorm() : NewSWType()
			{ };

		virtual ~SW_LightningStorm()
			{ };

		virtual const char * GetTypeString()
			{ return NULL; }

		virtual void LoadFromINI(
			SWTypeExt::ExtData *pData, SuperWeaponTypeClass *pSW, CCINIClass *pINI);
		virtual void Initialize(SWTypeExt::ExtData *pData, SuperWeaponTypeClass *pSW);
		virtual bool AbortFire(SuperClass* pSW, bool IsPlayer);
		virtual bool Launch(SuperClass* pThis, CellStruct* pCoords, byte IsPlayer);
		virtual bool HandlesType(int type);
		virtual SuperWeaponFlags::Value Flags();

		static SuperClass* CurrentLightningStorm;
};
#endif
