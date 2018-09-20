import { Component, OnInit, ViewChild, AfterViewInit, OnChanges, SimpleChanges} from '@angular/core';
import { Router } from '@angular/router';
import { WholeGraphService } from '../../../app-shared/services/whole-graph.service';
import { ConfirmDialogModule, ConfirmationService, ListboxModule} from 'primeng/primeng';
import { GraphManageService } from '../../../app-shared/services/graph-manage.service';
import { GraphChartService } from '../../../app-shared/services/graph-chart.service';
import { Subscription } from 'rxjs/Subscription';
import { ClusteringService } from '../../services/clustering.service'
import { BigDataService } from '../../services/big-data.service'
import { UserAnalysisService } from '../../services/user-analysis.service'
import * as gvis from 'gvis';
import * as L from 'leaflet';
const GraphChart = (<any>gvis).GraphChart;

@Component({
  selector: 'app-user-analysis',
  templateUrl: './user-analysis.component.html',
  styleUrls: ['./user-analysis.component.css'],
  providers: [ClusteringService]
})

export class UserAnalysisComponent implements OnInit {
  @ViewChild('topocontainer') container1: any;
  iH = window.innerHeight-120;
  iW = Math.min(window.innerWidth,2000);
  tab_sh:string;
  scroll_sh:string;
  public chart: any;
  private graph;
  nclus: number;
  sdate: Date;
  edate: Date;
  defaultDates = new Date('01/01/2013');
  defaultDatee = new Date('12/31/2013');
  typical_lines:any[];
  options1: Object;
  options2: Object;
  options3: Object;
  options4: Object;
  options5: any[];
  loadrank: any[];
  cols: any[];
  display: boolean=false;

  subscription: Subscription;
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
  public userid: any;

  constructor(
    private router: Router,
    private wholeGraphService: WholeGraphService,
    private graphManageService: GraphManageService,
    private confirmationService: ConfirmationService,
    private clusteringservice: ClusteringService,
    private bigdataserive: BigDataService,
    private useranalysisserive: UserAnalysisService,
    private graphChartService: GraphChartService
  ) {
    this.subscription = this.graphChartService.graphChart$.subscribe(
      chart => {
          this.chart = chart;
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
                          this.userid = layer['options']['name'];
                          //console.log("ID:");
                          //console.log(this.selectedmeter);
                          //this.plot_category_summary();
                          this.onRowUpdate();
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
    this.tab_sh=String(Math.round(this.iH*5/10))+"px";
    this.scroll_sh=String(Math.round(this.iH*5/7.9))+"px";
    //console.log(this.iH);
    //console.log(this.iW);
    this.nclus=36;
    this.sdate = this.defaultDates;
    this.edate = this.defaultDatee;
    this.options1 = {
      title: { text: '分行业用电行为分析' },
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
    this.options2 = {
      title: { text: '波形分布' },
      chart: {
        zoomType: 'x',
        height: 120,
        width: 290
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
      }]
    };
    this.options3 = {
      title: { text: '用户年用电量统计hahaha' },
      chart: {
        zoomType: 'x',
        height: 200,
        width: 500
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
        allowPointSelect: false
      }
      ]
    };
    this.options4 = {
      title: { text: '熵值分布' },
      chart: {
        zoomType: 'x',
        height: 120,
        width: 290
      },
      xAxis: {
      },
      yAxis: {
        title: {
          text: " "
        }
      },
      series: [{
        showInLegend: false,
        type: 'column',
        data: [[0,0]],
        allowPointSelect: false
      }]
    };
    this.cols = [
      { field: 'user', header: '用户' },
      { field: 'year_usage', header: '年用电量' },
      { field: 'morning', header: '早高峰' },
      { field: 'afternoon', header: '下午高峰' },
      { field: 'evening', header: '晚高峰' },
      { field: 'night', header: '夜间高峰' },
      { field: 'dualma', header: '早傍双高峰' },
      { field: 'dualme', header: '早晚双高峰' },
      { field: 'flat', header: '平滑' },
    ]
   }

  ngOnInit() {
  }

  ngAfterViewInit() {
    this.plot_category_summary();
    //this.plot_loadshape_summary();
    this.useranalysisserive.PlotCategoryTypical("公司").then(
      line => {
        this.typical_lines = line
        let i = 0;
        for (i = 0; i < this.typical_lines.length; i++) {
          this.typical_lines[i]["chart"]["height"] = this.iH*5/8/3;
          this.typical_lines[i]["chart"]["width"] = this.iW*3/4/4-18;
        }
      }
    )
    this.useranalysisserive.PlotCategoryLoadshape("公司").then(
      line => {
        this.options2 = line
        this.options2["chart"]["height"]=this.iH*5/8/2;
        this.options2["chart"]["width"]=this.iW*3/4/3.1;
      }
    )
    this.useranalysisserive.PlotCategoryEntropy("公司").then(
      line => {
        this.options4 = line
        this.options4["chart"]["height"]=this.iH*5/8/2;
        this.options4["chart"]["width"]=this.iW*3/4/3.1;
      }
    )
    this.useranalysisserive.PlotCategoryUserUsage("公司").then(
      line => {
        this.options3 = line[0];
        this.loadrank = line[1];
        this.options3["chart"]["height"]=this.iH*3/8;
        this.options3["chart"]["width"]=this.iW/4*3/2;
      }
    )
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
  runal(){  
    this.clusteringservice.run_algo(this.nclus)
  }
  plot_category_summary(): void {
    console.log("plot_category_summary");
    this.useranalysisserive.PlotCategorySummary().then(
      line => {
        this.options1 = line
        this.options1["chart"]["height"]=this.iH*3/8;
        this.options1["chart"]["width"]=this.iW/4*3/2;
      })
  }
  //plot_loadshape_summary(): void {
  //  console.log("plot_loadshape_summary");
  //  this.useranalysisserive.PlotLoadShapeSummary().then(
  //    line => {
  //      this.options3 = line
  //    })
  //}
  onPointSelect(e) {
    //console.log(e.context.category);
    this.useranalysisserive.PlotCategoryLoadshape(e.context.category).then(
      line => {
        this.options2 = line
        this.options2["chart"]["height"]=this.iH*5/8/2;
        this.options2["chart"]["width"]=this.iW*3/4/3.1;
      }
    )
    this.useranalysisserive.PlotCategoryEntropy(e.context.category).then(
      line => {
        this.options4 = line
        this.options4["chart"]["height"]=this.iH*5/8/2;
        this.options4["chart"]["width"]=this.iW*3/4/3.1;
      }
    )
    this.useranalysisserive.PlotCategoryTypical(e.context.category).then(
      line => {
        this.typical_lines = line
        let i = 0;
        for (i = 0; i < this.typical_lines.length; i++) {
          this.typical_lines[i]["chart"]["height"] = this.iH*5/8/3;
          this.typical_lines[i]["chart"]["width"] = this.iW*3/4/4-18;
        }
      }
    )
    this.useranalysisserive.PlotCategoryUserUsage(e.context.category).then(
      line => {
        this.options3 = line[0];
        this.loadrank = line[1];
        this.options3["chart"]["height"]=this.iH*3/8;
        this.options3["chart"]["width"]=this.iW/4*3/2;
      }
    )
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
    this.map.eachLayer((layer )=>{
       if (layer['options']['attribution'] != "Open Street Map" ){
           if (layer['_latlng']){
               if (layer['options']['name']){
                   if (layer['options']['name']== e.data.user){
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
                                                            name: e.data.user,
                                                            color: 'chocolate',
                                                            fillColor: 'chocolate',
                                                            opacity: 0.8,
                                                            fillOpacity: 0.8}).addTo(this.map);
        this.newlayer.bindTooltip("ID: " + String(e.data.user) + " Lat: " + String(this.latNew) + ", Lng: " +String(this.lngNew));
        this.map.addLayer(this.newlayer);

        this.newlayer2 = L.circle([this.lat, this.lng], {radius: 1000,
                                                            name: this.id,
                                                            color: 'darkcyan',
                                                            fillColor: 'darkcyan',
                                                            opacity: 0.8,
                                                            fillOpacity: 0.8}).addTo(this.map);
        this.newlayer2.bindTooltip("ID: " + String(this.id) + " Lat: " + String(this.lat) + ", Lng: " +String(this.lng));
        this.map.addLayer(this.newlayer2);
       }
    //}
    

  }

  onRowUpdate(){
    //console.log(e);
    //console.log(e.data.user);
    this.bigdataserive.PlotEvery(this.userid, this.sdate.getTime(), this.edate.getTime())
    .then(line => {
      console.log(line[0]);
      this.options5 = line[0];
      this.options5["chart"]["height"]=600;
      this.options5["chart"]["width"]=600;
    })
    this.display = true;
    //Added By Bowen
    //console.log(e.data.user);
    this.flag = 0;
    this.map.eachLayer((layer )=>{
       if (layer['options']['attribution'] != "Open Street Map" ){
           if (layer['_latlng']){
               if (layer['options']['name']){
                   if (layer['options']['name']== this.userid){
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
                                                            name: this.userid,
                                                            color: 'chocolate',
                                                            fillColor: 'chocolate',
                                                            opacity: 0.8,
                                                            fillOpacity: 0.8}).addTo(this.map);
        this.newlayer.bindTooltip("ID: " + String(this.userid) + " Lat: " + String(this.latNew) + ", Lng: " +String(this.lngNew));
        this.map.addLayer(this.newlayer);
        this.newlayer2 = L.circle([this.lat, this.lng], {radius: 1000,
                                                            name: this.id,
                                                            color: 'darkcyan',
                                                            fillColor: 'darkcyan',
                                                            opacity: 0.8,
                                                            fillOpacity: 0.8}).addTo(this.map);
        this.newlayer2.bindTooltip("ID: " + String(this.id) + " Lat: " + String(this.lat) + ", Lng: " +String(this.lng));
        this.map.addLayer(this.newlayer2);
       }

  }

  ngOnDestroy() {
    // prevent memory leak when component destroyed
    this.map.off("click");
    this.subscription.unsubscribe();
    this.subscriptionmap.unsubscribe();
  }
}
