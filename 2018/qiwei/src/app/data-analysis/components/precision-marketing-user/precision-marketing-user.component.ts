import { Component, OnInit, ViewChild, AfterViewInit, OnChanges, SimpleChanges} from '@angular/core';
import { Router } from '@angular/router';
import { WholeGraphService } from '../../../app-shared/services/whole-graph.service';
import { ConfirmDialogModule, ConfirmationService, ListboxModule} from 'primeng/primeng';
import { GraphManageService } from '../../../app-shared/services/graph-manage.service';
import { GraphChartService } from '../../../app-shared/services/graph-chart.service';
import { Subscription } from 'rxjs/Subscription';
import { PrecisionMarketingUserService } from '../../services/precision-marketing-user.service';
import { PrecisionMarketingService } from '../../services/precision-marketing.service';
import { DataQualityService } from '../../services/data-quality.service';
import { GlobalIdService } from '../../../app-shared/services/global-id.service';
import * as gvis from 'gvis';
import * as L from 'leaflet';
const GraphChart = (<any>gvis).GraphChart;

@Component({
  selector: 'app-precision-marketing-user',
  templateUrl: './precision-marketing-user.component.html',
  providers: [DataQualityService],
  styleUrls: ['./precision-marketing-user.component.css']
})

export class PrecisionMarketingUserComponent implements OnInit {
  @ViewChild('topocontainer') container1: any;
  iH = window.innerHeight-120;
  iW = Math.min(window.innerWidth,2000);
  public chart: any;
  private graph;
  date: Date;
  defaultDate = new Date('01/01/2013');
  user:number;
  meters: any;
  options1: Object;
  options2: Object;
  options3: Object;
  options4: Object;
  options5: Object;
  options6: Object;
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
  public changedLat: any;
  public changedLng: any;

  subscription: Subscription;
  constructor(
    private router: Router,
    private wholeGraphService: WholeGraphService,
    private graphManageService: GraphManageService,
    private confirmationService: ConfirmationService,
    private DataQualityService: DataQualityService,
    private precisionmarketinguserserive: PrecisionMarketingUserService,
    private precisionmarketingserive: PrecisionMarketingService,
    private graphChartService: GraphChartService,
    private idService: GlobalIdService
  ) {
    this.meters = [];
    this.date = this.defaultDate;
    this.user = 1;
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
              this.map.eachLayer((layer)=>{
                  if (layer['options']['color']){
                      //console.log(layer['_latlng']['lat']);
                      //console.log(layer['_latlng']['lng']);
                      if (this.changedLat == layer['_latlng']['lat'] && this.changedLng == layer['_latlng']['lng']){
                          this.idService.announceId(layer['options']['name']);
                          // this.user = layer['options']['name'];
                          // //console.log("ID:");
                          // //console.log(this.selectedmeter);
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
    this.idSubscription = this.idService.id$.subscribe(
      (id) => {
        this.user = id;
        // tslint:disable-next-line:max-line-length
        this.show();
      }
    )
   }

  ngOnInit() {
  }

  ngAfterViewInit() {
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
        this.show();
      }
    );
  }

  treeLayout(): void {
    // add root node
    this.chart.addRoot('bus_D', '150')
      .runLayout('tree');
  }
  staticLayout(): void {
    this.chart.getNodes().forEach(n => {
      n.x = n.attrs.pos_x;
      n.y = n.attrs.pos_y;
    });
    this.chart.runLayout('static');
  }
  onPointSelect(e) {
    console.log(e.context.category);
    this.user=e.context.category
    this.idService.announceId(e.context.category);
    this.precisionmarketinguserserive.PlotUserAnnulElasticity(e.context.category).then(
      line => {
        line[0].chart.width = this.iW*3*3/4/4;
        line[0].chart.height = this.iH/3-50;
        this.options3 = line[0];
        line[1].chart.width = this.iW*3*3/4/4;
        line[1].chart.height = this.iH/3+25;
        this.options4 = line[1];
      }
     )
    this.precisionmarketinguserserive.PlotUserLoad(e.context.category, this.date.getTime()).then(
      line => {
        line.chart.width = this.iW*3/4/4-8;
        line.chart.height = this.iH/2-25;
        this.options5 = line;
      }
    )
     //Added By Bowen
     this.flag = 0;
     this.map.eachLayer((layer )=>{
        if (layer['options']['attribution'] != "Open Street Map" ){
            if (layer['_latlng']){
                if (layer['options']['name']){
                    if (layer['options']['name']== this.user){
                        this.flag = 1;
                        this.latNew = layer['_latlng']['lat'];
                        this.lngNew = layer['_latlng']['lng'];
                        layer.remove();

                    }
                }
            }
        }
        })
     if (this.flag == 1){
         this.map.eachLayer((layer)=>{
            if (layer['options']['color']=='chocolate'){
                this.id = layer['options']['name'];
                this.lat = layer['_latlng']['lat'];
                this.lng = layer['_latlng']['lng'];
                layer.remove();
                }
         })
     }
      if (this.flag == 1){
          this.newlayer = L.circle([this.latNew, this.lngNew], {radius: 2000,
                                                            name: this.user,
                                                            color: 'chocolate',
                                                            fillColor: 'chocolate',
                                                            opacity: 0.8,
                                                            fillOpacity: 0.8}).addTo(this.map);
          this.newlayer.bindTooltip("ID: " + String(this.user) + " Lat: " + String(this.latNew) + ", Lng: " +String(this.lngNew));
          this.map.addLayer(this.newlayer);

          this.newlayer2 = L.circle([this.lat, this.lng], {radius: 1000,
                                                            name: this.user,
                                                            color: 'darkcyan',
                                                            fillColor: 'darkcyan',
                                                            opacity: 0.8,
                                                            fillOpacity: 0.8}).addTo(this.map);
          this.newlayer2.bindTooltip("ID: " + String(this.id) + " Lat: " + String(this.lat) + ", Lng: " +String(this.lng));
          this.map.addLayer(this.newlayer2);
       }
  }
  onSelect(event) {
    this.precisionmarketinguserserive.PlotUserLoad(this.user, this.date.getTime()).then(
        line => {
            line.chart.width = this.iW*3/4/4-8;
            line.chart.height = this.iH/2-25;
            this.options5 = line;
        }
      )
    this.precisionmarketingserive.PlotSystemLoad(this.date.getTime()).then(
        line => {
          line.chart.width = this.iW*3/4/4-8;
          line.chart.height = this.iH/2-35;
          this.options6 = line;
        }
      )
  }
  onMapUpdate(){
     this.flag = 0;
     this.map.eachLayer((layer )=>{
        if (layer['options']['attribution'] != "Open Street Map" ){
            if (layer['_latlng']){
                if (layer['options']['name']){
                    if (layer['options']['name']== this.user){
                        this.flag = 1;
                        this.latNew = layer['_latlng']['lat'];
                        this.lngNew = layer['_latlng']['lng'];
                        layer.remove();

                    }
                }
            }
        }
        })
     if (this.flag == 1){
         this.map.eachLayer((layer)=>{
            if (layer['options']['color']=='chocolate'){
                this.id = layer['options']['name'];
                this.lat = layer['_latlng']['lat'];
                this.lng = layer['_latlng']['lng'];
                layer.remove();
                }
         })
     }
      if (this.flag == 1){
          this.newlayer = L.circle([this.latNew, this.lngNew], {radius: 2000,
                                                            name: this.user,
                                                            color: 'chocolate',
                                                            fillColor: 'chocolate',
                                                            opacity: 0.8,
                                                            fillOpacity: 0.8}).addTo(this.map);
          this.newlayer.bindTooltip("ID: " + String(this.user) + " Lat: " + String(this.latNew) + ", Lng: " +String(this.lngNew));
          this.map.addLayer(this.newlayer);

          this.newlayer2 = L.circle([this.lat, this.lng], {radius: 1000,
                                                            name: this.user,
                                                            color: 'darkcyan',
                                                            fillColor: 'darkcyan',
                                                            opacity: 0.8,
                                                            fillOpacity: 0.8}).addTo(this.map);
          this.newlayer2.bindTooltip("ID: " + String(this.user) + " Lat: " + String(this.lat) + ", Lng: " +String(this.lng));
          this.map.addLayer(this.newlayer2);
       }
  }
  ngOnDestroy() {
    // prevent memory leak when component destroyed
    this.map.off("click");
    this.subscription.unsubscribe();
    this.subscriptionmap.unsubscribe();
    this.idSubscription.unsubscribe();
  }

  onChangeUser(event) {
    this.idService.announceId(event.value);
  }

  show():void{
    this.precisionmarketinguserserive.PlotUserElasticity().then(
      line => {
        line.chart.width = this.iW*3/4/2*1.1;
        line.chart.height = this.iH/3+25;
        this.options1 = line;
        //console.log(this.options1);
      }
    )
    this.precisionmarketinguserserive.PlotUserEntropy().then(
      line => {
        line.chart.width = this.iW*3/4/5;
        line.chart.height = this.iH/3+25;
        this.options2 = line;
      }
    )
    this.precisionmarketinguserserive.PlotUserAnnulElasticity(this.user).then(
      line => {
        line[0].chart.width = this.iW*3*3/4/4;
        line[0].chart.height = this.iH/3-50;
        this.options3 = line[0];
        line[1].chart.width = this.iW*3*3/4/4;
        line[1].chart.height = this.iH/3+25;
        this.options4 = line[1];
      }
    )
    this.precisionmarketinguserserive.PlotUserLoad(this.user, this.date.getTime()).then(
        line => {
            line.chart.width = this.iW*3/4/4-8;
            line.chart.height = this.iH/2-25;
            this.options5 = line;
        }
      )
    this.precisionmarketingserive.PlotSystemLoad(this.date.getTime()).then(
      line => {
        line.chart.width = this.iW*3/4/4-8;
        line.chart.height = this.iH/2-35;
        this.options6 = line;
      }
    )
  }
}
