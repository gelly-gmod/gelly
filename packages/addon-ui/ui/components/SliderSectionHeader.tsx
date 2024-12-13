import { ComponentChildren } from "preact";
import { ExplanationHeader } from "./Explanation.tsx";

import "./SliderSectionHeader.css";

export default function SliderSectionHeader({
	children,
	onResetRequested,
}: {
	children: ComponentChildren;
	onResetRequested: () => void;
}) {
	return (
		<ExplanationHeader className="slider-section-header">
			{children}
			<button className="slider-section-reset" onClick={onResetRequested}>
				<i className="bi bi-arrow-counterclockwise"></i>
			</button>
		</ExplanationHeader>
	);
}
