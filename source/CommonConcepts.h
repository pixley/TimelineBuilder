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

template<typename T>
concept NotStringType = !(std::is_same_v<std::remove_cvref_t<T>, class QString>);

template<typename T>
concept VariantCompatible = !(std::is_same_v<std::remove_cvref_t<T>, class QVariant>) && std::is_constructible_v<class QVariant, T>;

// Use in as "IsA<Base> T"
template<typename Derived, typename Base>
concept IsA = std::is_base_of_v<Base, Derived>;