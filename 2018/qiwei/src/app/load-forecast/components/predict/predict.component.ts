// tslint:disable-next-line:max-line-length
import { Component, OnInit, Input, OnDestroy, Inject, OnChanges, ViewChild, SimpleChange, AfterViewInit, ViewEncapsulation, Output, EventEmitter } from '@angular/core';
import { ActivatedRoute, Params } from '@angular/router';
import { Location } from '@angular/common';
import { Message } from 'primeng/components/common/api';
import { MessageService } from 'primeng/components/common/messageservice';

import { ModelService } from '../../services/model.service';
import { PredictService } from '../../services/predict.service';
import { DbinquiryService } from '../../services/dbinquiry.service';
import { TrainService } from '../../services/train.service';
import { DatePipe } from '@angular/common'
import { ReferenceService } from '../../services/reference.service';
import { CalStatsService } from '../../services/cal-stats.service';
import { PreprocessTrainLogService } from '../../services/preprocess-train-log.service';
import { LoadforecastNavService } from '../../services/loadforecast-nav.service';
import { CommonService } from '../../services/common.service';
import { Subscription } from 'rxjs/Subscription';
import { MatDialogModule, MatDialog, MatDialogRef, MAT_DIALOG_DATA } from '@angular/material';
import { PreprocessTrainComponent } from '../preprocess-train/preprocess-train.component';
import { TrainAnalysisComponent } from '../train-analysis/train-analysis.component';
import 'rxjs/add/operator/switchMap';
import 'rxjs/add/operator/toPromise';

import * as highcharts from 'highcharts';
require('highcharts-annotations')(highcharts);


@Component({
    selector: 'app-predict',
    templateUrl: './predict.component.html',
    styleUrls: ['./predict.component.css'],
    providers: [MessageService, ReferenceService, DbinquiryService, DatePipe, MatDialogModule, MatDialog],
    encapsulation: ViewEncapsulation.None
})


export class PredictComponent implements OnInit, AfterViewInit, OnDestroy {

    @Input() selectedmeter: any;
    @Output() redirectTrainAnalysis = new EventEmitter<any>();
    // @ViewChild('container1') container1: any;

    iH = window.innerHeight - 145;
    errorTableH: string;
    prevTableH: number;
    chartH: number;
    selectedSavedModel: string;
    date: Date = new Date('01/01/2014');
    modelList: any[];
    stats: any[];
    public predictResults: any[];
    messages: Message[] = [];

    defaultDate = new Date('01/01/2014');
    yearRange: string = "2011:2014";
    minDate: Date = new Date('01/01/2011');
    maxDate: Date = new Date('11/30/2014');

    savedModelInfo: any[] = [];
    detailsDisplay: boolean = false;
    public predictResultChart: any;

    options: any;
    selectedErrPoints: any;
    prevResults: any[];
    prevPredictModels: any[];
    prevPredictDates: any[];

    requestParaSubscription: Subscription;
    loadforecastSubscription: Subscription;

    addedWeather: boolean = false;
    logDisplay = false;
    selectedMeter: any;
    startDate: any;
    endDate: any;
    trainRes: any = {};

    constructor(
        public dialog: MatDialog,
        public datepipe: DatePipe,
        private route: ActivatedRoute,
        private trainService: TrainService,
        private messageService: MessageService,
        private predictService: PredictService,
        private preprocessTrainLogService: PreprocessTrainLogService,
        private modelService: ModelService,
        private location: Location,
        private referenceService: ReferenceService,
        private loadforecastnavService: LoadforecastNavService,
        private dbinquirySerice: DbinquiryService,
        private commonService: CommonService) {
        this.requestParaSubscription = this.commonService.loaddataParameterAnnounced$.subscribe(parameter => {
            this.selectedMeter = parameter['selectedMeter'];
            this.startDate = parameter['startDate'];
            this.endDate = parameter['endDate'];
            if (this.predictResultChart == null) {
                this.getModels(false, this.selectedMeter).then(res => {
                    this.predict();
                });
            } else {
                this.getModels(false, this.selectedMeter);
            }
        });
        this.chartH = this.iH - 170
        this.errorTableH = this.iH - 250 + "px";
        this.prevTableH = this.iH - 170;
    }

    ngOnInit(): void {

    }
    ngAfterViewInit(): void {
        this.commonService.announceLoaddataReinit();
    }

    ngOnDestroy(): void {
        this.requestParaSubscription.unsubscribe();
    }

    // test(): Promise<any> {
    //     return new Promise ((resolve,reject) => {this.intilizaeLinechart();resolve(1)})
    // }

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

    predict(): Promise<any> {
        // this.logDisplay = false;
        this.initChart(this.selectedMeter, this.date, this.selectedSavedModel);
        this.addedWeather = false;
        this.predictResultChart.annotations.allItems[0].hide(false);
        while (this.predictResultChart.series.length > 0) {
            this.predictResultChart.series[0].remove(false)
        };
        this.predictResultChart.showLoading();
        return this.predictService.Predict(this.selectedSavedModel, this.transDate(this.date))
            .then(res => {
                this.predictResultChart.addSeries({ data: res.real_data, type: 'line', name: '真值负荷', id: '真值负荷' }, false);
                this.predictResultChart.addSeries({ data: res.predict_data, type: 'line', name: '预测负荷', id: '预测负荷' }, false);
                this.stats = res.err_stats;
                let temp = [];
                for (let i = 0; i < res.ape.length; i++) {
                    temp.push({ time: i + ':00', predict: res.predict_data[i][1], real: res.real_data[i][1], ape: res.ape[i] })
                };
                temp.sort(function (a, b) { return Number.parseInt(a.time.split(':')[0]) - Number.parseInt(b.time.split(':')[0]) })
                this.predictResults = temp;
            })
            .then(res => {
                this.predictResultChart.redraw();
                this.predictResultChart.setTitle("仪表" + this.selectedMeter + "负荷预测结果");
                this.predictResultChart.hideLoading()
            })
            .catch(this.handleError);

    }


    getModels(newModel, selectedMeter?: any): Promise<any> {
        if (selectedMeter == null) {
            return this.modelService.getModels()
                .then(models => {
                    this.modelList = [];
                    for (let i = models.length - 1; i >= 0; i--) {
                        this.modelList.push({ label: models[i].model_referenceid, value: models[i].model_referenceid })
                    };
                    this.selectedSavedModel = models[models.length - 1].model_referenceid;
                    this.selectedmeter = this.selectedSavedModel.split('_')[0];
                    this.onModelSelect();
                    // console.log(this.selectedmodel);
                })
                .catch(this.handleError);
        } else {
            return this.modelService.getModel(selectedMeter)
                .then(models => {
                    this.modelList = [];
                    for (let i = models.length - 1; i >= 0; i--) {
                        this.modelList.push({ label: models[i].model_referenceid, value: models[i].model_referenceid })
                    };
                    // console.log(this.modelList);
                    if (models.length > 0) {
                        if (!newModel) {
                            this.selectedSavedModel = models[models.length - 1].model_referenceid;
                        }
                        this.onModelSelect();
                        // console.log(this.selectedmodel);
                    } else {
                        this.selectedSavedModel = null;
                        // console.log('no trained model exsited for this meterid')
                    }
                })
                .catch(this.handleError);
        }
    }

    goBack(): void {
        this.location.back();
    }

    confirm(): void {
        this.predict();
        // Promise.all([this.predict(),this.getReference()])
        // .then(res => this.calStats());
    }

    listPrevPredict(): void {
        this.dbinquirySerice.getPrevPredict(this.selectedmeter)
            .then(res => {
                console.log(res);
                this.prevPredictModels = [];
                this.prevPredictDates = [];
                res.models.forEach(model => { this.prevPredictModels.push({ label: model, value: model }) });
                res.dates.forEach(date => { this.prevPredictDates.push({ label: date, value: date }) })
                this.prevResults = res.tableData;
            })
            .catch(this.handleError);
    }

    refresh() {
        this.getModels(false);
    }

    onDetialsButtonClick() {
        if (this.selectedSavedModel == null) {
            this.loadforecastnavService.changeIndex(1);
        } else {
            this.detailsDisplay = true;
        }
    }

    onModelSelect() {
        this.dbinquirySerice.getModelInfo(this.selectedSavedModel)
            .then(res => {
                this.savedModelInfo = res;
                // this.initChart(this.selectedMeter, this.date, this.selectedSavedModel);
            })
            .catch(this.handleError);
    }

    onErrTableRowSelect(e) {
        let ind = e.data.time.split(':')[0];
        this.predictResultChart.series[0].points[ind].select(true, false);
        this.predictResultChart.series[1].points[ind].select(true, true);
        let infotip = "<span>时间:" + e.data.time + "<br>预测值(MV):" + e.data.predict + "<br>真值(MV):" + e.data.real + "<br>误差:" + e.data.ape + "%</span>";
        this.predictResultChart.annotations.allItems[0].update(
            {
                xValue: this.predictResultChart.series[0].data[ind].x,
                yValue: Math.max(this.predictResultChart.series[0].data[ind].y, this.predictResultChart.series[1].data[ind].y),
                title: { text: infotip }
            },
            false);
        this.predictResultChart.annotations.allItems[0].show(false)
        this.predictResultChart.redraw();
    }

    onErrTableRowUnselect(e) {
        this.predictResultChart.annotations.allItems[0].hide(false);
        let ind = e.data.time.split(':')[0];
        this.predictResultChart.series[0].points[ind].selected = false;
        this.predictResultChart.series[1].points[ind].selected = false;
        this.predictResultChart.redraw();
    }

    onTrainAnalysisClick() {
        this.redirectTrainAnalysis.emit(this.selectedSavedModel);
        this.loadforecastnavService.changeIndex(2);
    }

    addWeather(chart) {
        chart.showLoading();
        let loadStart = this.transDate(new Date(chart.series[0].data[0].x)),
            loadEnd = this.transDate(new Date(chart.series[0].data[chart.series[0].data.length - 1].x))
        // console.log(loadStart,loadEnd);
        this.referenceService.getWeatherReference(loadStart, loadEnd)
            .then(res => {
                res.forEach(series => {
                    series["id"] = this.commonService.dict[series["id"]];
                    series["name"] = this.commonService.dict[series["name"]];
                    chart.addSeries(series, false);
                })
            })
            .then(res => {
                chart.hideLoading(); this.addedWeather = true;
                chart.redraw();
            })
            .catch(this.handleError);
    }

    onCompareWeatherClick() {
        if (!this.addedWeather) {
            this.addWeather(this.predictResultChart)
        }

    }




    initChart(selectedMeter, selectedDate, selectedModel) {
        var title = "";
        var subtitle = "";
        if (selectedModel == null) {
            title = "请选择模型名称进行预测"
        } else {
            let split_temp = selectedModel.split('_');
            let modelName = split_temp[1];
            let date = split_temp[2];
            let time = split_temp[3];
            let datetime = "20" + date.substring(0, 2) + "." + date.substring(2, 4) + "." + date.substring(4, 6) + " " + time.substring(0, 2) + ":" + time.substring(2, 4) + ":" + time.substring(4, 6);
            title = "用户 " + selectedMeter + " 负荷预测结果";
            subtitle = "预测日期：" + this.datepipe.transform(this.date, 'yyyy-MM-dd') + " " + "选用模型：" + modelName + " 创建时间： " + datetime
        }
        this.options = {
            title: { text: title },
            subtitle: { text: subtitle },
            annotationsOptions: { enabledButtons: false },
            annotations: [{
                anchorX: 'center',
                anchorY: 'top',
                title: {
                    text: '<span>00:00<br>预测值:<br>real:<br>预测误差百分比:<br></span>',
                    x: 0,
                    y: -80,
                    style: {
                        color: 'white',
                        fontWeight: 'bolder'
                    }
                },
                shape: {
                    type: 'rect',
                    params: {
                        x: 0,
                        y: -80,
                        fill: '#bfbfbf',
                        width: 85,
                        height: 65,
                        stroke: '#bfbfbf'
                    }
                }
            }],
            chart: {
                zoomType: 'x',
                // width:this.iW*0.47,
                panning: true,
                panKey: 'shift',
                resetZoomButton: {
                    position: {
                        align: 'left'
                    }
                }
            },
            yAxis: [{
                linewidth: 1,
                name: "负荷 (MW)",
                title: { text: "负荷 (MW)" },
                opposite: false
            },
            {
                linewidth: 1,
                plotLines: [{ value: 0, width: 2, color: 'silver' }],
                name: "Weather Parameters",
                title: { text: "Weather Parameters" },
                opposite: true,
                visible: false
            }
            ],
            xAxis: { type: 'datetime' },
            legend: {
                enabled: true,
                align: 'center',
                backgroundColor: 'white',
                borderColor: 'black',
                borderWidth: 2,
                layout: 'horizontal',
                verticalAlign: 'bottom',
                y: 0,
                shadow: true
            },
            plotOptions: {
                series: {
                    showInLegend: true,
                    allowPointSelect: true,
                    visible: true
                },
            },
            tooltip: {
                pointFormat: '<span style="color:{series.color}">{series.name}</span>: <b>{point.y}',
                valueDecimals: 2,
                split: true
            },
            series: [{ data: [], type: 'line', name: "load_predict", id: "load_predict" },
            { data: [], type: 'line', name: "load_real", id: "load_real" }]
        };
        this.predictResultChart = highcharts.chart('container1', this.options);
    }


    test() {
        console.log(this.predictResultChart)
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
            this.predictResultChart.hideLoading()
        }
        finally {
            this.showMsg(true, 'Error', JSON.parse(error._body).message || error);
        }
    }

    train(selectedNorm, selectedModel, maxTrainTime, hyperParamSetting, njobs): Promise<any> {
        this.addedWeather = false;
        // this.trainResultChart.clearChart();
        // this.trainResultChart.resultChart.showLoading();
        this.preprocessTrainLogService.updateResultTr('正在计算...');
        return this.trainService.Train(selectedNorm, selectedModel, maxTrainTime, hyperParamSetting, njobs)
            .then(res => {
                let sum;
                if (res.error) {
                    sum = 'Error';
                } else {
                    sum = 'Success';
                }
                this.preprocessTrainLogService.updateStatus("模型训练完成！")
                this.showMsg(res.error, sum, res.message);
                this.getModels(true, this.selectedMeter)
                this.preprocessTrainLogService.updateResultTr(res.model_referenceid);
                this.preprocessTrainLogService.updateResultTr2(res.message_details);
                this.trainRes = res;
                return res.model_referenceid
            })
            .catch(this.handleError);
    }


    trainNewModel() {
        let dialogRef = this.dialog.open(PreprocessTrainComponent, {
            width: '1000px',
            height: '600px',
            data: {
                startDate: this.startDate,
                endDate: this.endDate,
                selectedMeter: this.selectedMeter
            }
        });

        dialogRef.afterClosed().subscribe(result => {
            if (result == null) {
                return;
            }
            console.log("received info from modal: " + result);
            this.logDisplay = true;
            this.train(result.selectedNorm, result.selectedModel, result.maxTrainTime, result.hyperParamSetting, result.njobs)
                .then(res => {
                    this.selectedSavedModel = res;
                    this.predict();
                });
        });
    }

    showTrainAnalysis() {
        let dialogRef = this.dialog.open(TrainAnalysisComponent, {
            width: '1500px',
            height: '800px',
            data: {
                selectedModel: this.selectedSavedModel
            }
        })
    }



}


