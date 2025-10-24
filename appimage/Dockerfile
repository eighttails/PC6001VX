FROM ubuntu:25.04
ENV DEBIAN_FRONTEND=noninteractive

ARG USERNAME=app_build
ARG GROUPNAME=app_build
ARG UID=1000
ARG GID=1000
RUN userdel ubuntu 2> /dev/null

RUN apt update
RUN apt install -y \
    file \
    wget \
    fuse \
    appstream \
    build-essential \
    libx11-dev \
    mesa-common-dev \
    libopengl-dev \
    libsdl2-dev \
    qtcreator \
    qt6-base-dev \
    qt6-multimedia-dev \
    libavformat-dev \
    libavcodec-dev \
    libswscale-dev

RUN groupadd -g $GID $GROUPNAME
RUN useradd -m -s /bin/bash -u $UID -g $GID $USERNAME

USER $USERNAME
WORKDIR /home/$USERNAME/
RUN mkdir bin
ENV PATH /home/$USERNAME/bin:$PATH
RUN wget -c https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage -O /home/$USERNAME/bin/linuxdeployqt
RUN chmod a+x /home/$USERNAME/bin/linuxdeployqt

RUN wget -c https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage -O /home/$USERNAME/bin/linuxdeploy
RUN chmod a+x /home/$USERNAME/bin/linuxdeploy

RUN wget -c https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage -O /home/$USERNAME/bin/linuxdeploy-plugin-qt-x86_64.AppImage
RUN chmod a+x /home/$USERNAME/bin/linuxdeploy-plugin-qt-x86_64.AppImage

