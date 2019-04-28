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

#ifndef SPACECLOUD_FILELOADWIDGET_H
#define SPACECLOUD_FILELOADWIDGET_H

#include <QtWidgets/QDialog>
#include <QtWidgets/QAction>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QButtonGroup>
#include <QtGui/QKeyEvent>

#include "ItemInfos.h"

/**
 * @brief A file read dialog
 * It load osg .osgb files to scene
 */
class FileLoadWidget : public QDialog {
Q_OBJECT
public:
    explicit FileLoadWidget(QWidget *parent = nullptr);

    ~FileLoadWidget() final = default;

    void init();

    Q_DISABLE_COPY(FileLoadWidget);

private:
    QLineEdit *names_;
    QLineEdit *coords_;
    QTextEdit *file_list_;
    QButtonGroup *radio_button_group_;
signals:

    void loadItem(ItemInfos infos);

private slots:

    void openFileReadDialog();

    void loadFileFromPaths();
};


#endif //SPACECLOUD_FILELOADWIDGET_H
