#pragma once

struct ArmorInfo
{
	short ID; // 0x0000
	char XBIRKLTEVJXQLQVQHJFD[4]; // 0x0002 - 0x0004
	short Model; // 0x0006
	char BAWFEADRYUTICYVCTMXT[34]; // 0x0008 - 0x0028
	short Shader1; // 0x002A
	char BBVVUSCEGJAUBMQYOPAX[2]; // 0x002C
	short Shader2; // 0x002E
	char VWCURNFEZLGKGHMZTPPI[2]; // 0x0030
	short Shader3; // 0x0032
}; // Total size of ArmorInfo is 52 bytes

/*struct WeaponInfo
{
	short ID = -1; // 0x0000
	short Model; // 0x0002
	char OCWUIEISESNGSSGNFXJF[34]; // 0x0004 - 0x0024
	short Shader1; // 0x0026
	char CEAGHBAWIGJTAUIAGPBL[2]; // 0x0028
	short Shader2; // 0x002A
	char ZGEYCYMSAZASDEHDCDEM[2]; // 0x002C
	short Shader3; // 0x002E
}; // Total size of WeaponInfo is 48 bytes
*/

struct ShipInfo
{
	short ID = -1; // 0x0000
	short Model; // 0x0002
	/*short ID; // 0x0000
	short Model; // 0x0002
	char OCWUIEISESNGSSGNFXJF[34]; // 0x0004 - 0x0024
	short Shader1; // 0x0026
	char CEAGHBAWIGJTAUIAGPBL[2]; // 0x0028
	short Shader2; // 0x002A
	char ZGEYCYMSAZASDEHDCDEM[2]; // 0x002C
	short Shader3; // 0x002E*/
}; // Total size of SparrowInfo is 48 bytes

struct SparrowInfo
{
	short ID = -1; // 0x0000
	short Model; // 0x0002
	/*short ID; // 0x0000
	short Model; // 0x0002
	char OCWUIEISESNGSSGNFXJF[34]; // 0x0004 - 0x0024
	short Shader1; // 0x0026
	char CEAGHBAWIGJTAUIAGPBL[2]; // 0x0028
	short Shader2; // 0x002A
	char ZGEYCYMSAZASDEHDCDEM[2]; // 0x002C
	short Shader3; // 0x002E*/
}; // Total size of SparrowInfo is 48 bytes

struct GhostInfo
{
	short ID; // 0x0000
	char XBIRKLTEVJXQLQVQHJFD[4]; // 0x0002 - 0x0004
	short Model; // 0x0006
	char BAWFEADRYUTICYVCTMXT[34]; // 0x0008 - 0x0028
	short Shader1; // 0x002A
	char BBVVUSCEGJAUBMQYOPAX[2]; // 0x002C
	short Shader2; // 0x002E
	char VWCURNFEZLGKGHMZTPPI[2]; // 0x0030
	short Shader3; // 0x0032
}; // Total size of GhostInfo is 52 bytes

/*class PlayerInventory
{
public:
	short PlayerID; // 0x0000
	char YPVHMRLFKUMTCMMOOJNS[62]; // 0x0002 - 0x003E
	float PowerLevel; // 0x0040
	char HKZVTXGHJFNVVVEEFNTC[4]; // 0x0044 - 0x0046
	short Grenade; // 0x0048
	char LPGSWBFTVULOVXMURAUM[66]; // 0x004A - 0x008A
	short Super; // 0x008C
	char PEVHCWBGFZNJRKEETVKH[66]; // 0x008E - 0x00CE
	short Melee; // 0x00D0
	char BSACUFLBTEYLRRFVVDCN[66]; // 0x00D2 - 0x0112
	short Jump; // 0x0114
	char LIQOGNLXQHWVDTYQHARO[134]; // 0x0116 - 0x019A
	short AirMove; // 0x019C
	char ZCKWJVMNSDKCZLVSMBCG[406]; // 0x019E - 0x0332
	short ClassAbility; // 0x0334	 //2C difference
	char RIAQPHLNHCUYOJDADMER[354]; // 0x0336 - 0x0496
	ArmorInfo Helmet; // 0x0498 - 0x04CA
	char LVBLPLRLEEKCILJUZLXX[28]; // 0x04CC - 0x04E6
	ArmorInfo Gauntlets; // 0x04E8 - 0x051A 
	char WXGXBPNSUNUNZGJCLWLM[108]; // 0x051C - 0x0586
	ArmorInfo Chest; // 0x0588 - 0x05BA
	char QHSHOTUHCFRTMCONCQLN[28]; // 0x05BC - 0x05D6
	ArmorInfo Boots; // 0x05D8 - 0x060A
	char WCDGAKMSXEZNZQJJPVLY[28]; // 0x060C - 0x0626
	ArmorInfo Class; // 0x0628 - 0x065A
	char DWCMVKJFVDDJDJXURKQG[32]; // 0x065C - 0x067A
	WeaponInfo Kinetic; // 0x067C - 0x06AA
	char IHXLXYWIFJJYQEUTJGIE[32]; // 0x06AC - 0x06CA
	WeaponInfo Energy; // 0x06CC - 0x06FA
	char AVSJLHCCFSYUOVWFTUTL[32]; // 0x06FC - 0X076A
	WeaponInfo Heavy; // 0x071C - 0x074A
	char SUCXZXWLZDKXAYAVDXEL[32]; // 0x074C - 0x076A
	ShipInfo Ship; // 0x076C - 0x079A
	char DUHASDBHGAWUYEQEBASD[32]; // 0x079C - 0x07BA
	SparrowInfo Sparrow; // 0x07BC - 0x07EA
	char SDPHEIPDMJSYOIKAQFVH[28]; // 0x07EC - 0x0806
	GhostInfo Ghost; // 0x0808 - 0x083A
	char GKUXXFLCYCMQFRTUNSYP[28]; // 0x083C - 0x0856
	short Emblem; // 0x0858
	char GJDFUHFNNCBASDHBGWAD[86]; // 0x085A - 0x08AE
	short TopEmote; // 0x08B0
	short BottomEmote; // 0x08B2
	short LeftEmote; // 0x08B4
	short RightEmote; // 0x08B6

	char PUSIWBIBDYMNQZUPCTRR[64]; // 0x08B8 - 0x08F6
	short ClanBanner; // 0x08F8
	char GJASDIGJAIDHABIUGBAS[238]; // 0x08FA - 0x09E6
	short Artifact; // 0x09E8
	char AIUHIFHAWUIDASDBJHBF[278]; // 0x09EA - 0x0AFE
	short ArmorPerks[69]; // 0x0B00 - 0x0B88
	char KDBHXVRSGINSLDTOPZSE[6]; // 0x0B8A - 0x0B8E
	short KineticPerks[11]; // 0x0B90 - 0x0BA4
	char HACFGOQNQOTEJFADCFJO[10]; // 0x0BA6 - 0x0BAE
	short EnergyPerks[11]; // 0x0BB0 - 0x0BC4
	char MFQLWHROHUBBKOTGGPHX[10]; // 0x0BC6 - 0x0BCE
	short HeavyPerks[11]; // 0x0BD0 - 0x0BE4
	char EPWBBVNQUWMIMUHTLCCR[30]; // 0x0BE6 - 0x0C02
	short Mobility; // 0x0C04
	char UQPGIXGFQEBMPZJGTSZQ[6]; // 0x0C06 - 0x0C0A
	short Resilience; // 0x0C0C
	char FTKIOEQWWXILNROSWQDN[6]; // 0x0C0E - 0x0C12
	short Recovery; // 0x0C14
	char MQDMCDBKQVIJGLSITNPD[6]; // 0x0C16 - 0x0C1A
	short Discipline; // 0x0C1C
	char ZCIPZPNWEWXQWMTZXAYC[6]; // 0x0C1E - 0x0C22
	short Intellect; // 0x0C24
	char DLUESNGYARAEZOKBENZD[6]; // 0x0C26 - 0x0C2A
	short Strength; // 0x0C2C
	char LKHFEWDCBNJYRECCHIIT[1024]; // 0x0C2E - 0x102C
	short Emblem2; // 0x102E
};*/


class WeaponInfo
{
public:
	int16_t ID; //0x0000
	int16_t Model; //0x0002
	char pad_0004[4]; //0x0004
}; //Size: 0x0008
static_assert(sizeof(WeaponInfo) == 0x8);

class PlayerInventory
{
public:
	int16_t PlayerId; //0x0000
	char pad_0002[62]; //0x0002
	int32_t N00003B03; //0x0040
	float PowerLevel; //0x0044
	int32_t Grenade; //0x0048
	char pad_004C[68]; //0x004C
	int32_t Super; //0x0090
	char pad_0094[68]; //0x0094
	int32_t Melee; //0x00D8
	char pad_00DC[68]; //0x00DC
	int32_t Jump; //0x0120
	char pad_0124[68]; //0x0124
	int32_t AirMove; //0x0168
	char pad_016C[500]; //0x016C
	int32_t ClassAbility; //0x0360
	char pad_0364[492]; //0x0364
	int32_t HelmetStuffStartsHere; //0x0550
	char pad_0554[76]; //0x0554
	int32_t GauntlessStuffHere; //0x05A0
	char pad_05A4[156]; //0x05A4
	int32_t ChestStuffHere; //0x0640
	char pad_0644[76]; //0x0644
	int32_t BootsStuffHere; //0x0690
	char pad_0694[76]; //0x0694
	int16_t ClassStuffHere; //0x06E0
	char pad_06E2[90]; //0x06E2
	class WeaponInfo Kinetic; //0x073C
	char pad_0744[72]; //0x0744
	class WeaponInfo Energy; //0x078C
	char pad_0794[72]; //0x0794
	class WeaponInfo Heavy; //0x07DC
	char pad_07E4[60]; //0x07E4
	int16_t ShipStuffHere; //0x0820
	char pad_0822[78]; //0x0822
	int16_t SparrowStuffHere; //0x0870
	char pad_0872[78]; //0x0872
	int16_t GhostStuffHere; //0x08C0
	char pad_08C2[78]; //0x08C2
	int16_t EmblemStuffHere; //0x0910
	char pad_0912[94]; //0x0912
	int16_t TopEmote; //0x0970
	int16_t BottomEmote; //0x0972
	int16_t LeftEmote; //0x0974
	int16_t RightEmote; //0x0976
	char pad_0978[584]; //0x0978
	int16_t ArmorPerks[134]; //0x0BC0
	char pad_0CCC[8]; //0x0CCC
	int16_t KineticPerks[29]; //0x0CD4
	int16_t EnergyPerks[29]; //0x0D0E
	int16_t HeavyPerks[29]; //0x0D48
	char pad_0D82[10]; //0x0D82
	int16_t PowerLevel2; //0x0D8C
	char pad_0D8E[6]; //0x0D8E
	int16_t PowerLevel3; //0x0D94
	char pad_0D96[6]; //0x0D96
	int16_t Mobility; //0x0D9C
	char pad_0D9E[6]; //0x0D9E
	int16_t Resilience; //0x0DA4
	char pad_0DA6[6]; //0x0DA6
	int16_t Recovery; //0x0DAC
	char pad_0DAE[6]; //0x0DAE
	int16_t Discipline; //0x0DB4
	char pad_0DB6[6]; //0x0DB6
	int16_t Intellect; //0x0DBC
	char pad_0DBE[6]; //0x0DBE
	int16_t Strength; //0x0DC4
}; //Size: 0x0D48

