import { ComponentChildren } from "preact";
import { ExplanationHeader } from "./Explanation.tsx";

import "./SliderSectionHeader.css";

export default function SliderSectionHeader({
	children,
}: {
	children: ComponentChildren;
}) {
	return (
		<ExplanationHeader className="slider-section-header">
			{children}
		</ExplanationHeader>
	);
}
