#!/usr/bin/env node
var parse = require('csv-parse');
const fs = require('fs');
const _ = require('lodash');

fs.readFile('./a.csv', (err, data) => {
	parse(data, {columns: true}, (err, output) => {
		let json = {};
		json = _.filter(output,(obj) => {
			return obj.INCLINATION != '';
		})
		// entre 0 i 360
		let RAAN = Math.random()*360;
		// entre 0 i 360
		let ARGP = Math.random()*360;
		// entre 0 i 360
		let M = Math.random()*360;
		let randomElements = {
			RAAN: RAAN,
			ARGP: ARGP,
			M: M
		}
		_.forEach(json, o => {
			_.assign(o, randomElements)
		})
		json = JSON.stringify(json);
		fs.writeFile('objects_in_orbit.json', json, 'utf8');
	});
})
