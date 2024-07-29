#pragma once

#include "EnumTypes.h"
#include "ScoreTypes.h"

class PathManager 
{
public:
	static constexpr const TCHAR* GetOccupationImagePath(EPlayerJob OType)
	{
		switch (OType)
		{
		case EPlayerJob::JOB_ARCHAEOLOGIST: return TEXT("/Game/image/character/pickaxe");
		case EPlayerJob::JOB_POACHER: return TEXT("/Game/image/character/gun");
		case EPlayerJob::JOB_ENVIRONMENTALIST: return TEXT("/Game/image/character/earth");
		case EPlayerJob::JOB_RINGMASTER: return TEXT("/Game/image/character/circus");
		case EPlayerJob::JOB_TERRORIST: return TEXT("/Game/image/character/bomb");
		case EPlayerJob::JOB_CHECK: return TEXT("/Game/image/ui/check");
		case EPlayerJob::JOB_CROSS: return TEXT("/Game/image/ui/cross");
			default: check(false) return nullptr;;
		}
	}
	
	static constexpr const TCHAR* GetWidgetPath(EWidget WType)
	{
		switch(WType)
		{
		case EWidget::JUDGE_LEVEL: return TEXT("/Game/widget/widget_judge.widget_judge_C");
		case EWidget::CALCULATE_WIDGET: return TEXT("/Game/widget/widget_calculate.widget_calculate_C");
		case EWidget::RESULT_WIDGET: return TEXT("/Game/widget/widget_result.widget_result_C");
		case EWidget::BOARD_ACTOR: return TEXT("/Game/widget/resultWidgetActor.resultWidgetActor_C");
		default: 
			checkf(false, TEXT("Invalid widget type: %d"), static_cast<int>(WType)) return nullptr;;
		}
	}
	
	static constexpr const TCHAR* GetMonsterPath(EAnimal MType)
	{
		switch(MType)
		{
		case EAnimal::ANIMAL_CRAB: return TEXT("/Game/model/animals/crab/crab.crab");
		case EAnimal::ANIMAL_DOLPHIN: return TEXT("/Game/model/animals/dolphin/Dolphin.Dolphin");
		case EAnimal::ANIMAL_EEL: return TEXT("/Game/model/animals/eel/eel.eel");
		case EAnimal::ANIMAL_ELEPHANT: return TEXT("/Game/model/animals/elephant/low_poly_elephant.low_poly_elephant");
		case EAnimal::ANIMAL_FOX: return TEXT("/Game/model/animals/fox_2/fox.fox");
		case EAnimal::ANIMAL_GIRAFFE: return TEXT("/Game/model/animals/giraffe/giraffe-lowpoly.giraffe-lowpoly");
		case EAnimal::ANIMAL_LION: return TEXT("/Game/model/animals/lion/low_poly_lion.low_poly_lion");
		case EAnimal::ANIMAL_LOBSTER: return TEXT("/Game/model/animals/lobstar/Lobster.Lobster");
		case EAnimal::ANIMAL_MAMMOTH: return TEXT("/Game/model/animals/mammoth/low_poly_mammoth.low_poly_mammoth");
		case EAnimal::ANIMAL_OCTOPUS: return TEXT("/Game/model/animals/octopus/Octopus.Octopus");
		case EAnimal::ANIMAL_PENGUIN: return TEXT("/Game/model/animals/penguin/penguin_swimming.penguin_swimming");
		// case EMonster::RABBIT: return TEXT("/Game/model/animals/rabbit/low_poly_rabbit.low_poly_rabbit");
		case EAnimal::ANIMAL_SABER_TOOTH_TIGER: return TEXT("/Game/model/animals/sabertoothTiger/low_poly_sabertooth_tiger.low_poly_sabertooth_tiger");
		case EAnimal::ANIMAL_SEAL: return TEXT("/Game/model/animals/seal/Seal.Seal");
		// case EMonster::SHEPHERD: return TEXT("/Game/model/animals/shepherd/stylized_low_poly_german_shepherd.stylized_low_poly_german_shepherd");
		case EAnimal::ANIMAL_SKUNK: return TEXT("/Game/model/animals/skunk/low_poly_skunk.low_poly_skunk");
		case EAnimal::ANIMAL_SLOTH: return TEXT("/Game/model/animals/sloth/low_poly_giant_sloth.low_poly_giant_sloth");
		case EAnimal::ANIMAL_SQUID: return TEXT("/Game/model/animals/squid/Squid.Squid");
		case EAnimal::ANIMAL_STAR_FISH: return TEXT("/Game/model/animals/starFish/star_fish.star_fish");
		case EAnimal::ANIMAL_STINGRAY: return TEXT("/Game/model/animals/stingRay/sting_ray.sting_ray");
		case EAnimal::ANIMAL_WHALE: return TEXT("/Game/model/animals/whale/Whale.Whale");
		case EAnimal::ANIMAL_HORSE: return TEXT("/Game/model/animals/horse/low_poly_western_horse.low_poly_western_horse");
		case EAnimal::ANIMAL_CHAMELEON: return TEXT("/Game/model/animals/chamaleon/lowpoly_animated_chamaleon_game_ready.lowpoly_animated_chamaleon_game_ready");
		default:
			checkf(false, TEXT("Invalid monster type: %d"), static_cast<int>(MType)) return nullptr;;
		}
	}
	
	static constexpr const TCHAR* GetMonsterAnimationPath(EAnimal MType)
	{
		switch (MType)
		{
		case EAnimal::ANIMAL_CRAB: return TEXT("/Game/animation/animals/crabAnimation.crabAnimation_C");
		case EAnimal::ANIMAL_DOLPHIN: return TEXT("/Game/animation/animals/dolphinAnimation.dolphinAnimation_C");
		case EAnimal::ANIMAL_EEL: return TEXT("/Game/animation/animals/eelAnimation.eelAnimation_C");
		case EAnimal::ANIMAL_ELEPHANT: return TEXT("/Game/animation/animals/elephantAnimation.elephantAnimation_C");
		case EAnimal::ANIMAL_FOX: return TEXT("/Game/animation/animals/foxAnimation.foxAnimation_C");
		case EAnimal::ANIMAL_GIRAFFE: return TEXT("/Game/animation/animals/giraffeAnimation.giraffeAnimation_C");
		case EAnimal::ANIMAL_LION: return TEXT("/Game/animation/animals/lionAnimation.lionAnimation_C");
		case EAnimal::ANIMAL_LOBSTER: return TEXT("/Game/animation/animals/lobsterAnimation.lobsterAnimation_C");
		case EAnimal::ANIMAL_MAMMOTH: return TEXT("/Game/animation/animals/mammothAnimation.mammothAnimation_C");
		case EAnimal::ANIMAL_OCTOPUS: return TEXT("/Game/animation/animals/octopusAnimation.octopusAnimation_C");
		case EAnimal::ANIMAL_PENGUIN: return TEXT("/Game/animation/animals/penguinAnimation.penguinAnimation_C");
		case EAnimal::ANIMAL_SABER_TOOTH_TIGER: return TEXT("/Game/animation/animals/saberToothTigerAnimation.saberToothTigerAnimation_C");
		case EAnimal::ANIMAL_SEAL: return TEXT("/Game/animation/animals/sealAnimation.sealAnimation_C");
		case EAnimal::ANIMAL_SKUNK: return TEXT("/Game/animation/animals/skunkAnimation.skunkAnimation_C");
		case EAnimal::ANIMAL_SLOTH: return TEXT("/Game/animation/animals/slothAnimation.slothAnimation_C");
		case EAnimal::ANIMAL_SQUID: return TEXT("/Game/animation/animals/squidAnimation.squidAnimation_C");
		case EAnimal::ANIMAL_STAR_FISH: return TEXT("/Game/animation/animals/starFishAnimation.starFishAnimation_C");
		case EAnimal::ANIMAL_STINGRAY: return TEXT("/Game/animation/animals/stingRayAnimation.stingRayAnimation_C");
		case EAnimal::ANIMAL_WHALE: return TEXT("/Game/animation/animals/whaleAnimation.whaleAnimation_C");
		case EAnimal::ANIMAL_HORSE: return TEXT("/Game/animation/animals/horseAnimation.horseAnimation_C");
		case EAnimal::ANIMAL_CHAMELEON: return TEXT("/Game/animation/animals/chameleonAnimation.chameleonAnimation_C");
		default:
			checkf(false, TEXT("Invalid monster type: %d"), static_cast<int>(MType)) return nullptr;;
		}
	}
};
