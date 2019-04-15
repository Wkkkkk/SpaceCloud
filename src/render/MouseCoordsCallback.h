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

#ifndef SPACECLOUD_MOUSECOORDSCALLBACK_H
#define SPACECLOUD_MOUSECOORDSCALLBACK_H

#include <osgText/Text>
#include <osgEarthUtil/MouseCoordsTool>

namespace osgEarth {
    class MapNode;

    class GeoPoint;
};

class MouseCoordsCallback : public osgEarth::Util::MouseCoordsTool::Callback {
public:
    explicit MouseCoordsCallback(osgText::Text *label);

    virtual ~MouseCoordsCallback() = default;

    virtual void set(const osgEarth::GeoPoint &mapCoords, osg::View *view, osgEarth::MapNode *mapNode);

    virtual void reset(osg::View *view, osgEarth::MapNode *mapNode);

protected:
    osg::ref_ptr<osgText::Text> label_;
};


#endif //SPACECLOUD_MOUSECOORDSCALLBACK_H
