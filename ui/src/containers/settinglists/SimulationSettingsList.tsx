import useSetting from "../../hooks/useSetting.ts";
import { Box, List, ListItem, ListItemText, Slider } from "@mui/material";
import useReversibleSettings from "../../hooks/useReversibleSettings.ts";

export function SimulationSettingsList() {
	const reversibleSettings = useReversibleSettings();

	const [substeps, setSubsteps, isSubstepsModified] = useSetting(
		reversibleSettings,
		"simSubsteps",
	);

	const [iterations, setIterations, isIterationsModified] = useSetting(
		reversibleSettings,
		"simIterations",
	);

	return (
		<List>
			<ListItem
				secondaryAction={
					<Box sx={{ width: 250 }}>
						<Slider
							value={substeps}
							valueLabelDisplay="auto"
							step={1}
							min={1}
							max={10}
							marks
							onChange={(_, value) => {
								setSubsteps(value as number);
							}}
						/>
					</Box>
				}
			>
				<ListItemText
					primary="Substeps"
					secondary="Determines the stability of the simulation"
					sx={isSubstepsModified ? { color: "red" } : {}}
				/>
			</ListItem>

			<ListItem
				secondaryAction={
					<Box sx={{ width: 250 }}>
						<Slider
							value={iterations}
							valueLabelDisplay="auto"
							step={1}
							min={3}
							max={10}
							marks
							onChange={(_, value) => {
								setIterations(value as number);
							}}
						/>
					</Box>
				}
			>
				<ListItemText
					primary="Iterations"
					secondary="Determines the accuracy of the simulation"
					sx={isIterationsModified ? { color: "red" } : {}}
				/>
			</ListItem>
		</List>
	);
}
