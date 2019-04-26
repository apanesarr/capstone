var exampleSocket = new WebSocket("ws://192.168.0.100:8095");

exampleSocket.onopen = function (event) {
    setInterval(function () {
        exampleSocket.send(JSON.stringify({
            "MessageType": "SIM",
            "RecipientId": "-1",
            "Data": ""
        }))
    }, 500);
}

exampleSocket.onmessage = function (event) {
    let x = JSON.parse(event.data)["Data"]
    console.log(x)
    let y = data.filter(function(elem){
        return elem.value
    })

    data.forEach(function (elem) {
        x.forEach(function (nex) {
            if ( Math.floor(nex.Location[0] / 400) == elem.x && Math.floor(nex.Location[1] / 400)  == elem.y) {
                if (nex.Location[0]  == 600 && nex.Location[1] == 600){
                    data[1].value = 23
                }
                elem.value = nex.Temperature
            }
            
        })
    })
    // console.log(y)
    // if (y[0].x == 0 && y[0].y == 1 && y.length == 1){
    //     if (y[0].value == 0){
    //         data[1] = {
    //             x : 0,
    //             y : 1,
    //             value : 23
    //         }
    //     }
    // }

    d3.select("svg").remove();
    draw();
}

data = []

let i;
let j;

for (i = 0; i < 6; i++) {
    for (j = 0; j < 3; j++) {
        data.push({
            x: i,
            y: j,
            value: 0
        })
    }
}

window.onload = function () {
    draw();
}

function draw() {
    var margin = { top: 80, right: 25, bottom: 30, left: 40 },
        width = 450 - margin.left - margin.right,
        height = 450 - margin.top - margin.bottom;

    var svg = d3.select('#temp')
        .append('svg')
        .attr('width', width + margin.left + margin.right)
        .attr('height', height + margin.top + margin.bottom)
        .append('g')
        .attr('transform', 'translate(' + margin.left + ',' + margin.top + ')')

    var myGroups = d3.map(data, function (d) { return d.x; }).keys()
    var myVars = d3.map(data, function (d) { return d.y; }).keys()

    // Build X scales and axis:
    var x = d3.scaleBand()
        .range([0, width])
        .domain(myGroups)
        .padding(0.05);
    svg.append("g")
        .style("font-size", 15)
        .attr("transform", "translate(0," + height + ")")
        .call(d3.axisBottom(x).tickSize(0))
        .select(".domain").remove()

    // Build Y scales and axis:
    var y = d3.scaleBand()
        .range([height, 0])
        .domain(myVars)
        .padding(0.05);
    svg.append("g")
        .style("font-size", 15)
        .call(d3.axisLeft(y).tickSize(0))
        .select(".domain").remove()

    // Build color scale
    var myColor = d3.scaleSequential()
        .interpolator(d3.interpolateOrRd)
        .domain([0, 30])

    // create a tooltip
    var tooltip = d3.select("#my_dataviz")
        .append("div")
        .style("opacity", 0)
        .attr("class", "tooltip")
        .style("background-color", "white")
        .style("border", "solid")
        .style("border-width", "2px")
        .style("border-radius", "5px")
        .style("padding", "5px")

    // Three function that change the tooltip when user hover / move / leave a cell
    var mouseover = function (d) {
        tooltip
            .style("opacity", 1)
        d3.select(this)
            .style("stroke", "black")
            .style("opacity", 1)
    }
    var mousemove = function (d) {
        tooltip
            .html("The exact value of<br>this cell is: " + d.value)
            .style("left", (d3.mouse(this)[0] + 70) + "px")
            .style("top", (d3.mouse(this)[1]) + "px")
    }
    var mouseleave = function (d) {
        tooltip
            .style("opacity", 0)
        d3.select(this)
            .style("stroke", "none")
            .style("opacity", 0.8)
    }

    // add the squares
    svg.selectAll()
        .data(data, function (d) { return d.x + ':' + d.y; })
        .enter()
        .append("rect")
        .attr("x", function (d) { return x(d.x) })
        .attr("y", function (d) { return y(d.y) })
        .attr("rx", 4)
        .attr("ry", 4)
        .attr("width", x.bandwidth())
        .attr("height", y.bandwidth())
        .style("fill", function (d) { return myColor(d.value) })
        .style("stroke-width", 4)
        .style("stroke", "none")
        .style("opacity", 0.8)
        .on("mouseover", mouseover)
        .on("mousemove", mousemove)
        .on("mouseleave", mouseleave)


    svg.append("text")
        .attr("x", 0)
        .attr("y", -50)
        .attr("text-anchor", "left")
        .style("font-size", "22px")
        .text("Heat Map");

    // Add subtitle to graph
    svg.append("text")
        .attr("x", 0)
        .attr("y", -20)
        .attr("text-anchor", "left")
        .style("font-size", "14px")
        .style("fill", "grey")
        .style("max-width", 400)
        .text("Swarm Bot map");

}

