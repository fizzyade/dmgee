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

#include "RibbonLineEdit.h"

#include "RibbonFontManager.h"
#include "ThemeSupport.h"

#include <QApplication>
#include <QSpacerItem>

constexpr auto ThemeStylesheet = R"(
    QLineEdit {
        background-color: [background-colour];
        height: 13px;
        border: 1px solid [border-colour];
        padding: 2px;
    }

    QLineEdit:focus {
        border-color: [border-colour];
    }
)";

Nedrysoft::Ribbon::RibbonLineEdit::RibbonLineEdit(QWidget *parent) :
        QLineEdit(parent),
        m_themeSupport(new Nedrysoft::Utils::ThemeSupport){

    setAttribute(Qt::WA_MacShowFocusRect,false);

    connect(m_themeSupport, &Nedrysoft::Utils::ThemeSupport::themeChanged, [=](bool isDarkMode) {
        updateStyleSheet(isDarkMode);
    });

    updateStyleSheet(Nedrysoft::Utils::ThemeSupport::isDarkMode());
}

Nedrysoft::Ribbon::RibbonLineEdit::~RibbonLineEdit() {
}

void Nedrysoft::Ribbon::RibbonLineEdit::updateStyleSheet(bool isDarkMode) {
    QString styleSheet(ThemeStylesheet);

    if (isDarkMode) {
        styleSheet.replace("[background-colour]", "#434343");
        styleSheet.replace("[border-colour]", "none");
    } else {
        styleSheet.replace("[background-colour]", "#ffffff");
        styleSheet.replace("[border-colour]", "#B9B9B9");
    }

    setStyleSheet(styleSheet);
}
