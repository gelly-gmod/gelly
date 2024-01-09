#include "ChangeSimWindow.h"

#include "GellyDataTypes.h"
#include "imgui.h"

static constexpr const char *simTypes[] = {
	"Debug simulation", "RTFR simulation", "FleX simulation"
};

static constexpr const char *simTypeDescriptions[] = {
	"Debug simulation which generates a random particle cloud at attach time.",
	"Dataset simulation which loads and plays back a RTFR dataset.",
	"Real-time simulation which simulates fluids using NVIDIA FleX."
};

static int selectedSimType = 0;

static const char *GetSimTypeDescription() {
	return simTypeDescriptions[selectedSimType];
}

static testbed::GellySimMode GetSimMode() {
	switch (selectedSimType) {
		case 0:
			return testbed::GellySimMode::DEBUG;
		case 1:
			return testbed::GellySimMode::RTFR;
		case 2:
			return testbed::GellySimMode::FLEX;
		default:
			return testbed::GellySimMode::DEBUG;
	}
}

DEFINE_UI_DATA(ChangeSim, simMode, testbed::GellySimMode::DEBUG);
DEFINE_UI_DATA(ChangeSim, debugMaxParticles, 1000);
DEFINE_UI_DATA(ChangeSim, flexMaxParticles, 100000);
DEFINE_UI_DATA(ChangeSim, rtfrFolderPath, std::string(256, '\0'));
DEFINE_UI_DATA(ChangeSim, popupVisible, byte(0));
DEFINE_UI_DATA(ChangeSim, lastPopupVisible, byte(0));
DEFINE_UI_DATA(ChangeSim, submitted, false);

static void RenderSpecificSimTypeUI() {
	switch (GetSimMode()) {
		case testbed::GellySimMode::DEBUG:
			ImGui::InputInt(
				"Max particles", &UI_DATA(ChangeSim, debugMaxParticles)
			);
			break;
		case testbed::GellySimMode::RTFR:
			ImGui::InputText(
				"RTFR folder path",
				UI_DATA(ChangeSim, rtfrFolderPath).data(),
				256
			);
			break;
		case testbed::GellySimMode::FLEX:
			ImGui::InputInt(
				"Max particles", &UI_DATA(ChangeSim, flexMaxParticles)
			);
			break;
		default:
			break;
	}
}

IMPLEMENT_WINDOW(ChangeSim) {
	UI_DATA(ChangeSim, submitted) = false;
	if (ImGui::BeginPopup("Change Simulation")) {
		ImGui::SeparatorText("Simulation type");
		ImGui::Separator();
		ImGui::ListBox(
			"##simType", &selectedSimType, simTypes, ARRAYSIZE(simTypes)
		);

		UI_DATA(ChangeSim, simMode) = GetSimMode();
		ImGui::Text(GetSimTypeDescription());

		ImGui::SeparatorText("Simulation options");
		RenderSpecificSimTypeUI();

		if (ImGui::Button("OK")) {
			UI_DATA(ChangeSim, popupVisible) = false;
			UI_DATA(ChangeSim, submitted) = true;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

testbed::GellySimInit UIData::ChangeSim::GetGellySimInit() {
	using namespace testbed;
	using namespace Gelly::DataTypes;

	GellySimInit init;
	init.mode = UI_DATA(ChangeSim, simMode);
	switch (init.mode) {
		case GellySimMode::DEBUG: {
			init.modeInfo =
				GellySimInit::DebugInfo{UI_DATA(ChangeSim, debugMaxParticles)};
			break;
		}
		case GellySimMode::RTFR: {
			// First we need to remove the trailing null characters from the
			// string.
			const uint realStringLength =
				strlen(UI_DATA(ChangeSim, rtfrFolderPath).data());

			const std::string folderPathString(
				UI_DATA(ChangeSim, rtfrFolderPath).data(), realStringLength
			);

			init.modeInfo = GellySimInit::RTFRInfo{folderPathString};
			break;
		}
		case GellySimMode::FLEX: {
			init.modeInfo =
				GellySimInit::FlexInfo{UI_DATA(ChangeSim, flexMaxParticles)};
			break;
		}
		default:
			break;
	}

	return init;
}