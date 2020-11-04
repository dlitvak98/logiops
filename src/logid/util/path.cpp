/*
 * Copyright 2019-2020 PixlOne, dlitvak98
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
 *
 */

#include <string>

#include "path.h"

#define PATH_SEP '/'

namespace logid {

    std::string concatPath(const std::string& p1, const std::string& p2) {
        std::string new_path = "";

        char p1_end = p1[p1.length() - 1];
        char p2_start = p2[0];

        if (p1_end != PATH_SEP && p2_start != PATH_SEP)
            // Neither has a separator, so one needs to be added
            return p1 + PATH_SEP + p2;
        else if (p1_end == PATH_SEP && p2_start == PATH_SEP)
            // Both of them have a separator, so we keep only the first one
            return p1 + p2.substr(1);
        else
            // Only one of them has a separator, so we can just concatenate the
            // strings
            return p1 + p2;
    }
}
