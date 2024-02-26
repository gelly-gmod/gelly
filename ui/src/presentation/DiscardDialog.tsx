import {
	Button,
	Dialog,
	DialogActions,
	DialogContent,
	DialogContentText,
	DialogTitle,
} from "@mui/material";

export default function DiscardDialog({
	onContinue,
	onCancel,
	open,
	setOpen,
}: {
	onContinue: () => void;
	onCancel: () => void;
	open: boolean;
	setOpen: (open: boolean) => void;
}) {
	const handleClose = () => {
		setOpen(false);
	};

	return (
		<Dialog open={open} onClose={handleClose}>
			<DialogTitle>Discard the currently modified settings?</DialogTitle>
			<DialogContent>
				<DialogContentText>
					If you continue, the currently modified settings will be
					discarded and the previous settings will be restored.
				</DialogContentText>
			</DialogContent>
			<DialogActions>
				<Button
					onClick={() => {
						handleClose();
						onCancel();
					}}
				>
					Cancel
				</Button>

				<Button
					onClick={() => {
						handleClose();
						onContinue();
					}}
				>
					Continue
				</Button>
			</DialogActions>
		</Dialog>
	);
}
