import "./ColorPicker.css";

export default function ColorPicker({
	color,
	label,
	onChange,
	onInputStart,
	onInputEnd,
}: {
	color: string;
	label: string;
	onChange: (color: string) => void;
	onInputStart?: () => void;
	onInputEnd?: () => void;
}) {
	return (
		<div id="color-picker">
			<span id="label">{label}</span>
			<input
				type="color"
				value={color}
				onInput={(event) => onChange(event.currentTarget.value)}
				onFocus={onInputStart}
				onBlur={onInputEnd}
			></input>
		</div>
	);
}
