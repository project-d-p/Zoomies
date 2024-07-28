#pragma once

enum class EMonster : uint8
{
	CRAB,
	LOBSTER,
	MAMMOTH,
	OCTOPUS,
	STAR_FISH,
	SLOTH,
	FOX,
	GIRAFFE,
	LION,
	RABBIT,
	ELEPHANT,
	SABER_TOOTH_TIGER,
	SHEPHERD,
	SKUNK,
	SQUID,
	DOLPHIN,
	EEL,
	PENGUIN,
	SEAL,
	WHALE,
	STINGRAY,
};

class PathManager 
{
public:
	static constexpr std::pair<EMonster, const TCHAR*> FileNamePairs[] = {
		{EMonster::CRAB, TEXT("/Game/model/animals/crab/crab.crab")},
		{EMonster::DOLPHIN, TEXT("/Game/model/animals/dolphin/Dolphin.Dolphin")},
		{EMonster::EEL, TEXT("/Game/model/animals/eel/eel.eel")},
		{EMonster::ELEPHANT, TEXT("/Game/model/animals/elephant/low_poly_elephant.low_poly_elephant")},
		{EMonster::FOX, TEXT("/Game/model/animals/fox_2/fox.fox")},
		{EMonster::GIRAFFE, TEXT("/Game/model/animals/giraffe/giraffe-lowpoly.giraffe-lowpoly")},
		{EMonster::LION, TEXT("/Game/model/animals/lion/low_poly_lion.low_poly_lion")},
		{EMonster::LOBSTER, TEXT("/Game/model/animals/lobstar/Lobster.Lobster")},
		{EMonster::MAMMOTH, TEXT("/Game/model/animals/mammoth/low_poly_mammoth.low_poly_mammoth")},
		{EMonster::OCTOPUS, TEXT("/Game/model/animals/octopus/Octopus.Octopus")},
		{EMonster::PENGUIN, TEXT("/Game/model/animals/penguin/penguin_swimming.penguin_swimming")},
		{EMonster::RABBIT, TEXT("/Game/model/animals/rabbit/low_poly_rabbit.low_poly_rabbit")},
		{EMonster::SABER_TOOTH_TIGER, TEXT("/Game/model/animals/sabertoothTiger/low_poly_sabertooth_tiger.low_poly_sabertooth_tiger")},
		{EMonster::SEAL, TEXT("/Game/model/animals/seal/Seal.Seal")},
		{EMonster::SHEPHERD, TEXT("/Game/model/animals/shepherd/stylized_low_poly_german_shepherd.stylized_low_poly_german_shepherd")},
		{EMonster::SKUNK, TEXT("/Game/model/animals/skunk/low_poly_skunk.low_poly_skunk")},
		{EMonster::SLOTH, TEXT("/Game/model/animals/sloth/low_poly_giant_sloth.low_poly_giant_sloth")},
		{EMonster::SQUID, TEXT("/Game/model/animals/squid/Squid.Squid")},
		{EMonster::STAR_FISH, TEXT("/Game/model/animals/starFish/star_fish.star_fish")},
		{EMonster::STINGRAY, TEXT("/Game/model/animals/stingRay/sting_ray.sting_ray")},
		{EMonster::WHALE, TEXT("/Game/model/animals/whale/Whale.Whale")},
	};

	static constexpr const TCHAR* GetPath(EMonster MType)
	{
		switch(MType)
		{
		case EMonster::CRAB: return FileNamePairs[0].second;
		case EMonster::DOLPHIN: return FileNamePairs[1].second;
		case EMonster::EEL: return FileNamePairs[2].second;
		case EMonster::ELEPHANT: return FileNamePairs[3].second;
		case EMonster::FOX: return FileNamePairs[4].second;
		case EMonster::GIRAFFE: return FileNamePairs[5].second;
		case EMonster::LION: return FileNamePairs[6].second;
		case EMonster::LOBSTER: return FileNamePairs[7].second;
		case EMonster::MAMMOTH: return FileNamePairs[8].second;
		case EMonster::OCTOPUS: return FileNamePairs[9].second;
		case EMonster::PENGUIN: return FileNamePairs[10].second;
		case EMonster::RABBIT: return FileNamePairs[11].second;
		case EMonster::SABER_TOOTH_TIGER: return FileNamePairs[12].second;
		case EMonster::SEAL: return FileNamePairs[13].second;
		case EMonster::SHEPHERD: return FileNamePairs[14].second;
		case EMonster::SKUNK: return FileNamePairs[15].second;
		case EMonster::SLOTH: return FileNamePairs[16].second;
		case EMonster::SQUID: return FileNamePairs[17].second;
		case EMonster::STAR_FISH: return FileNamePairs[18].second;
		case EMonster::STINGRAY: return FileNamePairs[19].second;
		case EMonster::WHALE: return FileNamePairs[20].second;
			default: check(0);
		}
		return nullptr;
	}
};
