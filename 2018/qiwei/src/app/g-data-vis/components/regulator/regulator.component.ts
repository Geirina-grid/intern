import { Component, OnInit, OnDestroy, ViewChild, AfterViewInit} from '@angular/core';
import { Router } from '@angular/router';
import { Regulator } from '../../models/regulator';
import { RegulatorService } from '../../services/regulator.service';
import { GraphChartService } from '../../../app-shared/services/graph-chart.service';
import { Subscription } from 'rxjs/Subscription';
import { GraphChart } from 'gvis';
// import * as gvis from 'gvis';
// const GraphChart = (<any>gvis).GraphChart;
// import { TableData } from './table-data';

@Component({
    selector: 'app-regulator-table',
    templateUrl: './regulator.component.html',
    styleUrls: ['./regulator.component.css'],
    providers: [RegulatorService]

})
export class RegulatorTableComponent implements OnInit, OnDestroy, AfterViewInit {
    // @ViewChild('container') container: any
    public chart: any;
    public regulators: Array<any>;
    private graph;
    selectedRegs: Regulator[] = [];
    regIDs: string[] = [];
    startnode: string[]= [];
    subscription: Subscription;

    public constructor(
        private router: Router,
        private regulatorService: RegulatorService,
        private graphCharService: GraphChartService
    ) {
        this.subscription = this.graphCharService.graphChart$.subscribe(
            chart => {
                this.chart = chart;
                this.highLightAll();
            }
        );
        this.graphCharService.announceSubscription();
    }

    public ngOnInit(): void {
        this.regulatorService.getRegulators()
            .then(regulators => {
                this.regulators = regulators;
                this.regulators.forEach(reg => {
                    this.regIDs.push(reg.regulator_name);
                    this.startnode.push(reg.startnode);
                });
            })
            .then(() => {
                if (this.chart !== undefined) {
                    this.highLightAll();
                }
            });
    }
    ngAfterViewInit() {
        /*
        this.chart = new GraphChart({
            render: {
                container: this.container.nativeElement,
                assetsUrlBase: './assets/gvis'
            },
            style: {
                node: [
                    {
                        condition: (node, chart) => {
                            return true
                        },
                        style: {
                            lowlighted: {
                                opacity: 0.3
                            }
                        }
                    },
                    {
                        condition: (node, chart) => {
                          return node.attrs.PdA != 0 && node.attrs.PdB == 0 && node.attrs.PdC == 0;
                        },
                        style: {
                           fillColor: '#FF0000' //red A
                        }
                      },
                      {
                       condition: (node, chart) => {
                         return node.attrs.PdA == 0 && node.attrs.PdB != 0 && node.attrs.PdC == 0;
                       },
                       style: {
                          fillColor: '#0000FF' //blue B
                       }
                     },
                     {
                       condition: (node, chart) => {
                         return node.attrs.PdA == 0 && node.attrs.PdB == 0 && node.attrs.PdC != 0;
                       },
                       style: {
                          fillColor: '#00FF00' //green C
                       }
                     },
                     {
                       condition: (node, chart) => {
                         return node.attrs.PdA != 0 && node.attrs.PdB != 0 && node.attrs.PdC == 0;
                       },
                       style: {
                          fillColor: '#FF00FF' //AB
                       }
                     },
                     {
                       condition: (node, chart) => {
                         return node.attrs.PdA != 0 && node.attrs.PdB == 0 && node.attrs.PdC != 0;
                       },
                       style: {
                          fillColor: '#FFFF00' //AC
                       }
                     },
                     {
                       condition: (node, chart) => {
                         return node.attrs.PdA == 0 && node.attrs.PdB != 0 && node.attrs.PdC != 0;
                       },
                       style: {
                          fillColor: '00FFFF' //BC
                       }
                     },
                     {
                       condition: (node, chart) => {
                         return node.attrs.PdA != 0 && node.attrs.PdB != 0 && node.attrs.PdC != 0;
                       },
                       style: {
                          fillColor: '#404040' //ABC
                       }
                     }
                     
               ],
               link: [
                {
                    condition: (link, chart) => {
                         return link.attrs.Config == "switch";
                     },
                     style: {
                        fillColor: '#404040',
                        radius: 30
                     }
                },
                {
                    condition: (link, chart) => {
                         return link.attrs.Config == "RGABC";
                     },
                     style: {
                        fillColor: '#404040',
                        radius: 30
                     }
                },
                {
                    condition: (link, chart) => {
                         return link.attrs.Config == "XF";
                     },
                     style: {
                        fillColor: '#404040',
                        radius: 30
                     }
                },
                {
                    condition: (link, chart) => {
                         return link.attrs.Config == "1";
                     },
                     style: {
                        fillColor: '#404040',
                        radius: 30
                     }
                },
               
                {
                    condition: (link, chart) => {
                         return link.attrs.Config == "2";
                     },
                     style: {
                        fillColor: '#404040',
                        radius: 30
                     }
                },
                {
                    condition: (link, chart) => {
                         return link.attrs.Config == "3";
                     },
                     style: {
                        fillColor: '#404040',
                        radius: 30
                     }
                },
                {
                    condition: (link, chart) => {
                         return link.attrs.Config == "4";
                     },
                     style: {
                        fillColor: '#404040',
                        radius: 30
                     }
                },
                {
                    condition: (link, chart) => {
                         return link.attrs.Config == "5";
                     },
                     style: {
                        fillColor: '#404040',
                        radius: 30
                     }
                },
                {
                    condition: (link, chart) => {
                         return link.attrs.Config == "6";
                     },
                     style: {
                        fillColor: '#404040',
                        radius: 30
                     }
                },
                {
                    condition: (link, chart) => {
                         return link.attrs.Config == "7"; //AC
                     },
                     style: {
                        fillColor: '#FFFF00',
                        radius: 20
                     }
                },
                {
                    condition: (link, chart) => {
                         return link.attrs.Config == "RGAC"; //AC
                     },
                     style: {
                        fillColor: '#FFFF00',
                        radius: 20
                     }
                },
                {
                    condition: (link, chart) => {
                         return link.attrs.Config == "8"; //AB
                     },
                     style: {
                        fillColor: '#FF00FF',
                        radius: 20
                     }
                },
               
               
                {
                    condition: (link, chart) => {
                         return link.attrs.Config == "9"; //A red
                     },
                     style: {
                        fillColor: '#FF0000',
                        radius: 10
                     }
                },
                {
                    condition: (link, chart) => {
                         return link.attrs.Config == "RGA"; //A red
                     },
                     style: {
                        fillColor: '#FF0000',
                        radius: 10
                     }
                },
               
                {
                     condition: (link, chart) => {
                        return link.attrs.Config == "10"; //B blue
                    },
                    style: {
                        fillColor: '#0000FF',
                        radius: 10
                    }
                },
                {
                    condition: (link, chart) => {
                       return link.attrs.Config == "11"; //C green
                   },
                   style: {
                       fillColor: '#00FF00',
                       radius: 10
                   }
               },
               {
                condition: (link, chart) => {
                     return link.attrs.Config == "12";
                 },
                 style: {
                    fillColor: '#404040',
                    radius: 30
                 }
            },
            {
                condition: (link, chart) => {
                    return true
                },
                style: {
                    lowlighted: {
                        fillColor: '#CBC5C4'
                    }
                }
            }
           
                                
            ]
        }
        });
        (<any>window).gsql = this.chart;
        this.regulatorService.getGraph()
            .then(graph => {
                this.graph = graph;
                this.chart
                .addData(this.graph, 'gsql')
                .addRoot('bus_D', this.startnode[0])
                .runLayout('tree');
            // this.staticLayout();
            this.chart.update();
                this.highLightAll();
            });
        // this.chart.runStressTest(3000);
        */
    }
    ngOnDestroy() {
        this.subscription.unsubscribe();
    }
    staticLayout(): void {
        this.chart.getNodes().forEach(n => {
          n.x = n.attrs.pos_x;
          n.y = n.attrs.pos_y;
        });
        this.chart.runLayout('force');
      }

    onRowSelect(event) {
        let selectedID = [];
        let selectedNode = [];
        this.selectedRegs.forEach(g => {
            selectedID.push(g.regulator_name);
        });
        selectedID.push(event.data.regulator_name);
        // console.log(selectedID);
        this.chart.getNodes().forEach(n => {
            if (selectedID.includes(String(n.exID))) {
                this.chart.unLowlightNode(n.exType, n.exID);
                selectedNode.push(n);
            } else {
                this.chart.lowlightNode(n.exType, n.exID);
            }
        });
        this.chart.getLinks().forEach(l => {
            this.chart.lowlightLink(l.exType, l.source.exType, l.source.exID, l.target.exType, l.target.exID);
        });
        this.chart.scrollIntoView(selectedNode);
        this.chart.update();
    }
    onRowUnselect(event) {
        // console.log(event);
        let selectedID = [];
        let selectedNode = [];
        this.selectedRegs.forEach(g => {
            selectedID.push(g.regulator_name);
        });
        console.log(selectedID);
        let idx = selectedID.indexOf(event.data.regulator_name);
        // console.log(idx);
        if (idx != -1) {
            selectedID.splice(idx, 1);
        }
        // console.log(selectedID);
        if (selectedID.length === 0) {
            console.log('highlight all');
            this.highLightAll();
            this.chart.getNodes().forEach(n =>{    
                selectedNode.push(n);    
        });
        this.chart.scrollIntoView(selectedNode);
        } else {
            this.chart.getNodes().forEach(n => {
                if (selectedID.includes(String(n.exID))) {
                    this.chart.unLowlightNode(n.exType, n.exID);
                    selectedNode.push(n);
                } else {
                    this.chart.lowlightNode(n.exType, n.exID);
                }
            });
            this.chart.getLinks().forEach(l => {
                this.chart.lowlightLink(l.exType, l.source.exType, l.source.exID, l.target.exType, l.target.exID);
            });
            this.chart.scrollIntoView(selectedNode);  
            this.chart.update();
        }
    }

    highLightAll(): void {
        let selectedNode = [];
        // highlight all loads
        this.chart.getNodes().forEach(n => {
            if (this.regIDs.includes(String(n.exID))) {
                this.chart.unLowlightNode(n.exType, n.exID);
                selectedNode.push(n);
            } else {
                this.chart.lowlightNode(n.exType, n.exID);
            }
        });
        // lowlight all edges
        this.chart.getLinks().forEach(l => {
            this.chart.lowlightLink(l.exType, l.source.exType, l.source.exID, l.target.exType, l.target.exID);
        });
      //  this.chart.scrollIntoView(selectedNode);
        this.chart.update();
        console.log('highlight all loads');
    }



}
