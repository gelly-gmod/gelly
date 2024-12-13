import "./ResetButton.css";

export default function ResetButton({
	onResetRequest,
}: {
	onResetRequest?: () => void;
}) {
	return (
		<i
			class="bi bi-arrow-counterclockwise reset-button"
			onClick={onResetRequest}
		></i>
	);
}
