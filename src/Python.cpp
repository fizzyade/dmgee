/*
 * Copyright (C) 2020 Adrian Carpenter
 *
 * This file is part of dmgee
 *
 * Created by Adrian Carpenter on 30/10/2020.
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

#include "Python.h"

#include <QApplication>
#include <QByteArray>
#include <QDebug>
#include <QDirIterator>
#include <QFile>
#include <QTextStream>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <thread>
#include <utility>

Nedrysoft::Python::Python() {
    Py_Initialize();
}

Nedrysoft::Python::~Python() {
    Py_Finalize();
}

QMap<QString, Py_tss_t *> Nedrysoft::Python::m_variables = QMap<QString, Py_tss_t *>();

void Nedrysoft::Python::run(QString &filename) {
    QFile pythonFile(filename);

    if (pythonFile.open(QFile::ReadOnly)) {
        QByteArray pythonContent = pythonFile.readAll();

        if (pythonContent.length()) {
            runScript(QString::fromUtf8(pythonContent), nullptr);
        } else {
            Q_EMIT finished(ScriptInvalid, 0);
        }
    } else {
        Q_EMIT finished(ScriptNotFound, 0);
    }
}

void Nedrysoft::Python::addModulePaths(QStringList modulePaths) {
    m_modulePaths = std::move(modulePaths);
}

void Nedrysoft::Python::runScript(const QString& script, PyObject *locals) {
    auto threadState = PyEval_SaveThread();

    auto thread = std::thread([this, script, locals, threadState]() {
        PyGILState_STATE gilState;

        gilState = PyGILState_Ensure();

        PyObject *systemModule = PyImport_ImportModule("sys");
        PyObject *systemPath = PyObject_GetAttrString(systemModule, "path");

        // add our local packages & dependencies to load first

        for (const auto& modulePath : m_modulePaths) {
            QDirIterator dirIterator(modulePath);

            while (dirIterator.hasNext()) {
                QFileInfo fileInfo(dirIterator.next());

                if (fileInfo.fileName().startsWith(".")) {
                    continue;
                }

                auto localModulePath = PyUnicode_FromString(fileInfo.absoluteFilePath().toLatin1().data());

                PyList_Insert(systemPath, 0, localModulePath);

                Py_DECREF(localModulePath);
            }
        }

        auto dict = PyDict_New();

        QMapIterator<QString, PyMethodDef *> moduleIterator(m_modules);

        while (moduleIterator.hasNext()) {
            moduleIterator.next();

            auto dmgeeModule = PyImport_AddModule(moduleIterator.key().toLatin1().constData());

            PyModule_AddFunctions(dmgeeModule, moduleIterator.value());
        }

        QMapIterator<QString, void *> variableIterator(m_threadVariables);

        while (variableIterator.hasNext()) {
            variableIterator.next();

            addVariable(variableIterator.key(), variableIterator.value());
        }

        // TODO: this should actually be PyObject_CallObject as that will allow us to get a return value

        auto result = PyRun_String(script.toLatin1().data(), Py_file_input, dict, locals);

        PyErr_Print();

        Py_DECREF(systemModule);
        Py_DECREF(systemPath);

        PyGILState_Release(gilState);

        Q_EMIT finished(Ok, 0);

        QMetaObject::invokeMethod(qobject_cast<QApplication *>(QCoreApplication::instance()), [threadState]() {
            PyEval_RestoreThread(threadState);
        });
    });

    thread.detach();
}

void Nedrysoft::Python::addModule(const QString &moduleName, PyMethodDef moduleMethods[]) {
    PythonModule module;

    if (!m_modules.contains(moduleName)) {
        m_modules[moduleName] = moduleMethods;
    }
}

void Nedrysoft::Python::setVariable(const QString &key, void *value) {
    m_threadVariables[key] = value;
}

void Nedrysoft::Python::addVariable(const QString &key, void *value) {
    Py_tss_t *variable;

    if (m_variables.contains(key)) {
        variable = m_variables[key];
    } else {
        variable = PyThread_tss_alloc();

        PyThread_tss_create(variable);

        m_variables[key] = variable;
    }

    PyThread_tss_set(variable, value);
}

void *Nedrysoft::Python::variable(const QString &key) {
    if (m_variables.contains(key)) {
        return PyThread_tss_get(m_variables[key]);
    }

    return nullptr;
}
