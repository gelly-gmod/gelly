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
import CheckBox from "../components/CheckBox.tsx";
import SliderSectionHeader from "../components/SliderSectionHeader.tsx";
import { PresetMenu } from "../components/PresetMenu.tsx";
import { useEffect, useState } from "preact/hooks";

export default function Presets() {
	const [fluidRadius, setFluidRadius, resetFluidRadius] =
		useSettingValue("fluid_radius");
	const [viscosity, setViscosity, resetViscosity] =
		useSettingValue("fluid_viscosity");
	const [cohesion, setCohesion, resetCohesion] =
		useSettingValue("fluid_cohesion");
	const [adhesion, setAdhesion, resetAdhesion] =
		useSettingValue("fluid_adhesion");
	const [friction, setFriction, resetFriction] =
		useSettingValue("fluid_friction");
	const [restDistanceRatio, setRestDistanceRatio, resetRestDistanceRatio] =
		useSettingValue("fluid_rest_distance_ratio");
	const [surfaceTension, setSurfaceTension, resetSurfaceTension] =
		useSettingValue("fluid_surface_tension");
	const [colorHexCode, setColorHexCode, resetColorHexCode] =
		useSettingValueString("fluid_color_hex");
	const [colorScale, setColorScale, resetColorScale] =
		useSettingValue("fluid_color_scale");
	const [roughness, setRoughness, resetRoughness] =
		useSettingValue("fluid_roughness");
	const [opaque, setOpaque, resetOpaque] = useSettingValue("fluid_opaque");
	const [metal, setMetal, resetMetal] = useSettingValue("fluid_metal");
	const [scatter, setScatter, resetScatter] =
		useSettingValue("fluid_scatter");

	const [useWhitewater, setUseWhitewater, resetUseWhitewater] =
		useSettingValue("fluid_use_whitewater");

	const [presets, setPresets] = useState<gelly.Preset[]>([]);

	useEffect(() => {
		gelly.getPresets(setPresets);
	}, []);

	return (
		<SlidersExplanationLayout>
			<Sliders>
				<SliderSectionHeader>Fluid Settings</SliderSectionHeader>
				<HorizontalSeparator />
				<Slider
					min={1}
					max={50}
					step={0.1}
					unit="hu"
					label="Radius"
					value={fluidRadius}
					setValue={setFluidRadius}
					onInputStart={enableTemporaryTranslucency}
					onInputEnd={disableTemporaryTranslucency}
					onResetRequest={resetFluidRadius}
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
					onResetRequest={resetViscosity}
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
					onResetRequest={resetCohesion}
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
					onResetRequest={resetAdhesion}
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
					onResetRequest={resetFriction}
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
					onResetRequest={resetRestDistanceRatio}
				/>
				<Slider
					min={0}
					max={1}
					step={0.01}
					unit="x"
					label="Surface Tension"
					value={surfaceTension}
					setValue={setSurfaceTension}
					onInputStart={enableTemporaryTranslucency}
					onInputEnd={disableTemporaryTranslucency}
					onResetRequest={resetSurfaceTension}
				/>
				<SliderSectionHeader>Fluid Material</SliderSectionHeader>
				<HorizontalSeparator />
				<ColorPicker
					color={colorHexCode}
					label="Color"
					onChange={setColorHexCode}
					onInputStart={enableTemporaryTranslucency}
					onInputEnd={disableTemporaryTranslucency}
					onResetRequest={resetColorHexCode}
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
					onResetRequest={resetColorScale}
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
					onResetRequest={resetRoughness}
				/>

				<CheckBox
					label="Metal"
					checked={metal}
					onChange={setMetal}
					onResetRequest={resetMetal}
				/>

				<CheckBox
					label="Scatter"
					checked={scatter}
					onChange={setScatter}
					onResetRequest={resetScatter}
				/>

				<CheckBox
					label="Opaque"
					checked={opaque}
					onChange={setOpaque}
					onResetRequest={resetOpaque}
				/>

				<CheckBox
					label="Use Whitewater"
					checked={useWhitewater}
					onChange={setUseWhitewater}
					onResetRequest={resetUseWhitewater}
				/>

				<ExplanationHeader>Presets</ExplanationHeader>
				<HorizontalSeparator />
				<PresetMenu
					presets={presets}
					onPresetSelected={(preset) =>
						gelly.selectPreset(preset.name)
					}
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
