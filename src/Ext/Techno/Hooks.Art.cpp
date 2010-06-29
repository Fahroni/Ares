#include "Body.h"
#include "../TechnoType/Body.h"

/* #604 - customizable parachutes */
DEFINE_HOOK(5F5ADD, Parachute_Animation, 6)
{
	GET(TechnoClass *, T, ESI);
	RET_UNLESS(generic_cast<FootClass *>(T));
	TechnoTypeExt::ExtData *pTypeData = TechnoTypeExt::ExtMap.Find(T->GetTechnoType());
	if(pTypeData->Is_Bomb) {
		T->IsABomb = 1;
	}
	R->EDX<AnimTypeClass *>(pTypeData->Parachute_Anim);
	return 0x5F5AE3;
}

DEFINE_HOOK(73C725, UnitClass_DrawSHP_DrawShadowEarlier, 6)
{
	GET(UnitClass *, U, EBP);

	DWORD retAddr = (U->IsClearlyVisibleTo(HouseClass::Player))
		? 0
		: 0x73CE0D
	;

	if(U->CloakState || U->Type->Underwater) { // TODO: other conditions where it would not make sense to draw shadow - VisualCharacter?
		return retAddr;
	}

	GET(SHPStruct *, Image, EDI);

	if(Image) { // bug #960
		GET(int, FrameToDraw, EBX);
		GET_STACK(Point2D, coords, 0x12C);
		LEA_STACK(RectangleStruct *, BoundingRect, 0x134);

		if(U->unknown_bool_420) {
			coords.Y -= 14;
		}

		Point2D XYAdjust = {0, 0};
		U->Locomotor->Shadow_Point(&XYAdjust);
		coords += XYAdjust;

		int ZAdjust = U->GetZAdjustment() - 2;

		FrameToDraw += Image->Frames / 2;

		DSurface::Hidden_2->DrawSHP(FileSystem::THEATER_PAL, Image, FrameToDraw, &coords, BoundingRect, 0x2E01,
				0, ZAdjust, 0, 1000, 0, 0, 0, 0, 0);
	}

	return retAddr;
}

DEFINE_HOOK(73C733, UnitClass_DrawSHP_SkipTurretedShadow, 7)
{
	return 0x73C7AC;
}

/*
 * this was the old implementation of apcw, no longer needed
A_FINE_HOOK(73B672, UnitClass_DrawVXL, 6)
{
	GET(UnitClass *, U, EBP);
	TechnoTypeExt::ExtData *pData = TechnoTypeExt::ExtMap.Find(U->Type);
	if(pData->WaterAlt) {
		if(!U->OnBridge && U->GetCell()->LandType == lt_Water) {
			R->EAX(0);
			return 0x73B68B;
		}
	}
	return 0;
}
*/

DEFINE_HOOK(73B4A0, UnitClass_DrawVXL_WaterType, 9)
{
	R->ESI(0);
	GET(UnitClass *, U, EBP);
	TechnoExt::ExtData *pData = TechnoExt::ExtMap.Find(U);

	ObjectTypeClass * Image = U->Type;

	if(!U->IsClearlyVisibleTo(HouseClass::Player)) {
		Image = U->GetDisguise(true);
	}

	if(U->Deployed) {
		if(TechnoTypeClass * Unloader = U->Type->UnloadingClass) {
			Image = Unloader;
		}
	}

	if(UnitTypeClass * pCustomType = pData->GetUnitType()) {
		Image = pCustomType;
	}

	R->EBX<ObjectTypeClass *>(Image);
	return 0x73B4DA;
}


DEFINE_HOOK(73C5FC, UnitClass_DrawSHP_WaterType, 6)
{
	GET(UnitClass *, U, EBP);
	TechnoExt::ExtData *pData = TechnoExt::ExtMap.Find(U);

	SHPStruct * Image = U->GetImage();

	if(UnitTypeClass * pCustomType = pData->GetUnitType()) {
		Image = pCustomType->GetImage();
	}

	if(Image) {
		R->EAX<SHPStruct *>(Image);
		return 0x73C602;
	}
	return 0x73CE00;
}

DEFINE_HOOK(73C655, UnitClass_DrawSHP_ChangeType1, 6)
DEFINE_HOOK_AGAIN(73C69D, UnitClass_DrawSHP_ChangeType1, 6)
DEFINE_HOOK_AGAIN(73C702, UnitClass_DrawSHP_ChangeType1, 6)
{
	GET(UnitClass *, U, EBP);
	TechnoExt::ExtData *pData = TechnoExt::ExtMap.Find(U);

	if(UnitTypeClass * pCustomType = pData->GetUnitType()) {
		R->ECX<UnitTypeClass *>(pCustomType);
		return R->get_Origin() + 6;
	}

	return 0;
}