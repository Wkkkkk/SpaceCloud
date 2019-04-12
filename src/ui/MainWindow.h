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
#ifndef SPACECLOUD_MAINWINDOW_H
#define SPACECLOUD_MAINWINDOW_H

#include <memory>
#include <iostream>

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QTreeWidgetItem>
#include <QtWidgets/QAction>

//forward declaration
class OSGWidget;

class ItemInfos;

/**
 * @brief the Mainwindow class
 * It takes control of the whole program
 */
class MainWindow : public QMainWindow {
Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() final = default;

    Q_DISABLE_COPY(MainWindow);
protected:
    //!inherit from QMainWindow
    virtual void keyPressEvent(QKeyEvent *event);
private:
    /**
     * @brief It inits all members
     * @param none
     * */
    void initUI();

    void createMenu();

    void createToolBar();

    void createDockWidget();

    /**
     * @brief It inits all members
     * add infomations about a item to its value
     * */
    void addVariantToTreeWidgetItem(const ItemInfos &info, QTreeWidgetItem *item);

    //core widget
    OSGWidget *osgwidget_;

    //other widgets
    QDockWidget *dock_widget_;
    QTreeWidget *tree_widget_;

    //buttons
    QAction *open_file_action_;

signals:
    void test_signal();

private slots:
    void open();

    void TreeWidgetClicked(QTreeWidgetItem *, int);

    void TreeWidgetDoubleClicked(QTreeWidgetItem *, int);
};

#endif //SPACECLOUD_MAINWINDOW_H
