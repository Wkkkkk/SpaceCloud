/*
 * Copyright (c) 2018 Ally of Intelligence Technology Co., Ltd. All rights reserved.
 *
 * Created by WuKun on 2/15/19.
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
#include <utility>
#include <stdio.h>

#include <QtGui/QIcon>
#include <QtGui/QKeyEvent>
#include <QtWidgets/QMenu>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QApplication>

#include <boost/log/trivial.hpp>

#include "OSGWidget.h"
#include "MainWindow.h"
#include "ItemInfos.h"

//using namespace core;

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent)
{
    this->setWindowTitle("SpaceCloud");
    this->grabKeyboard();

    osgwidget_ = new OSGWidget(this);
    this->setCentralWidget(osgwidget_);
    osgwidget_->init();

    initUI();
}

void MainWindow::initUI() {
    createMenu();
    createToolBar();
    createDockWidget();
}

void MainWindow::open() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open OSG Model"),
                                                    "./model",
                                                    tr("OSG Models (*.osg *.osgt *.osgb)"));
    if (fileName.isEmpty()) return;

    QFileInfo f(fileName);
    osgwidget_->readDataFromFile(f);
}

void MainWindow::createMenu() {
    open_file_action_ = new QAction(tr("Open"), this);
    open_file_action_->setIcon(QIcon(":/images/file_open.png"));
    connect(open_file_action_, &QAction::triggered, this, &MainWindow::open);
}

void MainWindow::createToolBar() {
    QToolBar *toolBar = addToolBar("Tools");

    toolBar->addAction(open_file_action_);
    toolBar->addSeparator();
}

void MainWindow::createDockWidget() {
    tree_widget_ = new QTreeWidget(this);
    tree_widget_->setColumnCount(1);
    tree_widget_->setHeaderHidden(true);
    //tree_widget_->setColumnWidth(0, 100);
    //tree_widget_->setStyleSheet("QTreeWidget::item {height:25px;");

    // index: 0
    {
        QTreeWidgetItem *item = new QTreeWidgetItem(tree_widget_, QStringList("DEM/DOM"));
//        item->setIcon(0, QIcon(""));
        item->setExpanded(true);
    }

    // 1
    {
        QTreeWidgetItem *item = new QTreeWidgetItem(tree_widget_, QStringList("ShapeFile"));
        item->setExpanded(true);
    }

    // 2
    {
        QTreeWidgetItem *item = new QTreeWidgetItem(tree_widget_, QStringList("User"));
        item->setExpanded(true);

        QStringList all_items = {"Point Cloud", "Oblique Models", "OSG Models"};
        for (const auto &name : all_items) {
            QTreeWidgetItem *child_item = new QTreeWidgetItem(item, QStringList(name));
            child_item->setExpanded(true);
        }
    }

    // 3
    {
        QTreeWidgetItem *item = new QTreeWidgetItem(tree_widget_, QStringList("City Marker"));
        item->setExpanded(true);

        std::vector<ItemInfos> all_items = {
                {"ShenZhen", DataType::Others, "", osgEarth::Viewpoint("ShenZhen", 114.06, 22.55, 0, 0, -90,
                                                                       1000),                                      false},
                {"BeiJing",  DataType::Others, "", osgEarth::Viewpoint("BeiJing", 116.30, 39.90, 0, 0, -90,
                                                                       1000),                                      false},
                {"Boston",   DataType::Others, "", osgEarth::Viewpoint("Boston", -71.07, 42.34, 0, 0, -90,
                                                                       1000),                                      false}
        };

        for (const auto &info : all_items) {
            QTreeWidgetItem *child_item = new QTreeWidgetItem(item, QStringList(info.name));
            child_item->setExpanded(true);

            addVariantToTreeWidgetItem(info, child_item);
        }
    }

    dock_widget_ = new QDockWidget("Scene", this);
    dock_widget_->setFixedWidth(200);
    dock_widget_->setFeatures(QDockWidget::AllDockWidgetFeatures);
    dock_widget_->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    dock_widget_->setWidget(tree_widget_);
    this->addDockWidget(Qt::LeftDockWidgetArea, dock_widget_);

    //QTreeWidget connect
    connect(tree_widget_, &QTreeWidget::itemPressed, this, &MainWindow::TreeWidgetClicked);
    connect(tree_widget_, &QTreeWidget::itemDoubleClicked, this, &MainWindow::TreeWidgetDoubleClicked);
}

void MainWindow::addVariantToTreeWidgetItem(const ItemInfos &info, QTreeWidgetItem *item) {
    QVariant item_var;
    item_var.setValue(info);
    item->setData(0, Qt::UserRole, item_var);
}


void MainWindow::keyPressEvent(QKeyEvent *event) {
    osgwidget_->keyPressEvent(event);
//    QWidget::keyPressEvent(event);
}

void MainWindow::TreeWidgetClicked(QTreeWidgetItem *item, int column) {
    if (!(QApplication::mouseButtons() & Qt::RightButton)) return;

    // valid check
    QVariant item_var = item->data(column, Qt::UserRole);
    if (item_var.isNull()) return;

    ItemInfos infos = item_var.value<ItemInfos>();
    BOOST_LOG_TRIVIAL(trace) << "TreeWidgetClicked at " << infos;

    // type convert

    // menu exec

//
//    QAction *newAct = new QAction(QIcon(":/images/connection.png"), tr("&Act"), this);
//    newAct->setStatusTip(tr("some act"));
//    connect(newAct, SIGNAL(triggered()), this, SLOT(newDev()));
//
//    QMenu menu(this);
//    menu.addAction(newAct);
//
//    menu.exec(QCursor::pos());
}

void MainWindow::TreeWidgetDoubleClicked(QTreeWidgetItem *item, int column) {

    // valid check
    QVariant item_var = item->data(column, Qt::UserRole);
    if (item_var.isNull()) return;

    ItemInfos infos = item_var.value<ItemInfos>();
    BOOST_LOG_TRIVIAL(trace) << "TreeWidgetDoubleClicked at " << infos;

    osgwidget_->flyToViewPoint(infos.localtion);
}