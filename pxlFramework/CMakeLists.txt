cmake_minimum_required(VERSION 3.23)
project(pxlFramework)

add_library(pxlFramework STATIC pxl.h src/Application.h src/Application.cpp)