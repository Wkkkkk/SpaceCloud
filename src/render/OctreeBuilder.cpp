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

#include <QtCore/QDir>

#include <pcl/point_cloud.h>
#include <pcl/io/pcd_io.h>
#include <pcl/octree/octree_search.h>
#include <pcl/octree/octree_iterator.h>
#include <pcl/octree/octree_base.h>

#include <osg/Vec3d>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/PagedLOD>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/StateSetManipulator>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgDB/FileNameUtils>

#include <boost/log/trivial.hpp>

#include "OctreeBuilder.h"

using namespace pcl;
using namespace octree;

class WriteOutPagedLODSubgraphsVistor : public osg::NodeVisitor {
public:
    explicit WriteOutPagedLODSubgraphsVistor(std::string output_dir) :
            osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN),
            output_dir_(std::move(output_dir)) {}

    virtual void apply(osg::Switch &node) {
        std::string file_name = node.getName();

        std::string file_path = output_dir_ + file_name;
        osg::notify(osg::NOTICE) << "Writing out " << node.className() << " to: " << file_path << std::endl;
        osgDB::writeNodeFile(node, file_path);

        traverse(node);
    }

    virtual void apply(osg::PagedLOD &plod) {
        // go through all the named children and write them out to disk.
        for (unsigned int i = 0; i < plod.getNumChildren(); ++i) {
            osg::Node *child = plod.getChild(i);
            std::string file_name = plod.getFileName(i);
            if (!file_name.empty()) {
                std::string file_path = output_dir_ + file_name;
                osg::notify(osg::NOTICE) << "Writing out " << child->className() << " to: " << file_path << std::endl;
                osgDB::writeNodeFile(*child, file_path);
            }
        }

        traverse(plod);
    }

private:
    std::string output_dir_;
};

class ConvertToPageLODVistor : public osg::NodeVisitor {
public:
    ConvertToPageLODVistor(std::string basename, std::string extension, bool makeAllChildrenPaged) :
            osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN),
            _basename(std::move(basename)),
            _extension(std::move(extension)),
            _makeAllChildrenPaged(makeAllChildrenPaged) {
    }

    ~ConvertToPageLODVistor() final = default;

    virtual void apply(osg::LOD &lod) {
        _lodSet.insert(&lod);

        traverse(lod);
    }

    virtual void apply(osg::PagedLOD &plod) {
        // do thing, but want to avoid call LOD.
        traverse(plod);
    }

    void convert() {
        unsigned int lodNum = 0;
        for (auto itr = _lodSet.begin(); itr != _lodSet.end(); ++itr, ++lodNum) {
            osg::ref_ptr<osg::LOD> lod = const_cast<osg::LOD *>(itr->get());

            if (lod->getNumParents() == 0) {
                osg::notify(osg::NOTICE) << "Warning can't operator on root node." << std::endl;
                break;
            }

            if (!_makeAllChildrenPaged && lod->getNumRanges() < 2) {
                osg::notify(osg::NOTICE) << "Leaving LOD with one child as is." << std::endl;
                break;
            }

            osg::ref_ptr<osg::PagedLOD> plod = new osg::PagedLOD;

            const osg::LOD::RangeList &originalRangeList = lod->getRangeList();
            typedef std::multimap<osg::LOD::MinMaxPair, unsigned int> MinMaxPairMap;
            MinMaxPairMap rangeMap;
            unsigned int pos = 0;
            for (auto ritr = originalRangeList.begin(); ritr != originalRangeList.end(); ++ritr, ++pos) {
                rangeMap.insert(std::multimap<osg::LOD::MinMaxPair, unsigned int>::value_type(*ritr, pos));
            }

            pos = 0;
            for (auto mitr = rangeMap.rbegin(); mitr != rangeMap.rend(); ++mitr, ++pos) {
                if (pos == 0 && !_makeAllChildrenPaged) {
                    plod->addChild(lod->getChild(mitr->second), mitr->first.first, mitr->first.second);
                } else {
                    std::ostringstream os;
                    os << _basename << "_" << lodNum << "_" << pos << _extension;
                    std::string filename = os.str();

                    plod->addChild(lod->getChild(mitr->second), mitr->first.first, mitr->first.second, os.str());
                }
            }

            osg::Node::ParentList parents = lod->getParents();
            for (auto parent : parents) {
                parent->replaceChild(lod.get(), plod);
            }

            plod->setRadius(lod->getRadius());
            plod->setCenter(lod->getCenter());
        }
    }

    typedef std::set<osg::ref_ptr<osg::LOD>> LODSet;
    LODSet _lodSet;
    std::string _basename;
    std::string _extension;
    bool _makeAllChildrenPaged;
};


osg::Geometry *createGeomFrom(const pcl::PointCloud<pcl::PointXYZ>::Ptr &ptr) {

    osg::ref_ptr<osg::Geometry> pointsGeom = new osg::Geometry();

    osg::ref_ptr<osg::Vec3dArray> vertices = new osg::Vec3dArray;
    for (const auto &point : ptr->points) {
        vertices->push_back(osg::Vec3(point.x, point.y, point.z));
    }
    pointsGeom->setVertexArray(vertices);

    osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
    colors->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
    pointsGeom->setColorArray(colors, osg::Array::BIND_OVERALL);

    // You should disable node's light when render point clouds
    osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
    normals->push_back(osg::Vec3(0.0f, -1.0f, 0.0f));
    pointsGeom->setNormalArray(normals, osg::Array::BIND_OVERALL);

    pointsGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS, 0, vertices->size()));

    return pointsGeom.release();
}

osg::Switch *OctreeBuilder::getAllLeafPoints() {
//    srand ((unsigned int) time (NULL));
    std::string file_path = pcd_file_path_.filePath().toStdString();
    BOOST_LOG_TRIVIAL(trace) << "pcd file reading... " << file_path << std::endl;

    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>);
    pcl::PCDReader reader;
    reader.read(file_path, *cloud);
    emit progress_value(10);

    float resolution = resolution_;
    BOOST_LOG_TRIVIAL(trace) << "init octree with resolution: " << resolution << std::endl;
    pcl::octree::OctreePointCloudSearch<pcl::PointXYZ> octree(resolution);

    octree.setInputCloud(cloud);
    octree.addPointsFromInputCloud();
    BOOST_LOG_TRIVIAL(trace) << "octree building..." << std::endl;
    emit progress_value(20);

    osg::Vec3d min_p, max_p;
    octree.getBoundingBox(min_p.x(), min_p.y(), min_p.z(), max_p.x(), max_p.y(), max_p.z());
    // TODO: color map by height

    unsigned int leafNodeCounter = 0;
    auto leafCounter = octree.getLeafCount();
    osg::ref_ptr<osg::Switch> root_node = new osg::Switch;

    for (auto it1 = octree.leaf_depth_begin(), it1_end = octree.leaf_depth_end();
         it1 != it1_end; ++it1, leafNodeCounter++) {
        BOOST_LOG_TRIVIAL(trace) << "leat building... " << leafNodeCounter << " / " << leafCounter << std::endl;
        int process = leafNodeCounter * 40 / leafCounter;
        OctreeContainerPointIndices &container = it1.getLeafContainer();

        std::vector<int> tmpVector;
        container.getPointIndices(tmpVector);

        pcl::PointCloud<pcl::PointXYZ>::Ptr temp_cloud(new pcl::PointCloud<pcl::PointXYZ>);
        pcl::copyPointCloud<pcl::PointXYZ>(*cloud, tmpVector, *temp_cloud);

        osg::ref_ptr<osg::LOD> lod = new osg::LOD;

        Eigen::Vector3f min_pt, max_pt;
        octree.getVoxelBounds(it1, min_pt, max_pt);

        osg::Vec3d max_point(max_pt.x(), max_pt.y(), max_pt.z());
        osg::Vec3d min_point(min_pt.x(), min_pt.y(), min_pt.z());
        osg::Vec3d node_center = (max_point + min_point) / 2;
        double node_radius = ((max_point - min_point) / 2).length();
        double lod_range = std::max(std::min(node_radius * 20, DBL_MAX), 500.0);

        lod->setCenterMode(osg::LOD::USER_DEFINED_CENTER);
        lod->setRangeMode(osg::LOD::DISTANCE_FROM_EYE_POINT);
        lod->setCenter(node_center);
        lod->setRadius(node_radius);
        lod->setRange(0, 0, lod_range);
        osg::ref_ptr<osg::Geometry> geom = createGeomFrom(temp_cloud);
        lod->addChild(geom);

        root_node->addChild(lod);
        emit progress_value(process + 20);
    }
    emit progress_value(60);

    return root_node.release();
}


OctreeBuilder::OctreeBuilder() = default;

void OctreeBuilder::build() {
    emit progress_value(0);

    // 0~60
    osg::ref_ptr<osg::Switch> model = getAllLeafPoints();

    if (model.valid()) {
        // 60~80
        std::string basename = output_file_path_.fileName().toStdString();
        std::string ext = output_file_path_.suffix().toStdString();
        ConvertToPageLODVistor converter(basename, ext, true);
        model->accept(converter);

        BOOST_LOG_TRIVIAL(trace) << "convert to pagelod..." << basename << " / " << ext << std::endl;
        converter.convert();
        emit progress_value(80);

        // 80~100
        std::string file_dir = output_file_path_.dir().path().toStdString();

        BOOST_LOG_TRIVIAL(trace) << "write to dir: " << file_dir << std::endl;
        WriteOutPagedLODSubgraphsVistor woplsv(file_dir);
        model->accept(woplsv);
    }
    emit progress_value(100);
}
