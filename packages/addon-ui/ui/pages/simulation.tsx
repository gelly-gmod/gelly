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
import useSettingValue from "../hooks/useSettingValue.ts";
import {
	disableTemporaryTranslucency,
	enableTemporaryTranslucency,
} from "../util/temporary-translucency.ts";

export default function Simulation() {
	const [simRate, setSimRate] = useSettingValue("simulation_rate");
	const [iterations, setIterations] = useSettingValue(
		"simulation_iterations",
	);
	const [substeps, setSubsteps] = useSettingValue("simulation_substeps");
	const [presetRadiusScale, setPresetRadiusScale] = useSettingValue(
		"preset_radius_scale",
	);
	return (
		<SlidersExplanationLayout>
			<Sliders>
				<Slider
					min={20}
					max={60}
					step={1}
					unit="Hz"
					value={simRate}
					setValue={setSimRate}
					label="Simulation Rate"
					onInputStart={enableTemporaryTranslucency}
					onInputEnd={disableTemporaryTranslucency}
				/>

				<Slider
					min={3}
					max={6}
					step={1}
					unit=""
					value={iterations}
					setValue={setIterations}
					label="Iterations"
					onInputStart={enableTemporaryTranslucency}
					onInputEnd={disableTemporaryTranslucency}
				/>

				<Slider
					min={2}
					max={8}
					step={1}
					unit=""
					value={substeps}
					setValue={setSubsteps}
					label="Substeps"
					onInputStart={enableTemporaryTranslucency}
					onInputEnd={disableTemporaryTranslucency}
				/>

				<Slider
					min={1}
					max={5}
					step={0.1}
					unit="x"
					value={presetRadiusScale}
					setValue={setPresetRadiusScale}
					label="Radius Scale"
					onInputStart={enableTemporaryTranslucency}
					onInputEnd={disableTemporaryTranslucency}
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
