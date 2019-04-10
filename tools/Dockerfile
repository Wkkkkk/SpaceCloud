FROM ubuntu:16.04 as builder
LABEL description="ubuntu"

#some fixed environment variables
ENV CMAKE_PREFIX_PATH=/usr/local/lib64

#basic dependencies.
#compile dependencies
#software-properties-common for add-apt-repository
#ca-certificates for verification
RUN apt-get update && apt-get install -y \
    software-properties-common \
    ca-certificates \
    build-essential \
    mesa-utils \
    glmark2 \
    cmake \
    sudo \
    vim \
    git \
    tar \
    unzip \
    wget \
    curl

#gcc8
RUN add-apt-repository -y ppa:jonathonf/gcc && \
    apt-get update && \
    apt-get install -y gcc-8 g++-8 && \
    rm -rf /usr/bin/gcc /usr/bin/g++ && \
    ln -s /usr/bin/g++-8 /usr/bin/g++ && \
    ln -s /usr/bin/gcc-8 /usr/bin/gcc

#boost pcl protobuf qt5.5
RUN apt-get update && \
    apt-get install -y \
    libboost-dev \
    libpcl-dev \
    libproj-dev \
    libprotobuf-dev \
    protobuf-compiler \
    qt5-default \
    qttools5-dev-tools \
    libqt5opengl5-dev
#fix pcl -lvtkproj4 bugs
RUN ln -s /usr/lib/x86_64-linux-gnu/libvtkCommonCore-6.2.so /usr/lib/libvtkproj4.so

#3rd party
WORKDIR /home/zhihui/library

#osg 3.7
RUN git clone https://github.com/openscenegraph/OpenSceneGraph.git && \
    cd OpenSceneGraph && mkdir build && cd build \
    && cmake .. && make -j6 && make install

#osgearth 2.10
RUN git clone https://github.com/gwaldron/osgearth.git && \
    cd osgearth && mkdir build && cd build \
    && cmake -DOSGEARTH_USE_QT=ON .. && make -j6 && make install

#draco 1.3.5
RUN git clone https://github.com/google/draco.git && \
    cd draco && mkdir build && cd build \
    && cmake .. && make -j6 && make install

#muduo
RUN git clone https://github.com/chenshuo/muduo.git && \
    cd muduo && mkdir build && cd build \
    && cmake .. && make -j6 && make install

#self
WORKDIR /home/zhihui/workspace
RUN git clone https://github.com/Wkkkkk/SLAMMapping.git && \
    cd SLAMMapping && mkdir build && cd build \
    && cmake .. && make -j6

##################################################
#pack here
ENV des=/home/zhihui/workspace/SLAMMapping/bin
ENV client_exe=$des/client
ENV server_exe=$des/server
RUN deplist=$(ldd $client_exe | awk  '{if (match($3,"/")){ printf("%s "),$3 } }') && \
    cp $deplist $des
RUN deplist=$(ldd $server_exe | awk  '{if (match($3,"/")){ printf("%s "),$3 } }') && \
    cp $deplist $des

RUN tar -czvf /home/zhihui/workspace/all.tar.gz $des
RUN cd /usr/local/lib64 && tar -czvf /home/zhihui/workspace/osgPlugins.tar.gz ./osgPlugins*/*

###################################################
FROM nvidia/opengl:1.0-glvnd-runtime-ubuntu16.04 as runtime
LABEL description="Run container"

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    qt5-default \
    qttools5-dev-tools \
    libqt5opengl5-dev

ENV bin_path=/home/zhihui/workspace/SLAMMapping/bin/
RUN mkdir -p $bin_path
COPY --from=builder /home/zhihui/workspace/*.tar.gz /home/zhihui/workspace/

RUN tar -xzvf /home/zhihui/workspace/all.tar.gz
RUN tar -xzvf /home/zhihui/workspace/osgPlugins.tar.gz -C $bin_path

#COPY --from=builder /usr/lib/x86_64-linux-gnu/libQt5DBus.so* /home/Demo/bin/
#some fixed environment variables
ENV LD_LIBRARY_PATH=/usr/local/lib64:$bin_path

CMD $bin_path/client
EXPOSE 2000


