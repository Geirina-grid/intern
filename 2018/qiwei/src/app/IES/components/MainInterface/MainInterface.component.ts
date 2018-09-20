import { Component, OnInit, ViewChild, AfterViewInit, OnChanges, SimpleChanges, ViewEncapsulation, OnDestroy } from '@angular/core';
import { Router, NavigationEnd } from '@angular/router';
import { GraphChartService } from '../../../app-shared/services/graph-chart.service';

@Component({
  selector: 'main-interface',
  templateUrl: './MainInterface.component.html',
  styleUrls: ['./MainInterface.component.css'],
  providers: [],
  encapsulation: ViewEncapsulation.None
})

export class MainInterfaceComponent implements OnInit {
  ecIntance: any;
  public chartOption: any = {
    title: {
      show: false
    },
    tooltip: {},
    animationDurationUpdate: 1500,
    coordinateSystem: 'cartesian2d',
    animationEasingUpdate: 'quinticInOut',
    series: [
      {
        type: 'graph',
        layout: 'none',

        roam: true,
        label: {
          normal: {
            show: true
          }
        },
        edgeSymbol: ['circle', 'arrow'],
        edgeSymbolSize: [4, 10],
        edgeLabel: {
          normal: {
            textStyle: {
              fontSize: 20
            }
          }
        },
        data: [{
          name: '分布式发电预测',
          x: 125,
          y: 0,
          symbol: 'Rect',
          symbolSize: [300, 150],
          label: {
            fontSize: 28
          }
        }, {
          name: '馈线负荷预测',
          x: 150,
          y: 0,
          symbol: 'Rect',
          symbolSize: [300, 150],
          label: {
            fontSize: 28
          }
        }, {
          name: '综合能源网络仿真',
          x: 137.5,
          y: 17,
          symbol: 'Rect',
          symbolSize: [550, 150],
          label: {
            fontSize: 28
          }
        }, {
          name: '用户细分与精准营销',
          x: 137.5,
          y: 34,
          symbol: 'Rect',
          symbolSize: [550, 150],
          label: {
            fontSize: 28
          }
        }],
        // links: [],
        links: [
          {
            source: '分布式发电预测',
            target: '综合能源网络仿真'
          },
          {
            source: '馈线负荷预测',
            target: '综合能源网络仿真'
          },
          {
            source: '用户细分与精准营销',
            target: '综合能源网络仿真'
          }],
        lineStyle: {
          normal: {
            opacity: 0.9,
            width: 2,
            curveness: 0
          }
        }
      }
    ]
  }
  constructor(private router: Router) {
    this.router = router
  }
  ngOnInit() {

  }
  chartInit(ec) {
    this.ecIntance = ec;
    this.ecIntance.on('click', $ele => {
      if ($ele.data.name == "馈线负荷预测") {
        this.router.navigate(["/data/forecast/visualization"])
        // location.replace("/data/forecast/visualization")
      }else if($ele.data.name == "综合能源网络仿真"){
        this.router.navigate(["/view/welcome"])
      }else if($ele.data.name == "用户细分与精准营销"){
        this.router.navigate(["/data/precisionmarketing"])
      }
      
    });
  }
}