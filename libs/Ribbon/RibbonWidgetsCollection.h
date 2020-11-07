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

#ifndef NEDRYSOFT_RIBBONWIDGETSCOLLECTION_H
#define NEDRYSOFT_RIBBONWIDGETSCOLLECTION_H

#include <QObject>
#include <QtUiPlugin/QDesignerCustomWidgetCollectionInterface>

class RibbonWidgetsCollection :
    public QObject,
    public QDesignerCustomWidgetCollectionInterface {
        private:
            Q_OBJECT
            Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QDesignerCustomWidgetCollectionInterface")
            Q_INTERFACES(QDesignerCustomWidgetCollectionInterface)

        public:
            RibbonWidgetsCollection(QObject *parent = 0);

            QList<QDesignerCustomWidgetInterface*> customWidgets() const override;

        private:
            QList<QDesignerCustomWidgetInterface*> m_widgets;
};

#endif // NEDRYSOFT_RIBBONWIDGETSCOLLECTION_H
