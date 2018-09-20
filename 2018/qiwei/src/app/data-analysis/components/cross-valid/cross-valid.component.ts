import { Component, OnInit, ViewChild, AfterViewInit, OnChanges, SimpleChanges, ViewEncapsulation, OnDestroy } from '@angular/core';
import { Router } from '@angular/router';
import { WholeGraphService } from '../../../app-shared/services/whole-graph.service';
import { ConfirmDialogModule, ConfirmationService, ProgressSpinnerModule, DropdownModule, TabViewModule } from 'primeng/primeng';
import { GraphManageService } from '../../../app-shared/services/graph-manage.service';
import { CrossValidService } from '../../services/cross-valid.service';
import { GraphChartService } from '../../../app-shared/services/graph-chart.service';
import { DataQualityService } from '../../services/data-quality.service';
import { Subscription } from 'rxjs/Subscription';
import { GlobalIdService } from '../../../app-shared/services/global-id.service';

import * as gvis from 'gvis';
import * as L from 'leaflet';
const GraphChart = (<any>gvis).GraphChart;

@Component({
  selector: 'app-cross-valid',
  templateUrl: './cross-valid.component.html',
  styleUrls: ['./cross-valid.component.css'],
  providers: [CrossValidService, DataQualityService],
  encapsulation: ViewEncapsulation.None
})
export class CrossValidComponent implements OnInit, AfterViewInit, OnDestroy {
  @ViewChild('t1') t1: any;
  iH = window.innerHeight - 120;
  iW = Math.min(window.innerWidth, 2000);
  sdate: Date;
  edate: Date;
  defaultDates = new Date('01/01/2013');
  defaultDatee = new Date('12/31/2013');
  meters: any;
  public chart: any;
  subscription: Subscription;
  idSubscription: Subscription;
  user: number;
  options: Object;
  options2: Object;
  types: any[];
  phases: any[];
  diffrank_user: any[];
  cols: any[];
  selectedType: number;
  selectedPhase: number;
  selectedAlgo: number;
  statis: any[];
  top_position: number;
  left_position: number;
  display: boolean = false;
  scroll_sh: string;
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
    private DataQualityService: DataQualityService,
    private wholeGraphService: WholeGraphService,
    private graphManageService: GraphManageService,
    private confirmationService: ConfirmationService,
    private crossvalid: CrossValidService,
    private graphChartService: GraphChartService,
    private idService: GlobalIdService
  ) {
    this.sdate = this.defaultDates;
    this.edate = this.defaultDatee;
    this.top_position = 180;
    this.left_position = this.iW / 2;
    this.scroll_sh = "420px";
    // this.user = 1;
    this.selectedPhase = 1;
    this.selectedAlgo = 0;
    this.selectedType = 1;
    this.types = [
      { label: '相关系数（三相和）', value: 1 },
      { label: '平均差值（三相和）', value: 2 }
    ];
    this.meters = [];
    this.phases = [
      { label: 'A', value: 1 },
      { label: 'B', value: 2 },
      { label: 'C', value: 3 },
      { label: '三相和', value: 0 }
    ];
    this.cols = [
      { field: 'diff', header: '差值' },
      { field: 'date', header: '日期' },
    ];
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
            //console.log(this.map);
            //console.log(item);
            this.changedLat = item["latlng"]["lat"];
            this.changedLng = item["latlng"]["lng"];
            //console.log(this.changedLat);
            //console.log(this.changedLng);
            this.map.eachLayer((layer) => {
              if (layer['options']['color']) {
                //console.log(layer['_latlng']['lat']);
                //console.log(layer['_latlng']['lng']);
                if (this.changedLat == layer['_latlng']['lat'] && this.changedLng == layer['_latlng']['lng']) {
                  this.idService.announceId(layer['options']['name']);
                  // this.user = layer['options']['name'];
                  // this.onMapUpdate();
                  // this.ngAfterViewInit();
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
    

  }
  ngOnInit() {
  }
  ngOnDestroy() {
    // prevent memory leak when component destroyed
    this.map.off("click");
    this.chart.off('onClick');
    this.subscription.unsubscribe();
    this.subscriptionmap.unsubscribe();
    this.idSubscription.unsubscribe();
  }
  ngAfterViewInit(): void {
    this.DataQualityService.GetIDList().then(
      line => {
        line.result.forEach(element => {
          this.meters.push({ label: element, value: element })
        });
      }
    )
    this.idSubscription = this.idService.id$.subscribe(
      (id) => {
        this.user = id;
        this.showres();
      }
    );
    // this.showres();
  }
  showres(): void {
    this.t1.reset();
    if (this.selectedAlgo == 0) {
      this.crossvalid.PlotUIPG(this.sdate.getTime(), this.edate.getTime(), this.user, this.selectedPhase, 0).then(
        line => {
          line[0].chart.width = this.iW * 9 / 16 - 10;
          line[0].chart.height = this.iH - 80;
          this.options = line[0];
          this.statis = line[1];
          this.diffrank_user = line[2];
        }
      )
    }
    if (this.selectedAlgo == 1) {
      this.crossvalid.PlotPFPS(this.sdate.getTime(), this.edate.getTime(), this.user, this.selectedPhase, 0).then(
        line => {
          line[0].chart.width = this.iW * 9 / 16 - 10;
          line[0].chart.height = this.iH - 80;
          this.options = line[0];
          this.statis = line[1];
          this.diffrank_user = line[2];
        }
      )
    }
    if (this.selectedAlgo == 2) {
      this.crossvalid.PlotLoadEnergy(this.sdate.getTime(), this.edate.getTime(), this.user, 0).then(
        line => {
          line[0].chart.width = this.iW * 9 / 16 - 10;
          line[0].chart.height = this.iH - 80;
          this.options = line[0];
          this.statis = line[1];
          this.diffrank_user = line[2];
        }
      )
    }
  }
  onFocus(event) {
    this.crossvalid.PlotUserRanking(this.selectedAlgo, this.selectedType).then(
      line => {
        line.chart.width = 520;
        line.chart.height = 1500;
        this.options2 = line;
        this.display = true;
      }
    )
  }
  onBlur(event) {
    this.display = false;
  }
  onChangePhase(event) {

  }
  onChangeUser(event) {
    this.idService.announceId(event.value);
  }
  onChangeType(event) {
    // console.log
    this.crossvalid.PlotUserRanking(this.selectedAlgo, this.selectedType, ).then(
      line => {
        line.chart.width = 520;
        line.chart.height = 1500;
        this.options2 = line;
      }
    );
    // this.idService.announceId(event.value);
  }

  onTabChange(event) {
    this.selectedAlgo = event.index;
    //this.showres();
  }
  onRowSelect(e) {
    if (this.selectedAlgo == 0) {
      this.crossvalid.PlotUIPG(e.data.date, e.data.date, this.user, this.selectedPhase, 1).then(
        line => {
          line[0].chart.width = this.iW * 9 / 16 - 10;
          line[0].chart.height = this.iH - 80;
          this.options = line[0];
        }
      )
    }
    if (this.selectedAlgo == 1) {
      this.crossvalid.PlotPFPS(e.data.date, e.data.date, this.user, this.selectedPhase, 1).then(
        line => {
          line[0].chart.width = this.iW * 9 / 16 - 10;
          line[0].chart.height = this.iH - 80;
          this.options = line[0];
        }
      )
    }
    if (this.selectedAlgo == 2) {
      this.crossvalid.PlotLoadEnergy(e.data.date, e.data.date, this.user, 1).then(
        line => {
          line[0].chart.width = this.iW * 9 / 16 - 10;
          line[0].chart.height = this.iH - 80;
          this.options = line[0];
        }
      )
    }
  }
  onRowUnselect(e) {
    this.showres();
  }
  onPointSelect(e) {
    this.display = false;
    this.idService.announceId(e.context.category);
    // this.user = e.context.category;
    //this.showres();
    //Added By Bowen
    // this.flag = 0;
    // this.map.eachLayer((layer) => {
    //   if (layer['options']['attribution'] != "Open Street Map") {
    //     if (layer['_latlng']) {
    //       if (layer['options']['name']) {
    //         if (layer['options']['name'] == this.user) {
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
    //     name: this.user,
    //     color: 'chocolate',
    //     fillColor: 'chocolate',
    //     opacity: 0.8,
    //     fillOpacity: 0.8
    //   }).addTo(this.map);
    //   this.newlayer.bindTooltip("ID: " + String(this.user) + " Lat: " + String(this.latNew) + ", Lng: " + String(this.lngNew));
    //   this.map.addLayer(this.newlayer);

    //   this.newlayer2 = L.circle([this.lat, this.lng], {
    //     radius: 1000,
    //     name: this.user,
    //     color: 'darkcyan',
    //     fillColor: 'darkcyan',
    //     opacity: 0.8,
    //     fillOpacity: 0.8
    //   }).addTo(this.map);
    //   this.newlayer2.bindTooltip("ID: " + String(this.user) + " Lat: " + String(this.lat) + ", Lng: " + String(this.lng));
    //   this.map.addLayer(this.newlayer2);
    // }
  }
  // onMapUpdate(){
  //    this.flag = 0;
  //    this.map.eachLayer((layer )=>{
  //       if (layer['options']['attribution'] != "Open Street Map" ){
  //           if (layer['_latlng']){
  //               if (layer['options']['name']){
  //                   if (layer['options']['name']== this.user){
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
  //                                                           name: this.user,
  //                                                           color: 'chocolate',
  //                                                           fillColor: 'chocolate',
  //                                                           opacity: 0.8,
  //                                                           fillOpacity: 0.8}).addTo(this.map);
  //         this.newlayer.bindTooltip("ID: " + String(this.user) + " Lat: " + String(this.latNew) + ", Lng: " +String(this.lngNew));
  //         this.map.addLayer(this.newlayer);

  //         this.newlayer2 = L.circle([this.lat, this.lng], {radius: 1000,
  //                                                           name: this.user,
  //                                                           color: 'darkcyan',
  //                                                           fillColor: 'darkcyan',
  //                                                           opacity: 0.8,
  //                                                           fillOpacity: 0.8}).addTo(this.map);
  //         this.newlayer2.bindTooltip("ID: " + String(this.user) + " Lat: " + String(this.lat) + ", Lng: " +String(this.lng));
  //         this.map.addLayer(this.newlayer2);
  //      }
  // }
}
