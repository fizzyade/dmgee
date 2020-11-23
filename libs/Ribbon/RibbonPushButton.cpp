/*
 * Copyright (C) 2020 Adrian Carpenter
 *
 * https://github.com/fizzyade
 *
 * Created by Adrian Carpenter on 11/11/2020.
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

#include "RibbonPushButton.h"

#include "RibbonFontManager.h"
#include "RibbonWidget.h"
#include "ThemeSupport.h"

#include <QApplication>
#include <QSpacerItem>
#include <QStyle>

#include <QDebug>

constexpr auto ThemeStylesheet = R"(
    QPushButton {
        border: 0px;
        padding: 3px;
    }

    QPushButton::hover {
        background: [background-colour];
    }
)";

Nedrysoft::Ribbon::RibbonPushButton::RibbonPushButton(QWidget *parent) :
        QWidget(parent),
        m_iconSize(QSize(RibbonPushButtonDefaultIconWidth, RibbonPushButtonDefaultIconHeight)),
        m_themeSupport(new Nedrysoft::Utils::ThemeSupport) {

    m_layout = new QVBoxLayout;
    m_mainButton = new QPushButton;
    m_buttonLabel = new QLabel;

    m_buttonLabel->setAlignment(Qt::AlignHCenter);

    auto fontManager = RibbonFontManager::getInstance();

    auto font = QFont(fontManager->normalFont(), RibbonPushButtonDefaultFontSize);

    m_mainButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    m_buttonLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    m_mainButton->installEventFilter(this);

    m_buttonLabel->setFont(font);

    m_layout->addWidget(m_mainButton);
    m_layout->addWidget(m_buttonLabel);
    m_layout->addSpacerItem(new QSpacerItem(0,0, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding));

    m_layout->setContentsMargins(0,0,0,0);
    m_layout->setSpacing(0);

    m_mainButton->setFlat(true);

    setLayout(m_layout);

    connect(m_mainButton, &QPushButton::clicked, [=] (bool checked) {
        Q_EMIT clicked();
    });

    connect(m_themeSupport, &Nedrysoft::Utils::ThemeSupport::themeChanged, [=](bool isDarkMode) {
        updateStyleSheets(isDarkMode);
    });

    updateSizes();

    updateStyleSheets(Nedrysoft::Utils::ThemeSupport::isDarkMode());
}

Nedrysoft::Ribbon::RibbonPushButton::~RibbonPushButton() {
    m_mainButton->deleteLater();
    m_buttonLabel->deleteLater();
    m_layout->deleteLater();
}

QIcon Nedrysoft::Ribbon::RibbonPushButton::icon() {
    return m_mainButton->icon();
}

void Nedrysoft::Ribbon::RibbonPushButton::setIcon(QIcon &icon) {
    m_mainButton->setIcon(icon);
}

QSize Nedrysoft::Ribbon::RibbonPushButton::iconSize() {
    return m_iconSize;
}

void Nedrysoft::Ribbon::RibbonPushButton::setIconSize(QSize iconSize)
{
    m_iconSize = iconSize;

    updateSizes();
}

QString Nedrysoft::Ribbon::RibbonPushButton::text() {
    return m_buttonLabel->text();
}

void Nedrysoft::Ribbon::RibbonPushButton::setText(QString text) {
    m_buttonLabel->setText(text);

    m_buttonLabel->setVisible(!m_buttonLabel->text().isEmpty());
}

void Nedrysoft::Ribbon::RibbonPushButton::updateSizes() {
    m_mainButton->setMinimumSize(m_iconSize);
    m_mainButton->setIconSize(m_iconSize);
}

void Nedrysoft::Ribbon::RibbonPushButton::updateStyleSheets(bool isDarkMode) {
    QString styleSheet(ThemeStylesheet);

    styleSheet.replace("[background-colour]", Nedrysoft::Utils::ThemeSupport::getColor(Nedrysoft::Ribbon::PushButtonColor).name());

    m_mainButton->setStyleSheet(styleSheet);
    m_buttonLabel->setStyleSheet(styleSheet);
}

bool Nedrysoft::Ribbon::RibbonPushButton::eventFilter(QObject *object, QEvent *event) {
    if (event->type()==QEvent::MouseButtonPress) {
        QString styleSheet(ThemeStylesheet);

        styleSheet.replace("[background-colour]", "#292929");

        m_mainButton->setStyleSheet(styleSheet);
    } else if (event->type()==QEvent::MouseButtonRelease) {
        updateStyleSheets(m_themeSupport->isDarkMode());
    }

    return false;
}

