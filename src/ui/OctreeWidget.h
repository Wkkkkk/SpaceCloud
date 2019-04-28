/*
 * Copyright (c) 2018 Ally of Intelligence Technology Co., Ltd. All rights reserved.
 *
 * Created by WuKun on 4/28/19.
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

#ifndef SPACECLOUD_OCTREEWIDGET_H
#define SPACECLOUD_OCTREEWIDGET_H

#include <QtWidgets/QWidget>
#include <QtWidgets/QDialog>
#include <QtWidgets/QAction>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QProgressBar>

class OctreeWidget : public QDialog {
Q_OBJECT
public:
    explicit OctreeWidget(QWidget *parent = nullptr);

    ~OctreeWidget() final;

    Q_DISABLE_COPY(OctreeWidget);

private:
    void init();

    QLineEdit *input_file_path_edit_;
    QLineEdit *output_file_path_edit_;
    QSpinBox *depth_spin_box_;
    QSpinBox *resolution_spin_box_;

    QCheckBox *statistical_filter_box_;
    QDoubleSpinBox *search_k_box_;
    QDoubleSpinBox *std_factor_box_;

    QCheckBox *isolated_filter_box_;
    QDoubleSpinBox *search_radius_box_;
    QDoubleSpinBox *min_neighbors_in_radius_box_;

    QProgressBar *progress_bar_;
    QPushButton *generate_button_;

private slots:

    void openFileImportDialog();

    void openFileExportDialog();

    void generate();
};


#endif //SPACECLOUD_OCTREEWIDGET_H
