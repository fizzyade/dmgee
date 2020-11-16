/*
 * Copyright (C) 2020 Adrian Carpenter
 *
 * https://github.com/fizzyade
 *
 * Created by Adrian Carpenter on 15/11/2020.
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

#include "RibbonCheckBox.h"

#include "RibbonFontManager.h"
#include "ThemeSupport.h"

#include <QApplication>
#include <QSpacerItem>

constexpr auto ThemeStylesheet = R"(
    QCheckBox::indicator:checked {
        image: url(':/Nedrysoft/Ribbon/icons/checked-[theme]@2x.png');
        width: 14px;
        height: 14px;
        padding-right: 2px;
    }

    QCheckBox::indicator:checked:hover {
        image: url(':/Nedrysoft/Ribbon/icons/checked-hover-[theme]@2x.png');
        width: 14px;
        height: 14px;
        padding-right: 2px;
    }

    QCheckBox::indicator:unchecked {
        image: url(':/Nedrysoft/Ribbon/icons/unchecked-[theme]@2x.png');
        width: 14px;
        height: 14px;
        padding-right: 2px;
    }

    QCheckBox::indicator:unchecked:hover {
        image: url(':/Nedrysoft/Ribbon/icons/unchecked-hover-[theme]@2x.png');
        width: 14px;
        height: 14px;
        padding-right: 2px;
    }
)";

Nedrysoft::Ribbon::RibbonCheckBox::RibbonCheckBox(QWidget *parent) :
        QCheckBox(parent),
        m_themeSupport(new Nedrysoft::Utils::ThemeSupport) {

    setAttribute(Qt::WA_MacShowFocusRect,false);

    connect(m_themeSupport, &Nedrysoft::Utils::ThemeSupport::themeChanged, [=](bool isDarkMode) {
        updateStyleSheet(isDarkMode);
    });

    updateStyleSheet(Nedrysoft::Utils::ThemeSupport::isDarkMode());
}

Nedrysoft::Ribbon::RibbonCheckBox::~RibbonCheckBox() {
}

void Nedrysoft::Ribbon::RibbonCheckBox::updateStyleSheet(bool isDarkMode) {
    QString styleSheet(ThemeStylesheet);

    styleSheet.replace("[theme]", isDarkMode ? "dark" : "light");

    setStyleSheet(styleSheet);
}
