import { ComponentChildren } from "preact";
import "./Explanation.css";

export function ExplanationHeader({
	children,
}: {
	children: ComponentChildren;
}) {
	return <span class="explanation-header">{children}</span>;
}
export function ExplanationText({ children }: { children: ComponentChildren }) {
	return <span class="explanation-text">{children}</span>;
}

export default function Explanation({
	children,
}: {
	children: ComponentChildren;
}) {
	return <section class="explanation">{children}</section>;
}
