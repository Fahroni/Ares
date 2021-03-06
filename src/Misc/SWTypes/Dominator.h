#ifndef SUPERTYPE_EXT_DOMINATOR_H
#define SUPERTYPE_EXT_DOMINATOR_H

#include <xcompile.h>

#include "../SWTypes.h"

class SW_PsychicDominator : NewSWType
{
	public:
		SW_PsychicDominator() : NewSWType()
			{ };

		virtual ~SW_PsychicDominator()
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

		static SuperClass* CurrentPsyDom;

		typedef PsychicDominatorStateMachine TStateMachine;

		void newStateMachine(CellStruct XY, SuperClass *pSuper) {
			new TStateMachine(XY, pSuper, this);
		}
};
#endif
