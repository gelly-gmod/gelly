import "./ColorPicker.css";
import ResetButton from "./ResetButton.tsx";

export default function ColorPicker({
	color,
	label,
	onChange,
	onInputStart,
	onInputEnd,
	onResetRequest,
}: {
	color: string;
	label: string;
	onChange: (color: string) => void;
	onInputStart?: () => void;
	onInputEnd?: () => void;
	onResetRequest?: () => void;
}) {
	return (
		<div id="color-picker">
			<span id="label">{label}</span>
			<div id="spacer"></div>
			<input
				type="color"
				value={color}
				onInput={(event) => onChange(event.currentTarget.value)}
				onFocus={onInputStart}
				onBlur={onInputEnd}
			></input>
			<ResetButton onResetRequest={onResetRequest} />
		</div>
	);
}
