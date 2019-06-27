---
typora-root-url: ./
---

# Space Cloud(prototype)

#### Description
地理信息数据展示平台，支持点云数据、倾斜模型数据、遥感影像数据、矢量Shp数据等类型数据的加载与显示。

A geographic information data display platform,  which supports loading and display of various data such as point cloud, tilt model, tiff image, and shape file.

#### Software Architecture
软件由C++编写，利用Qt搭建框架，使用三维渲染引擎OSG做核心开发。

The software is written in C++, using Qt to build the framework, using [OpenSceneGraph](http://www.openscenegraph.org/) for 3D graphics.

#### Display

- shape file

  ![demo](/docs/demo.png)

- 3D models of oblique photogrammetry

  ![demo](/docs/demo2.png)

## Tools

- Octree building tool for point cloud data

  ![demo](/docs/octree.png)

## Product Features Display

- Welcome

  ![init](.\docs\init.png)

- 3D models

  ![SpaceCloud2](.\docs\SpaceCloud2.png)

- buildings

  ![SpaceCloud](.\docs\SpaceCloud.png)

- attribute editing

  ![building](.\docs\building.png)