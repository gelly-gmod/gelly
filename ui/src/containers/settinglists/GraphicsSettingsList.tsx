import { GraphicsQuality } from "../../repositories/SettingsRepository.ts";
import { List, ListItem, ListItemText, MenuItem, Select } from "@mui/material";
import useReversibleSettings from "../../hooks/useReversibleSettings.ts";
import useSetting from "../../hooks/useSetting.ts";

export function GraphicsSettingsList() {
	const reversibleSettings = useReversibleSettings();
	const [graphicsQuality, setGraphicsQuality, isGraphicsQualityModified] =
		useSetting(reversibleSettings, "graphicsQuality");

	return (
		<List>
			<ListItem
				secondaryAction={
					<Select
						value={graphicsQuality}
						label="Preset"
						onChange={(e) => {
							setGraphicsQuality(
								e.target.value as GraphicsQuality,
							);
						}}
					>
						<MenuItem value={"Low"}>Low</MenuItem>
						<MenuItem value={"Medium"}>Medium</MenuItem>
						<MenuItem value={"High"}>High</MenuItem>
						<MenuItem value={"Epic"}>Epic</MenuItem>
					</Select>
				}
			>
				<ListItemText
					primary="Graphics Quality"
					secondary="Determines the quality of the final image"
					sx={isGraphicsQualityModified ? { color: "red" } : {}}
				/>
			</ListItem>
		</List>
	);
}
