import SlidersExplanationLayout, {
	ExplanationColumn,
	HorizontalSeparator,
	Separator,
	Sliders,
} from "../layouts/SlidersExplanationLayout.tsx";
import Explanation, {
	ExplanationHeader,
	ExplanationText,
} from "../components/Explanation.tsx";
import useSettingValue, {
	useSettingValueString,
} from "../hooks/useSettingValue.ts";
import Slider from "../components/Slider.tsx";
import {
	disableTemporaryTranslucency,
	enableTemporaryTranslucency,
} from "../util/temporary-translucency.ts";
import ColorPicker from "../components/ColorPicker.tsx";

export default function Presets() {
	const [fluidRadius, setFluidRadius] = useSettingValue("fluid_radius");
	const [viscosity, setViscosity] = useSettingValue("fluid_viscosity");
	const [cohesion, setCohesion] = useSettingValue("fluid_cohesion");
	const [adhesion, setAdhesion] = useSettingValue("fluid_adhesion");
	const [friction, setFriction] = useSettingValue("fluid_friction");
	const [restDistanceRatio, setRestDistanceRatio] = useSettingValue(
		"fluid_rest_distance_ratio",
	);
	const [colorHexCode, setColorHexCode] =
		useSettingValueString("fluid_color_hex");
	const [colorScale, setColorScale] = useSettingValue("fluid_color_scale");
	const [roughness, setRoughness] = useSettingValue("fluid_roughness");

	return (
		<SlidersExplanationLayout>
			<Sliders>
				<ExplanationHeader>Fluid Settings</ExplanationHeader>
				<HorizontalSeparator />
				<Slider
					min={1}
					max={10}
					step={0.1}
					unit="hu"
					label="Radius"
					value={fluidRadius}
					setValue={setFluidRadius}
					onInputStart={enableTemporaryTranslucency}
					onInputEnd={disableTemporaryTranslucency}
				/>
				<Slider
					min={0}
					max={100}
					step={0.1}
					unit=""
					label="Viscosity"
					value={viscosity}
					setValue={setViscosity}
					onInputStart={enableTemporaryTranslucency}
					onInputEnd={disableTemporaryTranslucency}
				/>
				<Slider
					min={0}
					max={1}
					step={0.01}
					unit=""
					label="Cohesion"
					value={cohesion}
					setValue={setCohesion}
					onInputStart={enableTemporaryTranslucency}
					onInputEnd={disableTemporaryTranslucency}
				/>
				<Slider
					min={0}
					max={1}
					step={0.01}
					unit=""
					label="Adhesion"
					value={adhesion}
					setValue={setAdhesion}
					onInputStart={enableTemporaryTranslucency}
					onInputEnd={disableTemporaryTranslucency}
				/>
				<Slider
					min={0}
					max={10}
					step={0.1}
					unit=""
					label="Friction"
					value={friction}
					setValue={setFriction}
					onInputStart={enableTemporaryTranslucency}
					onInputEnd={disableTemporaryTranslucency}
				/>
				<Slider
					min={0.5}
					max={0.7}
					step={0.01}
					unit=""
					label="Rest Distance Ratio"
					value={restDistanceRatio}
					setValue={setRestDistanceRatio}
					onInputStart={enableTemporaryTranslucency}
					onInputEnd={disableTemporaryTranslucency}
				/>
				<ExplanationHeader>Fluid Material</ExplanationHeader>
				<HorizontalSeparator />
				<ColorPicker
					color={colorHexCode}
					label="Color"
					onChange={setColorHexCode}
					onInputStart={enableTemporaryTranslucency}
					onInputEnd={disableTemporaryTranslucency}
				/>
				<Slider
					min={0.5}
					max={10}
					step={0.5}
					unit={"x"}
					label="Color Scale"
					value={colorScale}
					setValue={setColorScale}
					onInputStart={enableTemporaryTranslucency}
					onInputEnd={disableTemporaryTranslucency}
				/>
				<Slider
					min={0}
					max={1}
					step={0.01}
					unit=""
					label="Roughness"
					value={roughness}
					setValue={setRoughness}
					onInputStart={enableTemporaryTranslucency}
					onInputEnd={disableTemporaryTranslucency}
				/>
			</Sliders>
			<Separator />
			<ExplanationColumn>
				<Explanation>
					<ExplanationHeader>Presets</ExplanationHeader>
					<ExplanationText>
						Presets allow you to save a specific configuration of
						the fluid in-game to be used later on. You can start
						with any immutable preset, such as the ones in the Gelly
						Gun.
					</ExplanationText>
					<ExplanationText>
						From there, you can modify the preset to your liking and
						save it.
					</ExplanationText>
				</Explanation>
			</ExplanationColumn>
		</SlidersExplanationLayout>
	);
}
