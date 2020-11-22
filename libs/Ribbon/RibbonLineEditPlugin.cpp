/*
 * Copyright (C) 2020 Adrian Carpenter
 *
 * https://github.com/fizzyade
 *
 * Created by Adrian Carpenter on 12/11/2020.
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

#include "RibbonLineEditPlugin.h"
#include "RibbonLineEdit.h"

#include <QtPlugin>

constexpr auto ConfigurationXML = R"(
    <ui language="c++" displayname="Ribbon Line Edit">
        <widget class="Nedrysoft::Ribbon::RibbonLineEdit" name="ribbonLineEdit">
            <property name="geometry">
                <rect>
                    <x>0</x>
                    <y>0</y>
                    <width>100</width>
                    <height>13</height>
                </rect>
            </property>
        </widget>
    </ui>
)";

RibbonLineEditPlugin::RibbonLineEditPlugin(QObject *parent) :
        QObject(parent),
        m_initialized(false) {

}

void RibbonLineEditPlugin::initialize(QDesignerFormEditorInterface *core) {
    Q_UNUSED(core);

    if (m_initialized) {
        return;
    }

    m_initialized = true;
}

bool RibbonLineEditPlugin::isInitialized() const {
    return m_initialized;
}

QWidget *RibbonLineEditPlugin::createWidget(QWidget *parent) {
    return new Nedrysoft::Ribbon::RibbonLineEdit(parent);
}

QString RibbonLineEditPlugin::name() const {
    return QStringLiteral("Nedrysoft::Ribbon::RibbonLineEdit");
}

QString RibbonLineEditPlugin::group() const {
    return QStringLiteral("Nedrysoft Ribbon Widgets");
}

QIcon RibbonLineEditPlugin::icon() const {
    return QIcon(":/Nedrysoft/Ribbon/icons/ribbon.png");
}

QString RibbonLineEditPlugin::toolTip() const {
    return tr("A Ribbon Line Edit");
}

QString RibbonLineEditPlugin::whatsThis() const {
    return tr("The Ribbon Line Edit is a QLineEdit subclass that is styled to match the Ribbon.");
}

bool RibbonLineEditPlugin::isContainer() const {
    return false;
}

QString RibbonLineEditPlugin::domXml() const {
    return ConfigurationXML;
}

QString RibbonLineEditPlugin::includeFile() const {
    return QStringLiteral("Ribbon/RibbonLineEdit.h");
}
