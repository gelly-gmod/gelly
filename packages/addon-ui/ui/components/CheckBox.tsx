import "./CheckBox.css";
import ResetButton from "./ResetButton.tsx";

export default function CheckBox({
	label,
	checked,
	onChange,
	onResetRequest,
}: {
	label: string;
	checked: number;
	onChange: (checked: number) => void;
	onResetRequest?: () => void;
}) {
	return (
		<div id="check-box">
			<span id="label">{label}</span>
			<div id="spacer"></div>
			<input
				type="checkbox"
				defaultChecked={checked === 1}
				onClick={(event) => {
					// Source-style booleans are 1 and 0
					onChange(event.currentTarget.checked ? 1 : 0);
				}}
			></input>
			<ResetButton onResetRequest={onResetRequest} />
		</div>
	);
}
