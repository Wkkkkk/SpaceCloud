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

#include <osg/Switch>

#include <QtCore/QString>
#include <QtCore/QFileInfo>
#include <QtCore/QThread>

class OctreeBuilder : public QThread {
Q_OBJECT
public:
    OctreeBuilder() = default;

    ~OctreeBuilder() final = default;

    Q_DISABLE_COPY(OctreeBuilder);

    void run() override;

    QFileInfo pcd_file_path_;
    QFileInfo output_file_dir_;
    unsigned int depth_ = 5;
    float resolution_ = 128.0f;

    bool filter_static_points_ = false;
    unsigned int seatch_k_ = 10;
    double std_factor_ = 0.5;

    bool filter_isolated_points_ = false;
    double search_radius_ = 1.0;
    unsigned int min_neighbors_in_radius_ = 1;

    bool pca_transform_ = false;
private:
    osg::Switch *getAllLeafNodes();

signals:

    void progressValue(int);
};


#endif //SPACECLOUD_OCTREEBUILDER_H
