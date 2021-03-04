#!/bin/bash

# Where you installed
install_home=/home/covscript/online-ide

# Git Repo
git_repo=https://hub.fastgit.org/covscript

# HTML Page
http_path=/var/www/html

# CGI Script
cgi_path=/var/www/cgi

# Preparing
cd $install_home
git fetch
git pull
git clean -dfx

# Build Sandbox Environment
cd csbuild
bash auto-build.sh
dpkg -i ./deb-src/*.deb
cd ..

# Build CGI Program
mkdir build
cd build
cmake ..
make -j4
cp cs-code-runner /usr/bin/
cp cs-online-cgi /usr/bin/
cd ..

# Install HTML Page
cp html/* $http_path

# Install CGI Script
cp cgi-bin/* $cgi_path