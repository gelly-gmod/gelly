import { ComponentChildren } from "preact";
import "./Explanation.css";

export function ExplanationHeader({
	children,
	className,
}: {
	children: ComponentChildren;
	className?: string;
}) {
	return (
		<span class={"explanation-header " + className ?? ""}>{children}</span>
	);
}
export function ExplanationText({
	children,
	warning,
}: {
	children: ComponentChildren;
	warning?: boolean;
}) {
	return (
		<span class={warning ? "explanation-text warning" : "explanation-text"}>
			{children}
		</span>
	);
}

export default function Explanation({
	children,
}: {
	children: ComponentChildren;
}) {
	return <section class="explanation">{children}</section>;
}
