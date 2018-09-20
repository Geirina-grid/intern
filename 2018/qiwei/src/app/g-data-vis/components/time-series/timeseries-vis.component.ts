import { Component, OnInit, ViewChild, AfterViewInit, OnChanges, SimpleChanges, OnDestroy } from '@angular/core';
import { Router } from '@angular/router';
import { WholeGraphService } from '../../../app-shared/services/whole-graph.service';
import { ConfirmDialogModule, ConfirmationService, ProgressBarModule} from 'primeng/primeng';
import { GraphManageService } from '../../../app-shared/services/graph-manage.service';
import { GraphChartService } from '../../../app-shared/services/graph-chart.service';
import { TimeseriesVisService } from '../../services/timeseries-vis.service'
import { Subscription } from 'rxjs/Subscription';
import { GlobalIdService } from '../../../app-shared/services/global-id.service';
import * as L from 'leaflet';

import * as gvis from 'gvis';
const GraphChart = (<any>gvis).GraphChart;
/*
declare namespace L {
  // there is a child namespace "vectorGrid"
  namespace vectorGrid {
    // which has a function call "slicer" that takes data and optional
    // configurations. To make it simple, we don't specify the input
    // and output types.
    export function slicer(data: any, options?: any): any;
  }
}
*/
@Component({
  selector: 'app-timeseries-vis',
  templateUrl: './timeseries-vis.component.html',
  styleUrls: ['./timeseries-vis.component.css'],
  providers: [TimeseriesVisService]
})
export class TimeseriesVisComponent implements OnInit, AfterViewInit, OnDestroy {

  // @ViewChild('topocontainer') container1: any;
  // @ViewChild('lineContainer1') container2: any;
  iH = window.innerHeight - 120;
  iW = Math.min(window.innerWidth, 2000);
  tab_sh: string;
  public chart: any;
  public line1: any;
  sdate: Date;
  edate: Date;
  defaultDates = new Date('01/01/2018');
  defaultDatee = new Date('12/31/2018');
  selectedmeter: number;
  meters: any;
  cols: any[];
  cols1: any[];
  cols2: any[];
  cols3: any[];
  difrank: any[];
  loadrank: any[];
  loadlowrank: any[];
  basicstats: any[];
  options1: any[];
  phaseNO: number[];
  private graph;
  subscription: Subscription;
  //Added by Bowen
  public map: any;
  subscriptionmap: Subscription;
  idSubscription: Subscription;
  public newlayer: any;
  public newlayer2: any;
  public lat: any;
  public lng: any;
  public latNew: any;
  public lngNew: any;
  public id: any;
  public flag: any;
  //End By Bowen
  public changedLat: any;
  public changedLng: any;
  calculating: boolean = false;


  constructor(
    private router: Router,
    private wholeGraphService: WholeGraphService,
    private confirmationService: ConfirmationService,
    private TimeseriesVisService: TimeseriesVisService,
    private graphChartService: GraphChartService,
    private idService: GlobalIdService
  ) {
    this.tab_sh = String(Math.round((this.iH - 220) / 2)) + "px";
    this.sdate = this.defaultDates;
    this.edate = this.defaultDatee;
    // this.selectedmeter = 1;
    this.meters = [];
    this.cols = [
      { field: 'power', header: '读数数值' },
      { field: 'phase', header: '相' },
      { field: 'time', header: '读数日期' }
    ];
    this.cols3 = [
      { field: 'power', header: '读数数值' },
      { field: 'phase', header: '相' },
      { field: 'time', header: '读数日期' }
    ];
    this.cols1 = [
      { field: 'Aa', header: 'A相平均' },
      { field: 'Ba', header: 'B相平均' },
      { field: 'Ca', header: 'C相平均' },
      { field: 'Am', header: 'A相最大' },
      { field: 'Bm', header: 'B相最大' },
      { field: 'Cm', header: 'C相最大' }
    ];

    this.subscription = this.graphChartService.graphChart$.subscribe(
      chart => {
        this.chart = chart;
        console.log(this.chart);
        if (this.chart !== undefined) {
          this.chart.on('onClick', (item) => {
            // Do something here. event call back.
            if (item !== undefined) {
              if (item.isNode) {
                // annouce selected id through id service to global
                this.idService.announceId(item.exID);
                // this.selectedmeter = item.exID;
                // this.plote();
              } else if (item.isLink) {
                console.log(`Link ${item.id} is clicked.`);
              }
            }
          })
        }
      }

    );
    //Added by Bowen
    this.subscriptionmap = this.graphChartService.map$.subscribe(
      map => {
        this.map = map;
        // console.log("Big data Outside of click...")
        if (this.map !== undefined) {
          // console.log("Big data inside of click-1");
          this.map.on('click', (item) => {
            // Do something here. event call back.
            // Example:
            // console.log("Big data inside of click-2");
            this.changedLat = item["latlng"]["lat"];
            this.changedLng = item["latlng"]["lng"];
            this.map.eachLayer((layer) => {
              if (layer['options']['color']) {
                if (this.changedLat == layer['_latlng']['lat'] && this.changedLng == layer['_latlng']['lng']) {
                  // annouce selected id
                  this.idService.announceId(layer['options']['name']);
                  // this.selectedmeter = layer['options']['name'];
                  // console.log('In BIG DATA');
                  // this.onMapUpdate();
                  // this.plote();
                }
              }
            })
          })
        }


      }
    );
    //End

    this.graphChartService.announceSubscription();
    this.graphChartService.announceMapSubscription();
    // subscribe id from id service
    this.idSubscription = this.idService.id$.subscribe(
      (id) => {
        // console.log('new selected id' + id + 'arrived');
        this.selectedmeter = id;
        this.calculating = true;
        this.plote();
      }
    )
  }
  ngOnInit() {
    // if (this.chart !== undefined) {
    //   this.initfunc();
    // }
  }
  ngAfterViewInit(): void {
    // this.plote();
    this.TimeseriesVisService.GetIDList().then(
      line => {
        line.result.forEach(element => {
          this.meters.push({ label: element, value: element })
        });
      }
    )
  }
  ngOnDestroy() {
    this.chart.off("onClick");
    // console.log("In Big Data test:")
    // console.log(this.map.hasEventListeners());
    this.map.off("click");
    console.log(this.map.hasEventListeners());
    // this.chart.off('onClick');
    this.subscription.unsubscribe();
    this.subscriptionmap.unsubscribe();
    this.idSubscription.unsubscribe();
  }
  plote(): void {
    console.log('plote')
    this.phaseNO=[];
    this.phaseNO.push(1);
    this.phaseNO.push(0);
    this.phaseNO.push(0);
    console.log(this.phaseNO);
    console.log(this.phaseNO[0]);
    console.log(this.phaseNO[2]);
    this.TimeseriesVisService.PlotTSD(this.selectedmeter, this.sdate.getTime(), this.edate.getTime(), this.phaseNO)
      .then(line => {
        // console.log(line[0]);
        this.calculating = false;
        line[0].chart.height = this.iH - 100;
        line[0].chart.width = this.iW / 2 - 20;
        console.log(line[0]);
        console.log(line[0].series[0].data[0][1]);
  
        this.options1 = line[0];
        this.loadrank = line[1];
        this.basicstats = line[3];
        this.loadlowrank = line[4];
        (<any>window).opt = this.options1;
      })
      .catch(this.handleError);
  }
  onRowSelect(e) {
    // console.log(e)
    this.TimeseriesVisService.Plot1Day(this.selectedmeter, e.data.time).then(
      res => {
        res[0].chart.height = this.iH - 100;
        res[0].chart.width = this.iW / 2 - 20;
        this.options1 = res[0];
        this.basicstats = res[1];
        (<any>window).opt = this.options1;
      }
    )
  }
  onRowUnselect(e) {
    this.plote();
  }
  initfunc() {
    this.chart.on('onClick', (item) => {
      // Do something here. event call back.
      // Example:
      if (item.isNode) {
        this.idService.announceId(item.exID);
        // console.log(item);
        // this.selectedmeter = item.exID;
        // this.plote();
      } else if (item.isLink) {
        console.log(`Link ${item.id} is clicked.`);
      }
    })
  }
  onChangeID(event) {
    this.idService.announceId(event.value);
  }

  private handleError(error: any) {
    console.error('An error occurred', error); // for demo purposes only
    alert(error.statusText);
  }


}
