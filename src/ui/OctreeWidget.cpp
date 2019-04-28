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

#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QSlider>
#include <QtWidgets/QFrame>
#include <QtCore/QtGlobal>

#include <boost/log/trivial.hpp>

#include "OctreeWidget.h"
#include "OctreeBuilder.h"

OctreeWidget::OctreeWidget(QWidget *parent) :
        QDialog(parent) {
    setWindowTitle(tr("OctreeWidget"));
    setAttribute(Qt::WA_DeleteOnClose);
    setMinimumSize(550, 330);

    init();
}

OctreeWidget::~OctreeWidget() = default;

void OctreeWidget::init() {
    auto main_layout = new QVBoxLayout;
    this->setLayout(main_layout);
    main_layout->setSpacing(6);
    main_layout->setContentsMargins(10, 10, 10, 10);

    // input area
    {
        auto layout = new QHBoxLayout;

        QPushButton *openFileButton = new QPushButton(tr("InputFile"), this);
        openFileButton->setIcon(QIcon(""));
        layout->addWidget(openFileButton);

        connect(openFileButton, &QPushButton::clicked, this, &OctreeWidget::openFileImportDialog);

        input_file_path_edit_ = new QLineEdit(this);
//        input_file_path_edit_->setText("/home/zhihui/workspace/data/PointCloud/dongxiao/global.pcd");
        layout->addWidget(input_file_path_edit_);
        main_layout->addLayout(layout);
    }

    // output area
    {
        auto layout = new QHBoxLayout;

        QPushButton *openFileButton = new QPushButton(tr("OutputDir"), this);
        openFileButton->setIcon(QIcon(""));
        layout->addWidget(openFileButton);

        connect(openFileButton, &QPushButton::clicked, this, &OctreeWidget::openFileExportDialog);

        output_file_path_edit_ = new QLineEdit(this);
//        output_file_path_edit_->setText("/home/zhihui/workspace/data/PointCloud/dongxiao/rsss");
        layout->addWidget(output_file_path_edit_);
        main_layout->addLayout(layout);
    }

    // advanced area
    {
        auto group = new QGroupBox(tr("Advanced"));
        main_layout->addWidget(group);
        auto vbox = new QVBoxLayout;
        group->setLayout(vbox);

        // depth
        {
            auto hbox = new QHBoxLayout;

            auto label = new QLabel(tr("depth"));
            auto slider = new QSlider(Qt::Horizontal);
            slider->setMinimum(2);
            slider->setMaximum(10);
            slider->setValue(5);
            slider->setSingleStep(1);
            slider->setTickInterval(1);
            slider->setTickPosition(QSlider::TicksAbove);

            depth_spin_box_ = new QSpinBox;
            depth_spin_box_->setMinimum(2);
            depth_spin_box_->setMaximum(10);
            depth_spin_box_->setValue(5);
            depth_spin_box_->setSingleStep(1);

            connect(slider, &QSlider::valueChanged, depth_spin_box_, &QSpinBox::setValue);
#if QT_VERSION <= QT_VERSION_CHECK(5, 7, 0)
            connect(depth_spin_box_, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), slider,
                    &QSlider::setValue);
#else
            connect(depth_spin_box_, qOverload<int>(&QSpinBox::valueChanged), slider, &QSlider::setValue);
#endif

            hbox->addWidget(label);
            hbox->addWidget(slider);
            hbox->addWidget(depth_spin_box_);
            vbox->addLayout(hbox);
        }

        // resolution
        {
            auto hbox = new QHBoxLayout;

            auto label = new QLabel(tr("resolution"));
            auto slider = new QSlider(Qt::Horizontal);
            slider->setMinimum(10);
            slider->setMaximum(500);
            slider->setValue(128);
            slider->setSingleStep(10);
            slider->setTickInterval(10);
            slider->setTickPosition(QSlider::TicksBothSides);

            resolution_spin_box_ = new QSpinBox;
            resolution_spin_box_->setMinimum(10);
            resolution_spin_box_->setMaximum(500);
            resolution_spin_box_->setValue(128);
            resolution_spin_box_->setSingleStep(10);

            connect(slider, &QSlider::valueChanged, resolution_spin_box_, &QSpinBox::setValue);
#if QT_VERSION <= QT_VERSION_CHECK(5, 7, 0)
            connect(resolution_spin_box_, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), slider,
                    &QSlider::setValue);
#else
            connect(resolution_spin_box_, qOverload<int>(&QSpinBox::valueChanged), slider, &QSlider::setValue);
#endif

            hbox->addWidget(label);
            hbox->addWidget(slider);
            hbox->addWidget(resolution_spin_box_);
            vbox->addLayout(hbox);
        }

        // line
        {
            auto line = new QFrame;
            line->setFrameShape(QFrame::HLine);
            line->setFrameShadow(QFrame::Sunken);
            vbox->addWidget(line);
        }

        //filter
        {
            {
                auto hbox = new QHBoxLayout;

                statistical_filter_box_ = new QCheckBox(tr("statistical filter"));
                auto search_k_label = new QLabel(tr("MeanK"));
                auto std_factor_label = new QLabel(tr("StddevMulThresh"));

                search_k_box_ = new QDoubleSpinBox;
                search_k_box_->setRange(1, 20);
                search_k_box_->setDecimals(0);
                search_k_box_->setSingleStep(1);
                search_k_box_->setValue(10);

                std_factor_box_ = new QDoubleSpinBox;
                std_factor_box_->setRange(0.5, 3.0);
                std_factor_box_->setDecimals(2);
                std_factor_box_->setSingleStep(0.1);
                std_factor_box_->setValue(0.5);

                hbox->addWidget(statistical_filter_box_);
                hbox->addStretch(1);
                hbox->addWidget(search_k_label);
                hbox->addWidget(search_k_box_);
                hbox->addWidget(std_factor_label);
                hbox->addWidget(std_factor_box_);

                vbox->addLayout(hbox);
            }

            {
                auto hbox = new QHBoxLayout;

                isolated_filter_box_ = new QCheckBox(tr("isolated filter"));
                auto search_radius_label = new QLabel(tr("RadiusSearch"));
                auto min_neighbors_label = new QLabel(tr("MinNeighborsInRadius"));

                search_radius_box_ = new QDoubleSpinBox;
                search_radius_box_->setRange(1, 20);
                search_radius_box_->setDecimals(0);
                search_radius_box_->setSingleStep(1);
                search_radius_box_->setValue(1);

                min_neighbors_in_radius_box_ = new QDoubleSpinBox;
                min_neighbors_in_radius_box_->setRange(1, 20);
                min_neighbors_in_radius_box_->setDecimals(0);
                min_neighbors_in_radius_box_->setSingleStep(1);
                min_neighbors_in_radius_box_->setValue(5);

                hbox->addWidget(isolated_filter_box_);
                hbox->addStretch(1);
                hbox->addWidget(search_radius_label);
                hbox->addWidget(search_radius_box_);
                hbox->addWidget(min_neighbors_label);
                hbox->addWidget(min_neighbors_in_radius_box_);

                vbox->addLayout(hbox);
            }
        }
    }

    // progress bar
    {
        progress_bar_ = new QProgressBar(this);
        progress_bar_->setOrientation(Qt::Horizontal);
        progress_bar_->setMinimum(0);
        progress_bar_->setMaximum(100);
        progress_bar_->setValue(0);

        main_layout->addWidget(progress_bar_);
    }

    // buttons
    {
        auto hbox = new QHBoxLayout;
        main_layout->addLayout(hbox);
        hbox->addStretch(1);

        generate_button_ = new QPushButton(tr("generate"), this);
        hbox->addWidget(generate_button_);

        connect(generate_button_, &QPushButton::clicked, this, &OctreeWidget::generate);

        QPushButton *cancel_button = new QPushButton(tr("cancel"), this);
        hbox->addWidget(cancel_button);

        connect(cancel_button, &QPushButton::clicked, this, &OctreeWidget::close);
    }
}

void OctreeWidget::generate() {
    QFileInfo input_file_info(input_file_path_edit_->text());
    QFileInfo output_file_info(output_file_path_edit_->text());
    if (!input_file_info.exists() || !output_file_info.isDir()) {
        QMessageBox::warning(this, tr("Warning"), tr("Invalid Input!"), QMessageBox::Yes, QMessageBox::NoButton);
        return;
    }

    auto octree_builder_ = new OctreeBuilder;
    octree_builder_->pcd_file_path_ = input_file_info;
    octree_builder_->output_file_dir_ = output_file_info;
    octree_builder_->depth_ = depth_spin_box_->value();
    octree_builder_->resolution_ = resolution_spin_box_->value();
    octree_builder_->filter_static_points_ = statistical_filter_box_->isChecked();
    octree_builder_->seatch_k_ = search_k_box_->cleanText().toUInt();
    octree_builder_->std_factor_ = std_factor_box_->value();
    octree_builder_->filter_isolated_points_ = isolated_filter_box_->isChecked();
    octree_builder_->search_radius_ = search_radius_box_->cleanText().toUInt();
    octree_builder_->min_neighbors_in_radius_ = min_neighbors_in_radius_box_->cleanText().toUInt();

    connect(octree_builder_, &OctreeBuilder::progressValue, progress_bar_, &QProgressBar::setValue);
    connect(octree_builder_, &OctreeBuilder::finished, octree_builder_, &QObject::deleteLater);
    connect(octree_builder_, &OctreeBuilder::started, [this]() { generate_button_->setEnabled(false); });
    connect(octree_builder_, &OctreeBuilder::finished, [this]() {
        generate_button_->setEnabled(true);
        BOOST_LOG_TRIVIAL(trace) << "Octree Build successfully!";
    });
    octree_builder_->start();
}

void OctreeWidget::openFileImportDialog() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open PCD File"),
                                                    "",
                                                    tr("Files (*.pcd)"));
    if (!fileName.isEmpty()) {
        input_file_path_edit_->setText(fileName);
        BOOST_LOG_TRIVIAL(trace) << "find file: " << fileName.toStdString();
    }
}

void OctreeWidget::openFileExportDialog() {
    QString dir = QFileDialog::getExistingDirectory(this, tr("Save Directory"),
                                                    ".",
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty()) {
        output_file_path_edit_->setText(dir);
        BOOST_LOG_TRIVIAL(trace) << "export to: " << dir.toStdString();
    }
}