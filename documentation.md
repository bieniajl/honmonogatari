# Documentation of ほんものがたり

## Book

A book contains a specific set of characters, chapters, locations and items. It is stored in one file inside the library
folder, for the user to use. This file is a json file. For the structure and contents see the example below.

### Example Book
```json
{
	"characters": [
		{
			"data": [
				{
					"name": "Vorname",
					"type": "string",
					"value": "Max"
				}
			],
			"abilities": [
				{
					"name": "fire",
					"value": 5
				},
				{
					"name": "smithing",
					"value": 100
				}
			],
			"items": [
				{
					"name": "dagger",
					"value": "100"
				}
			],
			"relations": []
		}
	],
	"locations": [],
	"items": [],
	"chapters": {}
}
```
## radarChart

To draw a radar chart simply create a new object of the Radar chart class

The data is an array of arrays, each sub array represents a new blob in the Chart

### Example

```js
//data for the chart

data = [
	[
		{axis: "Label1", value: 1}
		{axis: "smithing", value: 2}
	]
]

options = {
	roundStrokes = true;1
}

yourRadarChart = new RadarChart(<<DomLocation>>,data,options)
```
