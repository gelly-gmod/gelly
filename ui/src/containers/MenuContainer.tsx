import {
	AppBar,
	Box,
	Button,
	Stack,
	Tab,
	Tabs,
	Toolbar,
	Typography,
} from "@mui/material";
import React, { useState } from "react";
import SettingsContainer from "./SettingsContainer.tsx";
import DiscardDialog from "../presentation/DiscardDialog.tsx";
import useReversibleSettings from "../hooks/useReversibleSettings.ts";

function ApplyCancelButtons({
	setDiscardDialogOpen,
}: {
	setDiscardDialogOpen: (open: boolean) => void;
}) {
	const reversibleSettings = useReversibleSettings();

	return (
		<Stack
			direction={"row"}
			padding={2}
			gap={2}
			justifyContent={"flex-end"}
		>
			<Button
				variant="contained"
				onClick={() => {
					reversibleSettings.saveSettings();
				}}
			>
				APPLY
			</Button>
			<Button
				variant="contained"
				color={"inherit"}
				onClick={() => {
					setDiscardDialogOpen(true);
				}}
			>
				CANCEL
			</Button>
		</Stack>
	);
}

export default function MenuContainer() {
	const [tabIndex, setTabIndex] = useState(0);
	const [discardDialogOpen, setDiscardDialogOpen] = useState(false);
	const reversibleSettings = useReversibleSettings();

	return (
		<>
			<DiscardDialog
				onContinue={() => {
					reversibleSettings.revert();
				}}
				onCancel={() => {}}
				open={discardDialogOpen}
				setOpen={setDiscardDialogOpen}
			/>
			<Box
				sx={{
					width: "100%",
				}}
			>
				<AppBar position="static">
					<Toolbar variant="dense">
						<Typography
							variant="h6"
							component="div"
							color="inherit"
						>
							Gelly
						</Typography>
					</Toolbar>
				</AppBar>

				<Stack spacing={2} padding={2}>
					<Tabs
						value={tabIndex}
						onChange={(_: React.SyntheticEvent, value: number) =>
							setTabIndex(value)
						}
					>
						<Tab label="SETTINGS" />
						<Tab label="PRESETS" />
						<Tab label="DEBUG INFO" />
					</Tabs>
				</Stack>

				<Box padding={2}>{tabIndex === 0 && <SettingsContainer />}</Box>

				<ApplyCancelButtons
					setDiscardDialogOpen={setDiscardDialogOpen}
				/>
			</Box>
		</>
	);
}
