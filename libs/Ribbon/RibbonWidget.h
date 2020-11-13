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

#ifndef NEDRYSOFT_RIBBONWIDGET_H
#define NEDRYSOFT_RIBBONWIDGET_H

#include "RibbonSpec.h"
#include <QTabWidget>
#include <QHBoxLayout>
#include <QMetaProperty>

namespace Nedrysoft::Ribbon {
    class RibbonTabBar;

    enum ThemeMode {
        Light = 0,                                                              //! Draw ribbon in light mode
        Dark = 1                                                                //! Draw ribbon in dark mode
    };

    static constexpr QRgb TextColor[] = {                                       //! Colour of text on the control
        qRgb(73, 69, 66),
        qRgb(0xDC, 0xDC, 0xDC)
    };


    static constexpr QRgb TabBarBackgroundColor[] = {                           //! Background colour of the tab bar
        qRgb(0xf5, 0xf5, 0xf5),
        qRgb(0x2d, 0x2d, 0x2d)
    };

    static constexpr QRgb TabHoverColor[] = {                                   //! Highlight colour of a tab when hovered over
        qRgb(254, 251, 255),
        TabBarBackgroundColor[Dark]
    };

    static constexpr QRgb TabSelectedColor[] = {                                //! Colour of a tab when the mouse is being held down
        qRgb(51, 84, 151),
        qRgb(0xdf, 0xdf, 0xdf)
    };

    static constexpr QRgb RibbonBorderColor[] = {                               //! Colour of the ribbon divider at the bottom of the control
        qRgb(214, 211, 207),
        qRgb(0, 0, 0)
    };

    static constexpr QRgb GroupDividerColor[] = {                               //! Colour of the group divider
        qRgb(180, 178, 173),
        qRgb(0x57, 0x57, 0x57)
    };

#if defined(Q_OS_MACOS)

#else

#endif

#if defined(Q_OS_MACOS)
    static constexpr int TabBarHeight = 34;                                     //! Height of the tab bar portion of the control
    static constexpr int DefaultFontSize = 12;                                  //! Default text size for macOS
#else
    static constexpr int TabBarHeight = 28;                                     //! Height of the tab bar portion of the control
    static constexpr int DefaultFontSize = 10;                                  //! Default text size for targets other than macOS
#endif
    static constexpr int RibbonBarHeight = 100;                                 //! Height of the entire control
    static constexpr int TabHighlightHeight = 3;                                //! Height of the highlight mark on the selected page
    static constexpr int GroupDividerMargin = 2;                                //! Width of the group divider

    /**
     * @brief       Ribbon Bar Widget
     *
     * @details     Subclass of QTabWidget to create a lookalike of the Ribbon
     *              bar control.
     */
    class NEDRYSOFT_RIBBON_WIDGET_EXPORT RibbonWidget :
        public QTabWidget {
            private:
                Q_OBJECT

            public:
                /**
                 * @brief       Constructor
                 *
                 * @param[in]   parent is the owner parent widget
                 */
                explicit RibbonWidget(QWidget *parent = nullptr);

                friend class RibbonTabBar;
                friend class RibbonGroup;

            protected:
                /**
                 * @brief       paintEvent
                 *
                 * @details     Overridden paintEvent for drawing widget
                 *
                 * @param[in]   event contains information for painting this widget
                 */
                void paintEvent(QPaintEvent *event) override;

            private:
                Nedrysoft::Ribbon::RibbonTabBar *m_tabBar;                                  //! The tab bar associated with this ribbon
    };
}

#endif // NEDRYSOFT_RIBBONWIDGET_H
