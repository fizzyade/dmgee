/*
 * Copyright (C) 2020 Adrian Carpenter
 *
 * This file is part of dmgee
 *
 * Created by Adrian Carpenter on 28/11/2020.
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

#ifndef NEDRYSOFT_SETTINGSMANAGER_H
#define NEDRYSOFT_SETTINGSMANAGER_H

#include "Nedrysoft.h"

#include <QSettings>
\
namespace Nedrysoft {
    class SettingsManager {
        public:
            SettingsManager();

            NEDRY_SETTING(QString, "user/fullname", fullname, setFullname, "John Doe");
            NEDRY_SETTING(QString, "user/username", username, setUsername, "john.doe");
            NEDRY_SETTING(QString, "user/email", email, setEmail, "john@example.com");

        private:
            QSettings m_settings;
    };
};

#endif //NEDRYSOFT_SETTINGSMANAGER_H
