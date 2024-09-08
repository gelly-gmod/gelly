import "./Slider.css";
import { useState } from "preact/hooks";

function SliderValueDropdown({
	setValueDropdownVisible,
	setValue,
	initalValue,
}: {
	setValueDropdownVisible: (visible: boolean) => void;
	setValue: (value: number) => void;
	initalValue: number;
}) {
	return (
		<div id="slider-value-dropdown">
			<input
				type="number"
				value={initalValue}
				onChange={(event) => {
					if (event.currentTarget) {
						setValue(parseFloat(event.currentTarget.value));
					}
				}}
			></input>
			<button
				onClick={() => {
					setValueDropdownVisible(false);
				}}
			>
				OK
			</button>
		</div>
	);
}

export default function Slider({
	min,
	max,
	step,
	unit,
	label,
	onChange,
}: {
	min: number;
	max: number;
	step: number;
	unit: string;
	label: string;
	onChange: (value: number) => void;
}) {
	const [valueDropdownVisible, setValueDropdownVisible] = useState(false);
	const [value, setValue] = useState(min);

	return (
		<div id="slider">
			<span id="label">{label}</span>
			<input
				type="range"
				min={min}
				max={max}
				step={step}
				value={value}
				onChange={(event) => {
					if (event.currentTarget) {
						onChange(parseFloat(event.currentTarget.value));
						setValue(parseFloat(event.currentTarget.value));
					}
				}}
			></input>
			<section id="value-display">
				<span id="value" onClick={() => setValueDropdownVisible(true)}>
					{value + unit}
				</span>
				{valueDropdownVisible && (
					<SliderValueDropdown
						setValueDropdownVisible={setValueDropdownVisible}
						setValue={setValue}
						initalValue={value}
					/>
				)}
			</section>
		</div>
	);
}
