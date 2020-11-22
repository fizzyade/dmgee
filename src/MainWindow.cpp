/*
 * Copyright (C) 2020 Adrian Carpenter
 *
 * This file is part of dmgee
 *
 * Created by Adrian Carpenter on 05/11/2020.
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

#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "AboutDialog.h"
#include "AnsiEscape.h"
#include "ImageLoader.h"
#include "SettingsDialog.h"
#include "ThemeSupport.h"

#include <QAction>
#include <QClipboard>
#include <QDesktopServices>
#include <QElapsedTimer>
#include <QFileInfo>
#include <QJsonDocument>
#include <QList>
#include <QMenu>
#include <QMessageBox>
#include <QMimeData>
#include <opencv2/opencv.hpp>
#include <QPaintEvent>
#include <QPainter>
#include <QResizeEvent>
#include <QStyleFactory>
#include <QTemporaryDir>
#include <QTimer>
#include <QWebEngineProfile>
#include <QWebEngineSettings>
#include <QWindow>
#include <utility>

// convenience macros for ansi escape sequences

#define fore Nedrysoft::AnsiEscape::fore
#define back Nedrysoft::AnsiEscape::back
#define style Nedrysoft::AnsiEscape::style
#define hyperlink Nedrysoft::AnsiEscape::link
#define reset Nedrysoft::AnsiEscape::reset()
#define underline(state) Nedrysoft::AnsiEscape::underline(state)

using namespace std::chrono_literals;

constexpr auto splashScreenDuration = 100ms;//3s;
constexpr auto repositoryUrl = "https://github.com/fizzyade/dmgee";
constexpr auto menuIconSize = 32;

Nedrysoft::MainWindow *Nedrysoft::MainWindow::m_instance = nullptr;

// python3 -m pip install git+https://github.com/fizzyade/dmgbuild.git@master --force

Nedrysoft::MainWindow::MainWindow() :
        QMainWindow(nullptr),
        ui(new Ui::MainWindow),
        m_minimumPixelArea(10000),
        m_backgroundImage(),
        m_builder(new Builder),
        m_settingsDialog(nullptr) {

    ui->setupUi(this);

    m_themeSupport = new Nedrysoft::Utils::ThemeSupport;

    /*auto p = ui->previewContainer->palette();

    p.setColor(QPalette::Base, Qt::red);

    ui->previewContainer->setBackgroundRole(QPalette::Base);
    ui->previewContainer->setPalette(p);*/

    initialiseLoader();

    setupStatusBar();

    qobject_cast<QApplication *>(QCoreApplication::instance())->installEventFilter(this);

    QTimer::singleShot(splashScreenDuration, []() {
        Nedrysoft::SplashScreen::getInstance()->close();
    });

    updatePixmap();

    QDesktopServices::setUrlHandler("dmgee", this, SLOT("handleOpenByUrl"));

    // set up gui controls

    ui->gridVisibleCheckbox->setCheckState(configValue("gridVisible", false).toBool() ? Qt::Checked : Qt::Unchecked);
    ui->gridSnapCheckbox->setCheckState(configValue("gridShouldSnap", false).toBool() ? Qt::Checked : Qt::Unchecked);
    ui->gridXLineEdit->setValidator(new QIntValidator(0, 100));
    ui->gridYLineEdit->setValidator(new QIntValidator(0, 100));
    ui->previewWidget->setGrid(QSize(ui->gridXLineEdit->text().toInt(),ui->gridYLineEdit->text().toInt()),
                               ui->gridVisibleCheckbox->isChecked(),
                               ui->gridSnapCheckbox->isChecked());

    ui->featureAutoDetectCheckbox->setCheckState(configValue("snapToFeatures", true).toBool() ? Qt::Checked : Qt::Unchecked);
    ui->featureAutoDetectCheckbox->setCheckState(Qt::Checked);

    ui->showIconsCheckBox->setCheckState(configValue("iconsVisible", true).toBool() ? Qt::Checked : Qt::Unchecked);
    ui->iconsSizeLineEdit->setValidator(new QIntValidator(16, 512));
    ui->previewWidget->setIconSize(configValue("iconSize", 64).value<int>());

    ui->fontSizeLineEdit->setValidator(new QIntValidator(6, 72));

    ui->positionComboBox->addItems(QStringList() << tr("Bottom") << tr("Right"));
    ui->positionComboBox->setCurrentIndex(configValue("textPosition", Nedrysoft::Builder::Bottom).value<Nedrysoft::Builder::TextPosition>());

    processBackground();

    setupDiskImageFormatCombo();

    // connect signals

    connect(ui->fontSizeLineEdit, &QLineEdit::textChanged, this, &MainWindow::onFontSizeChanged);
    connect(ui->iconsSizeLineEdit, &QLineEdit::textChanged, this, &MainWindow::onIconSizeChanged);
    connect(ui->gridSnapCheckbox, &QCheckBox::clicked, this, &MainWindow::onGridSnapChanged);
    connect(ui->buildButton, &Nedrysoft::Ribbon::RibbonPushButton::clicked, this, &MainWindow::onCreateDMG);
    connect(ui->actionAbout, &QAction::triggered, this, &Nedrysoft::MainWindow::onAboutDialogTriggered);
    connect(ui->minFeatureSlider, &QSlider::valueChanged, this, &Nedrysoft::MainWindow::onFeatureSliderMinimumValueChanged);
    connect(ui->gridVisibleCheckbox, &QCheckBox::stateChanged, this, &Nedrysoft::MainWindow::onGridVisibilityChanged);
    connect(ui->showIconsCheckBox, &QCheckBox::stateChanged, this, &Nedrysoft::MainWindow::onIconsVisibilityChanged);
    connect(ui->featureAutoDetectCheckbox, &QCheckBox::stateChanged, this, &Nedrysoft::MainWindow::onFeatureVisibilityChanged);
    connect(ui->designFilesAddButton, &Nedrysoft::Ribbon::RibbonDropButton::clicked, this, &Nedrysoft::MainWindow::onDesignFilesAddButtonClicked);
    connect(m_builder, &Nedrysoft::Builder::progressUpdate, this, &Nedrysoft::MainWindow::onProgressUpdate, Qt::QueuedConnection);
    connect(ui->terminalWidget, &Nedrysoft::HTermWidget::terminalReady, this, &Nedrysoft::MainWindow::onTerminalReady);
    connect(ui->terminalWidget, &Nedrysoft::HTermWidget::contextMenu, this, &Nedrysoft::MainWindow::onTerminalContextMenuTriggered);
    connect(ui->terminalWidget, &Nedrysoft::HTermWidget::openUrl, this, &Nedrysoft::MainWindow::onTerminalUrlClicked);
    connect(ui->actionQuit, &QAction::triggered, this, &Nedrysoft::MainWindow::close);
    connect(ui->terminalWidget, &Nedrysoft::HTermWidget::terminalBuffer, this, &Nedrysoft::MainWindow::copyTerminalBufferToClipboard);

    connect(ui->actionPreferences, &QAction::triggered, [=]() {
        if (m_settingsDialog) {
            m_settingsDialog->raise();

            return;
        }

        m_settingsDialog = new SettingsDialog(this);

        m_settingsDialog->show();

        connect(m_settingsDialog, &SettingsDialog::closed, [=](){
            m_settingsDialog->deleteLater();

            m_settingsDialog = nullptr;
        });
    });
}

Nedrysoft::MainWindow::~MainWindow() {
    delete ui;
}

Nedrysoft::MainWindow *Nedrysoft::MainWindow::getInstance() {
    static Nedrysoft::MainWindow *instance = nullptr;

    if (!instance) {
        m_instance = instance = new Nedrysoft::MainWindow;
    }

    return m_instance;
}

void Nedrysoft::MainWindow::handleOpenByUrl(const QUrl &url) {
    Q_UNUSED(url);
}

bool Nedrysoft::MainWindow::eventFilter(QObject *watched, QEvent *event) {
    if (event->type()==QEvent::FileOpen) {
        auto fileOpenEvent = dynamic_cast<QFileOpenEvent *>(event);

        /*if (!fileOpenEvent->url().isEmpty()) {
            // TODO: launched via url scheme
        } else if (!fileOpenEvent->file().isEmpty()) {
            // TODO: launched via file association
        }*/

        return false;
    }

    return QObject::eventFilter(watched, event);
}

void Nedrysoft::MainWindow::closeEvent(QCloseEvent *closeEvent) {
    if (m_settingsDialog) {
        m_settingsDialog->close();
        m_settingsDialog->deleteLater();
        m_settingsDialog = nullptr;
    }

    closeEvent->accept();
}

void Nedrysoft::MainWindow::processBackground()
{
    if (m_backgroundImage.isValid()) {
        std::vector<std::vector<cv::Point> > contours;
        std::vector<cv::Vec4i> hierarchy;
        cv::Mat image = m_backgroundImage.mat();

        // convert the image to grey scale for contour detection

        cv::cvtColor(image, image, cv::COLOR_BGR2GRAY);

        m_centroids.clear();

        // apply thresholding

        cv::threshold(image, image, 1, 32, cv::THRESH_TRUNC);

        // apply second stage thresholding (to black and white)

        cv::threshold(image, image, 230, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

        // find contours in image

        cv::findContours(image, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

        // find centre of discovered objects in image

        for (auto &contour : contours) {
            float sumX = 0, sumY = 0;
            float size = contour.size();
            QPointF centroid;

            if (size > 0) {
                for (auto &point : contour) {
                    sumX += point.x;
                    sumY += point.y;
                }

                centroid = QPointF(sumX / size, sumY / size);
            }

            auto area = cv::contourArea(contour);

            if (area > m_minimumPixelArea) {
                m_centroids.append(centroid);
            }
        }

        ui->previewWidget->setCentroids(m_centroids);
    }
}

bool Nedrysoft::MainWindow::setConfigValue(const QString& valueName, QVariant value) {
    if (m_builder->property(valueName.toLatin1().constData()).isValid()) {
        m_builder->setProperty(valueName.toLatin1().constData(), value);

        return true;
    }

    return false;
}

QVariant Nedrysoft::MainWindow::configValue(const QString& valueName, QVariant defaultValue) {
    if (m_builder->property(valueName.toLatin1().constData()).isValid()) {
        return m_builder->property(valueName.toLatin1().constData());
    }

    return defaultValue;
}

bool Nedrysoft::MainWindow::loadConfiguration(QString filename) {
    if (m_builder->loadConfiguration(std::move(filename))) {
        ui->gridSnapCheckbox->setCheckState(configValue("snapToGrid", false).toBool() ? Qt::Checked : Qt::Unchecked);
        ui->gridVisibleCheckbox->setCheckState(configValue("gridVisible", false).toBool() ? Qt::Checked : Qt::Unchecked);

        ui->gridXLineEdit->setText(QString("%1").arg(configValue("gridSize", 20).toPoint().x()));
        ui->gridYLineEdit->setText(QString("%1").arg(configValue("gridSize", 20).toPoint().y()));

        ui->iconsSizeLineEdit->setText(QString("%1").arg(configValue("iconSize", 128).toInt()));
        ui->minFeatureSlider->setValue(configValue("featureSize", 10000).toInt());
        ui->fontSizeLineEdit->setText(QString("%1").arg(configValue("textSize", 12).toInt()));

        ui->featureAutoDetectCheckbox->setCheckState(configValue("detectFeatures", true).toBool() ? Qt::Checked : Qt::Unchecked);

        // add the icons from the configuration to the preview window.

        auto iconSize = configValue("iconSize", 128).toInt();
        auto files = m_builder->property("files").value<QList<Nedrysoft::Builder::File *>>();

        for (auto file : files) {
            auto applicationIcon = new Nedrysoft::Image(file->file, false, iconSize, iconSize);

            ui->previewWidget->addIcon(applicationIcon, QPoint(file->x, file->y), PreviewWidget::Icon, [=](QPoint& point){
                file->x = point.x();
                file->y = point.y();
            });
        }

        auto symlinks = m_builder->property("symlinks").value<QList<Nedrysoft::Builder::Symlink *>>();

        for (auto symlink : symlinks) {
            QTemporaryDir temporaryDir;

            if (temporaryDir.isValid()) {
                auto temporaryName = temporaryDir.path() + symlink->shortcut;

                if (QFile::link(symlink->shortcut, temporaryName)) {
                    auto applicationsShortcutImage = new Nedrysoft::Image(temporaryName, false, iconSize, iconSize);

                    ui->previewWidget->addIcon(applicationsShortcutImage, QPoint(symlink->x, symlink->y), PreviewWidget::Shortcut, [=](QPoint& point){
                        symlink->x = point.x();
                        symlink->y = point.y();
                    });
                }
            }
        }

        updatePixmap();
    }

    return true;
}

void Nedrysoft::MainWindow::updatePixmap() {
    QFileInfo fileInfo(configValue("background", "").value<QString>());

    if (!fileInfo.absoluteFilePath().isEmpty()) {
        m_backgroundImage = Nedrysoft::Image(fileInfo.absoluteFilePath(), true);

        m_backgroundPixmap = QPixmap::fromImage(m_backgroundImage.image());

        ui->previewWidget->setPixmap(m_backgroundPixmap);

        if (ui->featureAutoDetectCheckbox->isChecked()) {
            processBackground();
        }
    } else {
        m_backgroundPixmap = QPixmap();

        ui->previewWidget->setPixmap(m_backgroundPixmap);
        ui->previewWidget->clearCentroids();
    }

    ui->previewWidget->fitToView();
}

void Nedrysoft::MainWindow::resizeEvent(QResizeEvent *event) {
    ui->previewWidget->fitToView();
}


QString Nedrysoft::MainWindow::timespan(int milliseconds) {
    QString outputString;
    int seconds = milliseconds / 1000;
    int minutes = seconds / 60;
    int hours = minutes / 60;

    milliseconds = milliseconds % 1000;
    seconds = seconds % 60;
    minutes  = minutes % 60;

    if (hours!=0) {
        outputString.append(QString(tr("%1 %2 ").arg(hours).arg("hours")));
    }

    if ((minutes!=0) || (!outputString.isEmpty())) {
        outputString.append(QString(tr("%1 %2 ").arg(minutes).arg("minutes")));
    }

    if ((seconds!=0) || (!outputString.isEmpty())) {
        outputString.append(QString(tr("%1 %2 ").arg(seconds).arg("seconds")));
    }

    return outputString.trimmed();
}

void Nedrysoft::MainWindow::onProgressUpdate(QString updateData) {
    auto updateMap = QJsonDocument::fromJson(updateData.toUtf8()).toVariant().toMap();
    QString updateMessage;
    QStringList type = updateMap["type"].toString().split("::");
    auto normalColour = fore(QColor("#A8C023"));

    if (type[0]=="build") {
        static QElapsedTimer durationTimer;
        bool showActivity = false;

        if (type[1]=="started") {
            durationTimer.restart();

            updateMessage =
                    fore(AnsiColour::BLUE)+
                    style(AnsiStyle::BRIGHT)+
                    tr("Build Started at %1.").arg(QDateTime::currentDateTime().toString())+
                    reset;

            showActivity = true;

            m_stateLabel->setText(tr("Building Image..."));
        } else if (type[1]=="finished") {
            QString duration = timespan(durationTimer.elapsed());

            updateMessage =
                    fore(AnsiColour::BLUE)+
                    style(AnsiStyle::BRIGHT)+
                    tr("Build Finished at %1.\r\n").arg(QDateTime::currentDateTime().toString())+
                    reset+
                    "\r\n"+
                    fore(AnsiColour::WHITE)+
                    style(AnsiStyle::BRIGHT)+
                    tr("Build took %1.").arg(duration);
            //TODO: this should actually return separate strings which can then
            //      be used by tr.  As it stands the duration string is a single string
            //      which means that it cannot be translated properly.

            m_stateLabel->setText(tr("Idle"));
        }

        m_progressSpinner->setVisible(showActivity);
        m_progressBar->setVisible(showActivity);
    } else if (type[0]=="operation") {
        if (type[1]=="start") {
            QStringList operation = updateMap["operation"].toString().split("::");

            updateMessage.clear();

            if (operation[0]=="settings") {
                if (operation[1] == "load") {
                    updateMessage = normalColour+tr("Loading settings...")+reset;
                }
            } else if (operation[0]=="size") {
                if (operation[1] == "calculate") {
                    updateMessage = normalColour+tr("Calculating DMG size...")+reset;
                }
            } else if (operation[0]=="dmg") {
                if (operation[1]=="create") {
                    updateMessage = normalColour+tr("Creating DMG...")+reset;
                } else if (operation[1]=="shrink") {
                    updateMessage = normalColour+tr("Shrinking DMG...")+reset;
                }
            } else if (operation[0]=="background") {
                if (operation[1]=="create") {
                    updateMessage = normalColour+tr("Creating Background Image...")+reset;
                }
            } else if (operation[0]=="files") {
                if (operation[1]=="add") {
                    updateMessage = normalColour+tr("Adding files to DMG...")+reset;
                }
            } else if (operation[0]=="file") {
                if (operation[1]=="add") {
                    QFileInfo fileInfo(updateMap["file"].toString());

                    QString filename =
                            fore(AnsiColour::WHITE)+
                            "\""+
                            fore(0xb0,0x85, 0xbe)+
                            underline(true)+
                            hyperlink(QUrl::fromLocalFile(fileInfo.filePath()).toString(), fileInfo.fileName())+
                            underline(false)+
                            fore(AnsiColour::WHITE)+
                            "\""+
                            normalColour;

                    updateMessage =
                            normalColour+QString(tr("Adding file %1...")).arg(filename)+
                            normalColour+
                            reset;
                }
            } else if (operation[0]=="symlinks") {
                if (operation[1]=="add") {
                    updateMessage = normalColour+tr("Creating symlinks in DMG...")+reset;
                }
            } else if (operation[0]=="symlink") {
                if (operation[1]=="add") {
                    QString filename =
                            fore(AnsiColour::WHITE)+
                            "\""+
                            fore(0xb0,0x85, 0xbe)+
                            underline(true)+
                            hyperlink(QUrl::fromLocalFile(updateMap["target"].toString()).toString(), updateMap["target"].toString())+
                            underline(false)+
                            fore(AnsiColour::WHITE)+
                            "\""+
                            normalColour;

                    updateMessage =
                            normalColour+QString(tr("Adding symlink %1...")).arg(filename)+
                            normalColour+
                            reset;
                }
            } else if (operation[0]=="extensions") {
                if (operation[1] == "hide") {
                    updateMessage = normalColour + tr("Hiding files...") + reset;
                }
            } else if (operation[0]=="dsstore") {
                if (operation[1]=="create") {
                    updateMessage = normalColour+tr("Creating DS_Store...")+reset;
                }
            }  else if (operation[0]=="dsstore") {
                if (operation[1] == "addlicense") {
                    updateMessage = normalColour + tr("Adding license...") + reset;
                }
            } else {
                // unknown operation
            }
        } else if (type[1]=="finished") {
            // TODO: anything else?
        }
    }

    if (!updateMessage.isEmpty()) {
        int progressValue = static_cast<int>((static_cast<float>(m_progressBar->value())/static_cast<float>(m_progressBar->maximum()-1))*100);

        ui->terminalWidget->print(QString("[%1%] ").arg(progressValue, 3, 10));
        ui->terminalWidget->println(updateMessage);

        m_progressBar->setValue(m_progressBar->value()+1);
    }
}

void Nedrysoft::MainWindow::setupStatusBar() {

    m_progressBar = new QProgressBar;

    // load the spinner GIF and add to status bar

    m_spinnerMovie = new QMovie;

    if (Nedrysoft::Utils::ThemeSupport::isDarkMode()) {
        m_spinnerMovie->setFileName(":/images/spinner-dark.gif");
    } else {
        m_spinnerMovie->setFileName(":/images/spinner-light.gif");
    }

    connect(m_themeSupport, &Nedrysoft::Utils::ThemeSupport::themeChanged, [=](bool isDarkMode) {
        if (isDarkMode) {
            m_spinnerMovie->setFileName(":/images/spinner-dark.gif");
        } else {
            m_spinnerMovie->setFileName(":/images/spinner-light.gif");
        }
    });

    m_spinnerMovie->setScaledSize(QSize(16,16));
    m_spinnerMovie->start();

    m_progressSpinner = new QLabel();

    m_progressSpinner->setMovie(m_spinnerMovie);

    ui->statusbar->addWidget(m_progressSpinner);

    ui->statusbar->addWidget(m_progressBar);

    m_progressSpinner->setVisible(false);
    m_progressBar->setVisible(false);

    m_progressBar->setValue(0);
    m_progressBar->setRange(0, 12+m_builder->totalFiles()+m_builder->totalSymlinks());

    m_stateLabel = new QLabel("Idle");

    ui->statusbar->addPermanentWidget(m_stateLabel);
}

void Nedrysoft::MainWindow::setupDiskImageFormatCombo() {
    QList<QPair<QString, QString> > diskFormats;

    diskFormats << QPair<QString, QString>("UDRW", "UDIF read/write image");
    diskFormats << QPair<QString, QString>("UDRO", "UDIF read-only image");
    diskFormats << QPair<QString, QString>("UDCO", "UDIF ADC-compressed image");
    diskFormats << QPair<QString, QString>("UDZO", "UDIF zlib-compressed image");
    diskFormats << QPair<QString, QString>("UDBZ", "UDIF bzip2-compressed image (macOS 10.4+ only)");
    diskFormats << QPair<QString, QString>("UFBI", "UDIF entire image with MD5 checksum");
    diskFormats << QPair<QString, QString>("UDRo", "UDIF read-only (obsolete format)");
    diskFormats << QPair<QString, QString>("UDCo", "UDIF compressed (obsolete format)");
    diskFormats << QPair<QString, QString>("UDTO", "DVD/CD-R master for export");
    diskFormats << QPair<QString, QString>("UDxx", "UDIF stub image");
    diskFormats << QPair<QString, QString>("UDSP", "SPARSE (grows with content)");
    diskFormats << QPair<QString, QString>("UDSB", "SPARSEBUNDLE (grows with content; bundle-backed)");
    diskFormats << QPair<QString, QString>("RdWr", "NDIF read/write image (deprecated)");
    diskFormats << QPair<QString, QString>("Rdxx", "NDIF read-only image (Disk Copy 6.3.3 format)");
    diskFormats << QPair<QString, QString>("ROCo", "NDIF compressed image (deprecated)");
    diskFormats << QPair<QString, QString>("Rken", "NDIF compressed (obsolete format)");
    diskFormats << QPair<QString, QString>("DC42", "Disk Copy 4.2 image");

    for (auto format : diskFormats) {
        ui->formatComboBox->addItem(format.first);
    }

    ui->formatComboBox->setCurrentText("UDBZ");
}

void Nedrysoft::MainWindow::onDesignFilesAddButtonClicked(bool dropdown) {
    if (dropdown) {
        QMenu popupMenu;

        auto menuPos = ui->designFilesAddButton->mapToGlobal(ui->designFilesAddButton->rect().bottomLeft());

        popupMenu.addAction("Background Image...");
        popupMenu.addAction("Shortcut To Applications");
        popupMenu.addAction("Shortcut...");
        popupMenu.addAction("Icon...");

        popupMenu.exec(menuPos);
    }
}

void Nedrysoft::MainWindow::onFeatureSliderMinimumValueChanged(int newValue) {
    m_minimumPixelArea = newValue;

    if (ui->featureAutoDetectCheckbox->isChecked()) {
        processBackground();
    }
}

void Nedrysoft::MainWindow::onFontSizeChanged(const QString &text) {
    bool ok = false;
    int size = text.toInt(&ok);

    if (( ok ) && ( size != 0 )) {
        setConfigValue("textSize", size);

        ui->previewWidget->setTextSize(text.toInt());
    }
}

void Nedrysoft::MainWindow::onIconSizeChanged(const QString &text) {
    bool ok = false;
    int size = text.toInt(&ok);

    if (( ok ) && ( size != 0 )) {
        setConfigValue("iconSize", size);

        ui->previewWidget->setIconSize(text.toInt());
    }
}

void Nedrysoft::MainWindow::onAboutDialogTriggered(bool isChecked) {
    Nedrysoft::AboutDialog aboutDialog;

    aboutDialog.exec();
}

void Nedrysoft::MainWindow::onGridVisibilityChanged(int state) {
    ui->previewWidget->setGrid(configValue("grid", QSize(20,20)).value<QSize>(), ( state == Qt::Checked ) ? true : false, true);
}

void Nedrysoft::MainWindow::onIconsVisibilityChanged(int state) {
    ui->previewWidget->setIconsVisible(( state == Qt::Checked ) ? true : false);
}

void Nedrysoft::MainWindow::onFeatureVisibilityChanged(int state) {
    if (!state) {
        ui->previewWidget->clearCentroids();
    } else {
        processBackground();
    }
}

void Nedrysoft::MainWindow::onGridSnapChanged(bool checked) {
    setConfigValue("snapToGrid", checked);

    ui->previewWidget->setGrid(QSize(ui->gridXLineEdit->text().toInt(),ui->gridYLineEdit->text().toInt()),
            ui->gridVisibleCheckbox->isChecked(),
            ui->gridSnapCheckbox->isChecked());
}

void Nedrysoft::MainWindow::onCreateDMG() {
    ui->terminalWidget->println("");

    m_builder->createDMG("~/Desktop/test.dmg");
}

void Nedrysoft::MainWindow::onTerminalReady() {
    auto versionText = QString("%1.%2.%3 %4 %5").arg(APPLICATION_GIT_YEAR).arg(APPLICATION_GIT_MONTH).arg(APPLICATION_GIT_DAY).arg(APPLICATION_GIT_BRANCH).arg(APPLICATION_GIT_HASH);

    ui->stackedWidget->setCurrentIndex(TerminalView::Terminal);

    ui->terminalWidget->println(
            fore(Qt::lightGray)+
            hyperlink(repositoryUrl, "dmge²")+
            fore(Qt::white)+" ("+fore("#3d96f3")+
            hyperlink(QString("%1/commit/%2").arg(repositoryUrl).arg(APPLICATION_GIT_HASH), versionText)+
            fore(Qt::white)+")"+
            reset);

    ui->terminalWidget->println("\r\n"+fore(Qt::lightGray)+"Ready."+reset);
}

void Nedrysoft::MainWindow::initialiseLoader() {
    m_loadingMovie = new QMovie;

    ui->stackedWidget->setCurrentIndex(TerminalView::Loader);

    m_loadingMovie->setFileName(":/images/loading.gif");

    ui->loadingLabel->setMovie(m_loadingMovie);

    QPixmap image(m_loadingMovie->fileName());

    m_loadingMovie->setScaledSize(image.size() / 2);
    m_loadingMovie->start();
}

void Nedrysoft::MainWindow::onTerminalContextMenuTriggered() {
    QMenu menu(this);
    QIcon copyIcon, trashIcon;

    if (Nedrysoft::Utils::ThemeSupport::isDarkMode()) {
        copyIcon = QIcon(QPixmap(":/icons/copy-dark@2x.png").scaled(menuIconSize, menuIconSize));
        trashIcon = QIcon(QPixmap(":/icons/trash-dark@2x.png").scaled(menuIconSize, menuIconSize));
    } else {
        copyIcon = QIcon(QPixmap(":/icons/copy-light@2x.png").scaled(menuIconSize, menuIconSize));
        trashIcon = QIcon(QPixmap(":/icons/trash-light@2x.png").scaled(menuIconSize, menuIconSize));
    }

    auto clearTerminalAction = menu.addAction(trashIcon, tr("Clear"));
    auto copyToClipboardAction = menu.addAction(copyIcon, tr("Copy to clipboard"));

    auto selectedAction = menu.exec(QCursor::pos());

    if (selectedAction) {
        if (selectedAction == clearTerminalAction) {
            ui->terminalWidget->clear();
        } else if (selectedAction == copyToClipboardAction) {
            ui->terminalWidget->getTerminalBuffer();
        }
    }
}

void Nedrysoft::MainWindow::onTerminalUrlClicked(QString url) {
    QDesktopServices::openUrl(url);
}

void Nedrysoft::MainWindow::copyTerminalBufferToClipboard(QString terminalBuffer) {
    auto clipboard = QGuiApplication::clipboard();

    clipboard->setText(terminalBuffer);
}