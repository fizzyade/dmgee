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

#include "RibbonButton.h"
#include "RibbonButtonPlugin.h"
#include <QtPlugin>

RibbonButtonPlugin::RibbonButtonPlugin(QObject *parent) :
        QObject(parent),
        m_initialized(false) {
}

void RibbonButtonPlugin::initialize(QDesignerFormEditorInterface *core) {
    Q_UNUSED(core);

    if (m_initialized) {
        return;
    }

    m_initialized = true;
}

bool RibbonButtonPlugin::isInitialized() const {
    return m_initialized;
}

QWidget *RibbonButtonPlugin::createWidget(QWidget *parent) {
    return new Nedrysoft::Ribbon::RibbonButton(parent);
}

QString RibbonButtonPlugin::name() const {
    return QStringLiteral("Nedrysoft::Ribbon::RibbonButton");
}

QString RibbonButtonPlugin::group() const {
    return QStringLiteral("Ribbon Widgets");
}

QIcon RibbonButtonPlugin::icon() const {
    return QIcon();
}

QString RibbonButtonPlugin::toolTip() const {
    return QString();
}

QString RibbonButtonPlugin::whatsThis() const {
    return QString();
}

bool RibbonButtonPlugin::isContainer() const {
    return true;
}

QString RibbonButtonPlugin::domXml() const {
    return "<ui language=\"c++\" displayname=\"Ribbon Button\">\n"
           " <widget class=\"Nedrysoft::Ribbon::RibbonButton\" name=\"ribbonButton\">\n"

           "  <property name=\"geometry\">\n"
           "   <rect>\n"
           "    <x>0</x>\n"
           "    <y>0</y>\n"
           "    <width>100</width>\n"
           "    <height>100</height>\n"
           "   </rect>\n"
           "  </property>\n"

           " </widget>\n"
           "</ui>\n";
}

QString RibbonButtonPlugin::includeFile() const {
    return QStringLiteral("Ribbon/RibbonButton.h");
}
