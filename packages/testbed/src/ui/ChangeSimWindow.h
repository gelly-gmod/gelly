#ifndef CHANGESIMWINDOW_H
#define CHANGESIMWINDOW_H

#include "Gelly.h"
#include "UIData.h"

START_UI_DATA_GROUP(ChangeSim)
CREATE_UI_DATA(testbed::GellySimMode, simMode);
CREATE_UI_DATA(int, debugMaxParticles);
CREATE_UI_DATA(int, flexMaxParticles);
CREATE_UI_DATA(std::string, rtfrFolderPath);
CREATE_UI_DATA(byte, popupVisible);
CREATE_UI_DATA(byte, lastPopupVisible);
CREATE_UI_DATA(bool, submitted);
testbed::GellySimInit GetGellySimInit();
END_UI_DATA_GROUP();

DEFINE_WINDOW(ChangeSim);

#endif	// CHANGESIMWINDOW_H
