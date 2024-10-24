import Slider from "../components/Slider.tsx";
import Explanation, {
	ExplanationHeader,
	ExplanationText,
} from "../components/Explanation.tsx";
import SlidersExplanationLayout, {
	ExplanationColumn,
	HorizontalSeparator,
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
	const [relaxation, setRelaxation] = useSettingValue(
		"simulation_relaxation",
	);
	const [collisionDistance, setCollisionDistance] = useSettingValue(
		"simulation_collision_distance",
	);
	const [gravity, setGravity] = useSettingValue("simulation_gravity");
	const [maxParticles, setMaxParticles] = useSettingValue("max_particles");
	const [sprayThreshold, setSprayThreshold] =
		useSettingValue("spray_threshold");
	const [sprayDrag, setSprayDrag] = useSettingValue("spray_drag");
	const [sprayBuoyancy, setSprayBuoyancy] = useSettingValue("spray_buoyancy");
	const [sprayLifetime, setSprayLifetime] = useSettingValue("spray_lifetime");
	const [sprayScale, setSprayScale] = useSettingValue("spray_scale");
	const [sprayMotionBlur, setSprayMotionBlur] =
		useSettingValue("spray_motion_blur");

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

				<Slider
					min={10000}
					max={1500000}
					step={10000}
					unit=""
					label="Max Particles"
					value={maxParticles}
					setValue={setMaxParticles}
					onInputStart={enableTemporaryTranslucency}
					onInputEnd={disableTemporaryTranslucency}
					onlySetValuesOnMouseUp
				/>

				<Slider
					min={0}
					max={1}
					step={0.01}
					unit=""
					value={relaxation}
					setValue={setRelaxation}
					label="Relaxation Factor"
					onInputStart={enableTemporaryTranslucency}
					onInputEnd={disableTemporaryTranslucency}
				/>

				<Slider
					min={0}
					max={5}
					step={0.25}
					unit="hu"
					value={collisionDistance}
					setValue={setCollisionDistance}
					label="Collision Distance"
					onInputStart={enableTemporaryTranslucency}
					onInputEnd={disableTemporaryTranslucency}
				/>

				<Slider
					min={-100}
					max={100}
					step={0.5}
					unit="hu/s²"
					value={gravity}
					setValue={setGravity}
					label="Gravity"
					onInputStart={enableTemporaryTranslucency}
					onInputEnd={disableTemporaryTranslucency}
				/>

				<ExplanationHeader>Spray Settings</ExplanationHeader>
				<HorizontalSeparator />

				<Slider
					min={0}
					max={250}
					step={1}
					unit={"hu/s"}
					label="Spray Threshold"
					value={sprayThreshold}
					setValue={setSprayThreshold}
					onInputStart={enableTemporaryTranslucency}
					onInputEnd={disableTemporaryTranslucency}
				/>

				<Slider
					min={0}
					max={3}
					step={0.1}
					unit=""
					label="Spray Drag"
					value={sprayDrag}
					setValue={setSprayDrag}
					onInputStart={enableTemporaryTranslucency}
					onInputEnd={disableTemporaryTranslucency}
				/>

				<Slider
					min={0}
					max={1}
					step={0.01}
					unit=""
					label="Spray Buoyancy"
					value={sprayBuoyancy}
					setValue={setSprayBuoyancy}
					onInputStart={enableTemporaryTranslucency}
					onInputEnd={disableTemporaryTranslucency}
				/>

				<Slider
					min={0}
					max={10}
					step={0.1}
					unit="s"
					label="Spray Lifetime"
					value={sprayLifetime}
					setValue={setSprayLifetime}
					onInputStart={enableTemporaryTranslucency}
					onInputEnd={disableTemporaryTranslucency}
				/>

				<Slider
					min={0}
					max={4}
					step={0.1}
					unit="x"
					label="Spray Scale"
					value={sprayScale}
					setValue={setSprayScale}
					onInputStart={enableTemporaryTranslucency}
					onInputEnd={disableTemporaryTranslucency}
				/>

				<Slider
					min={0}
					max={10}
					step={0.1}
					unit=""
					label="Spray Motion Blur"
					value={sprayMotionBlur}
					setValue={setSprayMotionBlur}
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
					<ExplanationText warning>
						Max particles is a hard limit on the number of particles
						that can be simulated.
					</ExplanationText>
					<ExplanationText warning>
						You will crash if you set the limit higher than your
						computer can handle.
					</ExplanationText>
				</Explanation>
			</ExplanationColumn>
		</SlidersExplanationLayout>
	);
}
