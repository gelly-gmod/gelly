import React from "react";
import ReactDOM from "react-dom/client";
import App from "./App.tsx";
import "./index.css";
import { ReversibleSettingsContext } from "./utils/ReversibleSettingsContext.tsx";
import SettingsRepository from "./repositories/SettingsRepository.ts";
import ReversibleSettings from "./utils/ReversibleSettings.ts";

const settingsRepository = SettingsRepository.loadSettingsFromStorage();
const reversibleSettings = new ReversibleSettings(settingsRepository);

ReactDOM.createRoot(document.getElementById("root")!).render(
	<React.StrictMode>
		<ReversibleSettingsContext.Provider value={reversibleSettings}>
			<App />
		</ReversibleSettingsContext.Provider>
	</React.StrictMode>,
);
