import ReversibleSettings from "./ReversibleSettings.ts";
import SettingsRepository from "../repositories/SettingsRepository.ts";
import { createContext } from "react";

export const ReversibleSettingsContext = createContext<ReversibleSettings>(
	new ReversibleSettings(new SettingsRepository()),
);
