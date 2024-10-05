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
	const [smoothness, setSmoothness] = useSettingValue("smoothness");
	const [sunVisible, setSunVisible] = useSettingValue("sun_visible");

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
				/>

				<CheckBox
					label="Sun Enabled"
					checked={sunVisible}
					onChange={setSunVisible}
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
