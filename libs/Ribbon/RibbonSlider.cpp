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

#include "RibbonSlider.h"

#include "RibbonFontManager.h"
#include "ThemeSupport.h"

#include <QApplication>
#include <QSpacerItem>

constexpr auto ThemeStylesheet = R"(
    QSlider::groove:horizontal {
        height: 4px;
        background: [background-colour];
        margin: 2px 0;
    }

    QSlider::handle:horizontal {
        image: url(':/Nedrysoft/Ribbon/icons/slider-[theme]@2x.png');
        width: 10px;
        height: 10px;
        margin: -5px 0px;
    }
)";

Nedrysoft::Ribbon::RibbonSlider::RibbonSlider(QWidget *parent) :
        QSlider(parent),
        m_themeSupport(new Nedrysoft::Utils::ThemeSupport) {

    setAttribute(Qt::WA_MacShowFocusRect,false);

    connect(m_themeSupport, &Nedrysoft::Utils::ThemeSupport::themeChanged, [=](bool isDarkMode) {
        updateStyleSheet(isDarkMode);
    });

    updateStyleSheet(Nedrysoft::Utils::ThemeSupport::isDarkMode());
}

Nedrysoft::Ribbon::RibbonSlider::~RibbonSlider() {
}

void Nedrysoft::Ribbon::RibbonSlider::updateStyleSheet(bool isDarkMode) {
    QString styleSheet(ThemeStylesheet);

    styleSheet.replace("[theme]", isDarkMode ? "dark" : "light");

    if (isDarkMode) {
        styleSheet.replace("[background-colour]", "#434343");
    } else {
        styleSheet.replace("[background-colour]", "#c8c9c8");
    }

    setStyleSheet(styleSheet);
}
