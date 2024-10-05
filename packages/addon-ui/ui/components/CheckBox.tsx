import "./CheckBox.css";

export default function CheckBox({
	label,
	checked,
	onChange,
}: {
	label: string;
	checked: number;
	onChange: (checked: number) => void;
}) {
	return (
		<div id="check-box">
			<span id="label">{label}</span>
			<input
				type="checkbox"
				defaultChecked={checked === 1}
				onClick={(event) => {
					// Source-style booleans are 1 and 0
					onChange(event.currentTarget.checked ? 1 : 0);
				}}
			></input>
		</div>
	);
}
