import { Component, OnInit, Input, AfterViewInit, Output, EventEmitter } from '@angular/core';

import * as highstock from 'highcharts/highstock';
import * as highcharts from 'highcharts';
require('highcharts-annotations')(highcharts);
@Component({
  selector: 'app-train-result-chart',
  templateUrl: './train-result-chart.component.html',
  styleUrls: ['./train-result-chart.component.css']
})
export class TrainResultChartComponent implements OnInit, AfterViewInit {

  @Input() iH: any;
  @Output() onAnnotationClick = new EventEmitter<any>();
  
  chartH:any;
  options1:any;
  resultChart:any;
  errStats:any[];

  constructor() {
    this.options1 ={title: {text: '模型训练结果分析'},
    annotationsOptions:{enabledButtons:false},
    annotations:[{
        anchorX:'center',
        anchorY:'top',
        title: {
            text: '<span>00:00<br>预测值(MW):<br>真实值(MW):<br>误差百分比:<br></span>',
            x:0,
            y:-100,
            style:{
                color:'white',
                fontWeight:'bolder'
            }
        },
        shape: {
            type:'rect',
            params: {
                x:0,
                y:-100,
                fill:'#bfbfbf',
                width:120,
                height:65,
                stroke:'#bfbfbf'
            }
        },
        events: {
          click: (e)=> { this.onAnnotationClick.emit(this.options1.annotations); }
      }
      }],
    chart: {zoomType: 'x',
            panning: true,
            panKey: 'shift',
            resetZoomButton: {
              position: {
                  align: 'left'
              }
          }
            },
    navigator:{enabled:true,margin:2,height:35},
    scrollbar:{enabled:true},
    rangeselector:{enabled:false},
    yAxis: [{plotLines: [{value: 0,
                          width: 2,
                          color: 'silver'
                        }],
            name:"负荷(MV)",
            title:{text:"负荷(MV)"},
            opposite: false
            },
            {plotLines: [{value: 0,
              width: 2,
              color: 'silver'}],
  name:"天气参数",
  title:{'text':"天气参数"},
  opposite: true
}
          ],
    xAxis: {tickInterval: 16,
            type:'datetime'
          },
   legend: {enabled: true,
          align: 'center',
          backgroundColor: 'white',
          borderColor: 'black',
          borderWidth: 2,
          floating: false,
          layout: 'horizontal',
          verticalAlign: 'bottom',
          y: 0,
          shadow: true
          },   
    plotOptions: {series: {showInLegend: true,
                            allowPointSelect: false,
                            visible: true
                            },
                },
    tooltip: {pointFormat: '<span style="color:{series.color}">{series.name}</span>: <b>{point.y}',
              valueDecimals: 2,
              split: true
              },
    series: [{data:[],type:'line',name:"load_modeled",id:"load_modeled"},
    {data:[],type:'line',name:"load_real",id:"load_real"}]
  };
  }

  ngOnInit() {
    this.chartH = this.iH-120;
    this.resultChart = highcharts.chart('container-train-analysis', this.options1);
  }

  ngAfterViewInit(): void {
  //   highstock.setOptions({
  //     global: {
  //         timezoneOffset: -60
  //     }
  // });
    
  } 

  reinit(selectedModel) {
    var subtitle = "";
    if (selectedModel != null) {
        let split_temp = selectedModel.split('_');
        let modelName = split_temp[1];
        let date = split_temp[2];
        let time = split_temp[3];
        let datetime = "20" + date.substring(0, 2) + "." + date.substring(2, 4) + "." + date.substring(4, 6) + " " + time.substring(0, 2) + ":" + time.substring(2, 4) + ":" + time.substring(4, 6);
        subtitle = "选用模型：" + modelName + " 创建时间： " + datetime
    }
    this.options1["subtitle"] = { text: subtitle }
    this.resultChart = highcharts.chart('container-train-analysis', this.options1);
    while(this.resultChart.series.length > 0){
      this.resultChart.series[0].remove(false)};
  }

}
