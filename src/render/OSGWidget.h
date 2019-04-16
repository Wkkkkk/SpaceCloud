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

#ifndef PROTYPE_LDOSGWIDGET_H
#define PROTYPE_LDOSGWIDGET_H

//Osg
#include <osgViewer/GraphicsWindow>
#include <osgViewer/Viewer>
#include <osgGA/TerrainManipulator>
#include <osgGA/TrackballManipulator>
//Qt
#include <QtCore/QFileInfo>
#include <QtWidgets/QOpenGLWidget>
//osgEarth
#include <osgEarth/MapNode>
#include <osgEarth/Viewpoint>
#include <osgEarth/Layer>
#include <osgEarthUtil/EarthManipulator>
#include <osgText/Text>

//forward declaration
class ItemInfos;

/**
 * @brief The OSGWidget class is the bridge between OSG and Qt.
 * It renders the whole osg scene
 * and communicate with the main window(user input)
 * through signal&slot
 */
class OSGWidget : public QOpenGLWidget {
Q_OBJECT
public:
    /**
     * @brief Default constructor
     * @param parent parent window pointer
     * @param f windows flags
     */
    explicit OSGWidget(QWidget *parent = nullptr, Qt::WindowFlags f = 0);
    ~OSGWidget() final = default;

    Q_DISABLE_COPY(OSGWidget);

    /**
     * @brief init the scene, camera, etc...
     **/
    void init();

    /**
     * @brief load models into scene
     * @param ItemInfos: name, type, file_path
     **/
    void readDataFromFile(const QFileInfo &file_path);

    /**
     * @brief change current viewpoint to target
     **/
    void flyToViewPoint(const osgEarth::Viewpoint &viewpoint);

    void loadModelToScene(const ItemInfos &infos);

    void loadLayerToScene(const ItemInfos &infos);

    void removeModelFromScene(const ItemInfos &infos);

    //!inherit from QOpenGLWidget
    virtual void keyPressEvent(QKeyEvent *event);
protected:
    //!inherit from QOpenGLWidget
    virtual void paintEvent(QPaintEvent *paintEvent);

    //!inherit from QOpenGLWidget
    virtual void paintGL();

    //!inherit from QOpenGLWidget
    virtual void onResize(int width, int height);

    //!inherit from QOpenGLWidget
    virtual void resizeGL(int width, int height);

    //!inherit from QOpenGLWidget
    virtual void mouseMoveEvent(QMouseEvent *event);

    //!inherit from QOpenGLWidget
    virtual void mousePressEvent(QMouseEvent *event);

    //!inherit from QOpenGLWidget
    virtual void mouseReleaseEvent(QMouseEvent *event);

    //!inherit from QOpenGLWidget
    virtual void wheelEvent(QWheelEvent *event);

    //!inherit from QOpenGLWidget
    void mouseDoubleClickEvent(QMouseEvent *event);

    //!inherit from QOpenGLWidget
    virtual bool event(QEvent *event);

    //!inherit from QOpenGLWidget
    virtual void timerEvent(QTimerEvent *);
private:
    //! init all node
    void initSceneGraph();

    //! init the camera
    void initCamera();

    //! init the axis
    void initHelperNode();

    //! some fun
    void initQueryHandler();

    //! create hud node
    osg::Camera *createHUD();

    /**
     * @brief calculate the bounding box of a node
     **/
    osg::Geode *calculateBBoxForModel(osg::Node *node) const;

    //! Getter of osg event queue
    osgGA::EventQueue *getEventQueue() const;

    //! Ref of OSG Graphics Window
    osg::ref_ptr<osgViewer::GraphicsWindowEmbedded> graphics_window_;
    //! Ref of OSG Viewer
    osg::ref_ptr<osgViewer::Viewer> viewer_;
    //! root node of the scene
    osg::ref_ptr<osg::Group> root_node_;
    osg::ref_ptr<osgText::Text> text_geode_;

    osg::ref_ptr<osgEarth::MapNode> map_node_;

    //! some manipulators
    osg::ref_ptr<osgEarth::Util::EarthManipulator> earth_mani_;
public slots:

    //! Zoom Viewer to whole display
    void home();

    //! Center On, set to terrianManipulator if 0,0,0 input
    void trackballCenterOn(double x, double y, double z);
};


#endif //PROTYPE_LDOSGWIDGET_H
