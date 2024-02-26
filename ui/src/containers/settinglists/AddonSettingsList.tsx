import useSetting from "../../hooks/useSetting.ts";
import { List, ListItem, ListItemText, MenuItem, Select } from "@mui/material";
import useReversibleSettings from "../../hooks/useReversibleSettings.ts";

export function AddonSettingsList() {
	const reversibleSettings = useReversibleSettings();

	const [defaultMod, setDefaultMod, isDefaultModModified] = useSetting(
		reversibleSettings,
		"addonDefaultMod",
	);

	return (
		<List>
			<ListItem
				secondaryAction={
					<Select
						value={defaultMod}
						label="Default Mod"
						onChange={(e) => {
							setDefaultMod(e.target.value as string);
						}}
					>
						<MenuItem value={"None"}>None</MenuItem>
						{/* TODO: When we get actual data coming from the server, we can map the values here */}
					</Select>
				}
			>
				<ListItemText
					primary="Default Mod"
					secondary="Determines the default mod on each startup"
					sx={isDefaultModModified ? { color: "red" } : {}}
				/>
			</ListItem>
		</List>
	);
}
