/*
 * Copyright (C) 2020 Adrian Carpenter
 *
 * https://github.com/fizzyade
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

#ifndef NEDRYSOFT_UTILS_THEMESUPPORT_H
#define NEDRYSOFT_UTILS_THEMESUPPORT_H

#include <QtGlobal>

namespace Nedrysoft::Utils
{
    /**
     * @brief       theme support class
     *
     * @details     used by the macOS platform to detect changes between light and dark mode
     *
     */
    class ThemeSupport
    {
        public:
            /**
             * @brief       Check if dark mode is being used
             *
             * @returns     true if dark mode, else false
             *
             */
#if defined(Q_OS_MACOS)
            static bool isDarkMode();
#else
            static bool isDarkMode()
            {
                return false;
            }
#endif
    };
}

#endif // NEDRYSOFT_UTILS_THEMESUPPORT_H

