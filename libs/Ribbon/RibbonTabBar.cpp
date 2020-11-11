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

#include "RibbonFontManager.h"
#include "RibbonTabBar.h"
#include "RibbonWidget.h"
#include "ThemeSupport.h"
#include <QApplication>
#include <QDebug>
#include <QPaintEvent>
#include <QPainter>
#include <QWindow>

#if defined(Q_OS_MACOS)
constexpr int DefaultFontSize = 12;
#else
constexpr int DefaultFontSize = 10;
#endif

Nedrysoft::Ribbon::RibbonTabBar::RibbonTabBar(QWidget *parent) :
    QTabBar(parent) {

    auto fontManager = Nedrysoft::Ribbon::RibbonFontManager::getInstance();

    m_normalFont = QFont(fontManager->normalFont(), DefaultFontSize);
    m_selectedFont = QFont(fontManager->boldFont(), DefaultFontSize, QFont::Bold);
    m_mouseInWidget = false;

    setStyleSheet("QTabBar::tab{padding: 18px}");

#if defined(Q_OS_UNIX)
    setMouseTracking(true);
    installEventFilter(this);
#endif
}

bool Nedrysoft::Ribbon::RibbonTabBar::eventFilter(QObject *obj, QEvent *event) {
    Q_UNUSED(obj)

    static auto lastTabIndex = -1;

    switch(event->type())
    {
        case QEvent::Enter: {
            auto enterEvent = reinterpret_cast<QEnterEvent *>(event);

            lastTabIndex = tabAt(enterEvent->pos());

            m_mouseInWidget = true;

            update();

            break;
        }

        case QEvent::Leave: {
            lastTabIndex = -1;

            m_mouseInWidget = false;

            update();

            break;
        }

        case QEvent::MouseMove: {
            auto mouseEvent = reinterpret_cast<QMouseEvent *>(event);

            if (lastTabIndex != tabAt(mouseEvent->pos())) {
                update();

                lastTabIndex = tabAt(mouseEvent->pos());
            }

            break;
        }

        default: {
            break;
        }
    }

    return false;
}

QSize Nedrysoft::Ribbon::RibbonTabBar::tabSizeHint(int index) const {
    auto size = QTabBar::tabSizeHint(index);

    size.setHeight(Ribbon::TabBarHeight);

    return size;
}

void Nedrysoft::Ribbon::RibbonTabBar::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)
    auto globalCursorPos = QCursor::pos();
    auto textRect = QRect();
    auto cursorPos = this->mapFromGlobal(globalCursorPos);
    auto hoveredTab = tabAt(cursorPos);
    auto backgroundColor = QColor();
    QPainter painter(this);
    auto  currentTheme = Ribbon::Light;

    if (Nedrysoft::Utils::ThemeSupport::isDarkMode()) {
        currentTheme = Nedrysoft::Ribbon::Dark;
    }

#if defined(Q_OS_MACOS)
    if (!m_mouseInWidget)
        hoveredTab = -1;
#endif

    painter.save();

    for(auto tabIndex=0;tabIndex<this->count();tabIndex++) {
        auto rect = tabRect(tabIndex);

#if defined(Q_OS_MACOS)
        rect.adjust(0,0,0,-4);
#endif
        if (hoveredTab==tabIndex) {
            backgroundColor = Ribbon::TabHoverColor[currentTheme];
        } else {
            backgroundColor = Ribbon::TabBarBackgroundColor[currentTheme];
        }

        if (tabIndex==currentIndex()) {
            painter.setFont(m_selectedFont);
        } else {
            painter.setFont(m_normalFont);
        }

        painter.fillRect(rect, backgroundColor);

        painter.setPen(Ribbon::TextColor[currentTheme]);

        painter.drawText(rect, Qt::AlignHCenter | Qt::AlignVCenter, tabText(tabIndex), &textRect);

        rect.setTop(rect.bottom()-Ribbon::TabHighlightHeight);

        if (hoveredTab!=tabIndex) {
            rect.setLeft(textRect.left());
            rect.setRight(textRect.right());
        }

        if (tabIndex==currentIndex()) {
            painter.fillRect(rect, Ribbon::TabSelectedColor[currentTheme]);
        }
    }

    painter.restore();
}
