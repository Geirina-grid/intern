import { Component, OnInit, AfterViewInit, ViewChild, AfterViewChecked } from '@angular/core';
import { Router } from '@angular/router';
import { GraphChart } from 'gvis';
import * as gvis from 'gvis';
//import { Subscription } from 'rxjs/Subscription';
import { TopologyService} from '../topology.service'

@Component({
  selector: 'app-topology',
  templateUrl: './topology.component.html',
  styleUrls: ['./topology.component.css'],
  providers: [TopologyService]
})
export class TopologyComponent implements OnInit, AfterViewInit{
  @ViewChild('container') container: any;
  public chart: any;
  cols1: any[];
  cols2: any[];
  cols3: any[];
  cols4: any[];
  cols5: any[];
  list1: any[];
  list2: any[];
  list3: any[];
  list4: any[];
  list5: any[];
  substationID: number;
  public detailChart: any;
  private detailGraph;

  //private graph;
  //subscription: Subscription;
  constructor(
    private router: Router,
    private TopologyService: TopologyService
  ) { 
    this.substationID = 1;
    this.cols1 = [{field: 'discid', header: '序号'},
    {field: 'discname', header: '刀闸名'},
    {field: 'discvolt', header: '电压等级'},
    {field: 'discpoint', header: '遥信状态'},
    {field: 'discI_nd', header: 'I端物理节点'},
    {field: 'discJ_nd', header: 'J端物理节点'},
    {field: 'discI_node', header: 'I端所在拓扑节点'},
    {field: 'discJ_node', header: 'J端所在拓扑节点'}]

    this.cols2 = [{field: 'breid', header: '序号'},
    {field: 'bre_name', header: '断路器名'},
    {field: 'bre_volt', header: '电压等级'},
    {field: 'bre_point', header: '遥信状态'},
    {field: 'breI_nd', header: 'I端物理节点'},
    {field: 'breJ_nd', header: 'J端物理节点'},
    {field: 'breI_node', header: 'I端所在拓扑节点'},
    {field: 'breJ_node', header: 'J端所在拓扑节点'}]

    this.cols3 = [{field: 'busid', header: '序号'},
    {field: 'busname', header: '母线名'},
    {field: 'busvolt', header: '电压等级'},
    {field: 'busnode', header: '所在拓扑节点'},
    {field: 'busV', header: '电压'},
    {field: 'busAng', header: '相角'},
    {field: 'busoff', header: '停运标志'},
    {field: 'busV_meas', header: '电压量测'},
    {field: 'bus_ang', header: '相角量测'},
    {field: 'busnd', header: '物理节点'},
    {field: 'busbs', header: '计算节点'},
    {field: 'bus_island', header: '电气岛号'},
    {field: 'busV_max', header: '电压上限'},
    {field: 'busV_min', header: '电压下限'}]

    this.cols4 = [{field: 'idload', header: '序号'},
    {field: 'loadname', header: '负荷名'},
    {field: 'loadvolt', header: '电压等级'},
    {field: 'loadEq', header: '等值负荷标志'},
    {field: 'loadpos', header: '等值连接位置'},
    {field: 'loadNode', header: '所在拓扑节点'},
    {field: 'loadP', header: '有功功率'},
    {field: 'loadQ', header: '无功功率'},
    {field: 'loadOff', header: '停运标志'},
    {field: 'loadp_meas', header: '有功功率量测'},
    {field: 'loadQ_meas', header: '无功功率量测'},
    {field: 'loadnd', header: '物理节点'},
    {field: 'loadbs', header: '计算节点'},
    {field: 'load_island', header: '电气岛号'}]

    this.cols5 = [{field: 'unitid', header: '序号'},
    {field: 'unname', header: '发电机名'},
    {field: 'unEq', header: '等值发电机标志'},
    {field: 'unpos', header: '等值位置'},
    {field: 'unV_rate', header: '额定电压'},
    {field: 'unP_rate', header: '额定容量'},
    {field: 'unvolt_n', header: '接入电网电压等级'},
    {field: 'unnode', header: '所在拓扑节点'},
    {field: 'unP', header: '有功功率'},
    {field: 'unQ', header: '无功功率'},
    {field: 'unUe', header: '机端电压'},
    {field: 'unAng', header: '机端电压相角'},
    {field: 'unoff', header: '停运标志'},
    {field: 'unP_meas', header: '有功功率量测'},
    {field: 'unQ_meas', header: '无功功率量测'},
    {field: 'unUe_meas', header: '机端电压量测'},
    {field: 'unang_meas', header: '机端相角量测'},
    {field: 'unP_max', header: '有功功率上限'},
    {field: 'unP_min', header: '有功功率下限'},
    {field: 'unQ_max', header: '无功功率上限'},
    {field: 'unQ_min', header: '无功功率下限'},
    {field: 'unpf', header: '额定功率因数'},
    {field: 'unnd', header: '物理节点'},
    {field: 'unbs', header: '计算节点'},
    {field: 'unisland', header: '电气岛号'}]

  }


  plot(){
    console.log(this.substationID);
    this.TopologyService.getGraph(this.substationID)
    .then(graph => {
      console.log(graph);
      let temp1 = []; 
      let temp2 = [];
      let temp3 = [];
      let temp4 = [];
      let temp5 = [];
      // console.log(graph);
      graph.forEach(n => {
        if(Object.keys(n) === ['@@setedge']){} 
        else {
          switch(n.v_type) {
            case 'Disconnector':
            // temp1.push({"discid":n.v.discid});
            temp1.push(n.v);
            break;
            case 'Breaker':
            temp2.push(n.v);
            break;
            case 'BUS':
            temp3.push(n.v);
            break;
            case 'l_oad':
            temp4.push(n.v);
            break;
            case 'unit':
            temp5.push(n.v);
            break;
          }
        }
        this.list1=temp1;
        this.list2=temp2;
        this.list3=temp3;
        this.list4=temp4;
        this.list5=temp5;
      });
      //console.log(this.list1);
      console.log(temp3[0]);
      this.chart
        .reloadData(graph, 'gsql');
      this.chart.setRoot('BUS',''+temp3[0].busid);
      this.chart.runLayout('tree');
      this.chart.update();
      this.chart.getNodes().filter(n => 
        n.attrs.bre_point === 1).forEach(n => 
          n.styles.base.icon = 'Closed Breaker');
      this.chart.getNodes().filter(n => 
        n.attrs.bre_point === 0).forEach(n => 
          n.styles.base.icon = 'Open Breaker');
      this.chart.getNodes().filter(n =>
        n.attrs.discpoint === 1).forEach(n =>
          n.styles.base.icon = 'Closed Disconnector');
      this.chart.getNodes().filter(n =>
        n.attrs.discpoint === 0).forEach(n =>
          n.styles.base.icon = 'Open Disconnector');
      this.chart.getNodes().filter(n =>
        n.v_type === 'l_oad').forEach(n =>
          n.styles.base.icon = 'Load');
      this.chart.getNodes().filter(n =>
        n.v_type === 'unit').forEach(n =>
          n.styles.base.icon = 'Generator');
    })
  }

  ngOnInit() {
  }


  ngAfterViewInit(): void {
    this.chart = new GraphChart({
      render: {
        container: this.container.nativeElement,
        assetsUrlBase: './assets/gvis'
      },  
      style: {
        link: [{
            condition: 'true',
            style:{
              fillColor: '#000000'
            }
        }],
        node: [
          {
            condition: (node, chart) => {
              return node.exType === 'BUS';
            },
            style: {
              display: 'rectangle',
              aspectRatio: 2.75
            }
          },
          {
            condition: (node, chart) => {
              return node.exType === 'CN';
            },
            style: {
              radius: 1
            }
          },
          {
            condition: (node, chart) => {
              return node.exType === 'Disconnector';
            },
            style: {
              display: 'rectangle',
              aspectRatio: 0.35,
              fillColor:'#FF0000'
            }
          },
          {
            condition: (node, chart) => {
              return node.exType === 'Breaker';
            },
            style: {
              display: 'rectangle',
              aspectRatio: 0.35,
              fillColor:'#FF0000'
            }
          },
          {
            condition: (node, chart) => {
              return node.exType === 'unit';
            },
            style: {
              radius: 20
            }
          },
          {
            condition: (node, chart) => {
              return node.exType === 'l_oad';
            },
            style: {
              radius: 20
            }
          }
        ]
      }
    });
    (<any>window).gsql = this.chart;
    this.TopologyService.getGraph(this.substationID)
      .then(graph => {
        console.log(graph);
        let temp1 = []; 
        let temp2 = [];
        let temp3 = [];
        let temp4 = [];
        let temp5 = [];
        // console.log(graph);
        graph.forEach(n => {
          if(Object.keys(n) === ['@@setedge']){} 
          else {
            switch(n.v_type) {
              case 'Disconnector':
              // temp1.push({"discid":n.v.discid});
              temp1.push(n.v);
              break;
              case 'Breaker':
              temp2.push(n.v);
              break;
              case 'BUS':
              temp3.push(n.v);
              break;
              case 'l_oad':
              temp4.push(n.v);
              break;
              case 'unit':
              temp5.push(n.v);
              break;
            }
          }
          this.list1=temp1;
          this.list2=temp2;
          this.list3=temp3;
          this.list4=temp4;
          this.list5=temp5;
        });
        //console.log(this.list1);
        console.log(temp3[0]);
        this.chart
          .addData(graph, 'gsql');
        this.chart.setRoot('BUS',''+temp3[0].busid);
        this.chart.runLayout('tree');
        this.chart.update();
        this.chart.getNodes().filter(n => 
          n.attrs.bre_point === 1).forEach(n => 
            n.styles.base.icon = 'Closed Breaker');
        this.chart.getNodes().filter(n => 
          n.attrs.bre_point === 0).forEach(n => 
            n.styles.base.icon = 'Open Breaker');
        this.chart.getNodes().filter(n =>
          n.attrs.discpoint === 1).forEach(n =>
            n.styles.base.icon = 'Closed Disconnector');
        this.chart.getNodes().filter(n =>
          n.attrs.discpoint === 0).forEach(n =>
            n.styles.base.icon = 'Open Disconnector');
        this.chart.getNodes().filter(n =>
          n.v_type === 'l_oad').forEach(n =>
            n.styles.base.icon = 'Load');
        this.chart.getNodes().filter(n =>
          n.v_type === 'unit').forEach(n =>
            n.styles.base.icon = 'Generator');
      })

  }

}