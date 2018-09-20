import { Component, OnInit, ViewChild, AfterViewInit, OnChanges, SimpleChanges } from '@angular/core';
import { Router } from '@angular/router';
import { WholeGraphService } from '../../../../app-shared/services/whole-graph.service';
import { GraphManageService } from '../../../../app-shared/services/graph-manage.service';
import { ConfirmDialogModule, ConfirmationService } from 'primeng/primeng';
import { SwitchService } from '../../../services/switch.service';
import { GraphSwitchFormComponent } from '../graph-swich-form/graph-switch-form.component';
import { Switch } from '../../../models/switch';
import { ChangeSW } from '../../../models/ChangeSW';

 import * as gvis from 'gvis';
 const GraphChart = (<any>gvis).GraphChart;

@Component({
  selector: 'app-switch-table',
  templateUrl: './switch.component.html',
  styleUrls: ['./switch.component.css'],
  providers: [SwitchService]
})
export class SwitchTableComponent implements OnInit, AfterViewInit {
  @ViewChild('container') container: any;
  @ViewChild('container1') container1: any;
  @ViewChild('nodeDiag') nodeForm: GraphSwitchFormComponent;
  @ViewChild('linkDiag') linkForm: GraphSwitchFormComponent;
  iH = window.innerHeight-330;
  iW = Math.min(window.innerWidth,2000);
  h1 = this.iH*0.5+"px";
  h2 = this.iH*0.5+"px";
  public chart: any;
  public chart1: any;
  public switches: Array<any>;
  public changedswes: Array<any>;
  private graph;
  private graph1;
  private linktoreverse: any[];
  addNodeDisplay = false;
  addLinkDisplay = false;
  nodeQuestions: any[];
  linkQuestions: any[];
  selectedSwitches: Switch[] = [];
  selectedConfig: ChangeSW[] = [];
  switchEdges: any[] = [];
  startnode: string[]=[];
  Edgeindex: any[];
  Edgelist: any[];
  Nodelist: any[];
  Direction: any[];
  switchdel: string;
  switchadd: string;
  addswitchF: string;
  addswitchT: string;
  selectedopen: string;
  selectedclose: string;

  constructor(
    private router: Router,
    private wholeGraphService: WholeGraphService,
    private graphManageService: GraphManageService,
    private confirmationService: ConfirmationService,
    private switchService: SwitchService
  ) {
    this.nodeQuestions = this.switchService.getNodeQuestions();

   }

  public ngOnInit(): void {
    this.switchService.getSwitches()
        .then(switches => {
            this.switches = switches;
            this.switches.forEach(sw => {
                this.switchEdges.push(sw.switch_name);
                this.startnode.push(sw.startnode);
            });
        });
    this.switchService.getchangedsw()
        .then(changedswes => {
            this.changedswes = changedswes;
        });
}
  ngAfterViewInit(): void {
    this.chart = new GraphChart({
      render: {
        container: this.container.nativeElement,
        assetsUrlBase: './assets/gvis'
      },
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
            return 'Node ID:'+inNode.exID;
        },
        linkToolTipSetting: (inNode, node, chart) => {
          console.log(inNode);
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
              console.log(i, size, n, j);

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
         return 'Edge Name:'+inNode.attrs.edge_name;
        }
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
                return node.attrs.flag === 3;
              },
              style: {
                fillColor: '#000000',
                radius: 50
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
                    fillColor: '#CBC5C4'
                }
            }
        }



        ]
       }
    });

    this.chart1 = new GraphChart({
        render: {
          container: this.container1.nativeElement,
          assetsUrlBase: './assets/gvis'
        },
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
              return 'Node ID:'+inNode.exID;
          },
          linkToolTipSetting: (inNode, node, chart) => {
            console.log(inNode);
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
                console.log(i, size, n, j);
  
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
           return 'Edge Name:'+inNode.attrs.edge_name;
          }
      },
        style: {
            node: [
              {
                condition: (node, chart) => {
                  return node.exType === 'switchnode_D';
                },
                style: {
                  display: 'rectangle',
                  aspectRatio: 0.5,
                  radius: 100
                }
              },
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
              //   {
              //     condition: (node, chart) => {
              //       return node.attrs.switch_status === 'open';
              //     },
              //     style: {
              //       fillColor: '#FF0000',
              //       radius: 50
              //     }
              //   },
              //   {
              //    condition: (node, chart) => {
              //     return node.attrs.switch_status === 'closed';
              //    },
              //    style: {
              //     fillColor: '#0000FF',
              //     radius: 50
              //   }
              // }
            ],
            
             link: [
               
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
           },
           nodeMenu: {
            enabled: true,
            buttons: [{
              label: 'Change Switch Status',
              callback: (node, chart) => {
               // this.showUpdateNode();
               this.updateSwitchNode();
              }
            }
            
            ]
          },
      
       });
   
    (<any>window).gsql = this.chart;
    (<any>window).gsql = this.chart1;
    (<any>window).node = this.nodeForm;
    (<any>window).edge = this.linkForm;
    this.switchService.getGraph()
      .then(graph => {
        this.graph = graph;
        console.log(this.graph);
        this.chart
          .addData(this.graph, 'gsql');
         if (this.chart.getNodes()[0].attrs.pos_x === 0) {
          this.chart.addRoot('bus_D', '150')
           .runLayout('tree');
         } else {
             this.chart.getNodes().forEach(n => {
                 n.x = n.attrs.pos_x;
                 n.y = n.attrs.pos_y;
              });
               this.chart.runLayout('static');
         }
        this.chart.update();
      });

  this.switchService.getGraphswitch()
  .then(graph1 => {
    this.graph1 = graph1;
    console.log(this.graph1);
    this.chart1
      .addData(this.graph1, 'gsql');
    this.chart1.getNodes().filter(n =>
        n.attrs.switch_status === 'closed').forEach(n =>
          n.styles.base.icon = 'Closed Breaker');
    this.chart1.getNodes().filter(n =>
        n.attrs.switch_status === 'open').forEach(n =>
          n.styles.base.icon = 'Open Breaker');
    // this.treeLayout();
    if (this.chart1.getNodes()[0].attrs.loc_x === 0) {
      this.chart1.runLayout('force');
    } else {
         this.chart1.getNodes().forEach(n => {
             n.x = n.attrs.loc_x;
             n.y = n.attrs.loc_y;
          });
           this.chart1.runLayout('static');
     }
    this.chart1.update();
   // this.highLightAll1();
   });
 }




  treeLayout(): void {
    // add root node
    console.log('tree');
    this.chart.addRoot('bus_D', '150')
      .runLayout('tree');
  }
  staticLayout(): void {
    console.log('static');
    this.chart.getNodes().forEach(n => {
      n.x = n.attrs.pos_x;
      n.y = n.attrs.pos_y;
    });
    this.chart.runLayout('static');
  }

  staticLayout1(): void {
    console.log('static');
    this.chart1.getNodes().forEach(n => {
      n.x = n.attrs.loc_x;
      n.y = n.attrs.loc_y;
    });
    this.chart1.runLayout('static');
  }

  excute(){

    let selectedNodeO = [];
    let selectedNodeC = [];
    this.chart1.getNodes().forEach(n => {
      if (this.selectedopen === n.exID) {
        selectedNodeO.push(n);
      } 
      if (this.selectedclose === n.exID) {
        selectedNodeC.push(n);
      } 
    });
    let formValue_o ={type: selectedNodeO[0].exType, id:selectedNodeO[0].exID, exId: selectedNodeO[0].exID, from_bus: selectedNodeO[0].attrs.from_bus, 
    to_bus:selectedNodeO[0].attrs.to_bus, switch_status:'open', flag:selectedNodeO[0].attrs.flag};        
    console.log(this.nodeForm.form.value);
   // const formValue = this.nodeForm.form.value;
    // create post payload
    let ntype = formValue_o.type;
    let nid = formValue_o.id;
    let payload_o = { vertices: {} };
    payload_o.vertices[ntype] = {};
    payload_o.vertices[ntype][nid] = {};
    Object.keys(formValue_o).forEach(k => {
      if (k !== 'type' && k !== 'id' && k!) {
        if (!isNaN(formValue_o[k])) {
          payload_o.vertices[ntype][nid][k] = { value: formValue_o[k] };
        } else {
          payload_o.vertices[ntype][nid][k] = { value: formValue_o[k] };
        }
      } else if (k === 'id') {
        let tmp = 'exId';
        payload_o.vertices[ntype][nid][tmp] = { value: formValue_o[k] };
      }
    });
    let formValue_c ={type: selectedNodeC[0].exType, id:selectedNodeC[0].exID, exId: selectedNodeC[0].exID, from_bus: selectedNodeC[0].attrs.from_bus, 
      to_bus:selectedNodeC[0].attrs.to_bus, switch_status:'closed', flag:selectedNodeC[0].attrs.flag};        
      console.log(this.nodeForm.form.value);
     // const formValue = this.nodeForm.form.value;
      // create post payload
      let ntype_c = formValue_c.type;
      let nid_c = formValue_c.id;
      let payload_c = { vertices: {} };
      payload_c.vertices[ntype_c] = {};
      payload_c.vertices[ntype_c][nid_c] = {};
      Object.keys(formValue_c).forEach(k => {
        if (k !== 'type' && k !== 'id' && k!) {
          if (!isNaN(formValue_c[k])) {
            payload_c.vertices[ntype_c][nid_c][k] = { value: formValue_c[k] };
          } else {
            payload_c.vertices[ntype_c][nid_c][k] = { value: formValue_c[k] };
          }
        } else if (k === 'id') {
          let tmp = 'exId';
          payload_c.vertices[ntype_c][nid_c][tmp] = { value: formValue_c[k] };
        }
      });
    console.log(payload_c);
    console.log(payload_o);
    // post graph and then refresh
    this.switchService.postGraph(JSON.stringify(payload_c))
    .then(res => {
      console.log(res);
      if (!res.error) {
        return this.switchService.postGraph(JSON.stringify(payload_o));
      } else {

      }
    })
    .then(res => {
      console.log(res);
      if (!res.error) {
        return this.switchService.getGraphswitch();
      } else {

      }
    })  
    .then(graph1 => {
      console.log('update graph1');
      this.saveConfirm();
  });
 
 
   this.switchService.getSwitches()   
      .then(switches => {
       this.switches = switches;
      });
  
    
  }

  saveConfirm() {
    this.confirmationService.confirm({
      message: 'Save changes?',
      accept: () => {
   
      this.saveconfiguration();
      }
    });


  }
 

   saveconfiguration(): void { 
    let selectedNode =[];
    let Nodetoclose =[];
    let closedswitch=[]; 
    let switchtoclose=[]; 
    let switchtoopen =[];
    let openswitch=[];  
      this.switchService.getSwitches()
      .then(switches => {
          this.switches = switches;
          console.log(this.switches);
          this.switches.forEach(sw => {
            if (sw.status==='open') {
              openswitch.push(sw.switch_name);
            }else if (sw.status==='closed') {
              closedswitch.push(sw.switch_name);
            }else {}
          });
         this.chart.getNodes().forEach(n => {
            if (openswitch.includes(String(n.exID))) {
              switchtoopen.push(n);
            } 
            if (closedswitch.includes(String(n.exID))) {
              selectedNode.push(n.exID);
            } 
          //console.log(n.exID);              
         });
          
         Object.keys(closedswitch).forEach(k=>{
          if (selectedNode.includes(closedswitch[k])) {         
           }else{
            switchtoclose.push(closedswitch[k]);        
          }         
        });
       this.chart1.getNodes().forEach(n => {
        if (switchtoclose.includes(String(n.exID))) {
           Nodetoclose.push(n);
        } 
        });
      
      console.log(switchtoopen);
      console.log(Nodetoclose);
    this.switchdel=switchtoopen[0].exID;
    this.switchadd=Nodetoclose[0].exID;
    this.addswitchF=Nodetoclose[0].attrs.from_bus;
    this.addswitchT=Nodetoclose[0].attrs.to_bus;
    this.switchService.switchDirect({switchdel:this.switchdel, switchadd:this.switchadd, addswitchF:this.addswitchF, addswitchT:this.addswitchT })
    .then(data => {
        console.log(data); 
      this.Direction=[];
      this.Direction=data[0];    
       console.log(this.Direction);
      this.Edgeindex = [];
      this.Nodelist = [];
      this.Edgelist = [];
       Object.keys(data[1]).forEach(k => {
           this.Edgeindex.push(data[1][k]);
       });
       Object.keys(data[2]).forEach(k => {
        this.Edgelist.push(data[2][k]);
      });
       Object.keys(data[3]).forEach(k => {
        this.Nodelist.push(data[3][k]);
      });
       console.log(this.Edgeindex);
       console.log(this.Edgelist);
       console.log(this.Nodelist);
       if (!data.error) {
        return this.addswitch();
      }
      
      }); 
    });     
  }
  





  updategraph(): void {
    this.switchService.getGraph() 
    .then(graph => {
      console.log('update graph');
      this.chart = new GraphChart({
        render: {
          container: this.container.nativeElement,
          assetsUrlBase: './assets/gvis'
        },
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
              return 'Node ID:'+inNode.exID;
          },
          linkToolTipSetting: (inNode, node, chart) => {
            console.log(inNode);
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
                console.log(i, size, n, j);
  
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
           return 'Edge Name:'+inNode.attrs.edge_name;
          }
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
                  return node.attrs.flag === 3;
                },
                style: {
                  fillColor: '#000000',
                  radius: 50
                }
              },
              {
                condition: (node, chart) => {
                  return this.Nodelist.includes(node.exID);                },
                style: {
                  fillColor: '#FFFF00',
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
                      fillColor: '#CBC5C4'
                  }
              }
          },
          {
            condition: (link, chart) => {
                return this.Edgelist.includes(link.attrs.edge_name);
            },
            style: {
                  fillColor: '#FFFF00'
            }
        }
  
          ]
        }
      });
     this.graph = graph;
      console.log(this.graph);
     this.chart
       .reloadData(this.graph, 'gsql')
       .addRoot('bus_D', '150')
       .runLayout('tree')

       .update();
       let payload: { [k: string]: any } = {};
       this.chart.getNodes().forEach(n => {
         const key = n.exID;
         payload[key] = { pos_x: { value: n.x }, pos_y: { value: n.y } };
       });
       this.graphManageService.postGraph(JSON.stringify({ vertices: { bus_D: payload } }))
         .then(res => {
          console.log(res);
          if (!res.error) {
            return this.switchService.getchangedsw();
          }
        })
         .then(changedswes => {
             this.changedswes = changedswes;
             console.log(this.changedswes);
         });
  });

  //   $scope.gridOptions.onRegisterApi = function (gridApi) {
  //   $scope.gridApi = gridApi;
  // }; 
  // $scope.unSelectAll = function(){
  //   $scope.gridApi.selection.clearSelectedRows();
  // }

    this.switchService.getGraphswitch()
    .then(graph1 => {
      console.log('update graph1');
      this.chart1 = new GraphChart({
        render: {
          container: this.container1.nativeElement,
          assetsUrlBase: './assets/gvis'
        },
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
              return 'Node ID:'+inNode.exID;
          },
          linkToolTipSetting: (inNode, node, chart) => {
            console.log(inNode);
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
                console.log(i, size, n, j);
  
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
           return 'Edge Name:'+inNode.attrs.edge_name;
          }
      },
        style: {
            node: [
              {
                condition: (node, chart) => {
                  return node.exType === 'switchnode_D';
                },
                style: {
                  display: 'rectangle',
                  aspectRatio: 0.5,
                  radius: 100
                }
              },
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
                    return this.Nodelist.includes(node.exID);
                  },
                  style: {
                    fillColor: '#FFFF00',
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
                        fillColor: '#CBC5C4'
                    }
                }
            },
            {
              condition: (link, chart) => {
                  return this.Edgelist.includes(link.attrs.edge_name);
              },
              style: {
                    fillColor: '#FFFF00'
              }
          },
          {
            condition: (link, chart) => {
                return this.Nodelist.includes(link.source.exID)&&link.source.attrs.flag===3;
            },
            style: {
                  fillColor: '#FFFF00'
            }
          },
           {
          condition: (link, chart) => {
              return this.Nodelist.includes(link.target.exID)&&link.target.attrs.flag===3;
          },
          style: {
                fillColor: '#FFFF00'
           }
          }
             ]
           },
           nodeMenu: {
            enabled: true,
            buttons: [{
              label: 'Change Switch Status',
              callback: (node, chart) => {
               // this.showUpdateNode();
               this.updateSwitchNode();
              }
            }
            
            ]
          },
      
       });
      this.graph1 = graph1;
      console.log(this.graph1);
      this.chart1
        .reloadData(this.graph1, 'gsql');
      this.chart1.getNodes().filter(n =>
          n.attrs.switch_status === 'closed').forEach(n =>
            n.styles.base.icon = 'Closed Breaker');
      this.chart1.getNodes().filter(n =>
          n.attrs.switch_status === 'open').forEach(n =>
            n.styles.base.icon = 'Open Breaker');
      // this.treeLayout();
    if (this.chart1.getNodes()[0].attrs.loc_x === 0) {
      this.chart1.runLayout('force');
    } else {
         this.chart1.getNodes().forEach(n => {
             n.x = n.attrs.loc_x;
             n.y = n.attrs.loc_y;
          });
           this.chart1.runLayout('static');
     }
      this.chart1.update();
  });
  }

 config(){
  this.switchService.getchangedsw()
  .then(changedswes => {
      this.changedswes = changedswes;
      console.log(this.changedswes);
  });
 }

 saveLayout(): void {
  let payload_D: { [k: string]: any } = {};
  let payload_S: { [k: string]: any } = {};
  this.chart1.getNodes().forEach(n => {
    if (n.exType === 'bus_D'){
    const key_D = n.exID;
    payload_D[key_D] = { loc_x: { value: n.x }, loc_y: { value: n.y } };
    }else if (n.exType === 'switchnode_D'){
    const key_S = n.exID;
    payload_S[key_S] = { loc_x: { value: n.x }, loc_y: { value: n.y } };
    }
  });

  this.switchService.postGraph(JSON.stringify({ vertices: { switchnode_D: payload_S } }));
  this.switchService.postGraph(JSON.stringify({ vertices: { bus_D: payload_D } }))
    .then(responce => console.log(responce));
}

  showUpdateNode() {
    let selectedNodes = this.chart1.getSelection().nodes;
    if (selectedNodes[0]) {
      Object.keys(this.nodeForm.form.value).forEach(k => {
        if (k === 'type') {
          this.nodeForm.form.patchValue({
            type: selectedNodes[0].exType
          });
        } else if (k === 'id') {
          this.nodeForm.form.patchValue({
            id: selectedNodes[0].exID
          });
        } else {
          let kvPair = {};
          kvPair[k] = selectedNodes[0].attrs[k];
          this.nodeForm.form.patchValue(kvPair);
        }
      });
      this.addNodeDisplay = true;
    } else {
      alert('Select a Node first!');
    }
  }

  
  
  updateSwitchNode() {
    let selectedNodes = this.chart1.getSelection().nodes;
    if (selectedNodes[0].attrs.switch_status==='open') {
    let formValue ={type: selectedNodes[0].exType, id:selectedNodes[0].exID, exId: selectedNodes[0].exID, from_bus: selectedNodes[0].attrs.from_bus, 
    to_bus:selectedNodes[0].attrs.to_bus, switch_status:'closed', flag:selectedNodes[0].attrs.flag};        
    console.log(this.nodeForm.form.value);
   // const formValue = this.nodeForm.form.value;
    // create post payload
    let ntype = formValue.type;
    let nid = formValue.id;
    let payload = { vertices: {} };
    payload.vertices[ntype] = {};
    payload.vertices[ntype][nid] = {};
    Object.keys(formValue).forEach(k => {
      if (k !== 'type' && k !== 'id' && k!) {
        if (!isNaN(formValue[k])) {
          payload.vertices[ntype][nid][k] = { value: formValue[k] };
        } else {
          payload.vertices[ntype][nid][k] = { value: formValue[k] };
        }
      } else if (k === 'id') {
        let tmp = 'exId';
        payload.vertices[ntype][nid][tmp] = { value: formValue[k] };
      }
    });
    console.log(payload);
    // post graph and then refresh
    this.switchService.postGraph(JSON.stringify(payload))
    .then(res => {
      console.log(res);
      if (!res.error) {
        return this.switchService.getGraphswitch();
      } else {

      }
    })  
    .then(graph1 => {
      console.log('update graph1');
      this.graph1 = graph1;
      console.log(this.graph1);
      this.chart1
        .reloadData(this.graph1, 'gsql');
      this.chart1.getNodes().filter(n =>
          n.attrs.switch_status === 'closed').forEach(n =>
            n.styles.base.icon = 'Closed Breaker');
      this.chart1.getNodes().filter(n =>
          n.attrs.switch_status === 'open').forEach(n =>
            n.styles.base.icon = 'Open Breaker');
      this.chart1.update();
  });
 
  this.switchService.postGraph(JSON.stringify(payload))
  .then(res => {
    console.log(res);
    if (!res.error) {
      return this.switchService.getSwitches();
    } else {

    }
  })  
.then(switches => {
    this.switches = switches;
});
  }else if(selectedNodes[0].attrs.switch_status==='closed'){
    let formValue ={type: selectedNodes[0].exType, id:selectedNodes[0].exID, exId: selectedNodes[0].exID, from_bus: selectedNodes[0].attrs.from_bus, 
      to_bus:selectedNodes[0].attrs.to_bus, switch_status:'open', flag:selectedNodes[0].attrs.flag};        
      console.log(this.nodeForm.form.value);
     // const formValue = this.nodeForm.form.value;
      // create post payload
      let ntype = formValue.type;
      let nid = formValue.id;
      let payload = { vertices: {} };
      payload.vertices[ntype] = {};
      payload.vertices[ntype][nid] = {};
      Object.keys(formValue).forEach(k => {
        if (k !== 'type' && k !== 'id' && k!) {
          if (!isNaN(formValue[k])) {
            payload.vertices[ntype][nid][k] = { value: formValue[k] };
          } else {
            payload.vertices[ntype][nid][k] = { value: formValue[k] };
          }
        } else if (k === 'id') {
          let tmp = 'exId';
          payload.vertices[ntype][nid][tmp] = { value: formValue[k] };
        }
      });
      console.log(payload);
      // post graph and then refresh
      this.switchService.postGraph(JSON.stringify(payload))
      .then(res => {
        console.log(res);
        if (!res.error) {
          return this.switchService.getGraphswitch();
        } else {
  
        }
      })  
      .then(graph1 => {
        console.log('update graph1');
        this.graph1 = graph1;
        console.log(this.graph1);
        this.chart1
          .reloadData(this.graph1, 'gsql');
        this.chart1.getNodes().filter(n =>
            n.attrs.switch_status === 'closed').forEach(n =>
              n.styles.base.icon = 'Closed Breaker');
        this.chart1.getNodes().filter(n =>
            n.attrs.switch_status === 'open').forEach(n =>
              n.styles.base.icon = 'Open Breaker');
        this.chart1.update();
    });
   
    this.switchService.postGraph(JSON.stringify(payload))
    .then(res => {
      console.log(res);
      if (!res.error) {
        return this.switchService.getSwitches();
      } else {
  
      }
    })  
  .then(switches => {
      this.switches = switches;
  });
  }
    // close adding node dialogue
    this.addNodeDisplay = false;
  }

addswitch() {

  let selectedNode =[];
  let Nodetoclose =[];
  let closedswitch=[];
  let switchtoclose=[];
    this.switchService.getSwitches()
    .then(switches => {
        this.switches = switches;
        console.log(this.switches);
        this.switches.forEach(sw => {
          if (sw.status==='closed') {
            closedswitch.push(sw.switch_name);
          }
        });
        this.chart.getNodes().forEach(n => {
          if (closedswitch.includes(String(n.exID))) {
            selectedNode.push(n.exID);
          } 
       });
       Object.keys(closedswitch).forEach(k=>{
        if (selectedNode.includes(closedswitch[k])) {         
         }else{
          switchtoclose.push(closedswitch[k]);        
        }         
      });
     this.chart1.getNodes().forEach(n => {
      if (switchtoclose.includes(String(n.exID))) {
         Nodetoclose.push(n);
      } 
      });
       console.log(closedswitch);     
       console.log(selectedNode);
       console.log(switchtoclose);     
       console.log(Nodetoclose);

       // create post payload
       if (Nodetoclose[0]) {
       Nodetoclose.forEach(n=>{
        let formValue ={type: 'bus_D', id:n.exID, exId: n.attrs.exId, flag:n.attrs.flag, Vbase:2401.8};        
        let ntype = formValue.type;
        let nid = formValue.id;
        let payload = { vertices: {} };
        payload.vertices[ntype] = {};
        payload.vertices[ntype][nid] = {};
        Object.keys(formValue).forEach(k => {
          if (k !== 'type' && k !== 'id' && k!) {
            if (!isNaN(formValue[k])) {
              payload.vertices[ntype][nid][k] = { value: Number(formValue[k]) };
            } else {
              payload.vertices[ntype][nid][k] = { value: formValue[k] };
            }
          } else if (k === 'id') {
            let tmp = 'exId';
            payload.vertices[ntype][nid][tmp] = { value: formValue[k] };
          }
        });
        console.log(payload);
        if (this.Direction.includes('positive')) {
        let formValue_F ={stype:'bus_D', sid:n.attrs.from_bus, etype:'foree_D', ttype:'bus_D', tid:n.exID, edge_name:n.attrs.from_bus+n.exID, flag:0, Nt:0, 
        connection_T:'None', Raa:0, Rab:0, Rac:0, Rba:0, Rbb:0, Rbc:0, Rca:0, Rcb:0, Rcc:0, Xaa:0, Xab:0, Xac:0, Xba:0, Xbb:0, Xbc:0, Xca:0, Xcb:0, Xcc:0, 
        Gaa:0, Gab:0, Gac:0, Gba:0, Gbb:0, Gbc:0, Gca:0, Gcb:0, Gcc:0, Baa:0, Bab:0, Bac:0, Bba:0, Bbb:0, Bbc:0, Bca:0, Bcb:0, Bcc:0, linelength:0, Config:n.attrs.Config, 
        PdA_dis:0, QdA_dis:0, PdB_dis:0, QdB_dis:0, PdC_dis:0, QdC_dis:0, Load_Model_dis:'None', IAmag_cal:0, IAang_cal:0, IBmag_cal:0, IBang_cal:0, ICmag_cal:0, ICang_cal:0};  
        let payload_F = { edges: {} }
        payload_F.edges[formValue_F.stype] = {};
        payload_F.edges[formValue_F.stype][formValue_F.sid] = {};
        payload_F.edges[formValue_F.stype][formValue_F.sid][formValue_F.etype] = {};
        payload_F.edges[formValue_F.stype][formValue_F.sid][formValue_F.etype][formValue_F.ttype] = {};
        payload_F.edges[formValue_F.stype][formValue_F.sid][formValue_F.etype][formValue_F.ttype][formValue_F.tid] = {};
        Object.keys(formValue_F).forEach(k => {
          if (k !== 'stype' && k !== 'sid' && k !== 'ttype' && k !== 'tid' && k !== 'etype') {
            if (isNaN(formValue_F[k])) {
              payload_F.edges[formValue_F.stype][formValue_F.sid][formValue_F.etype][formValue_F.ttype][formValue_F.tid][k] = {
                value: formValue_F[k]
              };
            } else {
              payload_F.edges[formValue_F.stype][formValue_F.sid][formValue_F.etype][formValue_F.ttype][formValue_F.tid][k] = {
                value: Number(formValue_F[k])
              };
            }   
          }
        });
        console.log(payload_F);  
        let formValue_T ={stype: 'bus_D', sid:n.exID, etype:'foree_D',ttype:'bus_D',tid:n.attrs.to_bus, edge_name:n.attrs.from_bus+n.exID, flag:0, Nt:0, 
        connection_T:'None', Raa:0, Rab:0, Rac:0, Rba:0, Rbb:0, Rbc:0, Rca:0, Rcb:0, Rcc:0, Xaa:0, Xab:0, Xac:0, Xba:0, Xbb:0, Xbc:0, Xca:0, Xcb:0, Xcc:0, 
        Gaa:0, Gab:0, Gac:0, Gba:0, Gbb:0, Gbc:0, Gca:0, Gcb:0, Gcc:0, Baa:0, Bab:0, Bac:0, Bba:0, Bbb:0, Bbc:0, Bca:0, Bcb:0, Bcc:0, linelength:0, Config:n.attrs.Config, 
        PdA_dis:0, QdA_dis:0, PdB_dis:0, QdB_dis:0, PdC_dis:0, QdC_dis:0, Load_Model_dis:'None', IAmag_cal:0, IAang_cal:0, IBmag_cal:0, IBang_cal:0, ICmag_cal:0, ICang_cal:0};        
        let payload_T = { edges: {} }
        payload_T.edges[formValue_T.stype] = {};
        payload_T.edges[formValue_T.stype][formValue_T.sid] = {};
        payload_T.edges[formValue_T.stype][formValue_T.sid][formValue_T.etype] = {};
        payload_T.edges[formValue_T.stype][formValue_T.sid][formValue_T.etype][formValue_T.ttype] = {};
        payload_T.edges[formValue_T.stype][formValue_T.sid][formValue_T.etype][formValue_T.ttype][formValue_T.tid] = {};
        Object.keys(formValue_T).forEach(k => {
          if (k !== 'stype' && k !== 'sid' && k !== 'ttype' && k !== 'tid' && k !== 'etype') {
            if (isNaN(formValue_T[k])) {
              payload_T.edges[formValue_T.stype][formValue_T.sid][formValue_T.etype][formValue_T.ttype][formValue_T.tid][k] = {
                value: formValue_T[k]
              };
            } else {
              payload_T.edges[formValue_T.stype][formValue_T.sid][formValue_T.etype][formValue_T.ttype][formValue_T.tid][k] = {
                value: Number(formValue_T[k])
              };
            }
    
          }
        });
        console.log(payload_T);
        let formValue_FB ={stype: 'bus_D', sid:n.exID, etype:'backe_D',ttype:'bus_D',tid:n.attrs.from_bus, edge_name:n.attrs.from_bus+n.exID, flag:0, Nt:0, 
        connection_T:'None', Raa:0, Rab:0, Rac:0, Rba:0, Rbb:0, Rbc:0, Rca:0, Rcb:0, Rcc:0, Xaa:0, Xab:0, Xac:0, Xba:0, Xbb:0, Xbc:0, Xca:0, Xcb:0, Xcc:0, 
        Gaa:0, Gab:0, Gac:0, Gba:0, Gbb:0, Gbc:0, Gca:0, Gcb:0, Gcc:0, Baa:0, Bab:0, Bac:0, Bba:0, Bbb:0, Bbc:0, Bca:0, Bcb:0, Bcc:0, linelength:0, Config:n.attrs.Config, 
        PdA_dis:0, QdA_dis:0, PdB_dis:0, QdB_dis:0, PdC_dis:0, QdC_dis:0, Load_Model_dis:'None', IAmag_cal:0, IAang_cal:0, IBmag_cal:0, IBang_cal:0, ICmag_cal:0, ICang_cal:0};
        let payload_FB = { edges: {} }
        payload_FB.edges[formValue_FB.stype] = {};
        payload_FB.edges[formValue_FB.stype][formValue_FB.sid] = {};
        payload_FB.edges[formValue_FB.stype][formValue_FB.sid][formValue_FB.etype] = {};
        payload_FB.edges[formValue_FB.stype][formValue_FB.sid][formValue_FB.etype][formValue_FB.ttype] = {};
        payload_FB.edges[formValue_FB.stype][formValue_FB.sid][formValue_FB.etype][formValue_FB.ttype][formValue_FB.tid] = {};
        Object.keys(formValue_FB).forEach(k => {
          if (k !== 'stype' && k !== 'sid' && k !== 'ttype' && k !== 'tid' && k !== 'etype') {
            if (isNaN(formValue_FB[k])) {
              payload_FB.edges[formValue_FB.stype][formValue_FB.sid][formValue_FB.etype][formValue_FB.ttype][formValue_FB.tid][k] = {
                value: formValue_FB[k]
              };
            } else {
              payload_FB.edges[formValue_FB.stype][formValue_FB.sid][formValue_FB.etype][formValue_FB.ttype][formValue_FB.tid][k] = {
                value: Number(formValue_FB[k])
              };
            }   
          }
        });
        console.log(payload_FB);  
        let formValue_TB ={stype: 'bus_D', sid:n.attrs.to_bus, etype:'backe_D',ttype:'bus_D',tid:n.exID, edge_name:n.attrs.from_bus+n.exID, flag:0, Nt:0, 
        connection_T:'None', Raa:0, Rab:0, Rac:0, Rba:0, Rbb:0, Rbc:0, Rca:0, Rcb:0, Rcc:0, Xaa:0, Xab:0, Xac:0, Xba:0, Xbb:0, Xbc:0, Xca:0, Xcb:0, Xcc:0, 
        Gaa:0, Gab:0, Gac:0, Gba:0, Gbb:0, Gbc:0, Gca:0, Gcb:0, Gcc:0, Baa:0, Bab:0, Bac:0, Bba:0, Bbb:0, Bbc:0, Bca:0, Bcb:0, Bcc:0, linelength:0, Config:n.attrs.Config, 
        PdA_dis:0, QdA_dis:0, PdB_dis:0, QdB_dis:0, PdC_dis:0, QdC_dis:0, Load_Model_dis:'None', IAmag_cal:0, IAang_cal:0, IBmag_cal:0, IBang_cal:0, ICmag_cal:0, ICang_cal:0};        
        let payload_TB = { edges: {} }
        payload_TB.edges[formValue_TB.stype] = {};
        payload_TB.edges[formValue_TB.stype][formValue_TB.sid] = {};
        payload_TB.edges[formValue_TB.stype][formValue_TB.sid][formValue_TB.etype] = {};
        payload_TB.edges[formValue_TB.stype][formValue_TB.sid][formValue_TB.etype][formValue_TB.ttype] = {};
        payload_TB.edges[formValue_TB.stype][formValue_TB.sid][formValue_TB.etype][formValue_TB.ttype][formValue_TB.tid] = {};
        Object.keys(formValue_TB).forEach(k => {
          if (k !== 'stype' && k !== 'sid' && k !== 'ttype' && k !== 'tid' && k !== 'etype') {
            if (isNaN(formValue_TB[k])) {
              payload_TB.edges[formValue_TB.stype][formValue_TB.sid][formValue_TB.etype][formValue_TB.ttype][formValue_TB.tid][k] = {
                value: formValue_TB[k]
              };
            } else {
              payload_TB.edges[formValue_TB.stype][formValue_TB.sid][formValue_TB.etype][formValue_TB.ttype][formValue_TB.tid][k] = {
                value: Number(formValue_TB[k])
              };
            }
    
          }
        });
        console.log(payload_TB);
        // post graph and then refresh
        this.switchService.postGraph(JSON.stringify(payload))
        .then(res => {
          console.log(res);
          if (!res.error) {
            return   this.switchService.postGraph(JSON.stringify(payload_F));
            
          } else {
    
          }
        })  
        .then(res => {
          console.log(res);
          if (!res.error) {
            return   this.switchService.postGraph(JSON.stringify(payload_T));
            
          } else {
    
          }
        }) 
        .then(res => {
          console.log(res);
          if (!res.error) {
            return   this.switchService.postGraph(JSON.stringify(payload_FB));
            
          } else {
    
          }
        })  
        .then(res => {
          console.log(res);
          if (!res.error) {
            return   this.switchService.postGraph(JSON.stringify(payload_TB));
            
          } else {
    
          }
        })
        .then(res => {
          console.log(res);
          if (!res.error) {
            return   this.deleteNode(); 
            
          }
        });
      }else if (this.Direction.includes('negative')) {
        let formValue_F ={stype: 'bus_D', sid:n.attrs.to_bus, etype:'foree_D',ttype:'bus_D',tid:n.exID, edge_name:n.attrs.from_bus+n.exID, flag:0, Nt:0, 
        connection_T:'None', Raa:0, Rab:0, Rac:0, Rba:0, Rbb:0, Rbc:0, Rca:0, Rcb:0, Rcc:0, Xaa:0, Xab:0, Xac:0, Xba:0, Xbb:0, Xbc:0, Xca:0, Xcb:0, Xcc:0, 
        Gaa:0, Gab:0, Gac:0, Gba:0, Gbb:0, Gbc:0, Gca:0, Gcb:0, Gcc:0, Baa:0, Bab:0, Bac:0, Bba:0, Bbb:0, Bbc:0, Bca:0, Bcb:0, Bcc:0, linelength:0, Config:n.attrs.Config, 
        PdA_dis:0, QdA_dis:0, PdB_dis:0, QdB_dis:0, PdC_dis:0, QdC_dis:0, Load_Model_dis:'None', IAmag_cal:0, IAang_cal:0, IBmag_cal:0, IBang_cal:0, ICmag_cal:0, ICang_cal:0};
        let payload_F = { edges: {} }
        payload_F.edges[formValue_F.stype] = {};
        payload_F.edges[formValue_F.stype][formValue_F.sid] = {};
        payload_F.edges[formValue_F.stype][formValue_F.sid][formValue_F.etype] = {};
        payload_F.edges[formValue_F.stype][formValue_F.sid][formValue_F.etype][formValue_F.ttype] = {};
        payload_F.edges[formValue_F.stype][formValue_F.sid][formValue_F.etype][formValue_F.ttype][formValue_F.tid] = {};
        Object.keys(formValue_F).forEach(k => {
          if (k !== 'stype' && k !== 'sid' && k !== 'ttype' && k !== 'tid' && k !== 'etype') {
            if (isNaN(formValue_F[k])) {
              payload_F.edges[formValue_F.stype][formValue_F.sid][formValue_F.etype][formValue_F.ttype][formValue_F.tid][k] = {
                value: formValue_F[k]
              };
            } else {
              payload_F.edges[formValue_F.stype][formValue_F.sid][formValue_F.etype][formValue_F.ttype][formValue_F.tid][k] = {
                value: Number(formValue_F[k])
              };
            }   
          }
        });
        console.log(payload_F);  
        let formValue_T ={stype: 'bus_D', sid:n.exID, etype:'foree_D',ttype:'bus_D',tid:n.attrs.from_bus, edge_name:n.attrs.from_bus+n.exID, flag:0, Nt:0, 
        connection_T:'None', Raa:0, Rab:0, Rac:0, Rba:0, Rbb:0, Rbc:0, Rca:0, Rcb:0, Rcc:0, Xaa:0, Xab:0, Xac:0, Xba:0, Xbb:0, Xbc:0, Xca:0, Xcb:0, Xcc:0, 
        Gaa:0, Gab:0, Gac:0, Gba:0, Gbb:0, Gbc:0, Gca:0, Gcb:0, Gcc:0, Baa:0, Bab:0, Bac:0, Bba:0, Bbb:0, Bbc:0, Bca:0, Bcb:0, Bcc:0, linelength:0, Config:n.attrs.Config, 
        PdA_dis:0, QdA_dis:0, PdB_dis:0, QdB_dis:0, PdC_dis:0, QdC_dis:0, Load_Model_dis:'None', IAmag_cal:0, IAang_cal:0, IBmag_cal:0, IBang_cal:0, ICmag_cal:0, ICang_cal:0};        
        let payload_T = { edges: {} }
        payload_T.edges[formValue_T.stype] = {};
        payload_T.edges[formValue_T.stype][formValue_T.sid] = {};
        payload_T.edges[formValue_T.stype][formValue_T.sid][formValue_T.etype] = {};
        payload_T.edges[formValue_T.stype][formValue_T.sid][formValue_T.etype][formValue_T.ttype] = {};
        payload_T.edges[formValue_T.stype][formValue_T.sid][formValue_T.etype][formValue_T.ttype][formValue_T.tid] = {};
        Object.keys(formValue_T).forEach(k => {
          if (k !== 'stype' && k !== 'sid' && k !== 'ttype' && k !== 'tid' && k !== 'etype') {
            if (isNaN(formValue_T[k])) {
              payload_T.edges[formValue_T.stype][formValue_T.sid][formValue_T.etype][formValue_T.ttype][formValue_T.tid][k] = {
                value: formValue_T[k]
              };
            } else {
              payload_T.edges[formValue_T.stype][formValue_T.sid][formValue_T.etype][formValue_T.ttype][formValue_T.tid][k] = {
                value: Number(formValue_T[k])
              };
            }
    
          }
        });
        console.log(payload_T);
        let formValue_FB ={stype: 'bus_D', sid:n.exID, etype:'backe_D',ttype:'bus_D',tid:n.attrs.to_bus, edge_name:n.attrs.from_bus+n.exID, flag:0, Nt:0, 
        connection_T:'None', Raa:0, Rab:0, Rac:0, Rba:0, Rbb:0, Rbc:0, Rca:0, Rcb:0, Rcc:0, Xaa:0, Xab:0, Xac:0, Xba:0, Xbb:0, Xbc:0, Xca:0, Xcb:0, Xcc:0, 
        Gaa:0, Gab:0, Gac:0, Gba:0, Gbb:0, Gbc:0, Gca:0, Gcb:0, Gcc:0, Baa:0, Bab:0, Bac:0, Bba:0, Bbb:0, Bbc:0, Bca:0, Bcb:0, Bcc:0, linelength:0, Config:n.attrs.Config, 
        PdA_dis:0, QdA_dis:0, PdB_dis:0, QdB_dis:0, PdC_dis:0, QdC_dis:0, Load_Model_dis:'None', IAmag_cal:0, IAang_cal:0, IBmag_cal:0, IBang_cal:0, ICmag_cal:0, ICang_cal:0};
        let payload_FB = { edges: {} }
        payload_FB.edges[formValue_FB.stype] = {};
        payload_FB.edges[formValue_FB.stype][formValue_FB.sid] = {};
        payload_FB.edges[formValue_FB.stype][formValue_FB.sid][formValue_FB.etype] = {};
        payload_FB.edges[formValue_FB.stype][formValue_FB.sid][formValue_FB.etype][formValue_FB.ttype] = {};
        payload_FB.edges[formValue_FB.stype][formValue_FB.sid][formValue_FB.etype][formValue_FB.ttype][formValue_FB.tid] = {};
        Object.keys(formValue_FB).forEach(k => {
          if (k !== 'stype' && k !== 'sid' && k !== 'ttype' && k !== 'tid' && k !== 'etype') {
            if (isNaN(formValue_FB[k])) {
              payload_FB.edges[formValue_FB.stype][formValue_FB.sid][formValue_FB.etype][formValue_FB.ttype][formValue_FB.tid][k] = {
                value: formValue_FB[k]
              };
            } else {
              payload_FB.edges[formValue_FB.stype][formValue_FB.sid][formValue_FB.etype][formValue_FB.ttype][formValue_FB.tid][k] = {
                value: Number(formValue_FB[k])
              };
            }   
          }
        });
        console.log(payload_FB);  
        let formValue_TB ={stype: 'bus_D', sid:n.attrs.from_bus, etype:'backe_D',ttype:'bus_D',tid:n.exID, edge_name:n.attrs.from_bus+n.exID, flag:0, Nt:0, 
        connection_T:'None', Raa:0, Rab:0, Rac:0, Rba:0, Rbb:0, Rbc:0, Rca:0, Rcb:0, Rcc:0, Xaa:0, Xab:0, Xac:0, Xba:0, Xbb:0, Xbc:0, Xca:0, Xcb:0, Xcc:0, 
        Gaa:0, Gab:0, Gac:0, Gba:0, Gbb:0, Gbc:0, Gca:0, Gcb:0, Gcc:0, Baa:0, Bab:0, Bac:0, Bba:0, Bbb:0, Bbc:0, Bca:0, Bcb:0, Bcc:0, linelength:0, Config:n.attrs.Config, 
        PdA_dis:0, QdA_dis:0, PdB_dis:0, QdB_dis:0, PdC_dis:0, QdC_dis:0, Load_Model_dis:'None', IAmag_cal:0, IAang_cal:0, IBmag_cal:0, IBang_cal:0, ICmag_cal:0, ICang_cal:0};        
        let payload_TB = { edges: {} }
        payload_TB.edges[formValue_TB.stype] = {};
        payload_TB.edges[formValue_TB.stype][formValue_TB.sid] = {};
        payload_TB.edges[formValue_TB.stype][formValue_TB.sid][formValue_TB.etype] = {};
        payload_TB.edges[formValue_TB.stype][formValue_TB.sid][formValue_TB.etype][formValue_TB.ttype] = {};
        payload_TB.edges[formValue_TB.stype][formValue_TB.sid][formValue_TB.etype][formValue_TB.ttype][formValue_TB.tid] = {};
        Object.keys(formValue_TB).forEach(k => {
          if (k !== 'stype' && k !== 'sid' && k !== 'ttype' && k !== 'tid' && k !== 'etype') {
            if (isNaN(formValue_TB[k])) {
              payload_TB.edges[formValue_TB.stype][formValue_TB.sid][formValue_TB.etype][formValue_TB.ttype][formValue_TB.tid][k] = {
                value: formValue_TB[k]
              };
            } else {
              payload_TB.edges[formValue_TB.stype][formValue_TB.sid][formValue_TB.etype][formValue_TB.ttype][formValue_TB.tid][k] = {
                value: Number(formValue_TB[k])
              };
            }
    
          }
        });
        console.log(payload_TB);
        // post graph and then refresh
        this.switchService.postGraph(JSON.stringify(payload))
        .then(res => {
          console.log(res);
          if (!res.error) {
            return   this.switchService.postGraph(JSON.stringify(payload_F));
            
          } else {
    
          }
        })  
        .then(res => {
          console.log(res);
          if (!res.error) {
            return   this.switchService.postGraph(JSON.stringify(payload_T));
            
          } else {
    
          }
        }) 
        .then(res => {
          console.log(res);
          if (!res.error) {
            return   this.switchService.postGraph(JSON.stringify(payload_FB));
            
          } else {
    
          }
        })  
        .then(res => {
          console.log(res);
          if (!res.error) {
            return   this.switchService.postGraph(JSON.stringify(payload_TB));
            
          } else {
    
          }
        })
        .then(res => {
          console.log(res);
          if (!res.error) {
            return   this.deleteNode(); 
            
          }
        });

      }else{

      }
       
         
       });
     }    
   }); 

}


 deleteNode() {
  let selectedNode =[];
  let openswitch=[];  
    this.switchService.getSwitches()
    .then(switches => {
        this.switches = switches;
        console.log(this.switches);
        this.switches.forEach(sw => {
          if (sw.status==='open') {
            openswitch.push(sw.switch_name);
          }
        });
       this.chart.getNodes().forEach(n => {
          if (openswitch.includes(String(n.exID))) {
            selectedNode.push(n);
          } 
        //console.log(n.exID);              
       });
       console.log(openswitch);     
       console.log(selectedNode);
      selectedNode.forEach(n=>{
       if (n) {
        // delete all selected nodes
        this.switchService.deleteNode(n.exType, n.exID)
        .then(res => {
          console.log(res);
          if (!res.error) {
            
            return this.updateLink();
          } else {
    
          }
          });
           
       }
    });               
  });     
 }
 

  updateLink() { 
    let payload_Fore = { edges: {} }
    payload_Fore.edges['bus_D'] = {};
    let payload_Backe = { edges: {} }
    payload_Backe.edges['bus_D'] = {};
    
    this.chart.getLinks().forEach(l => {
      if (this.Edgeindex.includes(l.attrs.edge_name))
     {

    let formValue_Fore ={stype: l.source.exType, sid:l.source.exID, etype:'backe_D', ttype:l.target.exType, tid:l.target.exID, edge_name:l.attrs.edge_name, flag:l.attrs.flag, Nt:l.attrs.Nt, 
    connection_T:l.attrs.connection_T, Raa:l.attrs.Raa, Rab:l.attrs.Rab, Rac:l.attrs.Rac, Rba:l.attrs.Rba, Rbb:l.attrs.Rbb, Rbc:l.attrs.Rbc, Rca:l.attrs.Rca, Rcb:l.attrs.Rcb, Rcc:l.attrs.Rcc, 
    Xaa:l.attrs.Xaa, Xab:l.attrs.Xab, Xac:l.attrs.Xac, Xba:l.attrs.Xba, Xbb:l.attrs.Xbb, Xbc:l.attrs.Xbc, Xca:l.attrs.Xca, Xcb:l.attrs.Xcb, Xcc:l.attrs.Xcc, Gaa:l.attrs.Gaa, Gab:l.attrs.Gab, 
    Gac:l.attrs.Gac, Gba:l.attrs.Gba, Gbb:l.attrs.Gbb, Gbc:l.attrs.Gbc, Gca:l.attrs.Gca, Gcb:l.attrs.Gcb, Gcc:l.attrs.Gcc, Baa:l.attrs.Baa, Bab:l.attrs.Bab, Bac:l.attrs.Bac, Bba:l.attrs.Bba, 
    Bbb:l.attrs.Bbb, Bbc:l.attrs.Bbc, Bca:l.attrs.Bca, Bcb:l.attrs.Bcb, Bcc:l.attrs.Bcc, linelength:l.attrs.linelength, Config:l.attrs.Config, PdA_dis:l.attrs.PdA_dis, QdA_dis:l.attrs.QdA_dis, 
    PdB_dis:l.attrs.PdB_dis, QdB_dis:l.attrs.QdB_dis, PdC_dis:l.attrs.PdC_dis, QdC_dis:l.attrs.QdC_dis, Load_Model_dis:l.attrs.Load_Model_dis, IAmag_cal:l.attrs.IAmag_cal, IAang_cal:l.attrs.IAang_cal, 
    IBmag_cal:l.attrs.IBmag_cal, IBang_cal:l.attrs.IBang_cal, ICmag_cal:l.attrs.ICmag_cal, ICang_cal:l.attrs.ICang_cal};    
    
    payload_Fore.edges['bus_D'][formValue_Fore.sid] = {};
    payload_Fore.edges['bus_D'][formValue_Fore.sid][formValue_Fore.etype] = {};
    payload_Fore.edges['bus_D'][formValue_Fore.sid][formValue_Fore.etype][formValue_Fore.ttype] = {};
    payload_Fore.edges['bus_D'][formValue_Fore.sid][formValue_Fore.etype][formValue_Fore.ttype][formValue_Fore.tid] = {};
    Object.keys(formValue_Fore).forEach(k => {
      if (k !== 'stype' && k !== 'sid' && k !== 'ttype' && k !== 'tid' && k !== 'etype') {
        if (isNaN(formValue_Fore[k])) {
          payload_Fore.edges['bus_D'][formValue_Fore.sid][formValue_Fore.etype][formValue_Fore.ttype][formValue_Fore.tid][k] = {
            value: formValue_Fore[k]
          };
        } else {
          payload_Fore.edges['bus_D'][formValue_Fore.sid][formValue_Fore.etype][formValue_Fore.ttype][formValue_Fore.tid][k] = {
            value: Number(formValue_Fore[k])
          };
        }

      }
    });
    
    let formValue_Backe ={stype: l.target.exType, sid:l.target.exID, etype:'foree_D', ttype:l.source.exType, tid:l.source.exID, edge_name:l.attrs.edge_name, flag:l.attrs.flag, Nt:l.attrs.Nt, 
    connection_T:l.attrs.connection_T, Raa:l.attrs.Raa, Rab:l.attrs.Rab, Rac:l.attrs.Rac, Rba:l.attrs.Rba, Rbb:l.attrs.Rbb, Rbc:l.attrs.Rbc, Rca:l.attrs.Rca, Rcb:l.attrs.Rcb, Rcc:l.attrs.Rcc, 
    Xaa:l.attrs.Xaa, Xab:l.attrs.Xab, Xac:l.attrs.Xac, Xba:l.attrs.Xba, Xbb:l.attrs.Xbb, Xbc:l.attrs.Xbc, Xca:l.attrs.Xca, Xcb:l.attrs.Xcb, Xcc:l.attrs.Xcc, Gaa:l.attrs.Gaa, Gab:l.attrs.Gab, 
    Gac:l.attrs.Gac, Gba:l.attrs.Gba, Gbb:l.attrs.Gbb, Gbc:l.attrs.Gbc, Gca:l.attrs.Gca, Gcb:l.attrs.Gcb, Gcc:l.attrs.Gcc, Baa:l.attrs.Baa, Bab:l.attrs.Bab, Bac:l.attrs.Bac, Bba:l.attrs.Bba, 
    Bbb:l.attrs.Bbb, Bbc:l.attrs.Bbc, Bca:l.attrs.Bca, Bcb:l.attrs.Bcb, Bcc:l.attrs.Bcc, linelength:l.attrs.linelength, Config:l.attrs.Config, PdA_dis:l.attrs.PdA_dis, QdA_dis:l.attrs.QdA_dis, 
    PdB_dis:l.attrs.PdB_dis, QdB_dis:l.attrs.QdB_dis, PdC_dis:l.attrs.PdC_dis, QdC_dis:l.attrs.QdC_dis, Load_Model_dis:l.attrs.Load_Model_dis, IAmag_cal:l.attrs.IAmag_cal, IAang_cal:l.attrs.IAang_cal, 
    IBmag_cal:l.attrs.IBmag_cal, IBang_cal:l.attrs.IBang_cal, ICmag_cal:l.attrs.ICmag_cal, ICang_cal:l.attrs.ICang_cal};    
    
    payload_Backe.edges['bus_D'][formValue_Backe.sid] = {};
    payload_Backe.edges['bus_D'][formValue_Backe.sid][formValue_Backe.etype] = {};
    payload_Backe.edges['bus_D'][formValue_Backe.sid][formValue_Backe.etype][formValue_Backe.ttype] = {};
    payload_Backe.edges['bus_D'][formValue_Backe.sid][formValue_Backe.etype][formValue_Backe.ttype][formValue_Backe.tid] = {};
    Object.keys(formValue_Backe).forEach(k => {
      if (k !== 'stype' && k !== 'sid' && k !== 'ttype' && k !== 'tid' && k !== 'etype') {
        if (isNaN(formValue_Backe[k])) {
          payload_Backe.edges['bus_D'][formValue_Backe.sid][formValue_Backe.etype][formValue_Backe.ttype][formValue_Backe.tid][k] = {
            value: formValue_Backe[k]
          };
        } else {
          payload_Backe.edges['bus_D'][formValue_Backe.sid][formValue_Backe.etype][formValue_Backe.ttype][formValue_Backe.tid][k] = {
            value: Number(formValue_Backe[k])
          };
        }

      }
    });
    this.graphManageService.deleteLink(l.source.exType, l.source.exID, l.exType, l.target.exType, l.target.exID);
    this.graphManageService.deleteLink(l.target.exType, l.target.exID, 'backe_D', l.source.exType, l.source.exID);
  } 
});
console.log(payload_Fore);
console.log(payload_Backe);
    this.switchService.postGraph(JSON.stringify(payload_Backe))
    .then(res => {
      console.log(res);
      if (!res.error) {
        return  this.switchService.postGraph(JSON.stringify(payload_Fore));
      } else {

      }
    })   
      .then(res => {
        console.log(res);
        if (!res.error) {
          return this.updategraph()
        } else {

        }
      });
      
  

}
  
  
  
  revert() {
    this.nodeForm.form.reset();
    this.linkForm.form.reset();
  }
  cancel() {
    this.nodeForm.form.reset();
    this.linkForm.form.reset();
    this.addNodeDisplay = false;
    this.addLinkDisplay = false;
  }


  onRowSelect1(event) {
    let selectedID = [];
    let selectedNode = [];
    this.selectedSwitches.forEach(g => {
        selectedID.push(g.switch_name);
    });
    selectedID.push(event.data.switch_name);
    // console.log(selectedID);
    this.chart1.getNodes().forEach(n => {
        if (selectedID.includes(String(n.exID))) {
            this.chart1.unLowlightNode(n.exType, n.exID);
            selectedNode.push(n);
          //  console.log(selectedNode);
        } else {
            this.chart1.lowlightNode(n.exType, n.exID);
        }
    });
    //this.chart1.getLinks().forEach(l => {
    //    this.chart1.lowlightLink(l.exType, l.source.exType, l.source.exID, l.target.exType, l.target.exID);
    //});
    this.chart1.scrollIntoView(selectedNode);
    this.chart1.update();
}

onRowSelect2(event) {
  this.selectedopen=event.data.swtoopen;
  this.selectedclose=event.data.swtoclose;
  console.log(this.selectedopen,this.selectedclose);
}

onRowUnselect1(event) {
    // console.log(event);
    let selectedID = [];
    let selectedNode = [];
    this.selectedSwitches.forEach(g => {
        selectedID.push(g.switch_name);
    });
    console.log(selectedID);
    let idx = selectedID.indexOf(event.data.switch_name);
    // console.log(idx);
    if (idx != -1) {
        selectedID.splice(idx, 1);
    }
    // console.log(selectedID);
    if (selectedID.length === 0) {
       // this.highLightAll1();
        this.chart1.getNodes().forEach(n =>{    
            selectedNode.push(n);    
    });
    this.chart1.scrollIntoView(selectedNode);
    } else {
        this.chart1.getNodes().forEach(n => {
            if (selectedID.includes(String(n.exID))) {
                this.chart1.unLowlightNode(n.exType, n.exID);
                selectedNode.push(n);
            } else {
                this.chart1.lowlightNode(n.exType, n.exID);
            }
        });
       // this.chart1.getLinks().forEach(l => {
       //     this.chart1.lowlightLink(l.exType, l.source.exType, l.source.exID, l.target.exType, l.target.exID);
       // });
        this.chart1.scrollIntoView(selectedNode);  
        this.chart1.update();
    }
}

onRowUnselect2(event) {
}

 highLightAll1(): void {
  let selectedNode = [];
  // highlight all loads
  this.chart1.getNodes().forEach(n => {
      if (this.switchEdges.includes(String(n.exID))) {
          this.chart1.unLowlightNode(n.exType, n.exID);
          selectedNode.push(n);
      } else {
          this.chart1.lowlightNode(n.exType, n.exID);
      }
  });
  // lowlight all edges
  this.chart1.getLinks().forEach(l => {
      this.chart1.lowlightLink(l.exType, l.source.exType, l.source.exID, l.target.exType, l.target.exID);
  });
  this.chart1.update();
}

 }
