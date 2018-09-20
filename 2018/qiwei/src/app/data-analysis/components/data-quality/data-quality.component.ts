import { Component, OnInit, ViewChild, AfterViewInit, OnChanges, SimpleChanges, OnDestroy } from '@angular/core';
import { Router } from '@angular/router';
import { WholeGraphService } from '../../../app-shared/services/whole-graph.service';
import { DataQualityService } from '../../services/data-quality.service';
import { ClusteringService } from '../../services/clustering.service'
import { ConfirmDialogModule, ConfirmationService } from 'primeng/primeng';
import { GraphChartService } from '../../../app-shared/services/graph-chart.service';
import * as L from 'leaflet';
import { Subscription } from 'rxjs/Subscription';
import { Message } from 'primeng/components/common/api';
import { GlobalIdService } from '../../../app-shared/services/global-id.service';
import * as gvis from 'gvis';
const GraphChart = (<any>gvis).GraphChart;

@Component({
  selector: 'app-data-quality',
  templateUrl: './data-quality.component.html',
  styleUrls: ['./data-quality.component.css'],
  providers: [DataQualityService, ConfirmationService, ClusteringService]
})
export class DataQualityComponent implements OnInit, AfterViewInit, OnDestroy {
  subscription: Subscription;
  idSubscription: Subscription;
  iH = window.innerHeight - 120;
  iW = Math.min(window.innerWidth, 2000);
  options: Object;
  algos: any;
  chosenalgo: any;
  phases: any;
  processcount: any;
  chosenphase: any;
  chosenprocess: any;
  sdate: Date;
  edate: Date;
  defaultDates = new Date('01/01/2013');
  defaultDatee = new Date('12/31/2013');
  meters: any;
  selectedmeter: any;
  private graph;
  public chart: any;
  public line: any;
  stats = [];
  msgs: Message[] = [];
  display1: boolean = false;
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
    private DataQualityService: DataQualityService,
    private wholeGraphService: WholeGraphService,
    private confirmationService: ConfirmationService,
    private clusteringservice: ClusteringService,
    private graphChartService: GraphChartService,
    private idService: GlobalIdService
  ) {
    
    
    this.stats = [{ 'numm': 0, 'mean1': 0, 'corr': 0, 'diff': 0 }];
    this.chosenalgo = "li";
    this.chosenphase = 1;
    this.chosenprocess = 1;
    // this.selectedmeter = 1
    this.sdate = this.defaultDates;
    this.edate = this.defaultDatee;
    this.phases = [
      { label: 'A', value: 1 },
      { label: 'B', value: 2 },
      { label: 'C', value: 3 }
    ];
    this.processcount = [
      { label: '1', value: 1 },
      { label: '2', value: 2 },
      { label: '4', value: 4 },
      { label: '8', value: 8 }
    ];
    //this.algos = [{label: "Linear Interpolation",value:"li"}, {label: "Smoothed Alternating Least Squares",value:"sas"},
    // {label: "Cluster-Based Best Match Scanning",value:"cbms"}, {label: "Kalman Filter Recovery Machine",value:"karm"}];
    this.algos = [
      { label: "Linear Interpolation", value: "li" },
      { label: "Smoothed Alternating Least Squares", value: "sas" }
    ];
    this.meters = [];
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
                  // this.selectedmeter = layer['options']['name'];
                  // this.onMapUpdate();
                  // this.ngAfterViewInit();
                  // this.clusteringservice.get_phase(this.selectedmeter).then(
                  //   res => {
                  //     this.phases = res;
                  //     this.chosenphase = this.phases[0].value;
                  //   }
                  // )
                }
              }
            })
          })
        }
      }
    );
    this.graphChartService.announceMapSubscription();
    //End
    this.graphChartService.announceSubscription();
    this.idSubscription = this.idService.id$.subscribe(
      (id) => {
        this.selectedmeter = id;
        this.showres();
      }
    )
  }

  ngOnInit() {
    this.DataQualityService.GetIDList().then(
      line => {
        line.result.forEach(element => {
          this.meters.push({ label: element, value: element })
        });
      }
    )
  }
  ngOnDestroy() {
    // prevent memory leak when component destroyed
    this.chart.off('onClick');
    this.map.off("click");
    this.subscription.unsubscribe();
    this.subscriptionmap.unsubscribe();
    this.idSubscription.unsubscribe();
  }
  ngAfterViewInit() {
    // this.showres();
  }
  runalgo(): void {
    console.log("run algo");
    this.confirmationService.confirm({
      message: '重新运行算法需要几分钟，您要现在运行吗?',
      header: 'Confirmation',
      icon: 'fa fa-question-circle',
      accept: () => {
        this.display1 = true;
        this.DataQualityService.rundq(this.chosenalgo, this.chosenprocess)
          .then(line => {
            this.display1 = false;
            this.showres();
          })
      },
      reject: () => {
        this.msgs = [{ severity: 'info', summary: 'Rejected', detail: '您拒绝了运行算法' }];
      }
    });
  }
  showres(): void {
    this.DataQualityService.dqimprove(this.chosenalgo, this.sdate.getTime(), this.edate.getTime(), this.selectedmeter, this.chosenphase)
      .then(line1 => {
        console.log(line1.stats)
        line1.results.chart.width = this.iW * 3 / 4 - 15;
        line1.results.chart.height = this.iH - 120;
        this.options = line1.results;
        this.stats = line1.stats;
      })
  }
  //Added By Bowen
  onChangeID(event) {
    console.log('id changes');
    this.clusteringservice.get_phase(this.selectedmeter).then(
      res => {
        this.phases = res;
        this.chosenphase = this.phases[0].value;
        this.chosenprocess = this.processcount[0].value;
      }
    )
    // don't need to update map, just announce new id
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
    // }
  }

  /*
   * map operation done in map component, no need here 
   */

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
