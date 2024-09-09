import SlidersExplanationLayout, {
	ExplanationColumn,
	Separator,
	Sliders,
} from "../layouts/SlidersExplanationLayout.tsx";
import Slider from "../components/Slider.tsx";
import Explanation, {
	ExplanationHeader,
	ExplanationText,
} from "../components/Explanation.tsx";

export default function Graphics() {
	return (
		<SlidersExplanationLayout>
			<Sliders>
				<Slider
					min={1}
					max={30}
					step={1}
					unit=""
					label="Smoothness"
					onChange={() => {}}
				/>
			</Sliders>
			<Separator />
			<ExplanationColumn>
				<Explanation>
					<ExplanationHeader>Graphics</ExplanationHeader>
					<ExplanationText>
						These settings control the appearance of the simulation,
						therefore providing ways to increase the visual quality
						at the cost of reduced performance.
					</ExplanationText>

					<ExplanationText>
						In particular, a higher smoothness value will result in
						less performance--but a smoother appearance.
					</ExplanationText>
				</Explanation>
			</ExplanationColumn>
		</SlidersExplanationLayout>
	);
}
