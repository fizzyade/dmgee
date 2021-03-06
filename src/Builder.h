/*
 * Copyright (C) 2020 Adrian Carpenter
 *
 * This file is part of dmgee
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

#ifndef NEDRYSOFT_BUILDER_H
#define NEDRYSOFT_BUILDER_H

#include "tomlplusplus/toml.hpp"
#include <QList>
#include <QMetaProperty>
#include <QSize>
#include <QString>
#include <fstream>

#include <Python.h>

namespace Nedrysoft {
    /**
     * @brief       The Builder class is capable of loading and saving configurations, it also provides the function
     *              to create the DMG based on the current configuration.
     */
    class Builder :
            public QObject {

        private:
            Q_OBJECT

        public:
            /**
             * @brief       Holds the information on files that are contained in the DMG, the x, y coordinates are
             *              relative to the DMG background
             */
            struct File {
                int x;                                          //! x coordinate of the file
                int y;                                          //! y coordinate of the file
                QString file;                                   //! the file name this file. (can be absolute, but recommended to be relative to this config

                bool operator==(const File& other) const {
                    return (x == other.x && y == other.y && file == other.file);
                }
            };

            /**
             * @brief       Holds the information on aliases/links that are contained in the DMG, the x, y coordinates are
             *              relative to the DMG background and the symlink is a link to a folder that exists on the destination.
             */
            struct Symlink {
                int x;                                          //! x coordinate of the symlink
                int y;                                          //! y coordinate of the symlink
                QString name;                                   //! the name to display below the icon
                QString shortcut;                               //! the target of tme symlink

                bool operator==(const Symlink& other) const {
                    return (x == other.x && y == other.y && name == other.name && shortcut == other.shortcut);
                }
            };

            /**
             * @brief       Sets the text position relative to the icon
             */
            enum TextPosition {
                Bottom = 0,                                     /**< Text is located below the icon. */
                Right = 1                                       /**< Text is located to the right of the icon. */
            };

        private:
            /**
             * @brief       Holds the configuration, this information is interchanged between this structure and a TOML format
             *              configuration file.
             */
            struct Configuration {
                QString m_background;                           //! filename of the background image
                QString m_icon;                                 //! icon to use when DMG is mounted
                QString m_filename;                             //! output filename for the DMG
                QString m_volumename;                           //! the name of the volume when mounted
                int m_iconsize;                                 //! the size of the icons to be shown
                QSize m_gridSize;                               //! the grid spacing
                bool m_snapToGrid;                              //! whether to snap points to grid
                bool m_gridVisible;                             //! whether the grid is visible
                bool m_snapToFeatures;                          //! whether to snap to features
                int m_featureSize;                              //! minimum size in px^2 for feature detection
                bool m_detectFeatures;                          //! whether we auto-detect features
                bool m_iconsVisible;                            //! whether icons are displayed on the preview
                QString m_format;                               //! format of the disk image
                int m_textSize;                                 //! size of the icon text in points
                TextPosition m_textPosition;                    //! the position of the text relative to the icon

                QList<Symlink *> m_symlinks;                    //! list of symlinks in the DMG
                QList<File *> m_files;                          //! list of files in the DMG
            };

        public:
            /**
             * @brief       Constructs a new Builder instance.
             */
            explicit Builder();

            /**
             * @brief       Uses the dmgbuild python module + configuration file to being a DMG.
             *
             * @param[in]   outputFilename the output name of the file to create (or empty to use the value in the configuration).
             *
             * @returns     true on success; otherwise false.
             */
            bool createDMG(QString outputFilename=QString());

            /**
             * @brief       Loads a configuration from a file.
             *
             * @param[in]   filename the name of the configuration to load.
             *
             * @returns     true if loaded without error; otherwise false.
             */
            bool loadConfiguration(const QString& filename);

            /**
             * @brief       Saves the current configuration to a file.
             *
             * @param[in]   filename the name of the configuration file to be created..
             *
             * @returns     true if saved without error; otherwise false.
             */
            bool saveConfiguration(const QString &filename);

            /**
             * @brief       Returns the number of files that are in the configuration.
             *
             * @returns     the number of files.
             */
            int totalFiles();

            /**
             * @brief       Returns the number of symlinks that are in the configuration.
             *
             * @returns     the number of symlinks.
             */
            int totalSymlinks();

            /**
             * @brief       Clears the current builder config to default.
             */
            void clear();

            /**
             * @brief       Returns the normalised output filename.
             *
             * @returns     the normalised output filename.
             */
            QString outputFilename();

            /**
             * @brief       Returns the filename of this configuration.
             *
             * @returns     the full path to the configuration file if set; otherwise a null string
             */
             QString filename();

             /**
              * @brief      Returns whether the configuration has been modified.
              *
              * @returns    true if modified; otherwise false.
              */
             bool modified();

             /**
              * @brief      Sets the modified state, overrides the internal modification logic.
              *
              * @param[ib]  isModified true if modified; otherwise false.
              */
             void setModified(bool isModified);

        private:
            /**
             * @brief       Python function which allows transfer of a string to c
             *
             * @param[in]   self the python object
             * @param[in]   updateData string containing the data being transferred
             *
             * @returns     PyTrue if handled; other PyFalse
             */
            static PyObject *update(PyObject *self, PyObject *updateData);

        public:
            /**
             * @brief       This signal is emitted when the python script progress is updated.
             *
             * @param[in]   progress the json progress update as a QString
             */
            Q_SIGNAL void progressUpdate(QString progress);

        private:
            /**
             * @brief       Sets the list of symlinks to be added to the DMG.
             *
             * @param[in]   symlinks the list of symlinks.
             */
            void setSymlinks(QList<Symlink *> symlinks);

            /**
             * @brief       Returns the list of symlinks.
             *
             * @returns     the symlinks list.
             */
            QList<Symlink *> symlinks();

            /**
             * @brief       Sets the list of files to be added to the DMG.
             *
             * @param[in]   files the list of files.
             */
            void setFiles(QList<File *> files);

            /**
             * @brief       Returns the list of files.
             *
             * @returns     the file list.
             */
            QList<File *> files();

            /**
             * @brief       Reuturns the normalized filename.
             *
             * @note        The filename is checked for tildes and if one exists then the filename is modified to
             *              have the full path to the user.  If the tile is relative, then it is concatenated with
             *              the path to the configuration file.  If none of the previous statements are true then
             *              the orignal filename is returned.
             *
             * @param[in]   filename the name of the file to normalise.
             *
             * @returns     the normalised filename.
             */
            QString normalisedFilename(QString filename);

        public:
            void setProperty(const char *name, const QVariant &value);

        public:
            Q_SIGNAL void iconSizeChanged(int iconSize);
            Q_SIGNAL void iconVisibilityChanged(bool isVisible);
            Q_SIGNAL void gridSnapChanged(bool snapToGrid);
            Q_SIGNAL void gridVisibilityChanged(bool isVisible);
            Q_SIGNAL void featureSnapChanged(bool snapToFeatures);
            Q_SIGNAL void textSizeChanged(bool textSize);
            Q_SIGNAL void gridSizeChanged(QSize gridSize);
            Q_SIGNAL void filesChanged(QList<Nedrysoft::Builder::File *> files);
            Q_SIGNAL void symlinksChanged(QList<Nedrysoft::Builder::Symlink *> symlinks);
            Q_SIGNAL void formatChanged(QString format);

        public:
            Q_PROPERTY(QString background MEMBER (m_configuration.m_background));
            Q_PROPERTY(QString icon MEMBER (m_configuration.m_icon));
            Q_PROPERTY(QString filename MEMBER (m_configuration.m_filename));
            Q_PROPERTY(QString volumename MEMBER (m_configuration.m_volumename));
            Q_PROPERTY(QString format MEMBER (m_configuration.m_format) NOTIFY formatChanged);
            Q_PROPERTY(int iconsize MEMBER (m_configuration.m_iconsize) NOTIFY iconSizeChanged);
            Q_PROPERTY(QSize gridsize MEMBER (m_configuration.m_gridSize) NOTIFY gridSizeChanged);
            Q_PROPERTY(bool snaptogrid MEMBER (m_configuration.m_snapToGrid) NOTIFY gridSnapChanged);
            Q_PROPERTY(bool snaptofeatures MEMBER (m_configuration.m_snapToFeatures) NOTIFY featureSnapChanged);
            Q_PROPERTY(bool gridvisible MEMBER (m_configuration.m_gridVisible) NOTIFY gridVisibilityChanged);
            Q_PROPERTY(bool iconsvisible MEMBER (m_configuration.m_iconsVisible) NOTIFY iconVisibilityChanged);
            Q_PROPERTY(int featuresize MEMBER (m_configuration.m_featureSize));
            Q_PROPERTY(bool detectfeatures MEMBER (m_configuration.m_detectFeatures));
            Q_PROPERTY(QList<Nedrysoft::Builder::Symlink *> symlinks MEMBER (m_configuration.m_symlinks) NOTIFY symlinksChanged);
            Q_PROPERTY(QList<Nedrysoft::Builder::File *> files MEMBER (m_configuration.m_files) NOTIFY filesChanged);
            Q_PROPERTY(int textsize MEMBER (m_configuration.m_textSize) NOTIFY textSizeChanged);
            Q_PROPERTY(Nedrysoft::Builder::TextPosition textposition MEMBER (m_configuration.m_textPosition));

        private:
            Configuration m_configuration;                      //! the configuration for the DMG
            QString m_filename;                                 //! the filename of the configuration that was loaded.
            QString m_outputFilename;                           //! the filename of the output file.
            bool m_isModified;                                  //! whether the configuration has changed.

            static PyMethodDef m_moduleMethods[];               //! module method table for the dmgee module
    };
}

Q_DECLARE_METATYPE(Nedrysoft::Builder::File *);
Q_DECLARE_METATYPE(Nedrysoft::Builder::Symlink *);
Q_DECLARE_METATYPE(Nedrysoft::Builder::TextPosition);

#endif //NEDRYSOFT_BUILDER_H
