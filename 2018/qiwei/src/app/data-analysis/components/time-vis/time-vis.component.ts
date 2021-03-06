import { Component, OnInit, ViewChild, AfterViewInit, OnChanges, SimpleChanges, OnDestroy } from '@angular/core';
import { Router } from '@angular/router';
import { WholeGraphService } from '../../../app-shared/services/whole-graph.service';
import { ConfirmDialogModule, ConfirmationService } from 'primeng/primeng';
import { GraphManageService } from '../../../app-shared/services/graph-manage.service';
import { BigDataService } from '../../services/big-data.service'
import { DataQualityService } from '../../services/data-quality.service';
import { GraphChartService } from '../../../app-shared/services/graph-chart.service';
import { Subscription } from 'rxjs/Subscription';
import { GlobalIdService } from '../../../app-shared/services/global-id.service';
import * as L from 'leaflet';
import * as gvis from 'gvis';
const GraphChart = (<any>gvis).GraphChart;

@Component({
  selector: 'app-time-vis',
  templateUrl: './time-vis.component.html',
  styleUrls: ['./time-vis.component.css'],
  providers: [DataQualityService]
  // styles: [`
  //   chart {
  //     display: block;
  //   }
  // `]
})
export class TimeVisComponent implements OnInit, AfterViewInit, OnDestroy {
  // @ViewChild('topocontainer') container1: any;
  iH = window.innerHeight - 120;
  iW = Math.min(window.innerWidth, 2000);

  h1 = this.iH / 5.3 + "px";
  h2 = (this.iH / 5.3 - 65) + "px"
  public chart: any;
  sdate: Date;
  edate: Date;
  defaultDates = new Date('01/01/2013');
  defaultDatee = new Date('12/31/2013');
  selectedmeter: number;
  selectedphase: any;
  phases: any;
  monthstats: any;
  daystats: any;
  hourstats: any;
  meters: any;
  met1: any;
  cols1: any[];
  cols2: any[];
  cols3: any[];
  private graph;
  options1: Object;
  options2: Object;
  options3: Object;
  options4: Object;
  sps: any;
  options_temp: Object;
  subscription: Subscription;
  idSubscription: Subscription;
  //Added by Bowen
  public map: any;
  subscriptionmap: Subscription;
  public newlayer: any;
  public newlayer2: any;
  public lat: any;
  public lng: any;
  public latNew: any;
  public lngNew: any;
  public id: any;
  public flag: any;
  public changedLat: any;
  public changedLng: any;
  //End By Bowen
  constructor(
    private router: Router,
    private wholeGraphService: WholeGraphService,
    private graphManageService: GraphManageService,
    private confirmationService: ConfirmationService,
    private bigdataserive: BigDataService,
    private DataQualityService: DataQualityService,
    private graphChartService: GraphChartService,
    private idService: GlobalIdService
  ) {
    this.cols1 = [{ field: 'phase', header: '相' },
    { field: 'maxm', header: '最大用电月份' },
    { field: 'lagc', header: '平滑度' },
    { field: 'sd', header: '方差' }]
    this.cols2 = [{ field: 'phase', header: '相' },
    { field: 'maxm', header: '最大用电日' },
    { field: 'lagc', header: '平滑度' },
    { field: 'sd', header: '方差' }]
    this.cols3 = [{ field: 'phase', header: '相' },
    { field: 'maxm', header: '最大用电时间' },
    { field: 'lagc', header: '平滑度' },
    { field: 'sd', header: '方差' }]

    this.options1 = {
      title: { text: 'Monthly Average' },
      chart: {
        zoomType: 'x',
        height: 300
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
    this.options2 = {
      title: { text: 'Daily Average' },
      chart: {
        zoomType: 'x',
        height: 300
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
    //let dat = []
    // this.data[j].forEach(dat.push())
    // this.options3.series.data=dat
    this.options3 = {
      title: { text: 'Hourly Average' },
      chart: {
        zoomType: 'x',
        height: 300
      },
      xAxis: {
        categories: [0]
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
      }]
    }
    this.options4 = {
      title: { text: '电能分布直方图' },
      chart: {
        zoomType: 'x',
        width: 420
      },
      xAxis: {},
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
      }]
    }
    this.selectedphase = 'load_a';
    // this.selectedmeter = 1;
    this.sdate = this.defaultDates;
    this.edate = this.defaultDatee;
    this.meters = [];
    this.met1 = [];
    this.phases = [{ label: "A", value: "load_a" }, { label: "B", value: "load_b" }, { label: "C", value: "load_c" }];
    this.subscription = this.graphChartService.graphChart$.subscribe(
      chart => {
        this.chart = chart;
        //console.log(this.chart);
        if (this.chart !== undefined) {
          this.chart.on('onClick', (item) => {
            // Do something here. event call back.
            // Example:
            if (item.isNode) {
              console.log(item);
              // console.log(this.met1.slice(1, ))
              // this.selectedmeter = item.exID;
              // this.plot_m();
              this.idService.announceId(item.exID);
            } else if (item.isLink) {
              console.log(`Link ${item.id} is clicked.`);
            }
          })
        }
      });
    //Added by Bowen
    this.subscriptionmap = this.graphChartService.map$.subscribe(
      map => {
        this.map = map;
        const map11 = this.map
        console.log("Timevis Outside of click...")
        //if (this.map !== undefined) {
        this.map.on('click', this.mapUpdate)
        console.log(this.map.hasEventListeners());
        console.log(this.map.listens());
        //}
        const map21 = this.map
        console.log(map11 === map21);
      }
    );
    //End
    this.graphChartService.announceSubscription();
    this.graphChartService.announceMapSubscription();
    this.idSubscription = this.idService.id$.subscribe(
      (id) => {
        this.selectedmeter = id;
        this.plot_m();
      }
    )
  }

  mapUpdate = (item) => {
    // Do something here. event call back.
    // Example:
    console.log("Inside of click");
    this.changedLat = item["latlng"]["lat"];
    this.changedLng = item["latlng"]["lng"];
    this.map.eachLayer((layer) => {
      if (layer['options']['color']) {
        if (this.changedLat == layer['_latlng']['lat'] && this.changedLng == layer['_latlng']['lng']) {
          this.idService.announceId(layer['options']['name']);
          // this.selectedmeter = layer['options']['name'];
          // this.onMapUpdate();
          // this.plot_m();
        }
      }
    })
  }


  ngOnInit() {
    // if (this.chart != undefined) {
    //   this.initfunc();
    // }
  }

  ngAfterViewInit() {
    this.plot_m();
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
    this.map.off();
    this.map.removeEventListener();
    this.map.clearAllEventListeners();
    this.subscription.unsubscribe();
    this.subscriptionmap.unsubscribe();
  }
  plot_m(): void {
    this.bigdataserive.PlotMonth(this.selectedmeter, this.selectedphase).then(
      line => {
        line[0].chart.height = this.iH / 3.3;
        line[0].chart.width = this.iW / 7 * 3;
        line[1].chart.height = this.iH / 3.3;
        line[1].chart.width = this.iW / 7 * 3;
        line[2].chart.height = this.iH / 3.3;
        line[2].chart.width = this.iW / 7 * 3;
        line[6].chart.height = this.iH * 0.4;
        this.options1 = line[0];
        this.options2 = line[1];
        this.options3 = line[2];
        this.monthstats = line[3];
        this.daystats = line[4];
        this.hourstats = line[5];
        this.options4 = line[6];
        this.options_temp = line[6];
      })
  }
  onPointSelect(e) {
    // console.log(e);
    this.bigdataserive.PlotDay(this.selectedmeter, this.selectedphase, e.context.category).then(
      line => {
        line[0].chart.height = this.iH / 3.3;
        line[1].chart.height = this.iH / 3.3;
        this.options2 = line[0];
        this.options3 = line[1];
        this.daystats = line[2];
        this.hourstats = line[3];
        this.options4 = line[4];
      }
    )
  }
  unselect() {
    this.bigdataserive.Nonservice(this.options_temp).then(res => {
      console.log(this.sps);
      this.options4 = res.hist;
    })
  }
  onPointSelect1(e) {
    this.bigdataserive.PlotHour(this.selectedmeter, this.selectedphase, e.context.series.name.substring(0, 8), e.context.category).then(
      line => {
        line[0].chart.height = this.iH / 3.3;
        this.options3 = line[0];
        this.hourstats = line[1];
      }
    )
  }
  initfunc() {
    this.chart.on('onClick', (item) => {
      // Do something here. event call back.
      // Example:
      if (item.isNode) {
        // console.log(item);
        console.log(this.met1.slice(1, ))
        this.selectedmeter = item.exID;
        this.plot_m();
      } else if (item.isLink) {
        console.log(`Link ${item.id} is clicked.`);
      }
    })
  }
  //Added By Bowen
  onChangeID(event) {
    this.idService.announceId(event.value);
    // this.flag = 0;
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
    // }his.flag = 0;
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
  // onMapUpdate() {
  //   this.flag = 0;
  //   this.map.eachLayer((layer) => {
  //     if (layer['options']['attribution'] != "Open Street Map") {
  //       if (layer['_latlng']) {
  //         if (layer['options']['name']) {
  //           if (layer['options']['name'] == this.selectedmeter) {
  //             this.flag = 1;
  //             this.latNew = layer['_latlng']['lat'];
  //             this.lngNew = layer['_latlng']['lng'];
  //             layer.remove();

  //           }
  //         }
  //       }
  //     }
  //   })
  //   if (this.flag == 1) {
  //     this.map.eachLayer((layer) => {
  //       if (layer['options']['color'] == 'chocolate') {
  //         this.id = layer['options']['name'];
  //         this.lat = layer['_latlng']['lat'];
  //         this.lng = layer['_latlng']['lng'];
  //         layer.remove();
  //       }
  //     })
  //   }
  //   if (this.flag == 1) {
  //     this.newlayer = L.circle([this.latNew, this.lngNew], {
  //       radius: 2000,
  //       name: this.selectedmeter,
  //       color: 'chocolate',
  //       fillColor: 'chocolate',
  //       opacity: 0.8,
  //       fillOpacity: 0.8
  //     }).addTo(this.map);
  //     this.newlayer.bindTooltip("ID: " + String(this.selectedmeter) + " Lat: " + String(this.latNew) + ", Lng: " + String(this.lngNew));
  //     this.map.addLayer(this.newlayer);

  //     this.newlayer2 = L.circle([this.lat, this.lng], {
  //       radius: 1000,
  //       name: this.selectedmeter,
  //       color: 'darkcyan',
  //       fillColor: 'darkcyan',
  //       opacity: 0.8,
  //       fillOpacity: 0.8
  //     }).addTo(this.map);
  //     this.newlayer2.bindTooltip("ID: " + String(this.selectedmeter) + " Lat: " + String(this.lat) + ", Lng: " + String(this.lng));
  //     this.map.addLayer(this.newlayer2);
  //   }
  // }

}
