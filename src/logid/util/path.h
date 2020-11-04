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

#ifndef LOGID_PATH_H
#define LOGID_PATH_H

#include <string>

namespace logid {

    /**
     * Concatenates two strings, treating them as paths, ensuring that there is
     * exactly one path separator between both of them.
     */
    std::string concatPath(const std::string& p1, const std::string& p2);
}

#endif // LOGID_PATH_H
