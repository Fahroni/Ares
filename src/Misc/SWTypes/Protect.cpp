#include "Protect.h"
#include "../../Ares.h"
#include "../../Utilities/Helpers.Alex.h"

bool SW_Protect::HandlesType(int type)
{
	return (type == SuperWeaponType::IronCurtain) || (type == SuperWeaponType::ForceShield);
}

void SW_Protect::Initialize(SWTypeExt::ExtData *pData, SuperWeaponTypeClass *pSW)
{
	int type = pSW->Type;

	// iron curtain and force shield, as well as protect
	pData->SW_AnimHeight = 5;

	if(type == SuperWeaponType::ForceShield) {
		// force shield
		pData->Protect_IsForceShield = true;
		pData->SW_RadarEvent = false;

		pData->Protect_Duration = &RulesClass::Instance->ForceShieldDuration;
		pData->Protect_PlayFadeSoundTime = &RulesClass::Instance->ForceShieldPlayFadeSoundTime;
		pData->Protect_PowerOutageDuration = &RulesClass::Instance->ForceShieldBlackoutDuration;
		pData->SW_WidthOrRange = (float)RulesClass::Instance->ForceShieldRadius;
		pData->SW_Anim = RulesClass::Instance->ForceShieldInvokeAnim;

		pData->EVA_Ready = VoxClass::FindIndex("EVA_ForceShieldReady");

		pData->SW_AITargetingType = SuperWeaponAITargetingMode::ForceShield;
		pData->SW_AffectsHouse = SuperWeaponAffectedHouse::Team;
		pData->SW_AffectsTarget = SuperWeaponTarget::Building;
		pData->SW_RequiresHouse = SuperWeaponAffectedHouse::Team;
		pData->SW_RequiresTarget = SuperWeaponTarget::Building;

		pData->SW_Cursor = MouseCursor::First[MouseCursorType::ForceShield];
		pData->SW_NoCursor = MouseCursor::First[MouseCursorType::NoForceShield];
	} else {
		// iron curtain and protect
		pData->SW_WidthOrRange = 3;
		pData->SW_Height = 3;

		pData->Protect_Duration = &RulesClass::Instance->IronCurtainDuration;
		pData->SW_Anim = RulesClass::Instance->IronCurtainInvokeAnim;

		pData->EVA_Ready = VoxClass::FindIndex("EVA_IronCurtainReady");
		pData->EVA_Detected = VoxClass::FindIndex("EVA_IronCurtainDetected");
		pData->EVA_Activated = VoxClass::FindIndex("EVA_IronCurtainActivated");

		pData->SW_Cursor = MouseCursor::First[MouseCursorType::IronCurtain];
	}
}

void SW_Protect::LoadFromINI(
	SWTypeExt::ExtData *pData, SuperWeaponTypeClass *pSW, CCINIClass *pINI)
{
	const char * section = pSW->ID;

	if(!pINI->GetSection(section)) {
		return;
	}

	INI_EX exINI(pINI);
	pData->Protect_Duration.Read(&exINI, section, "Protect.Duration");
	pData->Protect_PowerOutageDuration.Read(&exINI, section, "Protect.PowerOutage");
	pData->Protect_PlayFadeSoundTime.Read(&exINI, section, "Protect.PlayFadeSoundTime");
}

bool SW_Protect::Launch(SuperClass* pThis, CellStruct* pCoords, byte IsPlayer)
{
	SuperWeaponTypeClass *pSW = pThis->Type;
	SWTypeExt::ExtData *pData = SWTypeExt::ExtMap.Find(pSW);

	if(pThis->IsCharged) {
		CoordStruct Crd;
		CellClass *pTarget = MapClass::Instance->GetCellAt(pCoords);
		pTarget->GetCoords(&Crd);

		// play start sound
		if(pSW->StartSound > -1) {
			VocClass::PlayAt(pSW->StartSound, &Crd, NULL);
		}

		// set up the special sound when the effect wears off
		if(pThis->Type->SpecialSound > -1) {
			pThis->SpecialSoundDuration = (int)pData->Protect_Duration - (int)pData->Protect_PlayFadeSoundTime;
			pThis->SpecialSoundLocation = Crd;
		}

		// shut down power
		if(pData->Protect_PowerOutageDuration > 0) {
			pThis->Owner->CreatePowerOutage(pData->Protect_PowerOutageDuration);
		}

		bool force = pData->Protect_IsForceShield.Get();
		float width = pData->SW_WidthOrRange;
		int height = pData->SW_Height;
		
		auto IronCurtain = [&](ObjectClass* pObj) -> bool {
			if(TechnoClass* pTechno = generic_cast<TechnoClass*>(pObj)) {
				// we shouldn't do anything
				if(pTechno->IsImmobilized || pTechno->IsBeingWarpedOut()) {
					return true;
				}

				// is this thing affected at all?
				if(!pData->IsHouseAffected(pThis->Owner, pTechno->Owner)) {
					return true;
				}

				if(!pData->IsTechnoAffected(pTechno)) {
					return true;
				}

				// protect this techno
				pTechno->IronCurtain(pData->Protect_Duration, pThis->Owner, force);
			}

			return true;
		};

		// protect everything in range
		Helpers::Alex::DistinctCollector<ObjectClass*> items;
		Helpers::Alex::forEachObjectInRange(pCoords, width, height, items.getCollector());
		items.forEach(IronCurtain);
	}

	return true;
}