import { Component, OnInit, ViewChild, AfterViewInit, OnChanges, SimpleChanges, Input, Output, Inject, EventEmitter } from '@angular/core';
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

import * as highstock from 'highcharts/highstock';

import { TrainService } from '../../services/train.service';
import { NormalizerService } from '../../services/normalizer.service';
import { DbinquiryService } from '../../services/dbinquiry.service';

import { LoadforecastNavService } from '../../services/loadforecast-nav.service';
import { PreprocessTrainService } from '../../services/preprocess-train.service';
import { PreprocessTrainLogService } from '../../services/preprocess-train-log.service';

// import { IdSelectorComponent } from './id-selector.component';

// import { SystemService } from './system.service';

@Component({
  selector: 'app-preprocess-train',
  templateUrl: './preprocess-train.component.html',
  styleUrls: ['./preprocess-train.component.css'], 
  providers: [MessageService, ReferenceService, DbinquiryService]
})

export class PreprocessTrainComponent implements OnInit, AfterViewInit {
  selectedmeter: any;
  startdate: Date;
  enddate: Date;
  @Output() pageRedirected = new EventEmitter<any>();

  iH = window.innerHeight-140;
  preprocessH:number;
  trainH:number;
  messages: Message[] = [];
  stats: any = null;
  selectedModel = null;
  selectedNorm = null;
  norms: any[];
  models: any[];

  startDate = new Date('07/01/2013');
  endDate = new Date('07/31/2013')
  defaultDate = new Date('01/01/2011');
  yearRange:string = "2011:2014";
  minDate:Date = new Date('01/01/2011');
  maxDate:Date = new Date('11/30/2014');
  options: any;
  outlierMin:number=0;
  outlierMax:number=null;
  scalerChoices:any[];
  numNormalizer:string="MinMaxScaler";
  catNormalizer:string="MaxAbsScaler";
  timeFeatures:any[];
  weatherFeatures:any[];
  timeChoices:any[]=[];
  weatherChoices:any[]=[];
  prevhr:string;
  prevhravg:string=null;
  prevhrmaxmin:string=null;
  preprocessSettingCheck1:boolean=true;
  preprocessSettingCheck2:boolean;
  trainSettingCheck:boolean=true;
  checkeddisabled:boolean=false;
  checked2disabled:boolean=false;
  detailsDisplay:boolean=false;
  savedNormInfo:any;
  preprocessCheck:boolean=true;
  trainCheck:boolean=true;
  hyperParamSetting:string="0";
  maxTrainTime:number=null;
  njobs:number=4;
  logDisplay:boolean=false;

  constructor(
    public dialogRef: MatDialogRef<PreprocessTrainComponent>,
    @Inject(MAT_DIALOG_DATA) public data: any,
    private route: ActivatedRoute,
    private location: Location,
    private preprocessService: PreprocessService,
    private trainService: TrainService,
    private normalizerService: NormalizerService,
    private messageService: MessageService,
    private dbinquiryService: DbinquiryService,
    private loadforecastnavService: LoadforecastNavService,
    private preprocessTrainService: PreprocessTrainService,
    private preprocessTrainLogService: PreprocessTrainLogService,
    private referenceService: ReferenceService) {
      this.preprocessH=(this.iH-34-210)*0.67;
      this.trainH=(this.iH-34-210)*0.33;
      this.scalerChoices=[
        {value:"StandardScaler",label:"StandardScaler"},
        {value:"MaxAbsScaler",label:"MaxAbsScaler"},
        {value:"MinMaxScaler",label:"MinMaxScaler"},
        {value:"Normalizer",label:"Normalizer"}];
      this.timeFeatures=['year','month','day','hour','day_of_week','holiday_or_not','workday_or_not','spring_or_not','summer_or_not','autumn_or_not','winter_or_not'];
      this.weatherFeatures=['temp_hour','hum_hour','rain_or_not','snow_or_not','thunderstorm_or_not','fog_or_not'];
      this.dbinquiryService.getTableCols('weather_sum')
      .then(data=>data.forEach(col=>this.weatherChoices.push({label:col,value:col})))
      .catch(this.handleError);
      this.dbinquiryService.getTableCols('calendar_sum')
      .then(data=>data.forEach(col=>this.timeChoices.push({label:col,value:col})))
      .catch(this.handleError);
      this.options = {
        locale: { format: 'YYYY-MM-DD' },
        alwaysShowCalendars: false,
        startDate: data.startDate,
        endDate: data.endDate,
        minDate: this.minDate,
        maxDate: this.maxDate
      };
      this.startdate = data.startDate;
      this.enddate = data.endDate;
      this.selectedmeter = data.selectedMeter;
      this.prevhr = '1-23,24,25,47,48,49,71,72,73,95,96,97,119,120,121,143,144,145,167,168,169';
      this.prevhravg = '3, 24, 168';
      this.prevhrmaxmin = '3, 24';
      this.models = [];
      this.models.push({ label: 'GradientBoostedTree', value: 'gbr' });
      this.models.push({ label: 'MultiLayerPerceptron', value: 'mlp' });
      this.models.push({ label: 'Ridge', value: 'ridge' });
      this.models.push({ label: 'SupportVectorRegression', value: 'SVR' });
      this.models.push({ label: 'XtremGradientBoostedTree', value: 'xgboost' });
      this.selectedModel = 'gbr';
  }


  selectedDate(value: any, datepicker?: any) {
    this.startdate = value.start.toDate();
    this.enddate = value.end.toDate();
  }

  ngOnInit(): void {
    this.getNormalizers();
    
  }
  ngAfterViewInit(): void {
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

  getNormalizers(): void {
    if (this.selectedmeter == null) {
      this.normalizerService.getNormalizers().then(data => {
        this.norms = [];
        for (let i =data.length-1;i>=0;i--) {
          this.norms.push({ label: data[i].normalier_referenceid, value: data[i].normalier_referenceid })
        };
        // console.log(this.norms);
        this.selectedNorm = data[data.length - 1].normalier_referenceid;
        this.selectedmeter = this.selectedNorm.split('_')[0];
        this.onNormSelect();
    })
    .catch(this.handleError);
    } else {
      this.normalizerService.getNormalizer(this.selectedmeter).then(data => {
        this.norms = [];
        for (let i =data.length-1;i>=0;i--) {
          this.norms.push({ label: data[i].normalier_referenceid, value: data[i].normalier_referenceid })
        };
        // console.log(this.norms);
        if (data.length > 0) {
          this.selectedNorm = data[data.length - 1].normalier_referenceid;
          this.onNormSelect();
        } else {
          this.selectedNorm = null;
          this.checked2disabled = true;
          // console.log('no preprocessed data for selected meterid');
        }
        
    })
    .catch(this.handleError);
    }
  }

  preprocess(): Promise<any> {
    if (!this.selectedmeter || !this.startdate || !this.enddate) {
      this.showMsg(true, 'Error', 'Parameter misses!');
    } else {
      // console.log(this.selectedmeter, this.transDate(this.startdate), this.transDate(this.enddate));
      this.preprocessTrainLogService.updateResultPreprocess('pending');;
      return this.preprocessService.dataPreprocess(this.selectedmeter, this.transDate(this.startdate), this.transDate(this.enddate),
    this.outlierMin, this.outlierMax, this.numNormalizer, this.catNormalizer,
    this.timeFeatures,this.weatherFeatures, this.prevhr, this.prevhravg, this.prevhrmaxmin)
        .then(res => {
          let sum;
          if (res.error) {
            sum = 'Error';
          } else {
            sum = 'Success';
          }
          this.showMsg(res.error, sum, res.message);
          this.getNormalizers();
          this.preprocessTrainLogService.updateResultPreprocess(res.normalizer);
          this.selectedNorm = res.normalizer;
        })
        .catch(this.handleError);
    }
  }

  run() {
    this.logDisplay = false;
    this.preprocessTrainLogService.updateStatus(null);
    this.preprocessTrainLogService.updateResultPreprocess(null);
    this.preprocessTrainLogService.updateResultTr(null);
    this.preprocessTrainLogService.updateResultTr2([]);
    this.preprocessCheck = this.preprocessSettingCheck1;
    // if (this.trainCheck) {
    //   if (!this.trainSettingCheck) {
    //     this.logDisplay = true;
    //     this.preprocessTrainLogService.updateStatus("Train setting not confirmed yet, click the 'confirm train setting' in the train section if confirmed")
    //   } else if (this.preprocessCheck) {
        
    //   } else {
    //     this.preprocessTrainLogService.updateStatus("Using saved processed data, start training directly......");
    //     this.loadforecastnavService.changeIndex(2);
    //     this.preprocessTrainService.changeTrainParam({
    //       selectedNorm: this.selectedNorm, 
    //       selectedModel: this.selectedModel, 
    //       maxTrainTime: this.maxTrainTime, 
    //       hyperParamSetting: this.hyperParamSetting, 
    //       njobs: this.njobs
    //     });
    //     this.pageRedirected.emit(true);
    //   }
    // } else if (this.preprocessCheck) {
    //   this.logDisplay = true;
    //   this.preprocessTrainLogService.updateStatus("Choose to preprocess only, start processing......")
    //   this.preprocess()
    //   .then(res=>{this.preprocessTrainLogService.updateStatus("Processing finished!")})
    //   .catch(this.handleError);
    // }
    this.logDisplay = true;
        this.preprocessTrainLogService.updateStatus("正在预处理训练数据...");
        this.preprocess()
        .then (res => {
          this.preprocessTrainService.changeTrainParam({
            selectedNorm: this.selectedNorm, 
            selectedModel: this.selectedModel, 
            maxTrainTime: this.maxTrainTime, 
            hyperParamSetting: this.hyperParamSetting, 
            njobs: this.njobs});
            this.preprocessTrainLogService.updateStatus("数据预处理完成！训练模型初始化...");
            this.dialogRef.close({
              selectedNorm: this.selectedNorm, 
              selectedModel: this.selectedModel, 
              maxTrainTime: this.maxTrainTime, 
              hyperParamSetting: this.hyperParamSetting, 
              njobs: this.njobs});
          })
          .catch(this.handleError);
    // close modal window and transfer data to predict component

  }


  refresh() {

  }

  test() {
    console.log(this.preprocessSettingCheck1);
  }

  onCheckboxChange(e) {
    if (e.checked) {
      this.preprocessCheck=true;
    } else {
      this.preprocessCheck=false;
    };
    this.preprocessSettingCheck2 = !e.checked
  }

  onCheckboxChange2(e) {
    if (e.checked) {
      this.preprocessCheck=false;
    } else {
      this.preprocessCheck=true;
    };
    this.preprocessSettingCheck1 = !e.checked
  }

  onDetialsButtonClick() {
    this.detailsDisplay = true;
  }

  onNormSelect() {
    this.dbinquiryService.getNormInfo(this.selectedNorm)
    .then(res=>this.savedNormInfo=res)
    .catch(this.handleError);
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
    }
    finally {
      this.showMsg(true, 'Error', JSON.parse(error._body).message || error);
    }
  }

}
