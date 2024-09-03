class GraphicsPage extends HTMLElement {
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
				<gelly-label-slider id="smoothness" label="Smoothness" min="1" max="20" unit="" value="5"></gelly-label-slider>
			</section>
			<gelly-explanation id="explanation" header="Graphics">
				These settings control the look and feel of Gelly.
				Normally, you should not need to change these settings. In addition, some settings may reduce performance.
			</gelly-explanation>
		</main>
	  `;
	}
}

customElements.define('gelly-graphics-page', GraphicsPage);