import ReversibleSettings from "../utils/ReversibleSettings.ts";
import { useContext } from "react";
import { ReversibleSettingsContext } from "../utils/ReversibleSettingsContext.tsx";

export default function useReversibleSettings(): ReversibleSettings {
	return useContext(ReversibleSettingsContext);
}
