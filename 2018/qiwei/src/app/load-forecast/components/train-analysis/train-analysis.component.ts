import { Component, OnInit, ViewChild, AfterViewInit, OnChanges, SimpleChanges, Input, OnDestroy, Inject } from '@angular/core';
import { PreprocessService } from '../../services/preprocess.service';
// import { IdSelectorComponent } from './id-selector.component';
import { ActivatedRoute, Params } from '@angular/router';
import { Location } from '@angular/common';
import { SelectItem } from 'primeng/components/common/api';
import { Message } from 'primeng/components/common/api';
import { MessageService } from 'primeng/components/common/messageservice';
import { ReferenceService } from '../../services/reference.service';
import {MAT_DIALOG_DATA, MatDialogRef} from '@angular/material';

// import { DataSource } from '@angular/cdk';

import 'rxjs/add/operator/switchMap';
import 'rxjs/add/operator/toPromise';

import { TrainResultChartComponent } from '../train-result-chart/train-result-chart.component'

import * as highcharts from 'highcharts';
// require('highcharts-annotations')(highcharts);

import { TrainService } from '../../services/train.service';
import { PredictService } from '../../services/predict.service';
import { DbinquiryService } from '../../services/dbinquiry.service';
import { ModelService } from '../../services/model.service';
import { LoadforecastNavService } from '../../services/loadforecast-nav.service';
import { CommonService } from '../../services/common.service';

import { PreprocessTrainService } from '../../services/preprocess-train.service';
import { PreprocessTrainLogService } from '../../services/preprocess-train-log.service';
import { Subscription } from 'rxjs/Subscription';
require('highcharts-annotations')(highcharts);

@Component({
  selector: 'app-train-analysis',
  templateUrl: './train-analysis.component.html',
  styleUrls: ['./train-analysis.component.css'],
  providers: [MessageService, ReferenceService, DbinquiryService]
})
export class TrainAnalysisComponent implements OnInit, AfterViewInit {
  selectedmeter: any;
  @ViewChild(TrainResultChartComponent) trainResultChart;

  preprocesstrainSubscription:Subscription;
  iH = 700;
  errorTableH: string;
  ErroTableDivH:number;
  messages: Message[] = [];
  selectedModel = null;
  selectedNorm = null;
  norms: any[];
  models: any[];
  // trainResultChart.errStats: any[];
  // trainResultChart.resultChart:any;
  // test1:boolean=true;
  // test2:boolean

  // preprocessCheck:boolean=true;
  // trainCheck:boolean=true;
  hyperParamSetting:string;
  maxTrainTime:number;
  njobs:number;
  logDisplay:boolean=false;
  errorFull:any[]=[];
  maxError:any[];
  minError:any[];
  maxRank:any=20;
  minRank:any=20;
  selectedErrPoints:any;
  detailDisplay:boolean=false;
  detailErr:any[];
  options:any;
  detailChart:any;
  detailDate:string='2013-07-01'
  infoDisplay:boolean=false;
  savedModelInfo: any[];
  selectedSavedModel: string;
  modelList: any[];
  addedWeather:boolean = false;
  addedWeatherDetail:boolean = false;
  detailPointDayPos:any;
  detailPointInd:any;

  constructor(
    private route: ActivatedRoute,
    @Inject(MAT_DIALOG_DATA) public data: any,
    private location: Location,
    private preprocessService: PreprocessService,
    private commonService: CommonService, 
    private trainService: TrainService,
    private predictService: PredictService,
    private modelService: ModelService,
    private messageService: MessageService,
    private dbinquirySerice: DbinquiryService,
    private preprocessTrainService: PreprocessTrainService,
    private preprocessTrainLogService: PreprocessTrainLogService,
    private loadforecastnavService: LoadforecastNavService,
    private referenceService: ReferenceService) {
      this.errorTableH = 325 + "px";
      this.ErroTableDivH = 300;
      // this.preprocesstrainSubscription = preprocessTrainService.getTrainParam().subscribe(trainParam => {
      //   this.selectedNorm = trainParam.selectedNorm;
      //   this.selectedModel = trainParam.selectedModel;
      //   this.maxTrainTime = trainParam.maxTrainTime;
      //   this.hyperParamSetting = trainParam.hyperParamSetting;
      //   this.njobs = trainParam.njobs;
      // });
      this.options = {title: {text:"Prediction Result"},
      annotationsOptions:{enabledButtons:false},
      annotations:[{
          anchorX:'center',
          anchorY:'top',
          title: {
              text: '<span>00:00<br>predict:<br>real:<br>error:<br></span>',
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
          }
        }],
        chart: {zoomType: 'x',
                // width:this.iW*0.47,
                panning: true,
                panKey: 'shift',
                resetZoomButton: {
                  position: {
                      align: 'left'
                  }
              }
                },
        yAxis: [{linewidth:1,
                name:"Load (MW)",
                title:{text:"Load (MW)"},
                opposite: false
                },
                {linewidth:1,
                plotLines: [{value: 0,width: 2,color: 'silver'}],
                name:"Weather Parameters",
                title:{text:"Weather Parameters"},
                opposite: true,
                visible:false
                }
                ],
        xAxis: {type:'datetime'},
        legend: {enabled: true,
                align: 'center',
                backgroundColor: 'white',
                borderColor: 'black',
                borderWidth: 2,
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
        series: [{data:[],type:'line',name:"load_real",id:"load_real"},
          {data:[],type:'line',name:"load_modeled",id:"load_modeled"}]
        };
      this.selectedModel = data.selectedModel;
      this.selectedSavedModel = data.selectedModel;
  }

  ngOnInit(): void {
    //this.getModels();
    
  }
  ngAfterViewInit(): void {
    this.detailChart = highcharts.chart('container-detail-chart', this.options);
    this.trainAnalysis();
  } 

  getModels(): void {
    if (this.selectedmeter == null) {
        this.modelService.getModels()
            .then(models => {
                this.modelList = [];
                for (let i =models.length-1;i>=0;i--) {
                    this.modelList.push({ label: models[i].model_referenceid, value: models[i].model_referenceid })
                };
                this.selectedSavedModel = models[models.length - 1].model_referenceid;
                this.selectedmeter = this.selectedSavedModel.split('_')[0];
                // console.log(this.selectedmodel);
            })
            .catch(this.handleError);
    } else {
        this.modelService.getModel(this.selectedmeter)
            .then(models => {
                this.modelList = [];
                for (let i =models.length-1;i>=0;i--) {
                    this.modelList.push({ label: models[i].model_referenceid, value: models[i].model_referenceid })
                };
                // console.log(this.modelList);
                if (models.length>0) {
                    this.selectedSavedModel = models[models.length - 1].model_referenceid;
                    // console.log(this.selectedmodel);
                } else {
                    this.selectedSavedModel = null;
                    // console.log('no trained model exsited for this meterid')
                }
            })
            .catch(this.handleError);
    }
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



  goBack(): void {
    this.location.back();
  }

  trainAnalysis () {
    this.addedWeather = false;
    this.trainResultChart.reinit(this.selectedSavedModel);
    this.trainResultChart.resultChart.showLoading();
    this.predictService.TrainPredict(this.selectedSavedModel,null,null)
    .then(res => {            
      //compose table data
      this.errorFull =[];
      for (let i=0;i<res.ape.length;i++) {
        if (res.ape[i] != null) {
          this.errorFull.push({time:this.transDate(new Date(res.predict_data[i][0])),
            modeled:res.predict_data[i][1],real:res.real_data[i][1],ape:res.ape[i],index:i})
        }
    };
      // chart show
      this.trainResultChart.resultChart.addSeries({data:res.real_data,type:'line',name:'真实负荷值(MW)',id:'真实负荷值(MW)'},false);
      this.trainResultChart.resultChart.addSeries({data:res.predict_data,type:'line',name:'预测负荷值(MW)',id:'预测负荷值(MW)'},false);
      this.trainResultChart.errStats = res.err_stats;
      // sort error data
      this.errorFull.sort(function(a,b){return b.ape-a.ape})
  })
  .then(res => {
    this.trainResultChart.resultChart.redraw();
    this.trainResultChart.resultChart.hideLoading();
    this.maxError = this.errorFull.slice(0,this.maxRank);
    this.minError = this.errorFull.slice(this.errorFull.length-this.minRank,this.errorFull.length).reverse();
  })
  .catch(this.handleError);
  }

  trainNew () {
    this.loadforecastnavService.changeIndex(1);
  }


  runTrainAnalysis(e) {
    this.detailPointDayPos = null;
    this.detailPointDayPos = null;
    this.selectedSavedModel = e;
    this.trainAnalysis();
  }

  refresh() {
      // this.getNormalizers();
  }

  onAnnotationClick(a) {
    // TODO: Uncomment it if this feature is needed
    // console.log(a[0]);
    // this.detailDisplay = true;
    // this.detailChart.annotations.allItems[0].hide(false);
    // while(this.detailChart.series.length > 0){
    // this.detailChart.series[0].remove(false)};
    // this.detailChart.showLoading();
    // let modeledDatailData = this.trainResultChart.resultChart.get('load_modeled').options.data.slice(this.detailPointInd-this.detailPointDayPos,this.detailPointInd+24-this.detailPointDayPos),
    // realDatailData = this.trainResultChart.resultChart.get('load_real').options.data.slice(this.detailPointInd-this.detailPointDayPos,this.detailPointInd+24-this.detailPointDayPos);
    // //update detail error table data
    // this.detailErr = [];
    // for (let i=0;i<24;i++) {
    //   this.detailErr.push({time:i+':00',predict:modeledDatailData[i][1],real:realDatailData[i][1],ape:Math.abs((modeledDatailData[i][1]-realDatailData[i][1])/realDatailData[i][1]*100).toFixed(3)});
    // }
    // //update detail chart plot data
    // this.detailChart.addSeries({data:realDatailData,type:'line',name:'load_real',id:'load_real'},false);
    // this.detailChart.addSeries({data:modeledDatailData,type:'line',name:'load_modeled',id:'load_modeled'},false);
    // //updae highlighted points
    // this.detailChart.redraw();
    // this.detailChart.series[0].points[this.detailPointDayPos].select(true, false);
    // this.detailChart.series[1].points[this.detailPointDayPos].select(true, true);
    // //update annotation
    // this.detailChart.annotations.allItems[0].update(a[0],false);
    // this.detailChart.annotations.allItems[0].show(false);
    // //redraw chart
    // this.detailChart.redraw();
    // this.detailChart.hideLoading();
  }

  onErrTableRowSelect(e) {
    this.addedWeatherDetail = false;
    this.detailDate = e.data.time.slice(0,10);
    this.detailPointDayPos = parseInt(e.data.time.slice(11,13));
    this.detailPointInd = e.data.index;
    //update highlighted points
    this.trainResultChart.resultChart.series[0].points[this.detailPointInd].select(true, false);
    this.trainResultChart.resultChart.series[1].points[this.detailPointInd].select(true, true);
    //update annotation
    let infotip = "<span>"+e.data.time+"<br>predict:"+e.data.modeled+"<br>real:"+e.data.real+"<br>error:"+e.data.ape+"</span>";
    this.trainResultChart.resultChart.annotations.allItems[0].update(
      {xValue:this.trainResultChart.resultChart.series[0].data[e.data.index].x,
       yValue:Math.max(this.trainResultChart.resultChart.series[0].data[e.data.index].y,this.trainResultChart.resultChart.series[1].data[e.data.index].y),
       title:{text:infotip}},
      false);
    this.trainResultChart.resultChart.annotations.allItems[0].show(false);
    this.trainResultChart.resultChart.redraw();
    if (this.detailDisplay) {
      this.onAnnotationClick(this.trainResultChart.resultChart.options.annotations);
    }
  }

  onErrTableRowUnselect(e) {
    this.detailDisplay = false;
    this.addedWeatherDetail = false;
  }

  maxRankInput() {
    this.maxError = this.errorFull.slice(0,this.maxRank);
  }

  minRankInput() {
    this.minError = this.errorFull.slice(this.errorFull.length-this.minRank,this.errorFull.length).reverse();
  }

  onDetailsButtonClick() {
    this.infoDisplay = true;
    this.dbinquirySerice.getModelInfo(this.selectedSavedModel)
    .then(res=> {this.savedModelInfo=res;})
    .catch(this.handleError);
  }

  onResetZoomClick() {
    this.trainResultChart.resultChart.zoomOut()
  }

  onModelSelect() {

  }

  addWeather(chart) {
    chart.showLoading();
    let loadStart = this.transDate(new Date(chart.series[0].data[0].x)),
    loadEnd = this.transDate(new Date(chart.series[0].data[chart.series[0].data.length-1].x))
    // console.log(loadStart,loadEnd);
    this.referenceService.getWeatherReference(loadStart,loadEnd)
    .then(res => {res.forEach(series => {
      series["id"] = this.commonService.dict[series["id"]];
      series["name"] = this.commonService.dict[series["name"]];
      chart.addSeries(series,false)
    })})
    .then(res => {chart.hideLoading();
      chart.redraw();})
    .catch(this.handleError);
  }

  onCompareWeatherClick() {
    if (!this.addedWeather) {
      this.addWeather(this.trainResultChart.resultChart)
    };
    this.addedWeather = true;
  }

  onCompareWeatherDetailClick() {
    if (!this.addedWeatherDetail) {
      this.addWeather(this.detailChart)
    };
    this.addedWeatherDetail = true;
  }

  ngOnDestroy() {
    // unsubscribe to ensure no memory leaks
    // this.preprocesstrainSubscription.unsubscribe();
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

  handleError = (error: any) => {
    try {
      this.logDisplay = false;
      this.trainResultChart.resultChart.hideLoading();
    }
    finally {
      this.showMsg(true, 'Error', JSON.parse(error._body).message || error);
    }
  }

}
