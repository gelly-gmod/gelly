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
import useSettingValue from "../hooks/useSettingValue.ts";
import Slider from "../components/Slider.tsx";
import {
	disableTemporaryTranslucency,
	enableTemporaryTranslucency,
} from "../util/temporary-translucency.ts";
import SliderSectionHeader from "../components/SliderSectionHeader.tsx";
import { useEffect, useState } from "preact/hooks";
import { ModMenu } from "../components/ModMenu.tsx";
import { gellySync } from "../gelly-sync.ts";

export default function Mods() {
	const [forcefieldStrength, setForcefieldStrength, resetForcefieldStrength] =
		useSettingValue("gelly_gun_forcefield_strength");
	const [forcefieldRadius, setForcefieldRadius, resetForcefieldRadius] =
		useSettingValue("gelly_gun_forcefield_radius");
	const [distance, setDistance, resetDistance] =
		useSettingValue("gelly_gun_distance");
	const [primarySize, setPrimarySize, resetPrimarySize] = useSettingValue(
		"gelly_gun_primary_size",
	);
	const [secondaryVelocity, setSecondaryVelocity, resetSecondaryVelocity] =
		useSettingValue("gelly_gun_secondary_velocity");
	const [secondarySize, setSecondarySize, resetSecondarySize] =
		useSettingValue("gelly_gun_secondary_size");
	const [density, setDensity, resetDensity] =
		useSettingValue("gelly_gun_density");

	const [mods, setMods] = useState<gelly.Mod[]>([]);
	useEffect(() => {
		gelly.getMods(setMods);

		const listener = () => {
			gelly.getMods(setMods);
		};

		gellySync.addListener(listener);
		return () => gellySync.removeListener(listener);
	}, []);

	return (
		<SlidersExplanationLayout>
			<Sliders>
				<SliderSectionHeader>Mods</SliderSectionHeader>
				<HorizontalSeparator />
				<ModMenu
					mods={mods}
					onModSelected={(mod) => {
						gelly.selectMod(mod.id);
					}}
				/>
				<SliderSectionHeader>Gelly Gun Settings</SliderSectionHeader>
				<HorizontalSeparator />
				<Slider
					min={-100}
					max={100}
					step={5}
					unit=""
					label="Forcefield Strength"
					value={forcefieldStrength}
					setValue={setForcefieldStrength}
					onInputStart={enableTemporaryTranslucency}
					onInputEnd={disableTemporaryTranslucency}
					onResetRequest={resetForcefieldStrength}
				/>
				<Slider
					min={1}
					max={1000}
					step={10}
					unit="hu"
					label="Forcefield Radius"
					value={forcefieldRadius}
					setValue={setForcefieldRadius}
					onInputStart={enableTemporaryTranslucency}
					onInputEnd={disableTemporaryTranslucency}
					onResetRequest={resetForcefieldRadius}
				/>

				<Slider
					min={10}
					max={400}
					step={10}
					unit="hu"
					label="Distance"
					value={distance}
					setValue={setDistance}
					onInputStart={enableTemporaryTranslucency}
					onInputEnd={disableTemporaryTranslucency}
					onResetRequest={resetDistance}
				/>

				<Slider
					min={1}
					max={10}
					step={1}
					unit="hu³"
					label="Primary Size"
					value={primarySize}
					setValue={setPrimarySize}
					onInputStart={enableTemporaryTranslucency}
					onInputEnd={disableTemporaryTranslucency}
					onResetRequest={resetPrimarySize}
				/>

				<Slider
					min={1}
					max={10}
					step={1}
					unit="hu³"
					label="Secondary Size"
					value={secondarySize}
					setValue={setSecondarySize}
					onInputStart={enableTemporaryTranslucency}
					onInputEnd={disableTemporaryTranslucency}
					onResetRequest={resetSecondarySize}
				/>

				<Slider
					min={10}
					max={220}
					step={10}
					unit="hu"
					label="Secondary Velocity"
					value={secondaryVelocity}
					setValue={setSecondaryVelocity}
					onInputStart={enableTemporaryTranslucency}
					onInputEnd={disableTemporaryTranslucency}
					onResetRequest={resetSecondaryVelocity}
				/>

				<Slider
					min={1}
					max={1000}
					step={10}
					unit=""
					label="Density"
					value={density}
					setValue={setDensity}
					onInputStart={enableTemporaryTranslucency}
					onInputEnd={disableTemporaryTranslucency}
					onResetRequest={resetDensity}
				/>
			</Sliders>
			<Separator />
			<ExplanationColumn>
				<Explanation>
					<ExplanationHeader>Mods</ExplanationHeader>
					<ExplanationText>
						This page is only for the sandbox mod, for now. In the
						future you will be able to customize other mods like the
						blood mod and select them from here.
					</ExplanationText>
				</Explanation>
			</ExplanationColumn>
		</SlidersExplanationLayout>
	);
}
