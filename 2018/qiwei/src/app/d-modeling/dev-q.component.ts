import { Component, OnInit, ViewChild, AfterViewInit, OnChanges, SimpleChanges } from '@angular/core';
import { Router } from '@angular/router';
import { dModle } from './modle';
import{ModleserService} from '../d-modeling/modleser.service';
//import { Summary } from '../g-data-vis/models/summary';
//import { SummaryService } from '../g-data-vis/services//summary.service';
import * as gvis from 'gvis';
const GraphChart = (<any>gvis).GraphChart;

@Component({
  selector: 'app-dev-q',
  templateUrl: './dev-q.component.html',
  styleUrls: ['./dev-q.component.css'],
  providers: [ModleserService]
})
export class DevQComponent implements OnInit {

  @ViewChild('container') container: any
 // public chart: any;
  //public summarys: Array<any>;
  //private graph;
 // selectedSumm: Summary[] = [];
  //summIDs: any[] = [];
  public chart: any;
  public modlearrys: Array<any>;
  private graph;
  selectedmodle: dModle[] = [];
  DmIDs: any[] = [];

  public constructor(
      private router: Router,
      private modleser: ModleserService
  ) {}

  public ngOnInit(): void {
      this.modleser.getDmodles()
          .then(modlearrys => {
              this.modlearrys = modlearrys;
              this.modlearrys.forEach(summ => {
                  this.DmIDs.push(summ.Sys_id);
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
      this.modleser.getGraph()
          .then(graph => {
              this.graph = graph;
              this.chart
              .addData(this.graph, 'gsql')
              .addRoot('bus_D', '150')
              .runLayout('circle');
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


}
