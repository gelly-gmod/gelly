import Slider from "../components/Slider.tsx";
import Explanation, {
	ExplanationHeader,
	ExplanationText,
} from "../components/Explanation.tsx";
import SlidersExplanationLayout, {
	ExplanationColumn,
	Separator,
	Sliders,
} from "../layouts/SlidersExplanationLayout.tsx";

export default function Simulation() {
	return (
		<SlidersExplanationLayout>
			<Sliders>
				<Slider
					min={20}
					max={60}
					step={1}
					unit="Hz"
					label="Simulation Rate"
					onChange={() => {}}
				/>

				<Slider
					min={3}
					max={6}
					step={1}
					unit=""
					label="Iterations"
					onChange={() => {}}
				/>

				<Slider
					min={2}
					max={8}
					step={1}
					unit=""
					label="Substeps"
					onChange={() => {}}
				/>

				<Slider
					min={1}
					max={5}
					step={0.1}
					unit="x"
					label="Radius Scale"
					onChange={() => {}}
				/>
			</Sliders>
			<Separator />
			<ExplanationColumn>
				<Explanation>
					<ExplanationHeader>Simulation</ExplanationHeader>
					<ExplanationText>
						These settings control the behavior of the simulation,
						therefore providing ways to increase the performance at
						the cost of reduced simulation quality.
					</ExplanationText>
					<ExplanationText>
						Simulation rate, in particular, may cause visual lag, so
						it is not recommended to lower it below 60Hz.
					</ExplanationText>
				</Explanation>
			</ExplanationColumn>
		</SlidersExplanationLayout>
	);
}
