import { Component, OnInit, ViewChild, AfterViewInit, OnChanges, SimpleChanges, Input } from '@angular/core';
import { PreprocessService } from '../../services/preprocess.service';
// import { IdSelectorComponent } from './id-selector.component';
import { ActivatedRoute, Params } from '@angular/router';
import { Location } from '@angular/common';
import { SelectItem } from 'primeng/components/common/api';
import { Message } from 'primeng/components/common/api';
import { MessageService } from 'primeng/components/common/messageservice';
import { ReferenceService } from '../../services/reference.service';
import { WholeGraphService } from '../../../app-shared/services/whole-graph.service';


// import { DataSource } from '@angular/cdk';

import 'rxjs/add/operator/switchMap';
import 'rxjs/add/operator/toPromise';

import * as gvis from 'gvis';
const GraphChart = (<any>gvis).GraphChart;


@Component({
  selector: 'app-preprocess',
  templateUrl: './preprocess.component.html',
  styleUrls: ['./preprocess.component.css'],
  providers: [MessageService, ReferenceService]
})


export class PreprocessComponent implements OnInit, AfterViewInit {
  @Input() selectedmeter: any;
  @Input() startdate: Date;
  @Input() enddate: Date;

  @ViewChild('container') container: any
  @ViewChild('container1') container1: any;

  iH = window.innerHeight-175;
  iW = Math.min(window.innerWidth,2000);
  public chart: any;
  public lineChart: any;
  private line;
  private graph;
  messages: Message[] = [];
  meters: any[];
  result;
  stats: any = null;

  constructor(
    private route: ActivatedRoute,
    private location: Location,
    private wholeGraphService: WholeGraphService,
    private preprocessService: PreprocessService,
    private messageService: MessageService,
    private referenceService: ReferenceService) {
    this.meters = [];
        // this.wholeGraphService.getGraph()
    //   .then(graph => {
    //     let id: any = [];
    //     this.graph = graph;
    //     this.graph.forEach(element => {
    //       if (element.v_id) {
    //         id.push(Number(element.v_id));
    //       }
    //       // if (typeof element === 'object') {
    //       //   this.meters.push({ label: Number(element.v_id), value: element.v_id });
    //       // }
    //     });
    //     let sortedID = id.sort((n1, n2) => n1 - n2);
    //     sortedID.forEach(n => {
    //       this.meters.push({ label: 'test:'+ String(n), value: n });
    //     })
    //   });
  }
  ngOnInit(): void {
  }
  ngAfterViewInit(): void {
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
                          opacity: 0.4
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
                    fillColor: '#808080' //ABC
                 }
               }
               
         ],
         link: [
          {
              condition: (link, chart) => {
                  return true
              },
              style: {
                  lowlighted: {
                      fillColor: '#F7E2BD'
                  }
              }
          },
          {
              condition: (link, chart) => {
                   return link.attrs.Config == "1";
               },
               style: {
                  fillColor: '#808080',
                  radius: 30
               }
          },
         
          {
              condition: (link, chart) => {
                   return link.attrs.Config == "2";
               },
               style: {
                  fillColor: '#808080',
                  radius: 30
               }
          },
          {
              condition: (link, chart) => {
                   return link.attrs.Config == "3";
               },
               style: {
                  fillColor: '#808080',
                  radius: 30
               }
          },
          {
              condition: (link, chart) => {
                   return link.attrs.Config == "4";
               },
               style: {
                  fillColor: '#808080',
                  radius: 30
               }
          },
          {
              condition: (link, chart) => {
                   return link.attrs.Config == "5";
               },
               style: {
                  fillColor: '##808080',
                  radius: 30
               }
          },
          {
              condition: (link, chart) => {
                   return link.attrs.Config == "6";
               },
               style: {
                  fillColor: '##808080',
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
              fillColor: '#808080',
              radius: 30
           }
      },
     
                          
      ]
  }
  });

  (<any>window).gsql = this.chart;
  this.wholeGraphService.getGraph()
      .then(graph => {
          this.graph = graph;
          this.chart
              .addData(this.graph, 'gsql')
          this.staticLayout();
          this.chart.update();
          this.highLightAll();
      }
    )
    .catch(this.handleError);
    // this.chart.runStressTest(3000);
    this.initializeLineChart()
  }

  initializeLineChart(): void {
    this.lineChart = new gvis.LineChart({
      render: {
          container: this.container1.nativeElement,
          legend: {
              show: true
          },
          textStyle: {
              fontStyle: 'italic',
              fontWeight: 'bolder',
              fontFamily: '"Courier New", Courier, monospace',
              fontSize: 12,
          },
          dataZoom: {
            show: true,
            top: 455,
            bottom: 0,
            handleSize: '20%'
          },
          title: {
              text: 'Trained Data'
          }
      }
    });
    (<any>window).line = this.lineChart;
  }

  transDate(date: Date): string {
    const mon = date.getMonth() + 1; // getMonth() is zero-based
    const dd = date.getDate();
    const yyyy = date.getFullYear();
    const hh = date.getHours();
    const min = date.getMinutes();
    const sec = date.getSeconds();
    return [yyyy,
      (mon > 9 ? '-' : '-0') + mon,
      (dd > 9 ? '-' : '-0') + dd,
      (hh > 9 ? ' ' : ' 0') + hh,
      (min > 9 ? ':' : ':0') + min,
      (sec > 9 ? ':' : ':0') + sec
    ].join('');
  }

  preprocess(): void {
    // if (!this.selectedmeter || !this.startdate || !this.enddate) {
    //   this.showMsg(true, 'Error', 'Parameter misses!');
    // } else {
    //   // console.log(this.selectedmeter, this.transDate(this.startdate), this.transDate(this.enddate));
    //   this.result = null;
    //   this.result = 'pending';
    //   this.getReference();
    //   this.preprocessService.dataPreprocess(this.selectedmeter, this.transDate(this.startdate), this.transDate(this.enddate))
    //     .then(res => {
    //       console.log(res);
    //       let sum;
    //       if (res.error) {
    //         sum = 'Error';
    //       } else {
    //         sum = 'Success';
    //       }
    //       this.showMsg(res.error, sum, res.message);
    //       return this.result = res.normalizer;
    //     })
    //     .catch(this.handleError);
    // }

  }
  handleError(error: any) {
    this.showMsg(true, 'Error', 'Server Internal Error!');
  }
  showMsg(error: boolean, sum: string, msg: string) {
    this.messages = [];
    let sev;
    if (error) {
      sev = 'error';
    } else {
      sev = 'success'
    }
    this.messages.push({ severity: sev, summary: sum, detail: msg });
  }

  getReference() {
    this.referenceService.getRangeReference(this.selectedmeter, this.transDate(this.startdate), this.transDate(this.enddate))
    .then(res => {
      console.log(res);
      let oriData: any[] = [];
      oriData = res.result;
      // if(this.lineChart.getNodes())
      this.lineChart.reloadData(oriData).update();
    })
  }

  staticLayout(): void {
    this.chart.getNodes().forEach(n => {
      n.x = n.attrs.pos_x;
      n.y = n.attrs.pos_y;
    });
    this.chart.runLayout('static');
  }

  highLightAll(): void {

    this.chart.getNodes().forEach(n => {
        // if (this.selectedmeter == n.exID) {
        //     this.chart.lowlightNode(n.exType, n.exID);
        // } else {
            this.chart.unLowlightNode(n.exType, n.exID);
        // }
    });
    // lowlight all edges
    this.chart.getLinks().forEach(l => {
        this.chart.lowlightLink(l.exType, l.source.exType, l.source.exID, l.target.exType, l.target.exID);
    });
    this.chart.update();
}
}
