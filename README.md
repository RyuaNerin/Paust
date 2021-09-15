# PartyFilter

- PartyFilter 는 파이널 판타지 14 인게임 파티 찾기에 다양한 필터링과 유틸 기능을 제공하는 프로그램입니다.

- 이 프로그램의 사용으로 인한 모든 피해는 사용자에게 있고, 제작자는 그 어떤 책임도 지지 않습니다.

- 이 프로그램은 [Affero General Public License v3](/LICENSE) 하에 배포됩니다

## [LICENSE](/LICENSE)

## 요구 사항

- Windows 10 환경에서 실행하는 DX11 클라언트.

## 사용방법

- [여기](https://github.com/RyuaNerin/PartyFilter/releases/latest) 에서 마지막 릴리즈를 **적당한 위치** 에 다운로드 해주세요.

- 다운로드 받은 파일을 실행해주세요.

### 상수들

- [party 변수](#party_변수_)
- [파티슬롯](#파티슬롯)
- [직업](#직업)

#### Party 변수

- party 변수는 아래와 같은 형태의 구조를 가집니다.

```json
{
    "owner_name"       : "륜아린", // 파티 모집자 이름
    "description"      : "파티찾기 테스트입니다.",
    "remaining_seconds": 3038,
    "content_category" : 0,
    "content_id"       : 748,
    "is_roulette"      : false,
    "party_count"      : 1,
    "is_private"       : false,
    "is_24"            : false,
    "same_server"      : false,
    "no_dup_job"       : false,
    "welcome_beginner" : false,
    "undersize_player" : false,
    "minimum_level"    : 0,
    "silence_echo"     : false,
    "item_rule"        : 0,
    "slot_count"       : 8,
    "slot": [
        {
            "pgl": false,
            "mrd": false,
            "lnc": false,
            "arc": false,
            "cnj": false,
            "thm": false,
            "pld": false,
            "mnk": false,
            "war": false,
            "drg": false,
            "brd": false,
            "whm": false,
            "blm": false,
            "acn": false,
            "smn": false,
            "sch": false,
            "rog": false,
            "nin": false,
            "mch": false,
            "drk": false,
            "ast": false,
            "sam": false,
            "rdm": false,
            "blu": false,
            "gnb": false,
            "dnc": false,
            "_tank": false,
            "_heal": false,
            "_deal": false,
            "_deal_meele": false,
            "_deal_range": false,
            "_deal_caster": false,
            "gld": false,
            "in_slot": 19,
            "_raw": 256,
            "available_jobs": 0
        },
        ...
    ]
}
```

#### 직업고유번호

|상수|설명|
|:-:|-|
jobs.adv	(int) 모험가
jobs.gla	(int) 검술사
jobs.pgl	(int) 격투사
jobs.mrd	(int) 도끼술사
jobs.lnc	(int) 창술사
jobs.arc	(int) 궁술사
jobs.cnj	(int) 환술사
jobs.thm	(int) 주술사
jobs.crp	(int) 목수
jobs.bsm	(int) 대장장이
jobs.arm	(int) 갑주제작사
jobs.gsm	(int) 보석공예가
jobs.ltw	(int) 가죽공예가
jobs.wvr	(int) 재봉사
jobs.alc	(int) 연금술사
jobs.cul	(int) 요리사
jobs.min	(int) 광부
jobs.btn	(int) 원예가
jobs.fsh	(int) 어부
jobs.pld	(int) 나이트
jobs.mnk	(int) 몽크
jobs.war	(int) 전사
jobs.drg	(int) 용기사
jobs.brd	(int) 음유시인
jobs.whm	(int) 백마도사
jobs.blm	(int) 흑마도사
jobs.acn	(int) 비술사
jobs.smn	(int) 소환사
jobs.sch	(int) 학자
jobs.rog	(int) 쌍검사
jobs.nin	(int) 닌자
jobs.mch	(int) 기공사
jobs.drk	(int) 암흑기사
jobs.ast	(int) 점성술사
jobs.sam	(int) 사무라이
jobs.rdm	(int) 적마도사
jobs.blu	(int) 청마도사
jobs.gnb	(int) 건브레이커
jobs.dnc	(int) 무도가


slot_flags\.$			tank				(int) 탱커
slot_flags\.$			heal				(int) 힐러
slot_flags\.$			deal				(int) 딜러
slot_flags\.$			deal_meele			(int) 근딜
slot_flags\.$			deal_range			(int) 원딜
slot_flags\.$			deal_caster			(int) 캐스터
slot_flags\.$			pgl					(int) 격투가
slot_flags\.$			mrd					(int) 도끼술사
slot_flags\.$			lnc					(int) 창술사
slot_flags\.$			arc					(int) 궁술사
slot_flags\.$			cnj					(int) 환술사
slot_flags\.$			thm					(int) 주술사
slot_flags\.$			pld					(int) 나이트
slot_flags\.$			mnk					(int) 몽크
slot_flags\.$			war					(int) 전사
slot_flags\.$			drg					(int) 용기사
slot_flags\.$			brd					(int) 음유시인
slot_flags\.$			whm					(int) 백마도사
slot_flags\.$			blm					(int) 흑마도사
slot_flags\.$			acn					(int) 비술사
slot_flags\.$			smn					(int) 소환사
slot_flags\.$			sch					(int) 학자
slot_flags\.$			rog					(int) 쌍검사
slot_flags\.$			nin					(int) 닌자
slot_flags\.$			mch					(int) 기공사
slot_flags\.$			drk					(int) 암흑기사
slot_flags\.$			ast					(int) 점성술사
slot_flags\.$			sam					(int) 사무라이
slot_flags\.$			rdm					(int) 적마도사
slot_flags\.$			blu					(int) 청마도사
slot_flags\.$			gnb					(int) 건브레이커
slot_flags\.$			dnc					(int) 무도가

content_ids\.$			e9s					(int) 희망의 낙원 에덴: 재생편(영웅) 1
content_ids\.$			e10s				(int) 희망의 낙원 에덴: 재생편(영웅) 2
content_ids\.$			e11s				(int) 희망의 낙원 에덴: 재생편(영웅) 3
content_ids\.$			e12s				(int) 희망의 낙원 에덴: 재생편(영웅) 4
content_ids\.$			gunnhildr			(int) 군힐드 사원
content_ids\.$			gunnhildrs			(int) 군힐드 사원 (영웅)
content_ids\.$			leviathan			(int) 환 리바이어선 토벌전
content_ids\.$			diamond				(int) 극 다이아몬드 웨폰 포획작전
content_ids\.$			bahamut				(int) 절 바하무트 토벌전
content_ids\.$			ultima				(int) 절 알테마 웨폰 파괴작전
content_ids\.$			alexander			(int) 절 알렉산더 토벌전

item_rules\.$			normal				(int) 일반 전리품 규칙
item_rules\.$			greed				(int) 선입찰 금지
item_rules\.$			master				(int) 파티장 분배

```
