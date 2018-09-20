import { Component, OnInit, OnDestroy, ViewChild, AfterViewInit, OnChanges, SimpleChanges } from '@angular/core';
import { Router } from '@angular/router';
import { Welcome } from '../../models/welcome';
import { WelcomeService } from '../../services/welcome.service';
import { GraphChartService } from '../../../app-shared/services/graph-chart.service';
import { Subscription } from 'rxjs/Subscription';
import { GraphChart } from 'gvis';
import * as gvis from 'gvis';

import * as L from 'leaflet';

//const GraphChart = (<any>gvis).GraphChart;
const PieChart = (<any>gvis).PieChart;
const LineChart = (<any>gvis).LineChart;
const BarChart = (<any>gvis).BarChart;
// import { TableData } from './table-data';

@Component({
    selector: 'app-welcome-table',
    templateUrl: './welcome.component.html',
    styleUrls: ['./welcome.component.css'],
    providers: [WelcomeService]

})

export class WelcomeTableComponent implements OnInit, OnDestroy, AfterViewInit {
    // @ViewChild('container') container: any;
    @ViewChild('container1') container1: any;
    @ViewChild('container2') container2: any;
    @ViewChild('container3') container3: any;
    @ViewChild('container4') container4: any;
    iH = window.innerHeight - 80;
    iW = Math.min(window.innerWidth, 2000);
    h1 = this.iH * 0.15 + "px";
    h2 = this.iH * 0.15 + "px";
    md = this.iH * 0.025 + "px";
    w1 = this.iW * 0.166 + "px";
    w2 = this.iW * 0.166 + "px";
    public chart: any;
    public linechart: any;
    public loadPie: any;
    public lossPie: any;
    public barchart: any;
    private welcomes: Array<any>;
    private graph;
    private loadData: any[];
    private lossData: any[];
    private VoltageAData: any[];
    private VoltageBData: any[];
    private VoltageCData: any[];
    private LoadAData: any[];
    private LoadBData: any[];
    private LoadCData: any[];
    private LoadABData: any[];
    private LoadBCData: any[];
    private LoadCAData: any[];
    private PowerAData: any[];
    private PowerBData: any[];
    private PowerCData: any[];
    private VunbalData: any[];
    options1: any;
    options2: any;
    options3: any;
    options4: any;
    Nodeindex: String[];
    Edgeindex: String[];
    LineLengthindex: Number[];
    LineLengthindex_line: Number[];
    selectedWels: Welcome[] = [];
    welIDs: Number[] = [];
    startnode: string;
    endnode: string;
    subscription: Subscription;


    public constructor(
        private router: Router,
        private welcomeService: WelcomeService,
        private graphChartService: GraphChartService

    ) {

        this.options1 = {
            title: { text: 'Voltage Profile' },
            chart: {
                zoomType: 'x',
                height: 350,
                width: 500

            },
            xAxis: {
                categories: []
            },
            yAxis: {
                title: {
                    text: "voltage (p.u.)"
                }
            },
            series: [{
                name: 'Phase A',
                showInLegend: true,
                type: 'line',
                data: [],
                allowPointSelect: true
            }, {
                name: 'Phase B',
                showInLegend: true,
                type: 'line',
                data: [],
                allowPointSelect: true
            }, {
                name: 'Phase C',
                showInLegend: true,
                type: 'line',
                data: [],
                allowPointSelect: true
            }
            ]
        };
        this.options2 = {
            title: { text: 'Load Distribution' },
            chart: {
                zoomType: 'x',
                height: 350,
                width: 500

            },
            xAxis: {
                categories: []
            },
            yAxis: {
                title: {
                    text: "Load demand (kW)"
                }
            },
            series: [{
                name: 'Phase A',
                showInLegend: true,
                type: 'column',
                data: [],
                allowPointSelect: true
            }, {
                name: 'Phase B',
                showInLegend: true,
                type: 'column',
                data: [],
                allowPointSelect: true
            }, {
                name: 'Phase C',
                showInLegend: true,
                type: 'column',
                data: [],
                allowPointSelect: true
            }, {
                name: 'Phase AB',
                showInLegend: true,
                type: 'column',
                data: [],
                allowPointSelect: true
            }, {
                name: 'Phase BC',
                showInLegend: true,
                type: 'column',
                data: [],
                allowPointSelect: true
            }, {
                name: 'Phase CA',
                showInLegend: true,
                type: 'column',
                data: [],
                allowPointSelect: true
            }
            ]
        };
        this.options3 = {
            title: { text: 'Active Power Profile' },
            chart: {
                zoomType: 'x',
                height: 350,
                width: 500

            },
            xAxis: {
                categories: []
            },
            yAxis: {
                title: {
                    text: "active power (kW)"
                }
            },
            series: [{
                name: 'Phase A',
                showInLegend: true,
                type: 'line',
                data: [],
                allowPointSelect: true
            }, {
                name: 'Phase B',
                showInLegend: true,
                type: 'line',
                data: [],
                allowPointSelect: true
            }, {
                name: 'Phase C',
                showInLegend: true,
                type: 'line',
                data: [],
                allowPointSelect: true
            }
            ]
        };
        this.options4 = {
            title: { text: 'Voltage Unbalance' },
            chart: {
                zoomType: 'x',
                height: 350,
                width: 500

            },
            xAxis: {
                categories: []
            },
            yAxis: {
                title: {
                    text: "Degree of Unbalance (%)"
                }
            },
            series: [{
                name: 'voltage unbalance',
                showInLegend: true,
                type: 'column',
                data: [],
                allowPointSelect: true
            }
            ]
        };
        this.subscription = this.graphChartService.graphChart$.subscribe(
            (chart) => {
                console.log(this.chart);
                this.chart = chart;
                this.highLightAll();
                this.graphInitiate();
            }
        );
        this.graphChartService.announceSubscription();

    }

    public ngOnInit(): void {
        this.endnode = '150';
        this.welcomeService.getWelcome({ endnode: this.endnode })
            .then(data => {
                Object.keys(data[2])
                this.startnode = data[2];
                //   console.log(this.endnode);
            })
            .then(() => {
                if (this.chart !== undefined) {
                    this.highLightAll();
                    this.graphInitiate();
                }
            });

        //  console.log(this.startnode);
    }
    ngAfterViewInit() {



        this.loadPie = new PieChart({

            name: '分相有功负荷(kW)',
            render: {
                container: this.container1.nativeElement,
                assetsUrlBase: './assets/gvis',
                textStyle: {
                    fontStyle: 'italic',
                    fontWeight: 'bolder',
                    fontFamily: '"Courier New", Courier, monospace',
                    fontSize: 10,
                }
            },
            formatter: {
                tooltip: function (itemName, dataValue, percentage, color) {
                    return `<span style="color: ${color}">[${itemName}] : ${dataValue} (${percentage.toFixed(2)}%)</span>`;
                }
            }
        });
        this.lossPie = new PieChart({

            name: '分相系统损耗(kW)',
            render: {
                container: this.container2.nativeElement,
                assetsUrlBase: './assets/gvis',
                textStyle: {
                    fontStyle: 'italic',
                    fontWeight: 'bolder',
                    fontFamily: '"Courier New", Courier, monospace',
                    fontSize: 10,
                }
            },
            formatter: {
                tooltip: function (itemName, dataValue, percentage, color) {
                    return `<span style="color: ${color}">[${itemName}] : ${dataValue} (${percentage.toFixed(2)}%)</span>`;
                }
            }
        });
        this.welcomeService.getWelcome({ endnode: this.endnode })
            .then(data => {
                // console.log(data);
                this.loadData = [];
                this.lossData = [];
                this.VoltageAData = [];
                this.VoltageBData = [];
                this.VoltageCData = [];
                this.LoadAData = [];
                this.LoadBData = [];
                this.LoadCData = [];
                this.LoadABData = [];
                this.LoadBCData = [];
                this.LoadCAData = [];
                this.PowerAData = [];
                this.PowerBData = [];
                this.PowerCData = [];
                this.VunbalData = [];
                this.Nodeindex = [];
                this.Edgeindex = [];
                this.LineLengthindex = [];
                this.LineLengthindex_line = [];
                //Generator
                Object.keys(data[0]).forEach(k => {
                    this.loadData.push({ name: k, value: data[0][k].toFixed(2) });
                });
                // Load  
                Object.keys(data[1]).forEach(k => {
                    this.lossData.push({ name: k, value: data[1][k].toFixed(2) });
                });
                Object.keys(data[3]).forEach(k => {
                    this.VoltageAData.push(data[3][k]);
                });
                Object.keys(data[4]).forEach(k => {
                    this.VoltageBData.push(data[4][k]);
                });
                Object.keys(data[5]).forEach(k => {
                    this.VoltageCData.push(data[5][k]);
                });
                Object.keys(data[6]).forEach(k => {
                    this.LoadAData.push(data[6][k]);
                });
                Object.keys(data[7]).forEach(k => {
                    this.LoadBData.push(data[7][k]);
                });
                Object.keys(data[8]).forEach(k => {
                    this.LoadCData.push(data[8][k]);
                });
                Object.keys(data[9]).forEach(k => {
                    this.LoadABData.push(data[9][k]);
                });
                Object.keys(data[10]).forEach(k => {
                    this.LoadBCData.push(data[10][k]);
                });
                Object.keys(data[11]).forEach(k => {
                    this.LoadCAData.push(data[11][k]);
                });
                Object.keys(data[12]).forEach(k => {
                    this.PowerAData.push(data[12][k]);
                });
                Object.keys(data[13]).forEach(k => {
                    this.PowerBData.push(data[13][k]);
                });
                Object.keys(data[14]).forEach(k => {
                    this.PowerCData.push(data[14][k]);
                });
                Object.keys(data[15]).forEach(k => {
                    this.VunbalData.push(data[15][k] * 100);
                });
                Object.keys(data[16]).forEach(k => {
                    this.Nodeindex.push(data[16][k]);
                });
                Object.keys(data[17]).forEach(k => {
                    this.Edgeindex.push(data[17][k]);
                });
                Object.keys(data[18]).forEach(k => {
                    this.LineLengthindex.push(data[18][k]);
                });
                Object.keys(data[19]).forEach(k => {
                    this.LineLengthindex_line.push(data[19][k]);
                });

                // console.log(this.VoltageAData, this.VoltageBData, this.VoltageCData, this.LoadAData, this.LoadBData, this.LoadCData, this.Nodeindex, this.Edgeindex, this.VunbalData);


                this.loadPie
                    .reloadData(this.loadData)
                    .setColorByName('Phase A', '#FF0000')
                    .setColorByName('Phase B', '#0000FF')
                    .setColorByName('Phase C', '#00FF00')
                    .setColorByName('Phase AB', '#FF00FF')
                    .setColorByName('Phase BC', '#00FFFF')
                    .setColorByName('Phase CA', '#FFFF00')
                    .update();

                this.lossPie
                    .reloadData(this.lossData)
                    .setColorByName('Phase A', '#FF0000')
                    .setColorByName('Phase B', '#0000FF')
                    .setColorByName('Phase C', '#00FF00')
                    .update();


                // console.log(this.options1);
                let voltage = {
                    title: { text: 'Voltage Profile' },
                    chart: {
                        zoomType: 'x',
                        height: 350,
                        width: 500

                    },
                    xAxis: {
                        categories: [],
                        title: {
                            text: "Distance to source (mile)"
                        }
                    },
                    yAxis: {
                        title: {
                            text: "voltage(p.u.)"
                        }
                    },
                    series: [{
                        name: 'Phase A',
                        color: '#FF0000',
                        showInLegend: true,
                        type: 'line',
                        data: [],
                        allowPointSelect: true
                    }, {
                        name: 'Phase B',
                        color: '#0000FF',
                        showInLegend: true,
                        type: 'line',
                        data: [],
                        allowPointSelect: true
                    }, {
                        name: 'Phase C',
                        color: '#00FF00',
                        showInLegend: true,
                        type: 'line',
                        data: [],
                        allowPointSelect: true
                    }
                    ]
                };

                let load = {
                    title: { text: 'Load Distribution' },
                    chart: {
                        zoomType: 'x',
                        height: 350,
                        width: 500
                    },
                    xAxis: {
                        categories: [],
                        title: {
                            text: "Node name"
                        }
                    },

                    yAxis: {
                        title: {
                            text: "Load demand (kW)"
                        }
                    },
                    series: [{
                        name: 'Phase A',
                        color: '#FF0000',
                        showInLegend: true,
                        type: 'column',
                        data: [],
                        allowPointSelect: true
                    }, {
                        name: 'Phase B',
                        color: '#0000FF',
                        showInLegend: true,
                        type: 'column',
                        data: [],
                        allowPointSelect: true
                    }, {
                        name: 'Phase C',
                        color: '#00FF00',
                        showInLegend: true,
                        type: 'column',
                        data: [],
                        allowPointSelect: true
                    }, {
                        name: 'Phase AB',
                        color: '#FF00FF',
                        showInLegend: true,
                        type: 'column',
                        data: [],
                        allowPointSelect: true
                    }, {
                        name: 'Phase BC',
                        color: '#00FFFF',
                        showInLegend: true,
                        type: 'column',
                        data: [],
                        allowPointSelect: true
                    }, {
                        name: 'Phase CA',
                        color: '#FFFF00',
                        showInLegend: true,
                        type: 'column',
                        data: [],
                        allowPointSelect: true
                    }
                    ]
                };
                let power = {
                    title: { text: 'Active Power Profile' },
                    chart: {
                        zoomType: 'x',
                        height: 350,
                        width: 500

                    },
                    xAxis: {
                        categories: [],
                        title: {
                            text: "Distance to source (mile)"
                        }
                    },
                    yAxis: {
                        title: {
                            text: "active power (kW)"
                        }
                    },
                    series: [{
                        name: 'Phase A',
                        color: '#FF0000',
                        showInLegend: true,
                        type: 'line',
                        data: [],
                        allowPointSelect: true
                    }, {
                        name: 'Phase B',
                        color: '#0000FF',
                        showInLegend: true,
                        type: 'line',
                        data: [],
                        allowPointSelect: true
                    }, {
                        name: 'Phase C',
                        color: '#00FF00',
                        showInLegend: true,
                        type: 'line',
                        data: [],
                        allowPointSelect: true
                    }
                    ]
                };
                let volunbal = {
                    title: { text: 'Voltage Unbalance' },
                    chart: {
                        zoomType: 'x',
                        height: 350,
                        width: 500
                    },
                    xAxis: {
                        categories: [],
                        title: {
                            text: "Node name"
                        }
                    },

                    yAxis: {
                        title: {
                            text: "Degree of Unbalance (%)"
                        }
                    },
                    series: [{
                        name: 'voltage unbalance',
                        color: '#FF0000',
                        showInLegend: true,
                        type: 'column',
                        data: [],
                        allowPointSelect: true
                    }
                    ]
                };
                this.VoltageAData.forEach(k => {
                    voltage.series[0].data.push(k);
                });
                this.VoltageBData.forEach(k => {
                    voltage.series[1].data.push(k);
                });
                this.VoltageCData.forEach(k => {
                    voltage.series[2].data.push(k);
                });
                this.LoadAData.forEach(k => {
                    load.series[0].data.push(k);
                });
                this.LoadBData.forEach(k => {
                    load.series[1].data.push(k);
                });
                this.LoadCData.forEach(k => {
                    load.series[2].data.push(k);
                });
                this.LoadABData.forEach(k => {
                    load.series[3].data.push(k);
                });
                this.LoadBCData.forEach(k => {
                    load.series[4].data.push(k);
                });
                this.LoadCAData.forEach(k => {
                    load.series[5].data.push(k);
                });
                this.PowerAData.forEach(k => {
                    power.series[0].data.push(k);
                });
                this.PowerBData.forEach(k => {
                    power.series[1].data.push(k);
                });
                this.PowerCData.forEach(k => {
                    power.series[2].data.push(k);
                });
                this.VunbalData.forEach(k => {
                    volunbal.series[0].data.push(Number(k.toFixed(2)));
                });
                this.Nodeindex.forEach(k => {
                    load.xAxis.categories.push(k);
                    volunbal.xAxis.categories.push(k);
                });
                this.LineLengthindex.forEach(k => {
                    voltage.xAxis.categories.push(k.toFixed(3));
                });
                this.LineLengthindex_line.forEach(k => {
                    power.xAxis.categories.push(k.toFixed(3));
                });

                load.chart.height = this.iH * 0.4;
                load.chart.width = this.iW / 3;
                voltage.chart.height = this.iH / 2;
                voltage.chart.width = this.iW / 3;
                power.chart.height = this.iH / 2;
                power.chart.width = this.iW / 3;
                volunbal.chart.height = this.iH * 0.4;
                volunbal.chart.width = this.iW / 3;
                this.options1 = voltage;
                this.options2 = load;
                this.options3 = power;
                this.options4 = volunbal;

                this.highLightAll();

            });


    }
    ngOnDestroy() {
        // prevent memory leak when component destroyed
        // this.subscription.unsubscribe();
        // this.chart.off('onClick');
    }

    highlightselected(): void {
        // console.log( this.Edgeindex, this.Nodeindex);
        if (this.chart !== undefined) {
            this.chart.getLinks().forEach(l => {
                if (this.Edgeindex.includes(String(l.attrs.edge_name))) {
                    this.chart.unLowlightLink(l.exType, l.source.exType, l.source.exID, l.target.exType, l.target.exID);
                } else {
                    this.chart.lowlightLink(l.exType, l.source.exType, l.source.exID, l.target.exType, l.target.exID);
                }
            });
            this.chart.getNodes().forEach(n => {
                if (this.Nodeindex.includes(String(n.exID))) {
                    this.chart.unLowlightNode(n.exType, n.exID);
                } else {
                    this.chart.lowlightNode(n.exType, n.exID);
                }
            });
            this.chart.update();
        }

    }

    highLightAll(): void {
        if (this.chart !== undefined) {
            this.chart.getNodes().forEach(n => {
                this.chart.unLowlightNode(n.exType, n.exID);
            });
            this.chart.getLinks().forEach(l => {
                this.chart.unLowlightLink(l.exType, l.source.exType, l.source.exID, l.target.exType, l.target.exID);
            });
            this.chart.update();
        }

    }

    graphInitiate() {
        // console.log(this.chart);
        if (this.chart !== undefined) {
            this.chart.on('onClick', (item) => {
                if (item === undefined) {
                    return;
                }

                if (item.isNode) {
                    //   console.log(item);

                    this.endnode = String(item.exID);
                    //    console.log(this.endnode);
                    this.welcomeService.getWelcome({ endnode: this.endnode })
                        .then(data => {
                            // console.log(data);
                            this.loadData = [];
                            this.lossData = [];
                            this.VoltageAData = [];
                            this.VoltageBData = [];
                            this.VoltageCData = [];
                            this.LoadAData = [];
                            this.LoadBData = [];
                            this.LoadCData = [];
                            this.LoadABData = [];
                            this.LoadBCData = [];
                            this.LoadCAData = [];
                            this.PowerAData = [];
                            this.PowerBData = [];
                            this.PowerCData = [];
                            this.VunbalData = [];
                            this.Nodeindex = [];
                            this.Edgeindex = [];
                            this.LineLengthindex = [];
                            this.LineLengthindex_line = [];
                            //Generator
                            Object.keys(data[0]).forEach(k => {
                                this.loadData.push({ name: k, value: data[0][k].toFixed(2) });
                            });
                            // Load  
                            Object.keys(data[1]).forEach(k => {
                                this.lossData.push({ name: k, value: data[1][k].toFixed(2) });
                            });
                            Object.keys(data[3]).forEach(k => {
                                this.VoltageAData.push(data[3][k]);
                            });
                            Object.keys(data[4]).forEach(k => {
                                this.VoltageBData.push(data[4][k]);
                            });
                            Object.keys(data[5]).forEach(k => {
                                this.VoltageCData.push(data[5][k]);
                            });
                            Object.keys(data[6]).forEach(k => {
                                this.LoadAData.push(data[6][k]);
                            });
                            Object.keys(data[7]).forEach(k => {
                                this.LoadBData.push(data[7][k]);
                            });
                            Object.keys(data[8]).forEach(k => {
                                this.LoadCData.push(data[8][k]);
                            });
                            Object.keys(data[9]).forEach(k => {
                                this.LoadABData.push(data[9][k]);
                            });
                            Object.keys(data[10]).forEach(k => {
                                this.LoadBCData.push(data[10][k]);
                            });
                            Object.keys(data[11]).forEach(k => {
                                this.LoadCAData.push(data[11][k]);
                            });
                            Object.keys(data[12]).forEach(k => {
                                this.PowerAData.push(data[12][k]);
                            });
                            Object.keys(data[13]).forEach(k => {
                                this.PowerBData.push(data[13][k]);
                            });
                            Object.keys(data[14]).forEach(k => {
                                this.PowerCData.push(data[14][k]);
                            });
                            Object.keys(data[15]).forEach(k => {
                                this.VunbalData.push(data[15][k] * 100);
                            });
                            Object.keys(data[16]).forEach(k => {
                                this.Nodeindex.push(data[16][k]);
                            });
                            Object.keys(data[17]).forEach(k => {
                                this.Edgeindex.push(data[17][k]);
                            });
                            Object.keys(data[18]).forEach(k => {
                                this.LineLengthindex.push(data[18][k]);
                            });
                            Object.keys(data[19]).forEach(k => {
                                this.LineLengthindex_line.push(data[19][k]);
                            });

                            // console.log(this.VoltageAData, this.VoltageBData, this.VoltageCData, this.LoadAData, this.LoadBData, this.LoadCData, this.Nodeindex, this.Edgeindex, this.VunbalData);

                            this.loadPie
                                .reloadData(this.loadData)
                                .setColorByName('Phase A', '#FF0000')
                                .setColorByName('Phase B', '#0000FF')
                                .setColorByName('Phase C', '#00FF00')
                                .setColorByName('Phase AB', '#FF00FF')
                                .setColorByName('Phase BC', '#00FFFF')
                                .setColorByName('Phase CA', '#FFFF00')
                                .update();

                            this.lossPie
                                .reloadData(this.lossData)
                                .setColorByName('Phase A', '#FF0000')
                                .setColorByName('Phase B', '#0000FF')
                                .setColorByName('Phase C', '#00FF00')
                                .update();

                            // console.log(this.options1);
                            let voltage = {
                                title: { text: 'Voltage Profile' },
                                chart: {
                                    zoomType: 'x',
                                    height: 350,
                                    width: 500

                                },
                                xAxis: {
                                    categories: [],
                                    title: {
                                        text: "Distance to source (mile)"
                                    }
                                },
                                yAxis: {
                                    title: {
                                        text: "voltage(p.u.)"
                                    }
                                },
                                series: [{
                                    name: 'Phase A',
                                    color: '#FF0000',
                                    showInLegend: true,
                                    type: 'line',
                                    data: [],
                                    allowPointSelect: true
                                }, {
                                    name: 'Phase B',
                                    color: '#0000FF',
                                    showInLegend: true,
                                    type: 'line',
                                    data: [],
                                    allowPointSelect: true
                                }, {
                                    name: 'Phase C',
                                    color: '#00FF00',
                                    showInLegend: true,
                                    type: 'line',
                                    data: [],
                                    allowPointSelect: true
                                }
                                ]
                            };

                            let load = {
                                title: { text: 'Load Distribution' },
                                chart: {
                                    zoomType: 'x',
                                    height: 350,
                                    width: 500

                                },
                                xAxis: {
                                    categories: [],
                                    title: {
                                        text: "Node name"
                                    }
                                },

                                yAxis: {
                                    title: {
                                        text: "Load demand (kW)"
                                    }
                                },
                                series: [{
                                    name: 'Phase A',
                                    color: '#FF0000',
                                    showInLegend: true,
                                    type: 'column',
                                    data: [],
                                    allowPointSelect: true
                                }, {
                                    name: 'Phase B',
                                    color: '#0000FF',
                                    showInLegend: true,
                                    type: 'column',
                                    data: [],
                                    allowPointSelect: true
                                }, {
                                    name: 'Phase C',
                                    color: '#00FF00',
                                    showInLegend: true,
                                    type: 'column',
                                    data: [],
                                    allowPointSelect: true
                                }, {
                                    name: 'Phase AB',
                                    color: '#FF00FF',
                                    showInLegend: true,
                                    type: 'column',
                                    data: [],
                                    allowPointSelect: true
                                }, {
                                    name: 'Phase BC',
                                    color: '#00FFFF',
                                    showInLegend: true,
                                    type: 'column',
                                    data: [],
                                    allowPointSelect: true
                                }, {
                                    name: 'Phase CA',
                                    color: '#FFFF00',
                                    showInLegend: true,
                                    type: 'column',
                                    data: [],
                                    allowPointSelect: true
                                }
                                ]
                            };
                            let power = {
                                title: { text: 'Active Power Profile' },
                                chart: {
                                    zoomType: 'x',
                                    height: 350,
                                    width: 500

                                },
                                xAxis: {
                                    categories: [],
                                    title: {
                                        text: "Distance to source (mile)"
                                    }
                                },
                                yAxis: {
                                    title: {
                                        text: "active power (kW)"
                                    }
                                },
                                series: [{
                                    name: 'Phase A',
                                    color: '#FF0000',
                                    showInLegend: true,
                                    type: 'line',
                                    data: [],
                                    allowPointSelect: true
                                }, {
                                    name: 'Phase B',
                                    color: '#0000FF',
                                    showInLegend: true,
                                    type: 'line',
                                    data: [],
                                    allowPointSelect: true
                                }, {
                                    name: 'Phase C',
                                    color: '#00FF00',
                                    showInLegend: true,
                                    type: 'line',
                                    data: [],
                                    allowPointSelect: true
                                }
                                ]
                            };
                            let volunbal = {
                                title: { text: 'Voltage Unbalance' },
                                chart: {
                                    zoomType: 'x',
                                    height: 350,
                                    width: 500
                                },
                                xAxis: {
                                    categories: [],
                                    title: {
                                        text: "Node name"
                                    }
                                },

                                yAxis: {
                                    title: {
                                        text: "Degree of Unbalance (%)"
                                    }
                                },
                                series: [{
                                    name: 'voltage unbalance',
                                    color: '#FF0000',
                                    showInLegend: true,
                                    type: 'column',
                                    data: [],
                                    allowPointSelect: true
                                }
                                ]
                            };

                            this.VoltageAData.forEach(k => {
                                voltage.series[0].data.push(k);
                            });
                            this.VoltageBData.forEach(k => {
                                voltage.series[1].data.push(k);
                            });
                            this.VoltageCData.forEach(k => {
                                voltage.series[2].data.push(k);
                            });
                            this.LoadAData.forEach(k => {
                                load.series[0].data.push(k);
                            });
                            this.LoadBData.forEach(k => {
                                load.series[1].data.push(k);
                            });
                            this.LoadCData.forEach(k => {
                                load.series[2].data.push(k);
                            });
                            this.LoadABData.forEach(k => {
                                load.series[3].data.push(k);
                            });
                            this.LoadBCData.forEach(k => {
                                load.series[4].data.push(k);
                            });
                            this.LoadCAData.forEach(k => {
                                load.series[5].data.push(k);
                            });
                            this.PowerAData.forEach(k => {
                                power.series[0].data.push(k);
                            });
                            this.PowerBData.forEach(k => {
                                power.series[1].data.push(k);
                            });
                            this.PowerCData.forEach(k => {
                                power.series[2].data.push(k);
                            });
                            this.VunbalData.forEach(k => {
                                volunbal.series[0].data.push(Number(k.toFixed(2)));
                            });
                            this.Nodeindex.forEach(k => {
                                load.xAxis.categories.push(k);
                                volunbal.xAxis.categories.push(k);
                            });
                            this.LineLengthindex.forEach(k => {
                                voltage.xAxis.categories.push(k.toFixed(3));
                            });
                            this.LineLengthindex_line.forEach(k => {
                                power.xAxis.categories.push(k.toFixed(3));
                            });

                            load.chart.height = this.iH * 0.4;
                            load.chart.width = this.iW / 3;
                            voltage.chart.height = this.iH / 2;
                            voltage.chart.width = this.iW / 3;
                            power.chart.height = this.iH / 2;
                            power.chart.width = this.iW / 3;
                            volunbal.chart.height = this.iH * 0.4;
                            volunbal.chart.width = this.iW / 3;
                            this.options1 = voltage;
                            this.options2 = load;
                            this.options3 = power;
                            this.options4 = volunbal;
                            // console.log(this.endnode, this.startnode);
                            if (this.endnode === this.startnode) {
                                this.highLightAll();
                            } else {
                                this.highlightselected();
                            }

                        });



                } else if (item.isLink) {
                    //console.log(`Link ${item.id} is clicked.`);
                }
            })

            this.chart.update();
        }


    }



    staticLayout(): void {
        this.chart.getNodes().forEach(n => {
            n.x = n.attrs.pos_x;
            n.y = n.attrs.pos_y;
        });
        this.chart.runLayout('tree');
    }

}
