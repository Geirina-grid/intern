import { Component, OnInit, ViewChild, AfterViewInit, OnChanges, SimpleChanges, OnDestroy } from '@angular/core';
import { Router } from '@angular/router';
import { ActivatedRoute } from '@angular/router';
import { WholeGraphService } from '../../../app-shared/services/whole-graph.service';
import { ConfirmDialogModule, ConfirmationService } from 'primeng/primeng';
import { GraphManageService } from '../../../app-shared/services/graph-manage.service';
import { GraphChartService } from '../../../app-shared/services/graph-chart.service';
import { ClusteringService } from '../../services/clustering.service'
import { DataQualityService } from '../../services/data-quality.service';
import * as hc from 'highcharts';
import * as gvis from 'gvis';
const GraphChart = (<any>gvis).GraphChart;
const PieChart = (<any>gvis).PieChart;
import { platformBrowserDynamic } from '@angular/platform-browser-dynamic';
import { NgModule } from '@angular/core';
import { BrowserModule } from '@angular/platform-browser';
import { ChartModule } from 'angular2-highcharts';
import { Subscription } from 'rxjs/Subscription';
import { GlobalIdService } from '../../../app-shared/services/global-id.service';
import * as L from 'leaflet';

@Component({
  selector: 'app-clustering',
  templateUrl: './clustering.component.html',
  styleUrls: ['./clustering.component.css'],
  providers: [ClusteringService, DataQualityService]
})
export class ClusteringComponent implements OnInit, AfterViewInit, OnDestroy {
  subscription: Subscription;
  idSubscription: Subscription;
  public chart: any;
  public pie: any;
  private graph;
  private options;
  private piedat;
  iH = window.innerHeight - 120;
  iW = Math.min(window.innerWidth, 2000);
  options1: any;
  options2: any;
  clusterinfo: any;
  cols1: any;
  pieoptions: any;
  list_sh = Math.floor(this.iH * 0.5);
  chart1: any;
  meters: any;
  phases: any;
  typical_lines: any;
  selectedmeter: any;
  selectedphase: any;
  display: boolean = false;
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
    private route: ActivatedRoute,
    private router: Router,
    private wholeGraphService: WholeGraphService,
    private graphManageService: GraphManageService,
    private confirmationService: ConfirmationService,
    private clusteringservice: ClusteringService,
    private DataQualityService: DataQualityService,
    private graphChartService: GraphChartService,
    private idService: GlobalIdService
  ) {
    //End
    this.cols1 = [
      { field: 'stat', header: '统计量' },
      { field: 'value', header: '数值' },
    ];
    this.phases = [{ label: 'A', value: 1 }]
    this.selectedphase = 1
    // this.selectedmeter = 1
    this.options2 = {
      chart: { width: 400 },
      yAxis: { title: { text: 'MegaWatt' } },
      title: { text: '' },
      series: [{
        name: 'Load Profile',
        showInLegend: false,
        data: []
      }]
    };
    this.pieoptions = {
      chart: {
        plotBackgroundColor: null,
        plotBorderWidth: null,
        plotShadow: false,
        type: 'pie',
        width: 300
      },
      title: {
        text: ''
      },
      tooltip: {
        pointFormat: '{series.name}: <b>{point.percentage:.1f}%</b>'
      },
      plotOptions: {
        pie: {
          allowPointSelect: true,
          cursor: 'pointer',
          dataLabels: {
            enabled: true,
            format: '<b>{point.name}</b>: {point.percentage:.1f} %'
          }
        }
      },
      series: [{
        name: 'Brands',
        colorByPoint: true,
        data: [{ name: 0, y: 1 }]
      }]
    }
    this.meters = [];
    this.subscription = this.graphChartService.graphChart$.subscribe(
      chart => {
        this.chart = chart;
        if (this.chart !== undefined) {
          this.chart.on('onClick', (item) => {
            // Do something here. event call back.
            if (item !== undefined) {
              if (item.isNode) {
                // annouce selected id through id service to global
                this.idService.announceId(item.exID);
                // this.router.navigate(['data/clustering/' + this.selectedmeter]);
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
        if (this.map !== undefined) {
          this.map.on('click', (item) => {
            // Do something here. event call back.
            // Example:
            this.changedLat = item["latlng"]["lat"];
            this.changedLng = item["latlng"]["lng"];
            this.map.eachLayer((layer) => {
              if (layer['options']['color']) {
                if (this.changedLat == layer['_latlng']['lat'] && this.changedLng == layer['_latlng']['lng']) {
                  this.idService.announceId(layer['options']['name']);
                  // this.router.navigate(['data/clustering/' + this.selectedmeter]);
                  // this.selectedmeter = layer['options']['name'];
                  // this.onMapUpdate();
                  // this.plotscat();
                }
              }
            })
          })
        }


      }
    );
    this.graphChartService.announceMapSubscription();
    this.graphChartService.announceSubscription();
    this.idSubscription = this.idService.id$.subscribe(
      (id) => {
        console.log('new id' + id);
        this.selectedmeter = id;
        this.clusteringservice.get_phase(this.selectedmeter).then(
          res => {
            this.phases = res;
            this.selectedphase = this.phases[0].value;
          }
        )
        this.plotscat();
        // this.router.navigate(['data/clustering/' + id]);
      }
    )
  }
  ngOnInit() {
    // tslint:disable-next-line:prefer-const
    // let id = this.route.snapshot.paramMap.get('id');
    // this.selectedmeter = (+id);
    // this.idService.announceId(+id);
    
  }
  ngOnDestroy() {
    this.map.off("click");
    this.chart.off('onClick');
    this.subscription.unsubscribe();
    this.subscriptionmap.unsubscribe();
    this.idSubscription.unsubscribe();
  }
  ngAfterViewInit() {

    //const id = this.route.snapshot.params.id;
    // console.log(this.route.snapshot.params.id);    
    // console.log(this.list_sh)
    //this.selectedmeter = id;
    this.DataQualityService.GetIDList().then(
      line => {
        line.result.forEach(element => {
          this.meters.push({ label: element, value: element })
        });
        console.log(this.meters);
      }
    )
    this.idSubscription = this.idService.id$.subscribe(
      (id) => {
        this.selectedmeter = id;
        this.clusteringservice.get_phase(this.selectedmeter).then(
          res => {
            this.phases = res;
            this.selectedphase = this.phases[0].value;
          }
        )
        this.plotscat();
      }
    );
    (<any>window).opt = this.options;
    (<any>window).opt2 = this.options2;
  }
  saveInstance(chartInstance): void {
    this.chart1 = chartInstance;
  }
  onmouseOver(e) {
    // console.log(e)
    // console.log(this.selectedmeter)
    this.clusteringservice.plot_line(this.selectedmeter, this.selectedphase, e.context.category, e.context.y + 1).then(
      res => {
        res[0].chart.width = this.iW * 0.37;
        res[0].chart.height = (this.iH - 95) * 0.6;
        this.options2 = res[0];
        this.clusterinfo = res[1]
      }
    )
  }
  plotscat() {
    this.clusteringservice.get_phase(this.selectedmeter).then(
      res => {
        this.phases = res;
        // console.log(this.phases);
        // this.selectedphase = this.phases[0].value;
        this.clusteringservice.plot_scatter(this.selectedmeter, this.selectedphase).then(
          sp => {
            //console.log(sp);
            if (sp == null) {
              console.log("sp1");
              this.display = true;
              console.log("sp2");
            }
            else {
              sp[0].chart.width = this.iW * 0.48;
              sp[0].chart.height = (this.iH) * 0.4;
              this.options1 = sp[0];
              this.clusteringservice.plot_line(this.selectedmeter, this.selectedphase, '01/01/13', sp[0].series[0].data[0].y + 1).then(
                res => {
                  res[0].chart.width = this.iW * 0.37;
                  res[0].chart.height = (this.iH - 95) * 0.6;
                  this.options2 = res[0]
                  this.clusterinfo = res[1]
                })
              sp[1].chart.height = (this.iH - 95) * 0.4;
              this.pieoptions = sp[1];
              // console.log(piedat[1].length);
              let i = 0;
              for (i = 0; i < sp[2].length; i++) {
                // console.log(piedat[1][i])
                sp[2][i].chart.height = 0.6 * (this.iH - 95) / 2;
                sp[2][i].chart.width = this.iW * 0.75 / 4 - 15
              }
              this.typical_lines = sp[2];
            }
          }
        );
      }
    )
  }
  changeid() {
    // this.clusteringservice.get_phase(this.selectedmeter).then(
    //   res => {
    //     this.phases = res;
    //     this.selectedphase = this.phases[0].value;
    //   }
    // )
    this.idService.announceId(this.selectedmeter);
    // this.router.navigate(['data/clustering/' + this.selectedmeter]);
    //Added By Bowen
    //   this.flag = 0;
    //    this.map.eachLayer((layer )=>{
    //       if (layer['options']['attribution'] != "Open Street Map" ){
    //           if (layer['_latlng']){
    //               if (layer['options']['name']){
    //                   if (layer['options']['name']== this.selectedmeter){
    //                       this.flag = 1;
    //                       this.latNew = layer['_latlng']['lat'];
    //                       this.lngNew = layer['_latlng']['lng'];
    //                       layer.remove();

    //                   }
    //               }
    //           }
    //       }
    //       })
    //    if (this.flag == 1){
    //        this.map.eachLayer((layer)=>{
    //           if (layer['options']['color']=='chocolate'){
    //               this.id = layer['options']['name'];
    //               this.lat = layer['_latlng']['lat'];
    //               this.lng = layer['_latlng']['lng'];
    //               layer.remove();
    //               }
    //        })
    //    }
    //     if (this.flag == 1){
    //         this.newlayer = L.circle([this.latNew, this.lngNew], {radius: 2000,
    //                                                           name: this.selectedmeter,
    //                                                           color: 'chocolate',
    //                                                           fillColor: 'chocolate',
    //                                                           opacity: 0.8,
    //                                                           fillOpacity: 0.8}).addTo(this.map);
    //         this.newlayer.bindTooltip("ID: " + String(this.selectedmeter) + " Lat: " + String(this.latNew) + ", Lng: " +String(this.lngNew));
    //         this.map.addLayer(this.newlayer);

    //         this.newlayer2 = L.circle([this.lat, this.lng], {radius: 1000,
    //                                                           name: this.selectedmeter,
    //                                                           color: 'darkcyan',
    //                                                           fillColor: 'darkcyan',
    //                                                           opacity: 0.8,
    //                                                           fillOpacity: 0.8}).addTo(this.map);
    //         this.newlayer2.bindTooltip("ID: " + String(this.selectedmeter) + " Lat: " + String(this.lat) + ", Lng: " +String(this.lng));
    //         this.map.addLayer(this.newlayer2);
    //      }
  }
  // onMapUpdate() {
  //   //Added By Bowen
  //   this.flag = 0;
  //    this.map.eachLayer((layer )=>{
  //       if (layer['options']['attribution'] != "Open Street Map" ){
  //           if (layer['_latlng']){
  //               if (layer['options']['name']){
  //                   if (layer['options']['name']== this.selectedmeter){
  //                       this.flag = 1;
  //                       this.latNew = layer['_latlng']['lat'];
  //                       this.lngNew = layer['_latlng']['lng'];
  //                       layer.remove();

  //                   }
  //               }
  //           }
  //       }
  //       })
  //    if (this.flag == 1){
  //        this.map.eachLayer((layer)=>{
  //           if (layer['options']['color']=='chocolate'){
  //               this.id = layer['options']['name'];
  //               this.lat = layer['_latlng']['lat'];
  //               this.lng = layer['_latlng']['lng'];
  //               layer.remove();
  //               }
  //        })
  //    }
  //     if (this.flag == 1){
  //         this.newlayer = L.circle([this.latNew, this.lngNew], {radius: 2000,
  //                                                           name: this.selectedmeter,
  //                                                           color: 'chocolate',
  //                                                           fillColor: 'chocolate',
  //                                                           opacity: 0.8,
  //                                                           fillOpacity: 0.8}).addTo(this.map);
  //         this.newlayer.bindTooltip("ID: " + String(this.selectedmeter) + " Lat: " + String(this.latNew) + ", Lng: " +String(this.lngNew));
  //         this.map.addLayer(this.newlayer);

  //         this.newlayer2 = L.circle([this.lat, this.lng], {radius: 1000,
  //                                                           name: this.selectedmeter,
  //                                                           color: 'darkcyan',
  //                                                           fillColor: 'darkcyan',
  //                                                           opacity: 0.8,
  //                                                           fillOpacity: 0.8}).addTo(this.map);
  //         this.newlayer2.bindTooltip("ID: " + String(this.selectedmeter) + " Lat: " + String(this.lat) + ", Lng: " +String(this.lng));
  //         this.map.addLayer(this.newlayer2);
  //  }
  // }
}
