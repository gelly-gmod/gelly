import "./Slider.css";
import { useState, useRef, useEffect } from "preact/hooks";
import ResetButton from "./ResetButton.tsx";

function trimFloat(float: number) {
	// from: https://stackoverflow.com/a/11832950
	return Math.round((float + Number.EPSILON) * 100) / 100;
}

function SliderValueDropdown({
	setValueDropdownVisible,
	setValue,
	initalValue,
}: {
	setValueDropdownVisible: (visible: boolean) => void;
	setValue: (value: number) => void;
	initalValue: number;
}) {
	const inputRef = useRef<HTMLInputElement>(null);
	useEffect(() => {
		if (inputRef.current) {
			inputRef.current.focus();
		}
	}, [inputRef]);

	return (
		<input
			class="slider-value-dropdown"
			type="number"
			ref={inputRef}
			value={initalValue}
			onChange={(event) => {
				if (event.currentTarget) {
					const parsedFloat = parseFloat(event.currentTarget.value);
					if (isNaN(parsedFloat)) {
						setValueDropdownVisible(false);
						return;
					}

					setValue(parsedFloat);
				}
			}}
			onBlur={() => setValueDropdownVisible(false)}
			onKeyDown={(event) => {
				if (event.key === "Enter") {
					setValueDropdownVisible(false);
				}
			}}
		></input>
	);
}

export default function Slider({
	min,
	max,
	step,
	unit,
	label,
	value,
	setValue,
	onInputStart,
	onInputEnd,
	onResetRequest,
	onlySetValuesOnMouseUp,
}: {
	min: number;
	max: number;
	step: number;
	unit: string;
	label: string;
	value: number;
	setValue: (value: number) => void;
	onInputStart?: () => void;
	onInputEnd?: () => void;
	onResetRequest?: () => void;
	onlySetValuesOnMouseUp?: boolean;
}) {
	const [valueDropdownVisible, setValueDropdownVisible] = useState(false);

	return (
		<div id="slider">
			<span id="label">{label}</span>
			<input
				type="range"
				min={min}
				max={max}
				step={step}
				value={value}
				onInput={(event) => {
					if (event.target && !onlySetValuesOnMouseUp) {
						// @ts-ignore
						setValue(parseFloat(event.target.value));
					}
				}}
				onChange={(event) => {
					if (event.target && onlySetValuesOnMouseUp) {
						// @ts-ignore
						setValue(parseFloat(event.target.value));
					}
				}}
				onMouseDown={onInputStart}
				onMouseUp={onInputEnd}
			></input>
			<section id="value-display">
				<span id="value" onClick={() => setValueDropdownVisible(true)}>
					{trimFloat(value) + unit}
				</span>
				{valueDropdownVisible && (
					<SliderValueDropdown
						setValueDropdownVisible={setValueDropdownVisible}
						setValue={setValue}
						initalValue={value}
					/>
				)}
			</section>
			<ResetButton onResetRequest={onResetRequest} />
		</div>
	);
}
