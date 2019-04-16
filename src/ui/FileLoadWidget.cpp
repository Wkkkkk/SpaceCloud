/*
 * Copyright (c) 2018 Ally of Intelligence Technology Co., Ltd. All rights reserved.
 *
 * Created by WuKun on 4/15/19.
 * Contact with:wk707060335@gmail.com
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http: *www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>

#include <boost/log/trivial.hpp>

#include "FileLoadWidget.h"

FileLoadWidget::FileLoadWidget(QWidget *parent)
        : QDialog(parent) {
    setWindowTitle(tr("FileLoadDialog"));
    setAttribute(Qt::WA_DeleteOnClose);
//    setMinimumSize(640, 480);

    init();
}

void FileLoadWidget::init() {
    auto gridlayout = new QGridLayout(this);
    gridlayout->setSpacing(6);
    gridlayout->setContentsMargins(10, 10, 10, 10);

    //open
    {
        QPushButton *openFileButton = new QPushButton(tr("OpenFile"), this);
        openFileButton->setIcon(QIcon(""));
        gridlayout->addWidget(openFileButton, 0, 0, 1, 1);

        connect(openFileButton, &QPushButton::clicked, this, &FileLoadWidget::openFileReadDialog);

        file_list_ = new QTextEdit(this);
        gridlayout->addWidget(file_list_, 0, 1, 1, 3);
    }

    //name
    {
        QLabel *label = new QLabel(tr("Names"), this);
        gridlayout->addWidget(label, 1, 0, 1, 1);

        names_ = new QLineEdit(this);
        names_->setEnabled(true);
        names_->setPlaceholderText(tr("MyName"));
        gridlayout->addWidget(names_, 1, 1, 1, 3);
    }

    //location
    {
        QLabel *label = new QLabel(tr("Location"), this);
        gridlayout->addWidget(label, 2, 0, 1, 1);

        coords_ = new QLineEdit(this);
        coords_->setEnabled(true);
        coords_->setPlaceholderText(tr("lon, lat, hei"));
        gridlayout->addWidget(coords_, 2, 1, 1, 3);
    }

    //persistence
    {
        QLabel *label = new QLabel(tr("Persistence"), this);
        gridlayout->addWidget(label, 3, 0, 1, 1);

        radio_button_group_ = new QButtonGroup(this);

        auto yes_radio_button = new QRadioButton(tr("Yes"), this);
        yes_radio_button->setChecked(true);
        auto no_radio_button = new QRadioButton(tr("No"), this);

        radio_button_group_->addButton(yes_radio_button, 0);
        radio_button_group_->addButton(no_radio_button, 1);

        gridlayout->addWidget(yes_radio_button, 3, 1, 1, 1);
        gridlayout->addWidget(no_radio_button, 3, 2, 1, 1);
    }

    //buttons
    {
        QPushButton *loadButton = new QPushButton(tr("Load"), this);
        gridlayout->addWidget(loadButton, 4, 0, 1, 2);

        connect(loadButton, &QPushButton::clicked, this, &FileLoadWidget::loadFileFromPaths);


        QPushButton *cancelButton = new QPushButton(tr("Cancel"), this);
        gridlayout->addWidget(cancelButton, 4, 2, 1, 2);

        connect(cancelButton, &QPushButton::clicked, this, &FileLoadWidget::close);
    }
}

void FileLoadWidget::openFileReadDialog() {
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFiles);
    dialog.setNameFilter(tr("OSG Models (*.osg *.osgb)"));
    dialog.setViewMode(QFileDialog::Detail);
    QStringList fileNames;
    if (dialog.exec()) {
        fileNames = dialog.selectedFiles();

        QString file_list_str = fileNames.join("\n");
        file_list_->setText(file_list_str);

        BOOST_LOG_TRIVIAL(trace) << "Found files: " << file_list_str.toStdString();
    }
}

void FileLoadWidget::loadFileFromPaths() {

    QString name_str = names_->text().trimmed();
    QString file_list_str = file_list_->toPlainText().trimmed();
    QString location_str = coords_->text().trimmed();
    QStringList coords_str_list = location_str.split(QRegExp("[,*/^]"));
    bool persistence = radio_button_group_->checkedId() == 0;

    if (name_str.isEmpty() || file_list_str.isEmpty() || location_str.isEmpty() || coords_str_list.size() != 3) {
        QMessageBox::warning(this, tr("Warning"), tr("Invalid Input!"), QMessageBox::Yes, QMessageBox::NoButton);
        return;
    }

    double lon = coords_str_list[0].toDouble();
    double lat = coords_str_list[1].toDouble();
    double height = coords_str_list[2].toDouble();

    osgEarth::Viewpoint viewpoint(name_str.toStdString().data(), lon, lat, height, 0, -90, 1000);

    ItemInfos infos;
    infos.persistence = persistence;
    infos.name = name_str;
    for (const QString &file : file_list_str.split("\n")) {
        QFileInfo file_info(file);
        if (file_info.exists()) infos.file_path << file;
    }
    infos.type = ItemInfos::DataType::User;
    infos.localtion = viewpoint;

    BOOST_LOG_TRIVIAL(trace) << "FileLoadWidget emit item: " << infos;
    emit loadItem(infos);

    this->close();
}