import "./ModMenu.css";

interface ModMenuProps {
	mods: gelly.Mod[];
	onModSelected: (mod: gelly.Mod) => void;
}

export const ModMenu: preact.FunctionComponent<ModMenuProps> = ({
	mods,
	onModSelected,
}) => {
	return (
		<div id="mod-menu">
			{mods.map((mod) => (
				<button
					class={"mod-button" + (mod.enabled ? " enabled" : "")}
					onClick={() => onModSelected(mod)}
				>
					{mod.name}
				</button>
			))}
		</div>
	);
};
