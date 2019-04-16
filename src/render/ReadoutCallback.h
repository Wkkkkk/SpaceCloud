/*
 * Copyright (c) 2018 Ally of Intelligence Technology Co., Ltd. All rights reserved.
 *
 * Created by WuKun on 4/16/19.
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

#ifndef SPACECLOUD_READOUTCALLBACK_H
#define SPACECLOUD_READOUTCALLBACK_H

#include <osgEarth/Registry>
#include <osgEarth/ObjectIndex>
#include <osgEarthUtil/Controls>
#include <osgEarthUtil/RTTPicker>
#include <osgEarthFeatures/Feature>
#include <osgEarthFeatures/FeatureIndex>

using namespace osgEarth::Util;
using namespace osgEarth::Util::Controls;
using namespace osgEarth::Features;

/**
 * Query Callback that displays the targeted feature's attributes in a
 * user interface grid control.
 */
class ReadoutCallback : public RTTPicker::Callback {
public:
    ReadoutCallback(ControlCanvas *container) : _lastFID(~0) {
        _grid = new Grid();
        _grid->setBackColor(osg::Vec4(0, 0, 0, 0.7f));
        container->addControl(_grid);
    }

    void onHit(ObjectID id) {
        FeatureIndex *index = Registry::objectIndex()->get<FeatureIndex>(id).get();
        Feature *feature = index ? index->getFeature(id) : 0L;
        if (feature && feature->getFID() != _lastFID) {
            _grid->clearControls();
            unsigned r = 0;

            _grid->setControl(0, r, new LabelControl("FID", Color::Red));
            _grid->setControl(1, r, new LabelControl(Stringify() << feature->getFID(), Color::White));
            ++r;

            const AttributeTable &attrs = feature->getAttrs();
            for (AttributeTable::const_iterator i = attrs.begin(); i != attrs.end(); ++i, ++r) {
                _grid->setControl(0, r, new LabelControl(i->first, 14.0f, Color::Yellow));
                _grid->setControl(1, r, new LabelControl(i->second.getString(), 14.0f, Color::White));
            }
            if (!_grid->visible())
                _grid->setVisible(true);

            _lastFID = feature->getFID();
        }
    }

    void onMiss() {
        _grid->setVisible(false);
        _lastFID = 0u;
    }

    bool accept(const osgGA::GUIEventAdapter &ea, const osgGA::GUIActionAdapter &aa) {
        return ea.getEventType() == ea.RELEASE; // click
    }

    Grid *_grid;
    FeatureID _lastFID;
};

#endif //SPACECLOUD_READOUTCALLBACK_H
