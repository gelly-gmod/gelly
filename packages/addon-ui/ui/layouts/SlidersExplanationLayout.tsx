import { ComponentChildren } from "preact";
import "./SlidersExplanationLayout.css";

export function Sliders({ children }: { children: ComponentChildren }) {
	return <section id="sliders">{children}</section>;
}

export function ExplanationColumn({
	children,
}: {
	children: ComponentChildren;
}) {
	return <section id="explanation">{children}</section>;
}

export function Separator() {
	return <div id="separator" />;
}

export default function SlidersExplanationLayout({
	children,
}: {
	children: ComponentChildren;
}) {
	return <section id="sliders-explanation-layout">{children}</section>;
}
