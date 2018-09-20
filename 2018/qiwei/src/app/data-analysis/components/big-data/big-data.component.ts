import { Component, OnInit, ViewChild, AfterViewInit, OnChanges, SimpleChanges, OnDestroy } from '@angular/core';
import { Router } from '@angular/router';
import { WholeGraphService } from '../../../app-shared/services/whole-graph.service';
import { ConfirmDialogModule, ConfirmationService } from 'primeng/primeng';
import { GraphManageService } from '../../../app-shared/services/graph-manage.service';
import { GraphChartService } from '../../../app-shared/services/graph-chart.service';
import { DataQualityService } from '../../services/data-quality.service';
import { BigDataService } from '../../services/big-data.service'
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
  selector: 'app-big-data',
  templateUrl: './big-data.component.html',
  styleUrls: ['./big-data.component.css'],
  providers: [DataQualityService]
})
export class BigDataComponent implements OnInit, AfterViewInit, OnDestroy {

  // @ViewChild('topocontainer') container1: any;
  // @ViewChild('lineContainer1') container2: any;
  iH = window.innerHeight - 120;
  iW = Math.min(window.innerWidth, 2000);
  tab_sh: string;
  public chart: any;
  public line1: any;
  sdate: Date;
  edate: Date;
  defaultDates = new Date('01/01/2013');
  defaultDatee = new Date('12/31/2013');
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

  constructor(
    private router: Router,
    private wholeGraphService: WholeGraphService,
    private DataQualityService: DataQualityService,
    private confirmationService: ConfirmationService,
    private bigdataserive: BigDataService,
    private graphChartService: GraphChartService,
    private idService: GlobalIdService
  ) {
    this.tab_sh = String(Math.round((this.iH - 280) / 3)) + "px"
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
    this.cols2 = [
      { field: 'dif', header: '峰谷差' },
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
    this.DataQualityService.GetIDList().then(
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
    this.bigdataserive.PlotEvery(this.selectedmeter, this.sdate.getTime(), this.edate.getTime())
      .then(line => {
        // console.log(line[0]);
        line[0].chart.height = this.iH - 100;
        line[0].chart.width = this.iW / 2 - 20;
        this.options1 = line[0];
        this.loadrank = line[1];
        this.difrank = line[2];
        this.basicstats = line[3];
        this.loadlowrank = line[4];
        (<any>window).opt = this.options1;
      })
      .catch(this.handleError);
  }
  onRowSelect(e) {
    // console.log(e)
    this.bigdataserive.Plot1Day(this.selectedmeter, e.data.time).then(
      res => {
        this.options1 = res[0];
        this.basicstats = res[1];
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
    // this.flag = 0;
    // console.log(this.map);
    // this.map.eachLayer((layer) => {
    //   if (layer['options']['attribution'] != "Open Street Map") {
    //     if (layer['_latlng']) {
    //       if (layer['options']['name']) {
    //         if (layer['options']['name'] == this.selectedmeter) {
    //           this.flag = 1;
    //           this.latNew = layer['_latlng']['lat'];
    //           this.lngNew = layer['_latlng']['lng'];
    //           layer.remove();
    //         }
    //       }
    //     }
    //   }
    // })
    // if (this.flag == 1) {
    //   this.map.eachLayer((layer) => {
    //     if (layer['options']['color'] == 'chocolate') {
    //       this.id = layer['options']['name'];
    //       this.lat = layer['_latlng']['lat'];
    //       this.lng = layer['_latlng']['lng'];
    //       layer.remove();
    //     }
    //   })
    // }
    // if (this.flag == 1) {
    //   this.newlayer = L.circle([this.latNew, this.lngNew], {
    //     radius: 2000,
    //     name: this.selectedmeter,
    //     color: 'chocolate',
    //     fillColor: 'chocolate',
    //     opacity: 0.8,
    //     fillOpacity: 0.8
    //   }).addTo(this.map);
    //   this.newlayer.bindTooltip("ID: " + String(this.selectedmeter) + " Lat: " + String(this.latNew) + ", Lng: " + String(this.lngNew));
    //   this.map.addLayer(this.newlayer);

    //   this.newlayer2 = L.circle([this.lat, this.lng], {
    //     radius: 1000,
    //     name: this.id,
    //     color: 'darkcyan',
    //     fillColor: 'darkcyan',
    //     opacity: 0.8,
    //     fillOpacity: 0.8
    //   }).addTo(this.map);
    //   this.newlayer2.bindTooltip("ID: " + String(this.id) + " Lat: " + String(this.lat) + ", Lng: " + String(this.lng));
    //   this.map.addLayer(this.newlayer2);
    // }
  }

  private handleError(error: any) {
    console.error('An error occurred', error); // for demo purposes only
    alert(error.statusText);
  }


}
