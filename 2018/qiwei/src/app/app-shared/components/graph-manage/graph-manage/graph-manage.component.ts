import { Component, OnInit, ViewChild, AfterViewInit, AfterViewChecked, OnDestroy } from '@angular/core';
import { Router } from '@angular/router';
import { WholeGraphService } from '../../../services/whole-graph.service';
import { GraphManageService } from '../../../services/graph-manage.service';
import { ConfirmDialogModule, ConfirmationService } from 'primeng/primeng';
import { GraphManageFormComponent } from '../graph-manage-form/graph-manage-form.component';
import { EditGraphService } from '../../../services/edit-graph.service';
import { FormControl, FormGroup, FormArray, FormBuilder, Validators } from '@angular/forms';
import { FormsModule, ReactiveFormsModule } from '@angular/forms';
import { environment } from '../../../../../environments/environment';
import { Subscription } from 'rxjs/Subscription';

import * as gvis from 'gvis';
const GraphChart = (<any>gvis).GraphChart;

@Component({
  selector: 'app-graph-manage',
  templateUrl: './graph-manage.component.html',
  styleUrls: ['./graph-manage.component.css'],
})
export class GraphManageComponent implements OnInit, AfterViewInit, AfterViewChecked, OnDestroy {
  @ViewChild('container') container: any;
  @ViewChild('nodeDiag') nodeForm: GraphManageFormComponent;
  @ViewChild('linkDiag') linkForm: GraphManageFormComponent;
  @ViewChild('sidenav') sidenav: any;
  public chart: any;
  private graph;
  addNodeDisplay = false;
  addLinkDisplay = false;
  // !nodeForm.form.valid;
  nodeMark = 'node';
  linkMark = 'link';
  schema;
  nodeLabels: any[] = [{ label: 'id', checked: true }];
  linkLabels: any[] = [];
  selectedNodeLabels: any[];
  selectedLinkLabels: any[];
  nodeLabelSelector = false;
  linkLabelSelector = false;
  selectedNode: any;
  selectedLink: any;
  nodeAttrs: any[] = [];
  linkAttrs: any[] = [];
  searchNodeDisplay = false;
  searchLinkDisplay = false;
  searchingNode: any;
  searchingLink: any;
  subscriptions: Subscription[] = [];
  // nodeLabelForm: FormGroup;
  // linkLabelForm: FormGroup;
  constructor(
    private router: Router,
    private wholeGraphService: WholeGraphService,
    private graphManageService: GraphManageService,
    private confirmationService: ConfirmationService,
    private editGraphService: EditGraphService,
    private fb: FormBuilder
  ) {
    this.searchingNode = { exType: 'bus_D', exId: '' };
    this.searchingLink = { exType: 'foree_D', sourceId: '', sourceType: 'bus_D', targetId: '', targetType: 'bus_D', edge_name: '' };
  }

  ngOnInit() {
    this.wholeGraphService.getSchema().then(res => {
      this.schema = res;
    })
  }

  ngAfterViewInit(): void {
    this.chart = new GraphChart({
      render: {
        container: this.container.nativeElement,
        assetsUrlBase: './assets/gvis',
        info: {
          enabled: false
        }
      },
      style: {
        node: [
          {
            condition: (node, chart) => {
              return node.attrs.PdA !== 0 && node.attrs.PdB === 0 && node.attrs.PdC === 0;
            },
            style: {
              fillColor: '#FF0000' // red A
            }
          },
          {
            condition: (node, chart) => {
              return node.attrs.PdA === 0 && node.attrs.PdB !== 0 && node.attrs.PdC === 0;
            },
            style: {
              fillColor: '#0000FF' // blue B
            }
          },
          {
            condition: (node, chart) => {
              return node.attrs.PdA === 0 && node.attrs.PdB === 0 && node.attrs.PdC !== 0;
            },
            style: {
              fillColor: '#00FF00' // green C
            }
          },
          {
            condition: (node, chart) => {
              return node.attrs.PdA !== 0 && node.attrs.PdB !== 0 && node.attrs.PdC === 0;
            },
            style: {
              fillColor: '#FF00FF' // AB
            }
          },
          {
            condition: (node, chart) => {
              return node.attrs.PdA !== 0 && node.attrs.PdB === 0 && node.attrs.PdC !== 0;
            },
            style: {
              fillColor: '#FFFF00' // AC
            }
          },
          {
            condition: (node, chart) => {
              return node.attrs.PdA === 0 && node.attrs.PdB !== 0 && node.attrs.PdC !== 0;
            },
            style: {
              fillColor: '00FFFF' // BC
            }
          },
          {
            condition: (node, chart) => {
              return node.attrs.PdA !== 0 && node.attrs.PdB !== 0 && node.attrs.PdC !== 0;
            },
            style: {
              fillColor: '#404040' //ABC
            }
          }

        ],
        link: [
          {
            condition: (link, chart) => {
              return link.attrs.Config === 'Line1';
            },
            style: {
              fillColor: '#404040',
              radius: 30
            }
          },

          {
            condition: (link, chart) => {
              return link.attrs.Config === 'line2';
            },
            style: {
              fillColor: '#404040',
              radius: 30
            }
          },
          {
            condition: (link, chart) => {
              return link.attrs.Config === 'Line3';
            },
            style: {
              fillColor: '#404040',
              radius: 30
            }
          },
          {
            condition: (link, chart) => {
              return link.attrs.Config === 'Line4';
            },
            style: {
              fillColor: '#404040',
              radius: 30
            }
          },
          {
            condition: (link, chart) => {
              return link.attrs.Config === 'Line5';
            },
            style: {
              fillColor: '#404040',
              radius: 30
            }
          },
          {
            condition: (link, chart) => {
              return link.attrs.Config === 'Line6';
            },
            style: {
              fillColor: '#404040',
              radius: 30
            }
          },
          {
            condition: (link, chart) => {
              return link.attrs.Config === 'Line7'; // AC
            },
            style: {
              fillColor: '#FFFF00',
              radius: 20
            }
          },
          {
            condition: (link, chart) => {
              return link.attrs.Config === 'Line8'; // AB
            },
            style: {
              fillColor: '#FF00FF',
              radius: 20
            }
          },


          {
            condition: (link, chart) => {
              return link.attrs.Config === 'Line9'; // A red
            },
            style: {
              fillColor: '#FF0000',
              radius: 10
            }
          },

          {
            condition: (link, chart) => {
              return link.attrs.Config === 'Line10'; // B blue
            },
            style: {
              fillColor: '#0000FF',
              radius: 10
            }
          },
          {
            condition: (link, chart) => {
              return link.attrs.Config === 'Line11'; // C green
            },
            style: {
              fillColor: '#00FF00',
              radius: 10
            }
          },
          {
            condition: (link, chart) => {
              return link.attrs.Config === 'Line12';
            },
            style: {
              fillColor: '#404040',
              radius: 30
            }
          },


        ]
      },
      advanced: {
      },
      nodeMenu: {
        enabled: true,
        buttons: [{
          label: 'Update Node',
          callback: (node, chart) => {
            this.showUpdateNode();
          }
        },
        {
          label: 'Add Edge',
          callback: (node, chart) => {
            node.labels = { transformer: false, type: false };
            node.others.items = [
              {
                aspectRatio: 0,
                backgroudStyle: {
                  fillColor: "#f00",
                  lineColor: "transparent"
                },
                px: 0,
                py: -1,
                text: 'Select a vertex to create an edge',
                textStyle: {
                  fillColor: 'red',
                  font: '18px Arial'
                },
                x: 0,
                y: -5
              }
            ];
            chart.update();
          }
        },
        {
          label: 'Delete Node',
          callback: (node, chart) => {
            this.deleteNodeConfirm();
          }
        }
        ]
      },
      linkMenu: {
        enabled: true,
        buttons: [{
          label: 'Update Edge',
          callback: (link, chart) => {
            this.showUpdateLink();
          }
        },
        {
          label: 'Delete Edge',
          callback: (link, chart) => {
            this.deleteLinkConfirm();
          }
        }]
      }
    });

    (<any>window).gsql = this.chart;
    // (<any>window).nodelabel = this.nodeLabelForm;
    // (<any>window).linklabel = this.linkLabelForm;
    this.wholeGraphService.getGraph()
      .then(graph => {
        this.graph = graph;
        console.log(this.graph);
        this.chart
          .addData(this.graph, 'gsql');
        // this.treeLayout();
        if (this.chart.getNodes()[0].attrs.pos_x === 0) {
          this.treeLayout();
        } else {
          this.staticLayout();
        }
        // this.chart.showNodeLabelsByType('bus_D', this.selectedNodeLabels);
        // this.chart.showLinkLabelsByType('foree_D', this.selectedLinkLabels);
        this.chart.update();
      });
    // .then(() => { this.showLabels() });
    this.chart.on('onClick', (item) => {
      // Do something here. event call back.
      // Example:
      let source;
      let addintEdge = false;
      this.selectedLink = null;
      this.selectedNode = null;
      this.chart.getData().nodes.forEach(n => {
        // search for source node
        if (n.others.items) {
          source = n;
          addintEdge = true;
          n.labels = {};
          delete n.others.items;
        }
      });
      if (item === undefined) {
      } else if (item.isNode) {
        if (addintEdge) {
          let target = item;
          console.log(source, target);
          this.showAddLink(source, target);
        } else {
          this.selectedNode = item;
        }
      } else if (item.isLink) {
        this.selectedLink = item;
      }
      this.redrawChart();
    });
    this.wholeGraphService.getSchema()
      .then(schema => {
        this.schema = schema;
        console.log(this.schema);
        schema.VertexTypes.forEach(t => {
          if (t.Name === 'bus_D') {
            t.Attributes.forEach(a => {
              this.nodeAttrs.push(a.AttributeName);
              if (a.AttributeName !== 'exId') {
                this.nodeLabels.push({ label: a.AttributeName, checked: false });
              }
            })
          }
        });
        schema.EdgeTypes.forEach(t => {
          if (t.Name === 'foree_D') {
            t.Attributes.forEach(a => {
              this.linkLabels.push({ label: a.AttributeName, checked: false });
              this.linkAttrs.push(a.AttributeName);
            })
          }
        });
      })
      .then(() => {
        this.subscriptions.push(this.wholeGraphService.nodelabelAnnounced$.subscribe(
          (labels) => {
            this.selectedNodeLabels = labels;
            this.nodeLabels.forEach(element => {
              if (this.selectedNodeLabels.includes(element.label)) {
                element.checked = true;
              }
            });
            this.chart.showNodeLabelsByType('bus_D', this.selectedNodeLabels)
              .update();
          }
        ));
        this.subscriptions.push(this.wholeGraphService.linklabelAnnounced$.subscribe(
          (labels) => {
            this.selectedLinkLabels = labels;
            this.linkLabels.forEach(element => {
              if (this.selectedLinkLabels.includes(element.label)) {
                element.checked = true;
              }
            });
            this.chart.showLinkLabelsByType('foree_D', this.selectedLinkLabels)
              .update();
          }
        ));
        this.wholeGraphService.announceSubscription();
      });

  }
  ngAfterViewChecked() {
  }

  ngOnDestroy() {
    this.subscriptions.forEach(s => {
      s.unsubscribe();
    })
  }
  test() {
    alert('test');
  }
  redrawChart() {
    this.wholeGraphService.getGraph()
      .then(graph => {
        this.graph = graph;
        this.chart
          .reloadData(this.graph, 'gsql');
        // this.treeLayout();
        if (this.chart.getNodes()[0].attrs.pos_x === 0) {
          this.treeLayout();
        } else {
          this.staticLayout();
        }
        this.chart.update();
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
  saveLayout(): void {
    let payload: { [k: string]: any } = {};
    this.chart.getNodes().forEach(n => {
      const key = n.exID;
      payload[key] = { pos_x: { value: n.x }, pos_y: { value: n.y } };
    });
    this.graphManageService.postGraph(JSON.stringify({ vertices: { bus_D: payload } }))
      .then(responce => console.log(responce));
  }

  discardLayout(): void {
    this.chart.redraw();
  }

  saveConfirm() {
    this.confirmationService.confirm({
      message: 'Save changes?',
      accept: () => {
        this.saveLayout();
      }
    });
  }
  discardConfirm() {
    this.confirmationService.confirm({
      message: 'Discard all changes?',
      accept: () => {
        this.discardLayout();
      }
    });
  }
  showNodeLabelSelector() {
    this.nodeLabelSelector = true;
  }
  showLinkLabelSelector() {
    this.linkLabelSelector = true;
  }

  showAddNode() {
    this.revert();
    this.addNodeDisplay = true;
  }
  showAddLink(source, target) {
    this.revert();
    if (source !== undefined && target !== undefined) {
      this.linkForm.form.patchValue({ sid: source.exID });
      this.linkForm.form.patchValue({ stype: source.exType });
      this.linkForm.form.patchValue({ tid: target.exID });
      this.linkForm.form.patchValue({ ttype: target.exType });
    }
    this.addLinkDisplay = true;
  }
  showUpdateNode() {
    let selectedNodes = this.chart.getSelection().nodes;
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
  showUpdateLink() {
    let selectedLinks = this.chart.getSelection().links;
    if (selectedLinks[0]) {
      Object.keys(this.linkForm.form.value).forEach(k => {
        if (k === 'etype') {
          this.linkForm.form.patchValue({
            etype: selectedLinks[0].exType
          });
        } else if (k === 'stype') {
          this.linkForm.form.patchValue({
            stype: selectedLinks[0].source.exType
          });
        } else if (k === 'sid') {
          this.linkForm.form.patchValue({
            sid: selectedLinks[0].source.exID
          });
        } else if (k === 'ttype') {
          this.linkForm.form.patchValue({
            ttype: selectedLinks[0].target.exType
          });
        } else if (k === 'tid') {
          this.linkForm.form.patchValue({
            tid: selectedLinks[0].target.exID
          });
        } else {
          let kvPair = {};
          kvPair[k] = selectedLinks[0].attrs[k];
          this.linkForm.form.patchValue(kvPair);
        }
      })
      this.addLinkDisplay = true;
    } else {
      alert('Select an Edge first!');
    }
  }

  showLabels() {
    // erase all labels first
    this.chart.hideNodeLabelsByType('bus_D', this.selectedNodeLabels);
    this.chart.hideLinkLabelsByType('foree_D', this.selectedLinkLabels);
    // update selected labels then
    this.selectedNodeLabels = [];
    this.selectedLinkLabels = [];
    this.nodeLabels.forEach(element => {
      if (element.checked) {
        this.selectedNodeLabels.push(element.label);
      }
    });
    this.linkLabels.forEach(element => {
      if (element.checked) {
        this.selectedLinkLabels.push(element.label);
      }
    });
    // show labels
    // this.chart.showNodeLabelsByType('bus_D', this.selectedNodeLabels);
    // this.chart.showLinkLabelsByType('foree_D', this.selectedLinkLabels);
    this.wholeGraphService.announceNodeLabel(this.selectedNodeLabels);
    this.wholeGraphService.announceLinkLabel(this.selectedLinkLabels);
    this.chart.update();
    this.linkLabelSelector = false;
    this.nodeLabelSelector = false;
  }

  resetNodeLabels() {
    // alert('reset labels');
    this.nodeLabels.forEach(element => {
      if (element.label === 'id') {
        element.checked = true;
      } else {
        element.checked = false;
      }
    })
  }
  resetLinkLabels() {
    this.linkLabels.forEach(element => {
      element.checked = false;
    })

  }

  getType(k: string): String {
    let typeArray = this.schema.VertexTypes[1];
    let res
    typeArray.Attributes.forEach(e => {
      if(e.AttributeName === k) {
        res =  e.AttributeType;
      }
    });
    return res;
  }

  addNode() {
    console.log(this.nodeForm.form.value);
    const formValue = this.nodeForm.form.value;
    console.log(formValue.type, formValue.exID);
    if (!formValue.type || !formValue.exId) {
      alert('Error: Node Type and Id required');
    } else {
      // create post payload
      let ntype = formValue.type;
      let nid = formValue.exId;
      let payload = { vertices: {} };
      payload.vertices[ntype] = {};
      payload.vertices[ntype][nid] = {};
      Object.keys(formValue).forEach(k => {
        if (k !== 'type') {
          if(formValue[k] !== null) {
            if(this.getType(k) === 'UINT' || this.getType(k) === 'DOUBLE'){
              payload.vertices[ntype][nid][k] = { value: Number(formValue[k]) };
            } 
            else {
              payload.vertices[ntype][nid][k] = { value: formValue[k] };

            }
          }
        }
      });
      console.log(payload);
      // post graph and then refresh
      this.graphManageService.postGraph(JSON.stringify(payload))
        .then(res => {
          console.log(res);
          if (!res.error) {
            return this.wholeGraphService.getGraph();
          }
        })
        .then(graph => {
          console.log('update graph');
          this.graph = graph;
          console.log(this.graph);
          this.chart
            .addData(this.graph, 'gsql');
          // this.treeLayout();
          if (this.chart.getNodes()[0].attrs.pos_x === 0) {
            this.treeLayout();
          } else {
            this.staticLayout();
          }
          this.chart.update();
        });
      this.addNodeDisplay = false;
    }

  }
  deleteNode() {
    // at least one node selected
    let selectedNodes = this.chart.getSelection().nodes;
    if (selectedNodes[0]) {
      // delete all selected nodes
      this.graphManageService.deleteNode(selectedNodes[0].exType, selectedNodes[0].exID)
        .then(res => {
          console.log(res);
          if (!res.error) {
            return this.wholeGraphService.getGraph();
          }
        })
        .then(graph => {
          console.log('update graph');
          this.graph = graph;
          console.log(this.graph);
          this.chart
            .reloadData(this.graph, 'gsql');
          // this.treeLayout();
          if (this.chart.getNodes()[0].attrs.pos_x === 0) {
            this.treeLayout();
          } else {
            this.staticLayout();
          }
          this.chart.update();
        });
    } else {
      alert('Select a NODE first!');
    }

  }
  deleteNodeConfirm() {
    this.confirmationService.confirm({
      message: 'Delete selected Node?',
      accept: () => {
        this.deleteNode();
      }
    });
  }
  updateNode() {

  }
  addLink() {
    console.log(this.linkForm.form.value);
    // generate the payload
    const formValue = this.linkForm.form.value;
    let payload = { edges: {} }
    payload.edges[formValue.stype] = {};
    payload.edges[formValue.stype][formValue.sid] = {};
    payload.edges[formValue.stype][formValue.sid][formValue.etype] = {};
    payload.edges[formValue.stype][formValue.sid][formValue.etype][formValue.ttype] = {};
    payload.edges[formValue.stype][formValue.sid][formValue.etype][formValue.ttype][formValue.tid] = {};
    Object.keys(formValue).forEach(k => {
      if (k !== 'stype' && k !== 'sid' && k !== 'ttype' && k !== 'tid' && k !== 'etype') {
        if (isNaN(formValue[k])) {
          payload.edges[formValue.stype][formValue.sid][formValue.etype][formValue.ttype][formValue.tid][k] = {
            value: formValue[k]
          };
        } else {
          payload.edges[formValue.stype][formValue.sid][formValue.etype][formValue.ttype][formValue.tid][k] = {
            value: Number(formValue[k])
          };
        }

      }
    });
    console.log(payload);

    this.graphManageService.postGraph(JSON.stringify(payload))
      .then(res => {
        console.log(res);
        if (!res.error) {
          return this.wholeGraphService.getGraph();
        } else {

        }
      })
      .then(graph => {
        console.log('update graph');
        this.graph = graph;
        console.log(this.graph);
        this.chart
          .reloadData(this.graph, 'gsql');
        // this.treeLayout();
        if (this.chart.getNodes()[0].attrs.pos_x === 0) {
          this.treeLayout();
        } else {
          this.staticLayout();
        }
        this.chart.update();
      });
    this.addLinkDisplay = false;
  }
  deleteLink() {
    // at least one node selected
    let selectedLinks = this.chart.getSelection().links;
    if (selectedLinks[0]) {
      // delete all selected nodes
      // tslint:disable-next-line:max-line-length
      this.graphManageService.deleteLink(selectedLinks[0].source.exType, selectedLinks[0].source.exID, selectedLinks[0].exType, selectedLinks[0].target.exType, selectedLinks[0].target.exID)
        .then(res => {
          console.log(res);
          if (!res.error) {
            return this.wholeGraphService.getGraph();
          }
        })
        .then(graph => {
          console.log('update graph');
          this.graph = graph;
          console.log(this.graph);
          this.chart
            .reloadData(this.graph, 'gsql');
          // this.treeLayout();
          if (this.chart.getNodes()[0].attrs.pos_x === 0) {
            this.treeLayout();
          } else {
            this.staticLayout();
          }
          this.chart.update();
        });
    } else {
      alert('Select a NODE first!');
    }

  }
  deleteLinkConfirm() {
    this.confirmationService.confirm({
      message: 'Delete selected Edge?',
      accept: () => {
        this.deleteLink();
      }
    });
  }
  updateLink() {

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
    this.linkLabelSelector = false;
    this.nodeLabelSelector = false;
    this.searchLinkDisplay = false;
    this.searchNodeDisplay = false;
  }
  toggle() {
    this.sidenav.toggle();
    // get selected nodes or links and show them, if none exists, clear table
    this.selectedLink = null;
    this.selectedNode = null;
    let selection = this.chart.getSelection();
    if (selection.nodes.length > 0) {
      this.selectedNode = selection.nodes[0];
    } else if (selection.links.length > 0) {
      this.selectedLink = selection.links[0];
    }
  }
  showSearchNode() {
    this.searchNodeDisplay = true;
  }
  showSearchLink() {
    this.searchLinkDisplay = true;
  }
  nodeSearch() {
    // get searching node
    let resNode;
    let non_exist = true;
    this.chart.getNodes().forEach(n => {
      if (n.exID === this.searchingNode.exId) {
        resNode = n;
        non_exist = false;
      }
    });
    if (non_exist) {
      alert('Node does not exist');
    } else {
      this.selectedNode = null;
      this.selectedLink = null;
      this.selectedNode = resNode;
      this.chart.setSelection([resNode]);
      this.chart.scrollIntoView([resNode]);
      this.chart.update();
      this.searchNodeDisplay = false;
      console.log(this.selectedNode);
      console.log(this.selectedLink);
    }
  }
  linkSearch() {
    // test input valid
    let resLink;
    let non_exist = true;
    if (this.searchingLink.edge_name || (this.searchingLink.sourceId && this.searchingLink.targetId)) {
      // search link
      this.chart.getLinks().forEach(l => {
        // tslint:disable-next-line:max-line-length
        if ((l.source.exID === this.searchingLink.sourceId && l.target.exID === this.searchingLink.targetId) || (this.searchingLink.edge_name !== '' && l.attrs.edge_name === this.searchingLink.edge_name)) {
          resLink = l;
          non_exist = false;
        }
      });
      if (non_exist) {
        alert('Link does not exist');
      } else {
        this.selectedNode = null;
        this.selectedLink = null;
        this.selectedLink = resLink;
        this.chart.setSelection([resLink]);
        this.chart.scrollIntoView([resLink.source, resLink.target]);
        this.chart.update();
        this.searchLinkDisplay = false;
        console.log(this.selectedNode);
        console.log(this.selectedLink);
      }
    } else {
      alert('Invalid Input, Please input edge name or source and target ids');
    }
  }

  downloadGraph() {
    // // alert('download');
    const downloadUrl = environment.pyPre + '/graphdata/';
    this.wholeGraphService.downloadGraph()
      .then(resp => {
        if (!resp) {
          console.log('data prepared');
          window.open(downloadUrl);
        }
      })

  }

}
