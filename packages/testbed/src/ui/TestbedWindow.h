#ifndef TESTBEDWINDOW_H
#define TESTBEDWINDOW_H

#include "GellyDataTypes.h"
#include "UIData.h"

using namespace Gelly::DataTypes;

constexpr uint RASTERIZER_FLAG_WIREFRAME = 0b01;
constexpr uint RASTERIZER_FLAG_NOCULL = 0b10;

START_UI_DATA_GROUP(TestbedWindow);
CREATE_UI_DATA(int, filterIterations);
CREATE_UI_DATA(float, thresholdRatio);
CREATE_UI_DATA(float, particleRadius);
CREATE_UI_DATA(uint, rasterizerFlags);
CREATE_UI_DATA(uint, lastRasterizerFlags);
END_UI_DATA_GROUP();

DEFINE_WINDOW(TestbedWindow);

#endif	// TESTBEDWINDOW_H
