import { render } from "preact";
import { useEffect, useState } from "preact/hooks";
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
	disabled,
}: {
	tab: CustomizationTab;
	currentTab: CustomizationTab;
	setTab: (tab: CustomizationTab) => void;
	disabled?: boolean;
}) {
	return (
		<button
			className={tab === currentTab ? "tab-button active" : "tab-button"}
			disabled={disabled}
			onClick={() => setTab(tab)}
		>
			{CUSTOMIZATION_TAB_NAME_MAP[tab]}
		</button>
	);
}
function App() {
	const [tab, setTab] = useState(CustomizationTab.Presets);
	const [version, setVersion] = useState("");

	useEffect(() => {
		gelly.getVersion(setVersion);
	}, []);

	return (
		<main>
			<header>
				<section id={"tabs-list"}>
					{Object.keys(CUSTOMIZATION_TAB_NAME_MAP).map((tabKey) => (
						<TabButton
							tab={tabKey as CustomizationTab}
							currentTab={tab}
							setTab={setTab}
						/>
					))}
				</section>

				<span id="version">{version}</span>
			</header>

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
