FROM ros:foxy

ENV DISPLAY=host.docker.internal:0.0
ENV TZ=Europe/Moscow

RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

RUN apt-get update && apt-get install -y \  
ros-foxy-image-transport libboost-system-dev libboost-thread-dev libboost-program-options-dev libboost-test-dev build-essential wget cmake git ros-foxy-sensor-msgs ros-foxy-cv-bridge ros-foxy-camera-calibration-parsers ros-foxy-camera-info-manager ros-foxy-stereo-image-proc unzip checkinstall x11-apps python-dev python-numpy pkg-config libtool uuid-dev  libjpeg-dev libpng-dev libtiff-dev libtool pkg-config build-essential autoconf automake  libavcodec-dev libavformat-dev libswscale-dev  libgtk2.0-dev libcanberra-gtk*  python3-dev python3-numpy python3-pip  libxvidcore-dev libx264-dev libgtk-3-dev  libtbb2 libtbb-dev  gstreamer1.0-tools libv4l-dev v4l-utils  libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev  libvorbis-dev libxine2-dev  libfaac-dev libmp3lame-dev libtheora-dev libzmq3-dev  libopencore-amrnb-dev libopencore-amrwb-dev  libopenblas-dev libatlas-base-dev libblas-dev  liblapack-dev libeigen3-dev gfortran  libhdf5-dev protobuf-compiler  libprotobuf-dev libgoogle-glog-dev libgflags-dev* \    
&& rm -rf /var/lib/apt/lists/*

RUN wget https://archive.org/download/zeromq_4.0.5/zeromq-4.0.5.tar.gz

RUN tar -xvf zeromq-4.0.5.tar.gz

WORKDIR zeromq-4.0.5

RUN ./configure

RUN make install

WORKDIR ..

RUN wget https://github.com/opencv/opencv/archive/refs/tags/3.4.8.zip

RUN unzip 3.4.8.zip 

WORKDIR opencv-3.4.8

RUN mkdir build

WORKDIR build

RUN cmake -D CMAKE_BUILD_TYPE=RELEASE     -D CMAKE_INSTALL_PREFIX=/usr/local     -D INSTALL_PYTHON_EXAMPLES=OFF     -D WITH_TBB=ON     -D BUILD_TESTS=OFF   -D WITH_V4AL=OFF   -D BUILD_PERF_TESTS=OFF  -D WITH_LIBV4L=ON   -D ENABLE_PRECOMPILED_HEADERS=OFF     -D BUILD_EXAMPLES=OFF ..

RUN make -j4

RUN make install

RUN checkinstall --pkgname opencv --pkgversion 3.4.8 --install

WORKDIR /

COPY ./libtconfig /libtconfig

WORKDIR libtconfig

RUN mkdir build

WORKDIR build

RUN cmake ..
RUN make
RUN make install
RUN checkinstall --pkgname libtconfig --pkgversion 1.0 --install

WORKDIR /

COPY ./libtlive /libtlive

WORKDIR libtlive

RUN mkdir build

WORKDIR build

RUN cmake ..
RUN make
RUN make install
RUN checkinstall --pkgname libtlive --pkgversion 1.0 --install

WORKDIR /

COPY calib_ws depth_ws depth_view_ws stereo_ws ./packages/

WORKDIR packages

COPY ./setup.sh /setup.sh
RUN /bin/bash -c "chmod +x /setup.sh && /setup.sh"
