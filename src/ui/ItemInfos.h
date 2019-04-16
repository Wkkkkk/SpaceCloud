/*
 * Copyright (c) 2018 Ally of Intelligence Technology Co., Ltd. All rights reserved.
 *
 * Created by WuKun on 4/12/19.
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

#ifndef SPACECLOUD_ITEMINFOS_H
#define SPACECLOUD_ITEMINFOS_H

#include <iostream>

#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtCore/QDataStream>

#include <osgEarth/Viewpoint>

#include "Common.h"

struct ItemInfos {
    enum DataType {
        DemOrDom = 0,
        ShpFile,
        User,
        Others
    };

    QString name;
    DataType type;
    QStringList file_path;
    osgEarth::Viewpoint localtion;
    bool persistence;

    inline bool operator==(const ItemInfos &other) const {
        return this->name == other.name;
    }
};

Q_DECLARE_METATYPE(ItemInfos);

// print to iostrem
std::ostream &operator<<(std::ostream &output, const ItemInfos &info);

// save to / load from file
QDataStream &operator<<(QDataStream &out, const ItemInfos &info);

QDataStream &operator>>(QDataStream &out, ItemInfos &info);

#endif //SPACECLOUD_ITEMINFOS_H
