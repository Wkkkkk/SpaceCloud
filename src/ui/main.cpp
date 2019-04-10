/*
 * Copyright (c) 2018 Ally of Intelligence Technology Co., Ltd. All rights reserved.
 *
 * Created by WuKun on 4/10/19.
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

#include <QtWidgets/QApplication>
#include <QtCore/QTranslator>

#include "MainWindow.h"
#include "Version.h"

int main(int argc, char *argv[]) {
    qputenv("QT_STYLE_OVERRIDE", ""); // suppress the qt style warning
    QApplication app(argc, argv);

    QTranslator translator;
    if (!app.arguments().contains("en"))
        if (translator.load(QLocale("zh"), QLatin1String("lpd"), QLatin1String("_"), app.applicationDirPath() + "/tr"))
            app.installTranslator(&translator);

    MainWindow mainwindow;
    mainwindow.setMinimumSize(800, 600);  //avoid graphic context bugs!
    mainwindow.show();

    return app.exec();
}