import { Component, OnInit, ViewChild, AfterViewInit, OnChanges, SimpleChanges, ViewEncapsulation, OnDestroy } from '@angular/core';
import { Router } from '@angular/router';
import { GraphChartService } from '../../../app-shared/services/graph-chart.service';
import { ReferenceService } from '../../services/reference.service';

import { LoaddataComponent } from '../loaddata/loaddata.component';
import { PreprocessTrainComponent } from '../preprocess-train/preprocess-train.component';
import { TrainAnalysisComponent } from '../train-analysis/train-analysis.component';
import { PredictComponent } from '../predict/predict.component';

import { LoadforecastNavService } from '../../services/loadforecast-nav.service';
import { PreprocessTrainService } from '../../services/preprocess-train.service';
import { PreprocessTrainLogService } from '../../services/preprocess-train-log.service';
import { Subscription } from 'rxjs/Subscription';
import { Message } from 'primeng/components/common/api';
import { MessageService } from 'primeng/components/common/messageservice';
import { CommonService } from '../../services/common.service';
import { Daterangepicker } from 'ng2-daterangepicker';
import { DaterangepickerConfig } from 'ng2-daterangepicker';

import { BigDataService } from '../../../data-analysis/services/big-data.service';

@Component({
  selector: 'app-load-forecast',
  templateUrl: './load-forecast.component.html',
  styleUrls: ['./load-forecast.component.css'],
  providers: [ReferenceService,
    LoadforecastNavService, PreprocessTrainService, PreprocessTrainLogService, CommonService, BigDataService],
  encapsulation: ViewEncapsulation.None
})
export class LoadForecastComponent implements OnInit, AfterViewInit, OnDestroy {
  @ViewChild('container') container: any
  @ViewChild(LoaddataComponent) loaddata;
  @ViewChild(PreprocessTrainComponent) preprocesstrain;
  @ViewChild(TrainAnalysisComponent) trainanalysis;
  @ViewChild(PredictComponent) predict;
  public daterange: any = {};

  // see original project for full list of options
  // can also be setup using the config service to apply to multiple pickers

  messages: Message[] = [];
  pageIndexSubscription:Subscription;
  subscription: Subscription;
  index:number=0;
  iH = window.innerHeight-126;
  chart:any;
  private graph;
  meters: any[];
  dateRange
  selectedmeter: any = 1100007;
  defaultDate = new Date('01/01/2011');
  startdate:Date = new Date('07/01/2013');
  enddate:Date = new Date('07/31/2013');
  yearRange:string = "2011:2014";
  minDate:Date = new Date('01/01/2011');
  maxDate:Date = new Date('11/30/2014');
  parameterStartDate = new Date('07/01/2013');
  parameterEndDate = new Date('07/31/2013');
  reinitSubscription: Subscription;
  //Bowen
  options1: Object;
  options5: Object;
  sdate: Date;
  edate: Date;
  display: boolean=false;
  public flag: any;
  public options: any = {
    locale: { format: 'YYYY-MM-DD' },
    alwaysShowCalendars: false,
    startDate: this.startdate,
    endDate: this.enddate,
    minDate: this.minDate,
    maxDate: this.maxDate
    };
  constructor(
    private router: Router,
    private graphChartService: GraphChartService,
    private loadforecastnavService: LoadforecastNavService,
    private preprocessTrainService: PreprocessTrainService,
    private preprocessTrainLogService: PreprocessTrainLogService,
    private referenceService: ReferenceService,
    private bigdataserive: BigDataService,
    private commonService: CommonService) {
    this.pageIndexSubscription = loadforecastnavService.getIndex().subscribe(index => {this.index=index;});
    this.meters = [];
    this.subscription = this.graphChartService.graphChart$.subscribe(chart => {
        console.log("chart loaded!");
    })
    this.reinitSubscription = this.commonService.loaddataReinitAnnounced$.subscribe(event => {
        this.reinit();
    });
    this.referenceService.getIdMatch()
        .then(res=>{this.meters=res.result})
        .catch(this.handleError);

    //Bowen
    this.options1 = {
        title: { text: 'LoadPoint 负荷分布' },
        chart: {
          zoomType: 'x',
          height: 200,
          width: 600
        },
        xAxis: {
          categories: []
        },
        yAxis: {
          title: {
            text: " "
          }
        },
        series: [{
          showInLegend: false,
          type: 'column',
          data: [0],
          allowPointSelect: true
        }
        ]
      };
 
  }

  ngOnInit() {

  }

  ngAfterViewInit() {
    // this.announceParameterChange(this.selectedmeter, this.parameterStartDate, this.parameterEndDate);
  }

  reinit() {
    this.options = {
        locale: { format: 'YYYY-MM-DD' },
        alwaysShowCalendars: false,
        startDate: this.startdate,
        endDate: this.enddate,
        minDate: this.minDate,
        maxDate: this.maxDate
        };
      this.selectedmeter = 1100007;
      this.announceParameterChange(this.selectedmeter, this.parameterStartDate, this.parameterEndDate);
  }

  public selectedDate(value: any, datepicker?: any) {
    this.parameterStartDate = value.start.toDate();
    this.parameterEndDate = value.end.toDate();
    this.announceParameterChange(this.selectedmeter, this.parameterStartDate, this.parameterEndDate);
  }

  onMeterDropdownSelect() {

    this.announceParameterChange(this.selectedmeter, this.parameterStartDate, this.parameterEndDate);
  }


  announceParameterChange(selectedMeter, startDate, endDate) {
      this.commonService.announceLoaddataParameter({'selectedMeter': selectedMeter, 'startDate': startDate, 'endDate':endDate});
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

 onTabChange(e): void {
    switch (e.index) {
        case 0:
            this.index = 0;
            break
        case 1:
            this.index = 1;
            break
        case 2:
            this.index = 2;
            this.trainanalysis.refresh();
            break
        case 3:
            this.index = 3;
            this.predict.refresh();
    }
 }

 onRowSelect(e){
    console.log(e);
    console.log(e.data.user);
    this.bigdataserive.PlotEvery(e.data.user, this.sdate.getTime(), this.edate.getTime())
    .then(line => {
       console.log(line[0]);
       this.options5 = line[0];
       this.options5["chart"]["height"]=600;
       this.options5["chart"]["width"]=600;
    })
    this.display = true;
    //Added By Bowen
    console.log(e.data.user);
    this.flag = 0;
  }


 ngOnDestroy() {
    // unsubscribe to ensure no memory leaks
    this.pageIndexSubscription.unsubscribe();
    this.subscription.unsubscribe();
    // this.chart.off("onClick");
}

 test(){
     console.log(this.index);
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
    console.log(error._body);
    this.showMsg(true, 'Error', JSON.parse(error._body).message || error);
}
toIESIndex() {
    this.router.navigate(["IES/main"])
}



}

