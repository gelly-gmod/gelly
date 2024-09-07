class Explanation extends HTMLElement {
	static observedAttributes = ['header'];

	constructor() {
		super();
		this.attachShadow({mode: 'open'});
		this.shadowRoot.innerHTML = `
	  <style>
		:host {
			display: block;
			padding: 10px;
		}
		
		#header {
			font-family: sans-serif;
			font-size: 1.5em;
			color: #fff;
			margin-bottom: 30px;
			padding-bottom: 5px;
			font-weight: bold;
		}
		
		#content {
			font-family: sans-serif;
			color: #e5e5e5;
			font-size: 0.8em;
		}
		
		br {
			/* Break twice the line height */
			line-height: 2em;
		}
	  </style>
	  <span id="header"></span>
	  <br/>
	  <span id="content"></span>
	  `;

		this.content = this.innerHTML;
	}

	updateElements() {
		this.headerElement = this.shadowRoot.getElementById('header');
		this.contentElement = this.shadowRoot.getElementById('content');

		this.headerElement.textContent = this.getAttribute('header');
		// we use inner html to preserve anything like <br> or <a> tags
		this.contentElement.innerHTML = this.content;
	}

	connectedCallback() {
		this.updateElements();
	}

	attributeChangedCallback() {
		this.updateElements();
	}
}

customElements.define('gelly-explanation', Explanation);