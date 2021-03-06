/*
 * Copyright (C) 2020 Adrian Carpenter
 *
 * https://github.com/fizzyade
 *
 * Created by Adrian Carpenter on 07/11/2020.
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

#include "RibbonFontManager.h"

#include <QFontDatabase>

Nedrysoft::Ribbon::RibbonFontManager::RibbonFontManager() :
        m_regularFontId(QFontDatabase::addApplicationFont(":/Nedrysoft/Ribbon/OpenSans/OpenSans-Regular.ttf")),
        m_boldFontId(QFontDatabase::addApplicationFont(":/Nedrysoft/Ribbon/OpenSans/OpenSans-Bold.ttf")) {

}

Nedrysoft::Ribbon::RibbonFontManager *Nedrysoft::Ribbon::RibbonFontManager::getInstance() {
    static auto instance = new RibbonFontManager();

    return instance;
}

QString Nedrysoft::Ribbon::RibbonFontManager::normalFont() const {
    auto families = QFontDatabase::applicationFontFamilies(m_regularFontId);

    if (families.count()) {
        return families.at(0);
    }

    return QString();
}

QString Nedrysoft::Ribbon::RibbonFontManager::boldFont() const {
    auto families = QFontDatabase::applicationFontFamilies(m_boldFontId);

    if (families.count()) {
        return families.at(0);
    }

    return QString();
}
