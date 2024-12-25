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
import useSettingValue from "../hooks/useSettingValue.ts";
import {
	disableTemporaryTranslucency,
	enableTemporaryTranslucency,
} from "../util/temporary-translucency.ts";
import CheckBox from "../components/CheckBox.tsx";

export default function Graphics() {
	const [smoothness, setSmoothness, resetSmoothness] =
		useSettingValue("smoothness");
	const [resolutionScale, setResolutionScale, resetResolutionScale] =
		useSettingValue("resolution_scale");
	const [sunVisible, setSunVisible, resetSunVisible] =
		useSettingValue("sun_visible");
	const [glunkLightingFix, setGlunkLightingFix, resetGlunkLightingFix] =
		useSettingValue("glunk_lighting_fix");

	return (
		<SlidersExplanationLayout>
			<Sliders>
				<Slider
					min={1}
					max={30}
					step={1}
					value={smoothness}
					setValue={setSmoothness}
					unit=""
					label="Smoothness"
					onInputStart={enableTemporaryTranslucency}
					onInputEnd={disableTemporaryTranslucency}
					onResetRequest={resetSmoothness}
				/>

				<Slider
					min={0.25}
					max={1.25}
					step={0.05}
					value={resolutionScale}
					setValue={setResolutionScale}
					unit=""
					label="Resolution Scale"
					onInputStart={enableTemporaryTranslucency}
					onInputEnd={disableTemporaryTranslucency}
					onlySetValuesOnMouseUp
					onResetRequest={resetResolutionScale}
				/>

				<CheckBox
					label="Sun Enabled"
					checked={sunVisible}
					onChange={setSunVisible}
					onResetRequest={resetSunVisible}
				/>

				<CheckBox
					label="Glunk Lighting Fix"
					checked={glunkLightingFix}
					onChange={setGlunkLightingFix}
					onResetRequest={resetGlunkLightingFix}
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

					<ExplanationText warning>
						Lowering the resolution scale will reduce the quality of
						the fluid by a lot, but it will increase the
						performance. Likewise, increasing the resolution scale
						will increase the quality of the fluid, but it will
						decrease the performance.
					</ExplanationText>
				</Explanation>
			</ExplanationColumn>
		</SlidersExplanationLayout>
	);
}
