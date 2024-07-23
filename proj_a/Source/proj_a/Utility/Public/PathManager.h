#pragma once

#include "EnumTypes.h"

class PathManager 
{
public:
	static constexpr const TCHAR* GetOccupationImagePath(EOccupation OType)
	{
		switch (OType)
		{
		case EOccupation::ARCHAEOLOGIST: return TEXT("/Game/image/character/pickaxe");
		case EOccupation::POACHER: return TEXT("/Game/image/character/gun");
		case EOccupation::ENVIRONMENTALIST: return TEXT("/Game/image/character/earth");
		case EOccupation::RINGMASTER: return TEXT("/Game/image/character/circus");
		case EOccupation::TERRORIST: return TEXT("/Game/image/character/bomb");
		case EOccupation::CHECK: return TEXT("/Game/image/ui/check");
		case EOccupation::CROSS: return TEXT("/Game/image/ui/cross");
			default: check(false) return nullptr;;
		}
	}
	
	static constexpr const TCHAR* GetWidgetPath(EWidget WType)
	{
		switch(WType)
		{
		case EWidget::JUDGE_LEVEL: return TEXT("/Game/widget/widget_judge.widget_judge_C");
		default: 
			checkf(false, TEXT("Invalid widget type: %d"), static_cast<int>(WType)) return nullptr;;
		}
	}
	
	static constexpr const TCHAR* GetMonsterPath(EMonster MType)
	{
		switch(MType)
		{
		case EMonster::CRAB: return TEXT("/Game/model/animals/crab/crab.crab");
		case EMonster::DOLPHIN: return TEXT("/Game/model/animals/dolphin/Dolphin.Dolphin");
		case EMonster::EEL: return TEXT("/Game/model/animals/eel/eel.eel");
		case EMonster::ELEPHANT: return TEXT("/Game/model/animals/elephant/low_poly_elephant.low_poly_elephant");
		case EMonster::FOX: return TEXT("/Game/model/animals/fox_2/fox.fox");
		case EMonster::GIRAFFE: return TEXT("/Game/model/animals/giraffe/giraffe-lowpoly.giraffe-lowpoly");
		case EMonster::LION: return TEXT("/Game/model/animals/lion/low_poly_lion.low_poly_lion");
		case EMonster::LOBSTER: return TEXT("/Game/model/animals/lobstar/Lobster.Lobster");
		case EMonster::MAMMOTH: return TEXT("/Game/model/animals/mammoth/low_poly_mammoth.low_poly_mammoth");
		case EMonster::OCTOPUS: return TEXT("/Game/model/animals/octopus/Octopus.Octopus");
		case EMonster::PENGUIN: return TEXT("/Game/model/animals/penguin/penguin_swimming.penguin_swimming");
		case EMonster::RABBIT: return TEXT("/Game/model/animals/rabbit/low_poly_rabbit.low_poly_rabbit");
		case EMonster::SABER_TOOTH_TIGER: return TEXT("/Game/model/animals/sabertoothTiger/low_poly_sabertooth_tiger.low_poly_sabertooth_tiger");
		case EMonster::SEAL: return TEXT("/Game/model/animals/seal/Seal.Seal");
		case EMonster::SHEPHERD: return TEXT("/Game/model/animals/shepherd/stylized_low_poly_german_shepherd.stylized_low_poly_german_shepherd");
		case EMonster::SKUNK: return TEXT("/Game/model/animals/skunk/low_poly_skunk.low_poly_skunk");
		case EMonster::SLOTH: return TEXT("/Game/model/animals/sloth/low_poly_giant_sloth.low_poly_giant_sloth");
		case EMonster::SQUID: return TEXT("/Game/model/animals/squid/Squid.Squid");
		case EMonster::STAR_FISH: return TEXT("/Game/model/animals/starFish/star_fish.star_fish");
		case EMonster::STINGRAY: return TEXT("/Game/model/animals/stingRay/sting_ray.sting_ray");
		case EMonster::WHALE: return TEXT("/Game/model/animals/whale/Whale.Whale");
		default:
			checkf(false, TEXT("Invalid monster type: %d"), static_cast<int>(MType)) return nullptr;;
		}
	}
};
