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

void MouseCoordsCallback::set(const osgEarth::GeoPoint &wgsPoint, osg::View *view, osgEarth::MapNode *mapNode) {
    auto print2screen = [](const osgEarth::GeoPoint &geo_point) -> std::string {
        std::string coords_str = Stringify()
                << std::fixed
                << geo_point.x()
                << ", " << geo_point.y()
                << ", " << geo_point.z()
                << " | " << geo_point.getSRS()->getName()
                << "\n";

        return coords_str;
    };

    if (label_.valid()) {
        osg::Vec3d eye, center, up;
        view->getCamera()->getViewMatrixAsLookAt(eye, center, up);
        osg::Vec3d world;
        wgsPoint.toWorld(world);

        std::string coords_str = print2screen(wgsPoint);

        osg::ref_ptr<const SpatialReference> srs = SpatialReference::create("wgs84");
        osg::ref_ptr<const SpatialReference> utm = srs->createUTMFromLonLat(osgEarth::Angle(wgsPoint.x()),
                                                                            osgEarth::Angle(wgsPoint.y()));
        osgEarth::GeoPoint utmPoint = wgsPoint.transform(utm);
        if (utmPoint.isValid()) {
            coords_str += print2screen(utmPoint);
        }

        label_->setText(coords_str);
    }
}

void MouseCoordsCallback::reset(osg::View *view, osgEarth::MapNode *mapNode) {
    if (label_.valid()) {
        label_->setText("");
    }
}
