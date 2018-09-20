import { Component, OnInit, ViewChild, AfterViewInit, OnChanges, SimpleChanges } from '@angular/core';
import { Router } from '@angular/router';
import { Summary } from '../../models/summary';
import { SummaryService } from '../../services/summary.service';

import * as gvis from 'gvis';
const GraphChart = (<any>gvis).GraphChart;
// import { TableData } from './table-data';

@Component({
    selector: 'app-summary-table',
    templateUrl: './summary.component.html',
    styleUrls: ['./summary.component.css'],
    providers: [SummaryService]

})
export class SummaryTableComponent implements OnInit, AfterViewInit {
    @ViewChild('container') container: any
    public chart: any;
    public summarys: Array<any>;
    private graph;
    selectedSumm: Summary[] = [];
    summIDs: any[] = [];

    public constructor(
        private router: Router,
        private summaryService: SummaryService
    ) {}

    public ngOnInit(): void {
        this.summaryService.getSummarys()
            .then(summarys => {
                this.summarys = summarys;
                this.summarys.forEach(summ => {
                    this.summIDs.push(summ.Sys_id);
                });
            });
    }
    ngAfterViewInit() {
        this.chart = new GraphChart({
            render: {
                container: this.container.nativeElement,
                assetsUrlBase: './assets/gvis'
            },
            style: {
                node: [
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
                          fillColor: '#808080' //ABC
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
                        fillColor: '##404040',
                        radius: 30
                     }
                },
                {
                    condition: (link, chart) => {
                         return link.attrs.Config == "6";
                     },
                     style: {
                        fillColor: '##404040',
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
        this.summaryService.getGraph()
            .then(graph => {
                this.graph = graph;
                this.chart
                .addData(this.graph, 'gsql')
                .addRoot('bus_D', '150')
                .runLayout('tree');
            // this.staticLayout();
            this.chart.update();
              
     //           this.highLightAll();
            });
        // this.chart.runStressTest(3000);
    }

    staticLayout(): void {
        this.chart.getNodes().forEach(n => {
          n.x = n.attrs.pos_x;
          n.y = n.attrs.pos_y;
        });
        this.chart.runLayout('force');
      }

    /*
    onRowSelect(event) {
        let selectedID = [];
        this.selectedSumm.forEach(g => {
            selectedID.push(g.Sys_id);
        });
        selectedID.push(event.data.Sys_id);
        // console.log(selectedID);
        this.chart.getLinks().forEach(l => {
            if (selectedID.includes(String(l.attrs.edge_name))) {
                this.chart.unLowlightLink(l.exType, l.source.exType, l.source.exID, l.target.exType, l.target.exID);               
            } else {
                this.chart.lowlightLink(l.exType, l.source.exType, l.source.exID, l.target.exType, l.target.exID);                
            }
        });
        this.chart.getNodes().forEach(n => {
            this.chart.lowlightNode(n.exType, n.exID)
          });
        this.chart.update();

    }
    onRowUnselect(event) {
        // console.log(event);
        let selectedID = [];
        this.selectedSumm.forEach(g => {
            selectedID.push(g.Sys_id);
        });
        console.log(selectedID);
        let idx = selectedID.indexOf(event.data.Sys_id);
        // console.log(idx);
        if (idx != -1) {
            selectedID.splice(idx, 1);
        }
        // console.log(selectedID);
        if (selectedID.length === 0) {
            console.log('highlight all');
            this.highLightAll();
        } else {
            this.chart.getLinks().forEach(l => {
                if (selectedID.includes(String(l.attrs.edge_name))) {
                    this.chart.unLowlightLink(l.exType, l.source.exType, l.source.exID, l.target.exType, l.target.exID);
                } else {
                    this.chart.lowlightLink(l.exType, l.source.exType, l.source.exID, l.target.exType, l.target.exID);
                }
            });
            this.chart.getNodes().forEach(n => {
                this.chart.lowlightNode(n.exType, n.exID);
            });
            this.chart.update();
        }
    }


    highLightAll(): void {
        // highlight all branch violations
        this.chart.getLinks().forEach(l => {
            if (this.summIDs.includes(String(l.attrs.edge_name))) {
                this.chart.unlowlightLink(l.exType, l.source.exType, l.source.exID, l.target.exType, l.target.exID);
            } else {
                this.chart.lowlightLink(l.exType, l.source.exType, l.source.exID, l.target.exType, l.target.exID);  
            }

        });


        // lowlight all nodes
        this.chart.getNodes().forEach(n => {
            this.chart.lowlightNode(n.exType, n.exID);
        });
        this.chart.update();
        console.log('highlight all branch violations');
    }


*/
}
