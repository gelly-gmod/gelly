import "./PresetMenu.css";

interface PresetMenuProps {
	presets: gelly.Preset[];
	onPresetSelected: (preset: gelly.Preset) => void;
}

export const PresetMenu: preact.FunctionComponent<PresetMenuProps> = ({
	presets,
	onPresetSelected,
}) => {
	return (
		<div id="preset-menu">
			{presets.map((preset) => (
				<button
					className="preset-button"
					style={{ color: preset.color }}
					onClick={() => onPresetSelected(preset)}
				>
					{preset.name}
				</button>
			))}
		</div>
	);
};
