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

#include <osgEarthUtil/Common>
#include <osgEarthUtil/Controls>
#include <osgEarthUtil/Formatter>
#include <osgEarthUtil/MouseCoordsTool>

#include <osgEarth/MapNode>
#include <osgEarth/TerrainEngineNode>

#include "MouseCoordsCallback.h"

using namespace osgEarth;
using namespace osgEarth::Util;

MouseCoordsCallback::MouseCoordsCallback(osgText::Text *label)
        : label_(label) {

}

void MouseCoordsCallback::set(const osgEarth::GeoPoint &mapCoords, osg::View *view, osgEarth::MapNode *mapNode) {
    if (label_.valid()) {
        osg::Vec3d eye, center, up;
        view->getCamera()->getViewMatrixAsLookAt(eye, center, up);
        osg::Vec3d world;
        mapCoords.toWorld(world);
        double range = (eye - world).length();

        label_->setText(Stringify()
                                << std::fixed
                                << mapCoords.x()
                                << ", " << mapCoords.y()
                                << ", " << mapCoords.z()
                                << "; RNG:" << range
                                << "  |  "
                                << mapCoords.getSRS()->getName());

    }
}

void MouseCoordsCallback::reset(osg::View *view, osgEarth::MapNode *mapNode) {
    if (label_.valid()) {
        label_->setText("");
    }
}
