import { Component, OnInit, ViewChild, AfterViewInit, OnChanges, SimpleChanges } from '@angular/core';
import { Router } from '@angular/router';
import { WholeGraphService } from '../../../app-shared/services/whole-graph.service';
import { ConfirmDialogModule, ConfirmationService } from 'primeng/primeng';
import { GraphManageService } from '../../../app-shared/services/graph-manage.service';
import { ClusteringService } from '../../services/clustering.service'
import * as gvis from 'gvis';
const GraphChart = (<any>gvis).GraphChart;

@Component({
  selector: 'app-all-tl',
  templateUrl: './all-tl.component.html',
  styleUrls: ['./all-tl.component.css'],
  providers: [ClusteringService]
})
export class AllTlComponent implements OnInit, AfterViewInit {
  // @ViewChild('topocontainer') container1: any;
  public chart: any; 
  private graph;
  iH=window.innerHeight-120;
  nclus: number;
  grid:any;
  meters: any;
  options1: any;
  options2: any;
  clusterinfo: any;
  userinfo: any;
  display: boolean=false;
  popoptions: any;
  constructor(
    private router: Router,
    private wholeGraphService: WholeGraphService,
    private graphManageService: GraphManageService,
    private confirmationService: ConfirmationService,
    private clusteringservice: ClusteringService,
  ) { 
    this.clusteringservice.plot_all(30).then(
      res=>{
        this.nclus=res.length;
        let i=0;
        for (i=0;i<res.length;i++){
          res[i].options.chart.height=(this.iH)/Math.ceil(res.length/6)-2
        }
        this.grid=res;
      }
    )
  }

  ngOnInit() {

  }

  ngAfterViewInit() {
    // this.chart = new GraphChart({
    //   advanced: {
    //     nodeToolTipSetting: (inNode, node, chart) => {
    //       const type = inNode.exType;

    //       const rowSize = 20;
    //       const keys = Object.keys(inNode.attrs);
    //       const size = keys.length;
    //       const n = Math.floor(size / rowSize) + 1;

    //       let toolTip = '<table>';

    //       if (n > 1) {
    //         toolTip += `<tr><th>${chart.language.getVertexString('built-in-translate', 'Vertex')}</th>
    //           <td>${chart.language.getVertexString(type, 'id', inNode.exID)}</td>
    //           <th>${chart.language.getVertexString('built-in-translate', 'Type')}</th>
    //           <td>${chart.language.getVertexString(type)}</td></tr>`;
    //       } else {
    //         toolTip += `<tr><th>${chart.language.getVertexString('built-in-translate', 'Vertex')}</th>
    //           <td>${chart.language.getVertexString(type, 'id', inNode.exID)}</td></tr>`;
    //         toolTip += `<tr><th>${chart.language.getVertexString('built-in-translate', 'Type')}</th>
    //           <td>${chart.language.getVertexString(type)}</td></tr>`;
    //       }

    //       for (let i = 0; i < size; i = i + n) {
    //         let row = '<tr>';

    //         for (let j = 0; j < n; ++j) {
    //           // console.log(i, size, n, j);

    //           const attr = keys[i + j];
    //           if (attr === undefined) {
    //             continue;
    //           } else {
    //             const value = JSON.stringify(chart.language.getVertexString(type, attr, inNode.attrs[attr]));
    //             row += `<th>${chart.language.getVertexString(type, attr)}</th><td>${value}</td>`;
    //           }
    //         }

    //         row += '</tr>';
    //         toolTip += row;
    //       }

    //       toolTip += '</table>';
    //       // return toolTip;
    //       return 'Node ID:' + inNode.exID;
    //     },
    //     linkToolTipSetting: (inNode, node, chart) => {
    //       // console.log(inNode);
    //       const type = inNode.exType;

    //       const rowSize = 40;
    //       const keys = Object.keys(inNode.attrs);
    //       const size = keys.length;
    //       const n = Math.floor(size / rowSize) + 1;

    //       let toolTip = '<table>';

    //       if (n > 1) {
    //         toolTip += `<tr><th>${chart.language.getEdgeString('built-in-translate', 'Edge')}</th>
    //         <td>${chart.language.getEdgeString(type)}</td><th>${chart.language.getVertexString(type, keys[0])}</th>
    //         <td>${JSON.stringify(chart.language.getVertexString(type, keys[0], inNode.attrs[keys[0]]))}</td</tr>`;
    //       } else {
    //         toolTip += `<tr><th>${chart.language.getEdgeString('built-in-translate', 'Edge')}</th>
    //           <td>${chart.language.getEdgeString(type)}</td></tr>`;
    //       }

    //       for (let i = 1; i < size; i = i + n) {
    //         let row = '<tr>';

    //         for (let j = 0; j < n; ++j) {
    //           // console.log(i, size, n, j);

    //           const attr = keys[i + j];
    //           if (attr === undefined) {
    //             continue;
    //           } else {
    //             const value = JSON.stringify(chart.language.getVertexString(type, attr, inNode.attrs[attr]));
    //             row += `<th>${chart.language.getVertexString(type, attr)}</th><td>${value}</td>`;
    //           }
    //         }

    //         row += '</tr>';
    //         toolTip += row;
    //       }

    //       toolTip += '</table>';
    //       // return toolTip;  //`${JSON.stringify(a)
    //       return 'Edge Name:' + inNode.attrs.edge_name;
    //     }
    //   },
    //   render: {
    //     container: this.container1.nativeElement,
    //     assetsUrlBase: './assets/gvis'
    //   },
    //   style: {
    //     node: [
    //       {
    //         condition: (node, chart) => {
    //           return node.attrs.PdA != 0 && node.attrs.PdB == 0 && node.attrs.PdC == 0;
    //         },
    //         style: {
    //           fillColor: '#FF0000' //red A
    //         }
    //       },
    //       {
    //         condition: (node, chart) => {
    //           return node.attrs.PdA == 0 && node.attrs.PdB != 0 && node.attrs.PdC == 0;
    //         },
    //         style: {
    //           fillColor: '#0000FF' //blue B
    //         }
    //       },
    //       {
    //         condition: (node, chart) => {
    //           return node.attrs.PdA == 0 && node.attrs.PdB == 0 && node.attrs.PdC != 0;
    //         },
    //         style: {
    //           fillColor: '#00FF00' //green C
    //         }
    //       },
    //       {
    //         condition: (node, chart) => {
    //           return node.attrs.PdA != 0 && node.attrs.PdB != 0 && node.attrs.PdC == 0;
    //         },
    //         style: {
    //           fillColor: '#FF00FF' //AB
    //         }
    //       },
    //       {
    //         condition: (node, chart) => {
    //           return node.attrs.PdA != 0 && node.attrs.PdB == 0 && node.attrs.PdC != 0;
    //         },
    //         style: {
    //           fillColor: '#FFFF00' //AC
    //         }
    //       },
    //       {
    //         condition: (node, chart) => {
    //           return node.attrs.PdA == 0 && node.attrs.PdB != 0 && node.attrs.PdC != 0;
    //         },
    //         style: {
    //           fillColor: '00FFFF' //BC
    //         }
    //       },
    //       {
    //         condition: (node, chart) => {
    //           return node.attrs.PdA != 0 && node.attrs.PdB != 0 && node.attrs.PdC != 0;
    //         },
    //         style: {
    //           fillColor: '#808080' //ABC
    //         }
    //       }

    //     ],
    //     link: [
    //       {
    //         condition: (link, chart) => {
    //           return link.attrs.Config == "1";
    //         },
    //         style: {
    //           fillColor: '#808080',
    //           radius: 30
    //         }
    //       },

    //       {
    //         condition: (link, chart) => {
    //           return link.attrs.Config == "2";
    //         },
    //         style: {
    //           fillColor: '#808080',
    //           radius: 30
    //         }
    //       },
    //       {
    //         condition: (link, chart) => {
    //           return link.attrs.Config == "3";
    //         },
    //         style: {
    //           fillColor: '#808080',
    //           radius: 30
    //         }
    //       },
    //       {
    //         condition: (link, chart) => {
    //           return link.attrs.Config == "4";
    //         },
    //         style: {
    //           fillColor: '#808080',
    //           radius: 30
    //         }
    //       },
    //       {
    //         condition: (link, chart) => {
    //           return link.attrs.Config == "5";
    //         },
    //         style: {
    //           fillColor: '#808080',
    //           radius: 30
    //         }
    //       },
    //       {
    //         condition: (link, chart) => {
    //           return link.attrs.Config == "6";
    //         },
    //         style: {
    //           fillColor: '#808080',
    //           radius: 30
    //         }
    //       },
    //       {
    //         condition: (link, chart) => {
    //           return link.attrs.Config == "7"; //AC
    //         },
    //         style: {
    //           fillColor: '#FFFF00',
    //           radius: 20
    //         }
    //       },
    //       {
    //         condition: (link, chart) => {
    //           return link.attrs.Config == "8"; //AB
    //         },
    //         style: {
    //           fillColor: '#FF00FF',
    //           radius: 20
    //         }
    //       },


    //       {
    //         condition: (link, chart) => {
    //           return link.attrs.Config == "9"; //A red
    //         },
    //         style: {
    //           fillColor: '#FF0000',
    //           radius: 10
    //         }
    //       },

    //       {
    //         condition: (link, chart) => {
    //           return link.attrs.Config == "10"; //B blue
    //         },
    //         style: {
    //           fillColor: '#0000FF',
    //           radius: 10
    //         }
    //       },
    //       {
    //         condition: (link, chart) => {
    //           return link.attrs.Config == "11"; //C green
    //         },
    //         style: {
    //           fillColor: '#00FF00',
    //           radius: 10
    //         }
    //       },
    //       {
    //         condition: (link, chart) => {
    //           return link.attrs.Config == "12";
    //         },
    //         style: {
    //           fillColor: '#808080',
    //           radius: 30
    //         }
    //       },
    //     ]
    //   }
    // });
    // this.wholeGraphService.getGraph()
    // .then(graph => {
    //   this.graph = graph;
    //   this.chart
    //     .addData(this.graph, 'gsql');
    //   // this.treeLayout();
    //   if (this.chart.getNodes()[0].attrs.pos_x === 0) {
    //     this.treeLayout();
    //   } else {
    //     this.staticLayout();
    //   }
    //   this.chart.update();
    // });
  }
  // treeLayout(): void {
  //   // add root node
  //   this.chart.addRoot('bus_D', '600')
  //     .runLayout('tree');
  // }
  // staticLayout(): void {
  //   this.chart.getNodes().forEach(n => {
  //     n.x = n.attrs.pos_x;
  //     n.y = n.attrs.pos_y;
  //   });
  //   this.chart.runLayout('static');
  // }
  runal(){  
    this.clusteringservice.run_algo(this.nclus)
    this.clusteringservice.plot_all(this.nclus).then(
      res=>{
        let i=0;
        for (i=0;i<res.length;i++){
          res[i].options.chart.height=(this.iH-95)/Math.ceil(res.length/6)-2
        }
        this.grid=res;
      }
    )
  }
  onSeriesSelect(e){
    // console.log(e);
    let opt={title:{text:""},
    chart:{zoomType:'x',
             panning: true,
             panKey: 'shift',
             width: 600,
             height: 300
            },
     yAxis:{title:{text:'MegaWatt'},
              min:0},
     xAxis:{categories:e.context.xAxis.categories},
     series:[{
             name:'Cluster Center',
             type:'line',
             data:e.context.yData,
             allowPointSelect:true,
             showInLegend:false,
             color:'black'}]};
    this.options1=opt;
    this.clusteringservice.plot_one_pie(parseInt(e.context.options.name)).then(
      res=>{
        // console.log(res);
        this.options2=res[0];
        this.userinfo=res[1];
        this.clusterinfo = res[2]
      }
    )
    this.display=true;
  }
}
