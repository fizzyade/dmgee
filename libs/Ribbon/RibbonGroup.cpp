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

#include "RibbonGroup.h"

#include "RibbonFontManager.h"
#include "RibbonWidget.h"
#include "ThemeSupport.h"

#include <QApplication>
#include <QPaintEvent>
#include <QPainter>
#include <QPushButton>
#include <QRegularExpression>

Nedrysoft::Ribbon::RibbonGroup::RibbonGroup(QWidget *parent) :
        QWidget(parent),
        m_fontMetrics(QFont()) {

    auto fontManager = RibbonFontManager::getInstance();

    m_font = QFont(fontManager->normalFont(), RibbonGroupDefaultFontSize);
    m_fontMetrics = QFontMetrics(m_font);

    setGroupName(QString("Group"));

    connect(qobject_cast<QApplication *>(QCoreApplication::instance()), &QApplication::paletteChanged, [=] (const QPalette &) {
        // TODO: anything to do?
    });

    updateMargins();

    setMinimumWidth(m_fontMetrics.maxWidth());
}

void Nedrysoft::Ribbon::RibbonGroup::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    auto widgetRect = rect();
    auto  currentTheme = Nedrysoft::Ribbon::Light;

    if (Nedrysoft::Utils::ThemeSupport::isDarkMode()) {
        currentTheme = Nedrysoft::Ribbon::Dark;
    }

    widgetRect.setTop(widgetRect.bottom()-m_textRect.height());

    painter.save();

    painter.setClipRect(event->rect());

    painter.setFont(m_font);

    painter.setPen(Ribbon::TextColor[currentTheme]);

    auto textRect = contentsRect();

    textRect.setBottom(rect().bottom());

    painter.drawText(textRect, m_groupName, Qt::AlignBottom | Qt::AlignHCenter);

    painter.setPen(Ribbon::GroupDividerColor[currentTheme]);

    auto startPoint = QPoint(rect().right()-1, rect().top()+Ribbon::GroupDividerMargin);
    auto endPoint = QPoint(rect().right()-1, rect().bottom()-Ribbon::GroupDividerMargin);

    painter.drawLine(startPoint, endPoint);

    painter.restore();

    QWidget::paintEvent(event);
}

QString Nedrysoft::Ribbon::RibbonGroup::groupName() const {
    return m_groupName;
}

void Nedrysoft::Ribbon::RibbonGroup::setGroupName(const QString &groupName) {
    m_groupName = groupName;

    updateMargins();
}

void Nedrysoft::Ribbon::RibbonGroup::updateMargins() {
    m_textRect = m_fontMetrics.boundingRect(m_groupName);

    setContentsMargins(RibbonGroupHorizontalMargins, 0, RibbonGroupHorizontalMargins, m_textRect.height());

    update();
}