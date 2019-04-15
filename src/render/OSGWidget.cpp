/*
 * Copyright (c) 2018 Ally of Intelligence Technology Co., Ltd. All rights reserved.
 *
 * Created by WuKun on 3/18/19.
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
#include <stdio.h>

#include <QtGui/QKeyEvent>
#include <QtGui/QPainter>

#include <osg/Timer>
#include <osg/Geometry>
#include <osg/Material>
#include <osg/LineWidth>
#include <osg/PolygonMode>
#include <osg/ShapeDrawable>
#include <osg/MatrixTransform>
#include <osg/ComputeBoundsVisitor>
#include <osg/PositionAttitudeTransform>
#include <osgGA/TerrainManipulator>
#include <osgGA/TrackballManipulator>
#include <osgGA/StateSetManipulator>
#include <osgViewer/ViewerEventHandlers>
#include <osgDB/ReadFile>

#include <osgEarth/ImageLayer>
#include <osgEarth/GeoTransform>
#include <osgEarth/Registry>
#include <osgEarthUtil/Sky>
#include <osgEarthUtil/ObjectLocator>
#include <osgEarthUtil/EarthManipulator>
#include <osgEarthUtil/LogarithmicDepthBuffer>

#include <osgEarthFeatures/FeatureModelLayer>
#include <osgEarthDrivers/gdal/GDALOptions>
#include <osgEarthDrivers/feature_ogr/OGRFeatureOptions>
#include <osgEarthDrivers/agglite/AGGLiteOptions>

#include <boost/log/trivial.hpp>

#include "Common.h"
#include "OSGWidget.h"
#include "Singleton.h"
#include "NodeCallback.h"
#include "NodeTreeInfo.h"
#include "NodeTreeSearch.h"
#include "MouseCoordsCallback.h"

#include "ItemInfos.h"

using namespace osgHelper;

using namespace osgEarth;
using namespace osgEarth::Drivers;
using namespace osgEarth::Features;

OSGWidget::OSGWidget(QWidget *parent, Qt::WindowFlags f)
        : QOpenGLWidget(parent, f),
          graphics_window_(new osgViewer::GraphicsWindowEmbedded(this->x(), this->y(), this->width(), this->height())) {
    // enable keypress event
    this->setFocusPolicy(Qt::StrongFocus);
}

void OSGWidget::init() {
    initSceneGraph();
//    initHelperNode();
    initCamera();

    startTimer(1000 / 60.f);  // 60hz
}

void OSGWidget::initSceneGraph() {
    root_node_ = new osg::Group;
    root_node_->setName(root_node_name);

    //earth
    std::string earth_file = "./earth/gisms.earth";
    osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(earth_file);
    map_node_ = osgEarth::MapNode::findMapNode(node.get());
    map_node_->setName(earth_node_name);

    //sky----the only entrance of the whole scene node tree!
    osg::ref_ptr<osgEarth::Util::SkyNode> sky_node = osgEarth::Util::SkyNode::create(map_node_.get());
    sky_node->setName("sky_node");
    sky_node->setDateTime(osgEarth::DateTime(2019, 2, 4, 16));
    osg::ref_ptr<osgEarth::Util::Ephemeris> ephemeris = new osgEarth::Util::Ephemeris;
    sky_node->setEphemeris(ephemeris.get());
    sky_node->attach(viewer_, 0);
    sky_node->setLighting(true);
    sky_node->getSunLight()->setAmbient(osg::Vec4(0.2, 0.2, 0.2, 0.0));
    sky_node->addChild(map_node_.get());
    root_node_->addChild(sky_node);

    const osgEarth::SpatialReference *wgs84 = osgEarth::SpatialReference::get("wgs84");
    const osgEarth::SpatialReference *utm15 = osgEarth::SpatialReference::get(
            "+proj=utm +zone=15 +ellps=GRS80 +units=m");

    osgEarth::GeoPoint wgsPoint(wgs84, -93.0, 34.0);
    osgEarth::GeoPoint utmPoint = wgsPoint.transform(utm15);

    if (utmPoint.isValid()) {
        std::cout << "utm:" << utmPoint.toString() << std::endl;
    }
    // do something

    osg::ref_ptr<osg::Switch> user_node = new osg::Switch;
    user_node->setName(user_node_name);
    sky_node->addChild(user_node);

    osg::ref_ptr<osg::Camera> hud_node = createHUD();
    hud_node->setName(hud_node_name);
    {
        osg::ref_ptr<osg::Geode> text_node = new osg::Geode;
        text_node->setName(text_node_name);

        text_geode_ = new osgText::Text;
        text_node->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
        text_node->addDrawable(text_geode_);
        text_geode_->setCharacterSize(30.0);
        text_geode_->setFont("fonts/arial.ttf");
        text_geode_->setColor(osg::Vec4(0, 1, 1, 1));
        text_geode_->setText("This is a test");
        text_geode_->setPosition(osg::Vec3d(10, 20, 0));

        hud_node->addChild(text_node);
    }
    root_node_->addChild(hud_node);

//    osg::ref_ptr<osg::Switch> helper_node = new osg::Switch;
//    helper_node->setName(helper_node_name);
//    root_node_->addChild(helper_node);
}

void OSGWidget::initCamera() {
    viewer_ = new osgViewer::Viewer;

    float aspectRatio = static_cast<float>(this->width()) / static_cast<float>(this->height());
    osg::ref_ptr<osg::Camera> camera = viewer_->getCamera();
    camera->setViewport(0, 0, this->width(), this->height());
    camera->setClearColor(osg::Vec4(0., 0., 0., 1.0));
    camera->setProjectionMatrixAsPerspective(30.f, aspectRatio, 1.f, 10000.f);
    camera->setGraphicsContext(graphics_window_);
    camera->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
    camera->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    camera->setClearMask(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    camera->setProjectionMatrixAsPerspective(30.f,
                                             static_cast<double>(this->width()) / static_cast<double>(this->height()),
                                             1.0, 1000.0);
    camera->setNearFarRatio(0.0000002);
    camera->setComputeNearFarMode(osg::CullSettings::COMPUTE_NEAR_FAR_USING_BOUNDING_VOLUMES);

    //for status
    viewer_->addEventHandler(new osgViewer::StatsHandler);
    viewer_->addEventHandler(new osgGA::StateSetManipulator(camera->getStateSet()));

    //for node tree
    viewer_->addEventHandler(new NodeTreeHandler(root_node_));

    //print coords to screen
    {
        osg::ref_ptr<osgEarth::Util::MouseCoordsTool> mouse = new osgEarth::Util::MouseCoordsTool(map_node_);
        mouse->addCallback(new MouseCoordsCallback(text_geode_));
        viewer_->addEventHandler(mouse);
    }

    //for z-fighting
    osgEarth::Util::LogarithmicDepthBuffer logdepth;
    logdepth.install(camera);

    //for outline effects
    {
        osg::DisplaySettings::instance()->setMinimumNumStencilBits(1);
        unsigned int clearMask = camera->getClearMask();
        camera->setClearMask(clearMask | GL_STENCIL_BUFFER_BIT);
        camera->setClearStencil(0);
    }

    earth_mani_ = new osgEarth::Util::EarthManipulator;
    earth_mani_->setHomeViewpoint(osgEarth::Viewpoint("", 114.676, 34.1278, -700, 0, -90, 1.823e+07));
//    earth_mani_->setHomeViewpoint(osgEarth::Viewpoint("", -71.076262, 42.34425, 0, 24.261, -21.6, 100));
    viewer_->setCameraManipulator(earth_mani_);

    viewer_->setThreadingModel(osgViewer::Viewer::SingleThreaded);
    viewer_->setSceneData(root_node_);
}

void OSGWidget::initHelperNode() {
    static osg::ref_ptr<osg::Switch> helper_node = dynamic_cast<osg::Switch *>(
            NodeTreeSearch::findNodeWithName(root_node_, helper_node_name));

    osg::ref_ptr<osg::Geode> geode = new osg::Geode;
    osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
    osg::ref_ptr<osg::Vec3Array> colors = new osg::Vec3Array;

    float radius = 10.0;
    osg::Vec3d axis_center;
    vertices->push_back(axis_center);
    vertices->push_back(axis_center + osg::Vec3(radius, 0, 0));
    vertices->push_back(axis_center + osg::Vec3(0, radius, 0));
    vertices->push_back(axis_center + osg::Vec3(0, 0, radius));

    colors->push_back(osg::Vec3(1, 0, 0));
    colors->push_back(osg::Vec3(0, 1, 0));
    colors->push_back(osg::Vec3(0, 0, 1));

    geom->setVertexArray(vertices);
    geom->setColorArray(colors);
    geom->setColorBinding(osg::Geometry::BIND_PER_PRIMITIVE_SET);

    osg::ref_ptr<osg::DrawElementsUInt> line1 = new osg::DrawElementsUInt(osg::DrawElementsUInt::LINES, 2);
    (*line1)[0] = 0;
    (*line1)[1] = 1;
    osg::ref_ptr<osg::DrawElementsUInt> line2 = new osg::DrawElementsUInt(osg::DrawElementsUInt::LINES, 2);
    (*line2)[0] = 0;
    (*line2)[1] = 2;
    osg::ref_ptr<osg::DrawElementsUInt> line3 = new osg::DrawElementsUInt(osg::DrawElementsUInt::LINES, 2);
    (*line3)[0] = 0;
    (*line3)[1] = 3;

    geom->addPrimitiveSet(line1);
    geom->addPrimitiveSet(line2);
    geom->addPrimitiveSet(line3);

    osg::ref_ptr<osg::StateSet> state_set = geode->getOrCreateStateSet();
    osg::ref_ptr<osg::LineWidth> line_width = new osg::LineWidth(3.0);
    state_set->setAttributeAndModes(line_width);
    geode->addDrawable(geom);

    helper_node->addChild(geode);
}

osg::Camera *OSGWidget::createHUD() {
    osg::ref_ptr<osg::Camera> camera = new osg::Camera;

    camera->setProjectionMatrix(osg::Matrix::ortho2D(0, 1280, 0, 1024));
    camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
    camera->setViewMatrix(osg::Matrix::identity());
    camera->setClearMask(GL_DEPTH_BUFFER_BIT);
    camera->setRenderOrder(osg::Camera::POST_RENDER);
    camera->setAllowEventFocus(false);

    return camera.release();
}

void OSGWidget::readDataFromFile(const QFileInfo &file_info) {
    static osg::ref_ptr<osg::Switch> point_cloud_node = dynamic_cast<osg::Switch *>(
            NodeTreeSearch::findNodeWithName(root_node_, point_cloud_node_name));

    osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(file_info.filePath().toStdString());
    osg::ref_ptr<osg::Geode> bbox = calculateBBoxForModel(node);

    osg::ref_ptr<osg::MatrixTransform> matrix = new osg::MatrixTransform;
    matrix->addChild(node);
    matrix->addChild(bbox);
    matrix->setUpdateCallback(new NodeCallback());

    point_cloud_node->removeChildren(0, point_cloud_node->getNumChildren());
    point_cloud_node->addChild(matrix);
}

osg::Geode *OSGWidget::calculateBBoxForModel(osg::Node *node) const {
    osg::ref_ptr<osg::Geode> geode = new osg::Geode;

    osg::ComputeBoundsVisitor bounds_visitor;
    node->accept(bounds_visitor);
    osg::BoundingBox bb = bounds_visitor.getBoundingBox();
    float lengthX = bb.xMax() - bb.xMin();
    float lengthY = bb.yMax() - bb.yMin();
    float lengthZ = bb.zMax() - bb.zMin();
    osg::Vec3 center = bb.center();
    std::cout << "model center: " << center << std::endl;

    osg::ref_ptr<osg::ShapeDrawable> drawable = new osg::ShapeDrawable(new osg::Box(center, lengthX, lengthY, lengthZ));
    drawable->setColor(osg::Vec4(1.0, 1.0, 0.0, 1.0));

    osg::ref_ptr<osg::StateSet> state_set = drawable->getOrCreateStateSet();
    osg::ref_ptr<osg::PolygonMode> polygon = new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK,
                                                                  osg::PolygonMode::LINE);
    state_set->setAttributeAndModes(polygon);
    osg::ref_ptr<osg::LineWidth> line_width = new osg::LineWidth(3.0);
    state_set->setAttribute(line_width);
    geode->addDrawable(drawable);

    return geode.release();
}

osgGA::EventQueue *OSGWidget::getEventQueue() const {
    osgGA::EventQueue *eventQueue = graphics_window_->getEventQueue();

    if (eventQueue) {
        return eventQueue;
    } else {
        throw std::runtime_error("Unable to obtain valid event queue");
    }
}

void OSGWidget::paintEvent(QPaintEvent * /* paintEvent */) {
    this->makeCurrent();

//    QPainter painter(this);
//    painter.setRenderHint(QPainter::Antialiasing);

    this->paintGL();

//    painter.end();

    this->doneCurrent();
}

void OSGWidget::paintGL() {
    viewer_->frame();
}

void OSGWidget::onResize(int width, int height) {
    viewer_->getCamera()->setViewport(0, 0, this->width(), this->height());
}

void OSGWidget::resizeGL(int width, int height) {
    this->getEventQueue()->windowResize(this->x(), this->y(), width, height);
    graphics_window_->resized(this->x(), this->y(), width, height);

    this->onResize(width, height);
}

void OSGWidget::keyPressEvent(QKeyEvent *event) {
    char key_c = event->text().toStdString()[0];
//    LOG_INFO << "OSGWidget get key: " << key_c;

    this->getEventQueue()->keyPress(osgGA::GUIEventAdapter::KeySymbol(key_c));
}

void OSGWidget::mouseMoveEvent(QMouseEvent *event) {
    this->getEventQueue()->mouseMotion(static_cast<float>(event->x()), static_cast<float>(event->y()));
}

void OSGWidget::mousePressEvent(QMouseEvent *event) {
    // 1 = left mouse button
    // 2 = middle mouse button
    // 3 = right mouse button

    unsigned int button = 0;

    switch (event->button()) {
        case Qt::LeftButton:
            button = 1;
            break;

        case Qt::MiddleButton:
            button = 2;
            break;

        case Qt::RightButton:
            button = 3;
            break;

        default:
            break;
    }

    Qt::KeyboardModifiers mod = event->modifiers();
    quint16 modkeyosg = 0;
    if (mod & Qt::ShiftModifier)
        modkeyosg |= osgGA::GUIEventAdapter::MODKEY_SHIFT;
    if (mod & Qt::ControlModifier)
        modkeyosg |= osgGA::GUIEventAdapter::MODKEY_CTRL;
    if (mod & Qt::AltModifier)
        modkeyosg |= osgGA::GUIEventAdapter::MODKEY_ALT;
    if (mod & Qt::MetaModifier)
        modkeyosg |= osgGA::GUIEventAdapter::MODKEY_META;

    osgGA::GUIEventAdapter *adapter = this->getEventQueue()->mouseButtonPress(static_cast<float>(event->x()),
                                                                              static_cast<float>(event->y()),
                                                                              button);
    adapter->setModKeyMask(modkeyosg);
}

void OSGWidget::mouseReleaseEvent(QMouseEvent *event) {
    // 1 = left mouse button
    // 2 = middle mouse button
    // 3 = right mouse button

    unsigned int button = 0;

    switch (event->button()) {
        case Qt::LeftButton:
            button = 1;
            break;

        case Qt::MiddleButton:
            button = 2;
            break;

        case Qt::RightButton:
            button = 3;
            break;

        default:
            break;
    }
    Qt::KeyboardModifiers mod = event->modifiers();
    quint16 modkeyosg = 0;
    if (mod & Qt::ShiftModifier)
        modkeyosg |= osgGA::GUIEventAdapter::MODKEY_SHIFT;
    if (mod & Qt::ControlModifier)
        modkeyosg |= osgGA::GUIEventAdapter::MODKEY_CTRL;
    if (mod & Qt::AltModifier)
        modkeyosg |= osgGA::GUIEventAdapter::MODKEY_ALT;
    if (mod & Qt::MetaModifier)
        modkeyosg |= osgGA::GUIEventAdapter::MODKEY_META;

    osgGA::GUIEventAdapter *adapter = this->getEventQueue()->mouseButtonRelease(static_cast<float>(event->x()),
                                                                                static_cast<float>(event->y()),
                                                                                button);
    adapter->setModKeyMask(modkeyosg);
}

void OSGWidget::wheelEvent(QWheelEvent *event) {
    event->accept();
    int delta = event->delta();

    osgGA::GUIEventAdapter::ScrollingMotion motion = delta > 0 ? osgGA::GUIEventAdapter::SCROLL_UP
                                                               : osgGA::GUIEventAdapter::SCROLL_DOWN;
    Qt::KeyboardModifiers mod = event->modifiers();
    quint16 modkeyosg = 0;
    if (mod & Qt::ShiftModifier)
        modkeyosg |= osgGA::GUIEventAdapter::MODKEY_SHIFT;
    if (mod & Qt::ControlModifier)
        modkeyosg |= osgGA::GUIEventAdapter::MODKEY_CTRL;
    if (mod & Qt::AltModifier)
        modkeyosg |= osgGA::GUIEventAdapter::MODKEY_ALT;
    if (mod & Qt::MetaModifier)
        modkeyosg |= osgGA::GUIEventAdapter::MODKEY_META;

    osgGA::GUIEventAdapter *adapter = this->getEventQueue()->mouseScroll(motion);
    adapter->setModKeyMask(modkeyosg);
}

void OSGWidget::mouseDoubleClickEvent(QMouseEvent *event) {
    // 1 = left mouse button
    // 2 = middle mouse button
    // 3 = right mouse button

    unsigned int button = 0;

    switch (event->button()) {
        case Qt::LeftButton:
            button = 1;
            break;

        case Qt::MiddleButton:
            button = 2;
            break;

        case Qt::RightButton:
            button = 3;
            break;

        default:
            break;
    }
    Qt::KeyboardModifiers mod = event->modifiers();
    quint16 modkeyosg = 0;
    if (mod & Qt::ShiftModifier)
        modkeyosg |= osgGA::GUIEventAdapter::MODKEY_SHIFT;
    if (mod & Qt::ControlModifier)
        modkeyosg |= osgGA::GUIEventAdapter::MODKEY_CTRL;
    if (mod & Qt::AltModifier)
        modkeyosg |= osgGA::GUIEventAdapter::MODKEY_ALT;
    if (mod & Qt::MetaModifier)
        modkeyosg |= osgGA::GUIEventAdapter::MODKEY_META;

    osgGA::GUIEventAdapter *adapter = this->getEventQueue()->mouseDoubleButtonPress(static_cast<float>(event->x()),
                                                                                    static_cast<float>(event->y()),
                                                                                    button);
    adapter->setModKeyMask(modkeyosg);
}

bool OSGWidget::event(QEvent *event) {
    bool handled = QOpenGLWidget::event(event);
    // This ensures that the OSG widget is always going to be repainted after the
    // user performed some interaction. Doing this in the event handler ensures
    // that we don't forget about some event and prevents duplicate code.
    switch (event->type()) {
        case QEvent::KeyPress:
        case QEvent::KeyRelease:
        case QEvent::MouseButtonDblClick:
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseMove:
        case QEvent::Wheel:
            //        this->update();
            break;
        default:
            break;
    }

    return handled;
}

void OSGWidget::timerEvent(QTimerEvent *) {
    update();
}

void OSGWidget::home() {
    viewer_->home();
}

void OSGWidget::trackballCenterOn(double x, double y, double z) {
//    _trackballMani->setCenter(osg::Vec3d(x, y, z));
//    _trackballMani->setDistance(700);
}

void OSGWidget::flyToViewPoint(const osgEarth::Viewpoint &viewpoint) {
    osgEarth::Viewpoint currentVP = earth_mani_->getViewpoint();
    double distance = currentVP.focalPoint()->distanceTo(currentVP.focalPoint().get());
    double duration = osg::clampBetween(distance / 2500.0, 2.0, 8.0); // fly speed, minimum fly time, maximum fly time.

    earth_mani_->setViewpoint(viewpoint, duration);
}

void OSGWidget::loadModelToScene(const ItemInfos &infos) {

    static osg::ref_ptr<osg::Switch> user_node = dynamic_cast<osg::Switch *>(
            NodeTreeSearch::findNodeWithName(root_node_, user_node_name));

    std::string node_name = infos.name.toStdString();
    //locator
    osg::ref_ptr<GeoTransform> locator = new GeoTransform();
    locator->setName(node_name);
    locator->setTerrain(map_node_->getTerrain());
    locator->setAutoRecomputeHeights(true);
    locator->setPosition(infos.localtion.focalPoint().value());

    BOOST_LOG_TRIVIAL(trace) << "loadModelToScene: " << node_name << " at: "
                             << infos.localtion.focalPoint()->toString();
    for (const QString &file_path : infos.file_path) {
        BOOST_LOG_TRIVIAL(trace) << "loading: " << file_path.toStdString();

        osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(file_path.toStdString());
        locator->addChild(node);
    }

    // for texture
    osgEarth::Registry::shaderGenerator().run(locator);
    user_node->addChild(locator);

    flyToViewPoint(infos.localtion);
}

void OSGWidget::removeModelFromScene(const ItemInfos &infos) {
    static osg::ref_ptr<osg::Switch> user_node = dynamic_cast<osg::Switch *>(
            NodeTreeSearch::findNodeWithName(root_node_, user_node_name));

    std::string node_name = infos.name.toStdString();
    osg::ref_ptr<GeoTransform> locator = dynamic_cast<GeoTransform *>(
            NodeTreeSearch::findNodeWithName(user_node, node_name.data()));

    user_node->removeChild(locator);
    BOOST_LOG_TRIVIAL(trace) << "removeModelFromScene: " << infos;
}
