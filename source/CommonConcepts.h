/*
	Copyright (c) 2022 Tyler Pixley, all rights reserved.

	This file (CommonConcepts.h) is part of TimelineBuilder.

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

#include <type_traits>

/*
	Dear C++20,

	Thank you for concepts and making template specialization not frustrating as a result.

	Love,
	Tyler
*/

template<typename T>
concept NumberType = std::is_arithmetic_v<std::remove_reference_t<T>>;

template<typename T>
concept StringType = std::is_same_v<std::remove_cvref_t<T>, class QString>;

// Check whether the container is a QMap or a QHash
// C++20 does not have the means to compare whether two types specialize the same template, so
// we have to do this in a bit of a roundabout way by also bringing in the key and value types.
// There is currently a proposal to have this in C++23, but I'm working on this now, not once it's implemented.
// Reportedly, MSVC does have an implementation for this sort of thing as part of <type_traits>, but
// I'm scared to use it.  Also, it'd just make life harder for anyone who wants to port this to a
// different OS down the line.
template<typename Container, typename Key, typename Value>
concept MapType = std::is_same_v<std::remove_cvref_t<Container>, class QMap<Key, Value>> ||
	std::is_same_v<std::remove_cvref_t<Container>, class QHash<Key, Value>>;

// Check whether a type inherits from JsonableObject
template<typename T>
concept JsonableType = std::is_base_of_v<class JsonableObject, T>;