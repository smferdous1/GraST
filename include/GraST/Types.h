/*
 * Copyright (C) 2022  Ferdous, S M <ferdous.csebuet@gmail.com>
 * Author: Ferdous, S M <ferdous.csebuet@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once
#include <limits>

//user-defined types
using SIZE_T = unsigned int;
//Change this to float/double if the weight of the graph is real  
using VAL_T = double;
//The summation type
using SUM_T =double;
//Change this to long if number of nodes does not fit into integer
using NODE_T = int;
//Change this to long if number of edges does not fit into integer
//typedef  unsigned int EDGE_T;
using EDGE_T = unsigned long long;
// The color type
using COLOR_T = unsigned int;
constexpr COLOR_T UNCOLORED { std::numeric_limits<COLOR_T>::max() };
