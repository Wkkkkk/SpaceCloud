/*
 * Copyright (c) 2018 Ally of Intelligence Technology Co., Ltd. All rights reserved.
 *
 * Created by WuKun on 4/26/19.
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

#ifndef SPACECLOUD_OCTREEBUILDER_H
#define SPACECLOUD_OCTREEBUILDER_H

#include <QtCore/QString>
#include <QtCore/QFileInfo>
#include <QtCore/QThread>

class OctreeBuilder : public QThread {
Q_OBJECT
public:
    OctreeBuilder();

    ~OctreeBuilder() final = default;

    inline void setInputFilePath(const QFileInfo &pcd_file_path) {
        pcd_file_path_ = pcd_file_path;
    }

    inline void setOutPutFilePath(const QFileInfo &output_file_path) {
        output_file_path_ = output_file_path;
    }

    inline void setResolution(float resolution) {
        resolution_ = resolution;
    }

    inline void setFilterIsolatedPoints(bool filter_isolated_points) {
        filter_isolated_points_ = filter_isolated_points;
    }

    inline void setFilterStaticPoints(bool filter_static_points) {
        filter_static_points_ = filter_static_points;
    }

    void build();

    Q_DISABLE_COPY(OctreeBuilder);
private:

    osg::Switch *getAllLeafPoints();

    QFileInfo pcd_file_path_;
    QFileInfo output_file_path_;
    float resolution_ = 128.0f;
    bool filter_isolated_points_ = false;
    bool filter_static_points_ = false;

signals:

    void progress_value(int);
};


#endif //SPACECLOUD_OCTREEBUILDER_H
