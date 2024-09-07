class SimulationPage extends HTMLElement {
	constructor() {
		super();
		this.attachShadow({mode: 'open'});
		this.shadowRoot.innerHTML = `
		<style>
			:host {
				display: block;
				padding: 10px;
			}
			
			main {
				display: flex;
				flex-direction: row;
				font-family: sans-serif;
				gap: 20px;
				flex-wrap: nowrap;
				justify-content: flex-start;
				align-items: flex-start;
			}
			
			#sliders {
				flex-grow: 2;
				width: 100%;
				
				display: flex;
				flex-direction: column;
				justify-content: flex-start;
				align-items: flex-start;
				gap: 10px;
			}
			
			#explanation {
				width: 30%;
				flex-grow: 1;
			}
		</style>
		
		<main>
			<section id="sliders">
				<gelly-label-slider id="sim-rate" label="Simulation Rate" min="20" max="144" unit="Hz" value="60"></gelly-label-slider>
				<gelly-label-slider id="substeps" label="Substeps" min="1" max="10" unit="" value="3"></gelly-label-slider>
				<gelly-label-slider id="iterations" label="Iterations" min="1" max="10" unit="" value="3"></gelly-label-slider>
				<gelly-label-slider id="radius-scale" label="Radius Scale" min="1" max="5" unit="x" value="1"></gelly-label-slider>
			</section>
			<gelly-explanation id="explanation" header="Simulation">
				These settings control the behavior of the simulation, therefore providing ways to
				increase the performance at the cost of reduced simulation quality.
				<br />
				<br />
				Simulation rate, in particular, may cause visual lag, so it is not
				recommended to lower it below 60Hz.
			</gelly-explanation>
		</main>
	  `;
	}

	connectedCallback() {
		const shadowRoot = this.shadowRoot;
		const simRate = shadowRoot.getElementById('sim-rate');
		simRate.addEventListener('valueChanged', (e) => {
			const value = e.detail.value;
			gelly.setSimulationRate(value);
		});

		const radiusScale = shadowRoot.getElementById('radius-scale');
		radiusScale.addEventListener('valueChanged', (e) => {
			const value = e.detail.value;
			gelly.setRadiusScale(value);
		});
	}
}

customElements.define(
	'gelly-simulation-page',
	SimulationPage
);