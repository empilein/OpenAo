#pragma once

enum BUILDINGKIND : unsigned char
{
	BUILDINGKIND_1ST_WEAPONSHOP = 0,		// 기본무기파트
	BUILDINGKIND_2ND_WEAPONSHOP = 1,		// 고급무기파트
	BUILDINGKIND_COMPONENTSHOP = 2,		// 소모품파트
	BUILDINGKIND_PETSHOP = 3,		// 펫파트
	BUILDINGKIND_ANTIQUESHOP = 4,		// 골동품파트
	BUILDINGKIND_CITYWARP = 5,		// 워프상점(구 도시워프, 구 BUILDINGKIND_HANGAR)
	BUILDINGKIND_PUBLICOFFICE = 6,		// 관제파트
	BUILDINGKIND_ACTIONHOUSE = 7,		// 경매파트
	BUILDINGKIND_RACETRACK = 8,		// 레이싱파트
	BUILDINGKIND_REFINERY = 9,		// 수리파트 - 보급 상점
	BUILDINGKIND_LABORATORY = 10,	// 연구파트
	BUILDINGKIND_FACTORY = 11,	// 공장파트, 구 BUILDINGKIND_ROBOTHANGAR
	BUILDINGKIND_MILITARYACADEMY = 12,	// 사관학교파트
	BUILDINGKIND_STORE = 13,	// 창고
	BUILDINGKIND_SKILL_SHOP = 14,	// 스킬 상점
	BUILDINGKIND_AUCTION = 15,	// 경매, 물품 대행 상점
	BUILDINGKIND_CITY_OCCUPY_INFO = 16,	// 도시 점령 정보 상점
	BUILDINGKIND_TUNING_COLOR = 17,	// 2005-11-11 by cmkwon, 튜닝 색상 상점
	BUILDINGKIND_ARENA = 18,	// 2007-04-23 by dhjin, ARENA
	BUILDINGKIND_WARPOINT_SHOP = 19,	// 2007-05-17 by dhjin, WARPOINT SHOP
	BUILDINGKIND_BRIEFING_ROOM = 20,	// 2007-07-16 by dhjin, BRIEFING_ROOM
	BUILDINGKIND_TUTORIAL = 21,	// 2007-07-16 by dhjin, BUILDINGKIND_TUTORIAL
	BUILDINGKIND_CITYLEADER_LEADER = 22,	// 2007-08-27 by dhjin, BUILDINGKIND_CITYLEADER_LEADER
	BUILDINGKIND_CITYLEADER_OUTPOST = 23,	// 2007-08-27 by dhjin, BUILDINGKIND_CITYLEADER_OUTPOST
	BUILDINGKIND_LUCKY = 24,	// 2010-04-20 by cmkwon, 신규 러키 머신 구현 - 아머/무기/해피만 사용, 옵션머신은 아래에 따로 추가// 2008-11-04 by dhjin, 럭키머신, BUILDINGKIND_LUCKY
	BUILDINGKIND_WORLDRANKING = 25,	// 2009-02-12 by cmkwon, EP3-3 월드랭킹시스템 구현 - 
	BUILDINGKIND_INFINITY = 30,	// 2009-09-09 ~ 2010 by dhjin, 인피니티 - 
	BUILDINGKIND_INFINITY_SHOP = 31,	// 2009-09-09 ~ 2010 by dhjin, 인피니티 - 상점
	BUILDINGKIND_LUCKY_OPTION_MACHINE = 32,	// 2010-04-20 by cmkwon, 신규 러키 머신 구현 - 옵션머신을 위한 추가
	BUILDINGKIND_DISSOLUTION = 33,	// 2010-08-31 by shcho&jskim 아이템용해 시스템 -
	BUILDINGKIND_TRIGGER_CRYSTAL = 34,	// 2011-10-28 by hskim, EP4 [트리거 시스템] - 크리스탈 시스템
	BUILDINGKIND_PARTSSHOP = 35,	// 2012-02-29 by mspark, 상점 타이틀 넣기 - 파츠상점
	BUILDINGKIND_GEARSHOP = 36,	// 2012-02-29 by mspark, 상점 타이틀 넣기 - 기어상점
	BUILDINGKIND_CARDSHOP = 37,	// 2012-02-29 by mspark, 상점 타이틀 넣기 - 카드상점
	BUILDINGKIND_MYSTERY_SHOP = 38,	// 2012-07-04 by JHAHN, 미스터리 머신 구현
	BUILDINGKIND_INFLBUFF_SHOP = 39,	// 2013-05-09 by hskim, 세력 포인트 개선
	BUILDINGKIND_CASH_SHOP = 100,	// 유료화 상품 상점
};