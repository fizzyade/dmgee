/*
 * Copyright (C) 2020 Adrian Carpenter
 *
 * This file is part of dmgee
 *
 * Created by Adrian Carpenter on 18/11/2020.
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

#ifndef NEDRYSOFT_ILICENCE_H
#define NEDRYSOFT_ILICENCE_H

#include "Nedrysoft.h"

#include <QString>
#include <QWidget>

namespace Nedrysoft {
    /**
     * @brief       The Interface definition for a license.
     *
     * @details     Describes the interface contract that a license must adhere to.
     */
    class ILicence {
        public:
            /**
             * @brief       Returns the indenti for this type of license.
             *
             * @returns     the unqiue identifer.
             */
            virtual QString id() = 0;

            /**
             * @brief       Returns the licence text for this licence.
             *
             * @param[in]   replacements the map containing any field values that should be replaced in the licence text.
             *
             * @returns     A licence as a string.
             */
            virtual QString licence(StringMap replacements) = 0;

            /**
             * @brief       Returns a new viewer widget for this type of license.
             *
             * @returns     the widget.
             */
            virtual QWidget *widget() = 0;
    };
}

Q_DECLARE_METATYPE(Nedrysoft::ILicence *);


#endif // NEDRYSOFT_ILICENCE_H