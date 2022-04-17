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
	Boundary values
*/
// An IEEE-754 double-precision mantissa is 52 bits, meaning that it is possible for a float64 to represent numbers up to 2^53
// with at least integer precision.  This effectively provides bounds on the conversion between float64 and int64.
// Incidentally, 2^53 is over 9 quadrillion, which means that only the most ridiculous timelines would even reach these bounds.
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

ENUM_CLASS(TBDateFormat, uint8,
	MDY,
	DMY,
	YMD
)

ENUM_CLASS(TBMonthFormat, uint8,
	Name,
	Abbreviation,
	Number
)

ENUM_CLASS(TBWeekdayFormat, uint8,
	None,
	Full,
	Abbreviated
)

ENUM_CLASS(TBDateRounding, uint8,
	Floor,
	Nearest,
	Ceiling,
	ToZero,
	FromZero
)