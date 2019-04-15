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
#include <algorithm>

#include "ItemInfos.h"

std::ostream &operator<<(std::ostream &output, const ItemInfos &info) {
    output << info.name.toStdString() << " " << info.localtion.toString() << " " << info.file_path.first().toStdString()
           << " "
           << info.persistence;
    return output;
}

QDataStream &operator<<(QDataStream &out, const ItemInfos &info) {
    out << info.name \
 << info.type \
 << info.file_path \
 << info.localtion.getConfig().toJSON().c_str() \
 << info.persistence;

    return out;
}

QDataStream &operator>>(QDataStream &out, ItemInfos &info) {
    QString name;
    int type;
    QStringList file_path;
    char *config = new char(100);
    bool persistence;

    out >> name >> type >> file_path >> config >> persistence;
    std::string config_str = config;

    ItemInfos infos;
    infos.name = name;
    infos.type = ItemInfos::DataType(type);
    infos.file_path = file_path;
    infos.localtion = osgEarth::Viewpoint(osgEarth::Config::readJSON(config_str));
    infos.persistence = persistence;

    std::swap(info, infos);

    return out;
}
