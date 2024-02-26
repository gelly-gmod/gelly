import {
	Accordion,
	AccordionDetails,
	AccordionSummary,
	Box,
	Typography,
} from "@mui/material";
import ArrowDropDownIcon from "@mui/icons-material/ArrowDropDown";
import { AddonSettingsList } from "./settinglists/AddonSettingsList.tsx";
import { SimulationSettingsList } from "./settinglists/SimulationSettingsList.tsx";
import { GraphicsSettingsList } from "./settinglists/GraphicsSettingsList.tsx";

export default function SettingsContainer() {
	return (
		<Box>
			<Accordion>
				<AccordionSummary expandIcon={<ArrowDropDownIcon />}>
					<Typography sx={{ width: "33%", flexShrink: 0 }}>
						Graphics
					</Typography>
					<Typography sx={{ color: "text.secondary" }}>
						Settings relating to the graphics of Gelly
					</Typography>
				</AccordionSummary>
				<AccordionDetails>
					<GraphicsSettingsList />
				</AccordionDetails>
			</Accordion>
			<Accordion>
				<AccordionSummary expandIcon={<ArrowDropDownIcon />}>
					<Typography sx={{ width: "33%", flexShrink: 0 }}>
						Simulation
					</Typography>
					<Typography sx={{ color: "text.secondary" }}>
						Settings relating to the fluid simulation
					</Typography>
				</AccordionSummary>
				<AccordionDetails>
					<SimulationSettingsList />
				</AccordionDetails>
			</Accordion>
			<Accordion>
				<AccordionSummary expandIcon={<ArrowDropDownIcon />}>
					<Typography sx={{ width: "33%", flexShrink: 0 }}>
						Addon
					</Typography>
					<Typography sx={{ color: "text.secondary" }}>
						Settings relating to the addon
					</Typography>
				</AccordionSummary>
				<AccordionDetails>
					<AddonSettingsList />
				</AccordionDetails>
			</Accordion>
		</Box>
	);
}
