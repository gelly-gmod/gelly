// test web component
class LabelSlider extends HTMLElement {
	static observedAttributes = ['label', 'min', 'max', 'unit', 'value'];

	constructor() {
		super();
		this.attachShadow({mode: 'open'});
		this.shadowRoot.innerHTML = `
	  <style>
		:host {
			display: block;
			padding: 5px;
			
			width: 100%;
		}
		
		main {
		  display: flex;
		  font-family: sans-serif;
		  flex-direction: row;
		  justify-content: space-between;
		  align-items: center;
		  gap: 10px;
		}
		
		input {
			width: 500px;
		}
		
		#label {
			color: white;
		}
		
		#value {
			border: 1px solid #ccc;
			padding: 5px;
			border-radius: 5px;
			color: white;
		}
	  </style>
	  <main>
	  	<span id="label"></span>
	  	<input type="range" id="slider" min="0" max="100" value="50">
	  	<span contenteditable id="value"></span>
	  </main>
	`;
	}

	updateElements() {
		this.labelElement = this.shadowRoot.getElementById('label');
		this.sliderElement = this.shadowRoot.getElementById('slider');
		this.valueElement = this.shadowRoot.getElementById('value');

		this.labelElement.textContent = this.getAttribute('label');
		this.sliderElement.min = this.getAttribute('min');
		this.sliderElement.max = this.getAttribute('max');
		this.sliderElement.value = this.getAttribute('value');
		this.valueElement.textContent = this.getAttribute('value') + this.getAttribute('unit');

		this.sliderElement.addEventListener('input', () => {
			this.valueElement.textContent = this.sliderElement.value + this.getAttribute('unit');

			// dispatch an event to notify the parent
			this.dispatchValueChanged();
		});

		this.valueElement.addEventListener('input', () => {
			this.sliderElement.value = this.valueElement.textContent.replace(this.getAttribute('unit'), '');

			// append the unit if it doesn't exist
			if (!this.valueElement.textContent.includes(this.getAttribute('unit'))) {
				this.valueElement.textContent += this.getAttribute('unit');
			}

			this.dispatchValueChanged();
		});
	}

	dispatchValueChanged() {
		const sliderValue = this.sliderElement.value.replace(this.getAttribute('unit'), '');
		this.dispatchEvent(new CustomEvent('valueChanged', {
			detail: {
				value: sliderValue
			},
			bubbles: true
		}));
	}

	connectedCallback() {
		this.updateElements();
	}

	attributeChangedCallback(name, oldValue, newValue) {
		this.updateElements();
	}
}

customElements.define('gelly-label-slider', LabelSlider);