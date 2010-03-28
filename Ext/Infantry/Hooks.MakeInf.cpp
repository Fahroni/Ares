#include <InfantryClass.h>
#include <HouseTypeClass.h>
#include <HouseClass.h>
#include <TActionClass.h>
#include "Body.h"
#include "../WarheadType/Body.h"
#include "../AnimType/Body.h"

#include <SpecificStructures.h>

/* #188 - InfDeaths */

DEFINE_HOOK(5185C8, InfantryClass_ReceiveDamage_InfDeath, 6)
{
	GET(InfantryClass *, I, ESI);
	LEA_STACK(args_ReceiveDamage *, Arguments, 0xD4);
	GET(DWORD, InfDeath, EDI);
	--InfDeath;
	R->EDI(InfDeath);

	bool Handled = false;

	if(!I->Type->NotHuman) {
		if(I->GetHeight() < 10) {
			WarheadTypeExt::ExtData *pWHData = WarheadTypeExt::ExtMap.Find(Arguments->WH);
			if(AnimTypeClass *deathAnim = pWHData->InfDeathAnim) {
				AnimClass *Anim = NULL;
				GAME_ALLOC(AnimClass, Anim, deathAnim, &I->Location);

				AnimTypeExt::SetMakeInfOwner(Anim, Arguments->Attacker->Owner, I->Owner, Arguments->Attacker->Owner);

				Handled = true;
			}
		}
	}

	return (Handled || InfDeath >= 10)
		? 0x5185F1
		: 0x5185CE
	;
}

DEFINE_HOOK(51849A, InfantryClass_ReceiveDamage_DeathAnim, 5)
{
	GET(InfantryClass *, I, ESI);
	LEA_STACK(args_ReceiveDamage *, Arguments, 0xD4);
	GET(DWORD, InfDeath, EDI);

	// if you got here, a valid DeathAnim for this InfDeath has been defined, and the game has already checked the preconditions
	// just allocate the anim and set its owner/remap

	AnimClass *Anim = NULL;
	GAME_ALLOC(AnimClass, Anim, I->Type->DeathAnims[InfDeath], &I->Location);

	AnimTypeExt::SetMakeInfOwner(Anim, I->Owner, I->Owner, Arguments->Attacker->Owner);

	R->EAX<AnimClass *>(Anim);
	return 0x5184F2;
}

DEFINE_HOOK(6E232E, ActionClass_PlayAnimAt, 5)
{
	GET(TActionClass *, pAction, ESI);
	GET_STACK(HouseClass *, pHouse, 0x1C);
	LEA_STACK(CoordStruct *, pCoords, 0xC);

	AnimClass *Anim = NULL;
	AnimTypeClass *AnimType = AnimTypeClass::Array->GetItem(pAction->arg_90);

	GAME_ALLOC(AnimClass, Anim, AnimType, pCoords);

	if(AnimType->MakeInfantry > -1) {
		AnimTypeExt::SetMakeInfOwner(Anim, pHouse, pHouse, pHouse);
	}

	R->EAX<AnimClass *>(Anim);

	return 0x6E2368;
}