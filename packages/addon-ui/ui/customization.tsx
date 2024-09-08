import { render } from "preact";
import { useState } from "preact/hooks";
import "./customization.css";
import Presets from "./pages/presets.tsx";
import Graphics from "./pages/graphics.tsx";
import Mods from "./pages/mods.tsx";
import Simulation from "./pages/simulation.tsx";

enum CustomizationTab {
	Presets = "presets",
	Graphics = "graphics",
	Mods = "mods",
	Simulation = "simulation",
}

const CUSTOMIZATION_TAB_NAME_MAP = {
	[CustomizationTab.Presets]: "PRESETS",
	[CustomizationTab.Graphics]: "GRAPHICS",
	[CustomizationTab.Mods]: "MODS",
	[CustomizationTab.Simulation]: "SIMULATION",
};

function TabButton({
	tab,
	currentTab,
	setTab,
}: {
	tab: CustomizationTab;
	currentTab: CustomizationTab;
	setTab: (tab: CustomizationTab) => void;
}) {
	return (
		<button
			className={tab === currentTab ? "tab-button active" : "tab-button"}
			onClick={() => setTab(tab)}
		>
			{CUSTOMIZATION_TAB_NAME_MAP[tab]}
		</button>
	);
}
function App() {
	const [tab, setTab] = useState(CustomizationTab.Presets);

	return (
		<main>
			<section id={"tabs-list"}>
				{Object.keys(CUSTOMIZATION_TAB_NAME_MAP).map((tabKey) => (
					<TabButton
						tab={tabKey as CustomizationTab}
						currentTab={tab}
						setTab={setTab}
					/>
				))}
			</section>

			{tab === CustomizationTab.Presets ? <Presets /> : null}
			{tab === CustomizationTab.Graphics ? <Graphics /> : null}
			{tab === CustomizationTab.Mods ? <Mods /> : null}
			{tab === CustomizationTab.Simulation ? <Simulation /> : null}
		</main>
	);
}

const root = document.getElementById("root");

if (root) {
	render(<App />, root);
}
