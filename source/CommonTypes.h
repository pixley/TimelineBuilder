/*
	Copyright (c) 2022 Tyler Pixley, all rights reserved.

	This file (CommonTypes.h) is part of TimelineBuilder.

	TimelineBuilder is free software: you can redistribute it and/or modify it under
	the terms of the GNU General Public License as published by the Free Software
	Foundation, either version 3 of the License, or (at your option) any later version.

	TimelineBuilder is distributed in the hope that it will be useful, but WITHOUT ANY
	WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
	PARTICULAR PURPOSE. See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along with
	TimelineBuilder. If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

/*
	Unreal-style names for integer types because I like them.
*/
typedef unsigned char uint8;
typedef signed char int8;
typedef unsigned short uint16;
typedef signed short int16;
typedef unsigned int uint32;
typedef signed int int32;
typedef unsigned long long uint64;
typedef signed long long int64;
typedef float float32;
typedef double float64;

/*
	Project-wide selection as to what the default map container should be
*/
#define TBMap QHash
//#define TBMap QMap

/*
	Boundary values

	An IEEE-754 double-precision mantissa is 52 bits, meaning that it is possible for a float64 to represent numbers up to 2^53
	with at least integer precision.  This effectively provides bounds on the conversion between float64 and int64.
	Incidentally, 2^53 is over 9 quadrillion, which means that only the most ridiculous timelines would even reach these bounds.
	Specifically, in real life, 2^53 days is about 24.66 TRILLION YEARS.  THE SUN'S ENTIRE LIFESPAN WOULD BARELY DENT THAT.

	Now, you may ask, why not just use 32-bit floats and ints instead?  Because that would only allow for integer precision
	up to just shy of 46 thousand years.  While that would work for a real-life, Golarion, or Forgotten Realms timeline,
	modern PCs have more than enough space to handle 64-bit values for this stuff.  And it's not like I'm bothering with a 32-bit
	build in freakin' 2022 anyway.
*/
const int64 MAX_I64_TO_F64 = 1ll << 53;
const int64 MIN_I64_TO_F64 = -(1ll << 53);
const float64 MAX_F64_TO_I64 = 0x1p53;
const float64 MIN_F64_TO_I64 = -0x1p53;

/*
	Enum helper macro and template to allow for bounds-checking.
	These assume that the enum is continuous and that the specific numeric values the enum aren't important.
	This is not a general-case solution.  DO NOT USE THIS FOR BITFLAG ENUMS.
*/
#define ENUM_CLASS(enumTypeName, baseType, ...) \
enum class enumTypeName : baseType \
{ \
	_INVALIDVALUE_ = 0, \
	__VA_ARGS__, \
	_MAXVALUE_ \
};

template <typename enumType>
bool EnumValueIsValid(enumType testVal)
{
	return testVal > enumType::_INVALIDVALUE_ && testVal < enumType::_MAXVALUE_;
}

ENUM_CLASS(TBSignificance, uint8,
	Monumental,
	Major,
	Moderate,
	Minor,
	Minimal
)

ENUM_CLASS(TBEventType, uint8,
	Conflict,
	Disaster,
	Cosmic,
	Religious,
	Innovation
)