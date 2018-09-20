import { Component, OnInit, AfterViewInit, ViewChild, AfterViewChecked, OnDestroy } from '@angular/core';
import { Router } from '@angular/router';
import { WholeGraphService } from '../../../services/whole-graph.service';
import { GraphChart } from 'gvis';
import { GraphChartService } from '../../../services/graph-chart.service';
import * as gvis from 'gvis';
import { Subscription } from 'rxjs/Subscription';
import { LeafletmapComponent } from '../leafletmap/leafletmap.component';
import { MatTabChangeEvent } from '@angular/material';
import { GlobalIdService } from '../../../services/global-id.service';

// const GraphChart = (<any>gvis).GraphChart;
@Component({
  selector: 'app-graph-chart',
  templateUrl: './graph-chart.component.html',
  styleUrls: ['./graph-chart.component.css']
})
export class GraphChartComponent implements OnInit, AfterViewInit, AfterViewChecked, OnDestroy {
  @ViewChild('container') container: any;
  @ViewChild(LeafletmapComponent) map: LeafletmapComponent;
  public chart: any;
  subscriptions: Subscription[] = [];
  nodeLabels: any[] = [];
  linkLabels: any[] = [];
  curTab: number;
  selectedID: number;
  constructor(
    private router: Router,
    private wholeGraphService: WholeGraphService,
    private graphChartService: GraphChartService,
    private idService: GlobalIdService
  ) {
    this.subscriptions.push(this.wholeGraphService.nodelabelAnnounced$.subscribe(
      (labels) => {
        this.nodeLabels = labels;
        console.log(this.nodeLabels);
      }
    ));
    this.subscriptions.push(this.wholeGraphService.linklabelAnnounced$.subscribe(
      (labels) => {
        this.linkLabels = labels;
        console.log(this.linkLabels);
      }
    ));
    this.wholeGraphService.announceSubscription();

    // subscrie current selected ID
    this.subscriptions.push(this.idService.id$.subscribe(
      (id) => {
        this.selectedID = id;
        let selectedNode;
        console.log('new selected id arrived ' + id);
        if (this.chart !== undefined) {
          this.chart.getNodes().forEach(
            (n) => {
              if ( n.exID == id) {
                selectedNode = n;
                console.log(selectedNode);
              }
            }
          );
          this.chart
            .unselectAllElements()
            .setSelection([selectedNode])
            .scrollIntoView([selectedNode])
            .update();
        }
      }
    ))
  }

  ngOnInit() {
  }

  ngAfterViewInit() {
    this.chart = new GraphChart({
      advanced: {
        nodeToolTipSetting: (inNode, node, chart) => {
          const type = inNode.exType;

          const rowSize = 20;
          const keys = Object.keys(inNode.attrs);
          const size = keys.length;
          const n = Math.floor(size / rowSize) + 1;

          let toolTip = '<table>';

          if (n > 1) {
            toolTip += `<tr><th>${chart.language.getVertexString('built-in-translate', 'Vertex')}</th>
              <td>${chart.language.getVertexString(type, 'id', inNode.exID)}</td>
              <th>${chart.language.getVertexString('built-in-translate', 'Type')}</th>
              <td>${chart.language.getVertexString(type)}</td></tr>`;
          } else {
            toolTip += `<tr><th>${chart.language.getVertexString('built-in-translate', 'Vertex')}</th>
              <td>${chart.language.getVertexString(type, 'id', inNode.exID)}</td></tr>`;
            toolTip += `<tr><th>${chart.language.getVertexString('built-in-translate', 'Type')}</th>
              <td>${chart.language.getVertexString(type)}</td></tr>`;
          }

          for (let i = 0; i < size; i = i + n) {
            let row = '<tr>';

            for (let j = 0; j < n; ++j) {
              // console.log(i, size, n, j);

              const attr = keys[i + j];
              if (attr === undefined) {
                continue;
              } else {
                const value = JSON.stringify(chart.language.getVertexString(type, attr, inNode.attrs[attr]));
                row += `<th>${chart.language.getVertexString(type, attr)}</th><td>${value}</td>`;
              }
            }

            row += '</tr>';
            toolTip += row;
          }

          toolTip += '</table>';
          // return toolTip;
          return 'Node ID:' + inNode.exID;
        },
        linkToolTipSetting: (inNode, node, chart) => {
          // console.log(inNode);
          const type = inNode.exType;

          const rowSize = 40;
          const keys = Object.keys(inNode.attrs);
          const size = keys.length;
          const n = Math.floor(size / rowSize) + 1;

          let toolTip = '<table>';

          if (n > 1) {
            toolTip += `<tr><th>${chart.language.getEdgeString('built-in-translate', 'Edge')}</th>
            <td>${chart.language.getEdgeString(type)}</td><th>${chart.language.getVertexString(type, keys[0])}</th>
            <td>${JSON.stringify(chart.language.getVertexString(type, keys[0], inNode.attrs[keys[0]]))}</td</tr>`;
          } else {
            toolTip += `<tr><th>${chart.language.getEdgeString('built-in-translate', 'Edge')}</th>
              <td>${chart.language.getEdgeString(type)}</td></tr>`;
          }

          for (let i = 1; i < size; i = i + n) {
            let row = '<tr>';

            for (let j = 0; j < n; ++j) {
              // console.log(i, size, n, j);

              const attr = keys[i + j];
              if (attr === undefined) {
                continue;
              } else {
                const value = JSON.stringify(chart.language.getVertexString(type, attr, inNode.attrs[attr]));
                row += `<th>${chart.language.getVertexString(type, attr)}</th><td>${value}</td>`;
              }
            }

            row += '</tr>';
            toolTip += row;
          }

          toolTip += '</table>';
          // return toolTip;  //`${JSON.stringify(a)
          return 'Edge Name:' + inNode.attrs.edge_name;
        }
      },
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
              return link.attrs.Config == "Line1";
            },
            style: {
              fillColor: '#404040',
              radius: 30
            }
          },

          {
            condition: (link, chart) => {
              return link.attrs.Config == "Line2";
            },
            style: {
              fillColor: '#404040',
              radius: 30
            }
          },
          {
            condition: (link, chart) => {
              return link.attrs.Config == "Line3";
            },
            style: {
              fillColor: '#404040',
              radius: 30
            }
          },
          {
            condition: (link, chart) => {
              return link.attrs.Config == "Line4";
            },
            style: {
              fillColor: '#404040',
              radius: 30
            }
          },
          {
            condition: (link, chart) => {
              return link.attrs.Config == "Line5";
            },
            style: {
              fillColor: '#404040',
              radius: 30
            }
          },
          {
            condition: (link, chart) => {
              return link.attrs.Config == "Line6";
            },
            style: {
              fillColor: '#404040',
              radius: 30
            }
          },
          {
            condition: (link, chart) => {
              return link.attrs.Config == "Line7"; //AC
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
              return link.attrs.Config == "Line8"; //AB
            },
            style: {
              fillColor: '#FF00FF',
              radius: 20
            }
          },


          {
            condition: (link, chart) => {
              return link.attrs.Config == "Line9"; //A red
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
              return link.attrs.Config == "Line10"; //B blue
            },
            style: {
              fillColor: '#0000FF',
              radius: 10
            }
          },
          {
            condition: (link, chart) => {
              return link.attrs.Config == "Line11"; //C green
            },
            style: {
              fillColor: '#00FF00',
              radius: 10
            }
          },
          {
            condition: (link, chart) => {
              return link.attrs.Config == "Line12";
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
    this.wholeGraphService.getGraph()
      .then(graph => {
        this.chart
          .addData(graph, 'gsql');
        this.treeLayout();
        //this.staticLayout();
        this.chart.showNodeLabelsByType('bus_D', this.nodeLabels);
        this.chart.showLinkLabelsByType('foree_D', this.linkLabels);
        this.chart.update();
      }).then(() => {
        this.annouceGraphInitiation();
      });
    // this.graphChartService.announceGraphchart(this.chart);
  }

  ngAfterViewChecked() {
  }

  ngOnDestroy() {
    this.subscriptions.forEach(s => {
      s.unsubscribe();
    })
  }
  
  treeLayout(): void {
    // add root node
    console.log('tree');
    this.chart.addRoot('bus_D', '150')
      .runLayout('tree');
  }

  staticLayout(): void {
    this.chart.getNodes().forEach(n => {
      n.x = n.attrs.pos_x;
      n.y = n.attrs.pos_y;
    });
    this.chart.runLayout('tree');
  }
  annouceGraphInitiation() {
    console.log('annouce graph initiation');
    console.log(this.chart);
    this.graphChartService.announceInitiation(true);
  }
  tabChange(event: MatTabChangeEvent) {
    this.curTab = event.index;
    if (event.index === 1) {
      this.map.map.invalidateSize();
    }
  }
}
