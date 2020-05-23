#pragma once
#include <Icetrix.h>
// Created with ReClass.NET 1.2 by KN4CK3R

class Entity
{
public:
	char pad_0000[16]; //0x0000
	class N00000192* checkptr; //0x0010
	char pad_0018[32]; //0x0018
	Vector3 v1; //0x0038
	Vector3 coords; //0x0044
	Vector3 v3; //0x0050
	Vector3 v4; //0x005C
	char pad_0068[228]; //0x0068
	Vector3 rotation_readonly; //0x014C
	Vector3 rotation; //0x0158
	float N000005C4; //0x0164
	float N0000067B; //0x0168
	float N000005C6; //0x016C
	char pad_0170[1820]; //0x0170
	Vector3 b1; //0x088C
	Vector3 b2; //0x0898
	char pad_08A4[36]; //0x08A4
	Vector3 b3; //0x08C8
	char pad_08D4[232]; //0x08D4
	float health_readonly; //0x09BC
	char pad_09C0[328]; //0x09C0
}; //Size: 0x0B08
static_assert(sizeof(Entity) == 0xB08);

class N00000192
{
public:
	char pad_0000[136]; //0x0000
}; //Size: 0x0088
static_assert(sizeof(N00000192) == 0x88);

class N0000034C
{
public:
	class N00000360* N0000034D; //0x0000
	char pad_0008[256]; //0x0008
}; //Size: 0x0108
static_assert(sizeof(N0000034C) == 0x108);

class N00000360
{
public:
	char pad_0000[1240]; //0x0000
	class N00000659* N00000618; //0x04D8
}; //Size: 0x0584

class N00000659
{
public:
	char pad_0000[136]; //0x0000
}; //Size: 0x0088
static_assert(sizeof(N00000659) == 0x88);

class N0000093E
{
public:
	char pad_0000[112]; //0x0000
	class N00000952* N0000094D; //0x0070
	char pad_0078[16]; //0x0078
}; //Size: 0x0088
static_assert(sizeof(N0000093E) == 0x88);

class N00000952
{
public:
	char pad_0000[56]; //0x0000
	class N00000967* N0000095A; //0x0038
	char pad_0040[72]; //0x0040
}; //Size: 0x0088
static_assert(sizeof(N00000952) == 0x88);

class N00000967
{
public:
	char pad_0000[16]; //0x0000
	class N0000097B* N0000096A; //0x0010
	char pad_0018[112]; //0x0018
}; //Size: 0x0088
static_assert(sizeof(N00000967) == 0x88);

class N0000097B
{
public:
	char pad_0000[136]; //0x0000
}; //Size: 0x0088
static_assert(sizeof(N0000097B) == 0x88);

class N00000AF7
{
public:
	char pad_0000[128]; //0x0000
	class N00000CB1* N00000B08; //0x0080
	char pad_0088[2304]; //0x0088
}; //Size: 0x0988
static_assert(sizeof(N00000AF7) == 0x988);

class N00000CB1
{
public:
	char pad_0000[136]; //0x0000
}; //Size: 0x0088
static_assert(sizeof(N00000CB1) == 0x88);

class LocalPlayer1
{
public:
	class N0000047D* N0000046B; //0x0000
	char pad_0008[120]; //0x0008
}; //Size: 0x0080
static_assert(sizeof(LocalPlayer1) == 0x80);

class N0000047D
{
public:
	char pad_0000[32]; //0x0000
	class Entity* N00000482; //0x0020
	char pad_0028[96]; //0x0028
}; //Size: 0x0088
static_assert(sizeof(N0000047D) == 0x88);
