import { Component, OnInit, ViewChild, ChangeDetectorRef, NgZone, ElementRef, HostListener, AfterViewInit } from '@angular/core';

import { MatTableDataSource, MatSort } from '@angular/material';
// import { MatSort } from '@angular/material';

import { ResizeEvent } from 'angular-resizable-element';

import { FiltertypicalService, Product } from '../app-shared/services/filtertypical.service';
// import { GlobalMapComponent } from '../global-map/global-map.component';



import { FormControl } from '@angular/forms';

import { Observable } from 'rxjs/Observable';
import { startWith } from 'rxjs/operators/startWith';
import { map } from 'rxjs/operators/map';
import { Subscription } from 'rxjs/Subscription';

import { DomSanitizer } from '@angular/platform-browser';
import { MatIconRegistry } from '@angular/material';
// import { environment } from '../../../../environments/environment';



import { GlobalIdService } from '../app-shared/services/global-id.service';
import { NewelasticityService } from '../app-shared/services/newelasticity.service';
import { DataQualityService } from '../data-analysis/services/data-quality.service';
import { GraphChartService } from '../app-shared/services/graph-chart.service';

import { UserAnalysisService } from '../data-analysis/services/user-analysis.service'
import { BigDataService } from '../data-analysis/services/big-data.service'

// for marking the selected id
import { icon, latLng, Layer, marker, circle, tileLayer } from 'leaflet';
import { HackMapDirective } from '../app-shared/directives/hack-map.directive';
import { LeafletMapLatLngService } from '../app-shared/services/leafletmap.service';
//

import { DrawLineComponent } from '../app-shared/add-a-map/component/draw-line.component';

import * as gvis from 'gvis';
import * as L from 'leaflet';
const GraphChart = (<any>gvis).GraphChart;


@Component({
  selector: 'app-filterbusiness',
  templateUrl: './filterbusiness.component.html',
  styleUrls: ['./filterbusiness.component.css'],
  providers: [FiltertypicalService, NewelasticityService, GlobalIdService, DataQualityService, BigDataService]
})


export class FilterbusinessComponent implements OnInit {

  showValue=false;

  //autoComplete
  siteCtrl: FormControl;
  filteredSites: Observable<any[]>;
  subscriptions: Subscription[] = [];
  selectedID: number;
  public chart: any;
  meters: any;
  listtype:string;
  // for marking selected id
  map: any;
  idsubscription: Subscription;
  @ViewChild(HackMapDirective) mapDirective: HackMapDirective;
  @ViewChild(DrawLineComponent) dlMap;

  // for business data-table---Jack
  iH = window.innerHeight-120;
  iW = Math.min(window.innerWidth,2000);
  tab_sh:string;
  scroll_sh:string;
  // for business data-table popup---Jack
  nclus: number;
  sdate: Date;
  edate: Date;
  defaultDates = new Date('01/01/2013');
  defaultDatee = new Date('12/31/2013');
  typical_lines:any[];
  subscription: Subscription;
  // public map: any;
  // subscriptionmap: Subscription;
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

  options1: Object;
  options2: Object;
  options3: Object;
  options4: Object;
  options5: any[];
  options6: object;
  // 公司，房产开发，小区，工业，政府，路灯，公寓，宿舍
  loadrank: any[];
  // loadrank1: any[];
  // loadrank2: any[];
  // loadrank3: any[];
  // loadrank4: any[];
  // loadrank5: any[];
  // loadrank6: any[];
  // loadrank7: any[];
  // loadrank8: any[];

  apply = 0;
  cols: any[];
  display: boolean=false;

  mybusiness:string;

  columns: any[] = [{ label: 'Name', value: 'name' }, { label: 'Category', value: 'category' }, { label: 'Quantity', value: 'quantity' },
  { label: 'On Hand', value: 'onhand' },
  { label: 'In Transit', value: 'intransit' }];

  businessList = [{label:'全选', value: false,type:"all"},{label:'公司', value: false}, {label: '房产开发', value: false}, {label: '小区', value: false}, {label: '工业', value: false}, {label: '政府', value: false}, {label: '路灯', value: false}, {label: '公寓', value: false}, {label: '宿舍', value: false}];
  // parameters: string;
  matList = [{label: '成品', value: false}, {label: '组装原件', value: false}];
  satList = [{label: '运输中', value: false}, {label: '阻塞', value: false}, {label: '过期', value: false}, {label: '质量问题', value: false}, {label: '未受限', value: false}];
  clsList = [{label: 'V1', value: false}, {label: 'V2', value: false}, {label: 'V3', value: false}, {label: 'V4', value: false}, {label: 'V5', value: false}];
  retList = [{label: '关闭', value: false}, {label: '开放', value: false}];

  selectedmeter: any;

  //A list to track all data in tableS

  // loadDataList: any[] = [[],[]];
  loadDataList: any[] = [];
  allDataList: any[] = [];

  style_main = {};


  constructor(private filtertypicalService: FiltertypicalService,
              private ref: ChangeDetectorRef,
              private zone: NgZone,
              private newelasticityservice: NewelasticityService,
              private idService: GlobalIdService,
              private graphChartService: GraphChartService,
              private dataQualityService: DataQualityService,
              private bigdataserive: BigDataService,
              private useranalysisserive: UserAnalysisService,
              private mapinfoService: LeafletMapLatLngService,
             ) {

              this.subscription = this.graphChartService.graphChart$.subscribe(
                chart => {
                    this.chart = chart;
                }
              );




              this.idsubscription = this.idService.id$.subscribe(
                (id) => {
                  console.log('new selected id arrived ' + id);

                  if (this.map !== undefined) {
                          this.onMapUpdate(+id);
                  }
                        }

                )

      this.sdate = this.defaultDates;
      this.edate = this.defaultDatee;
      this.meters = [];



      this.subscriptions.push(this.idService.id$.subscribe(
        (id) => {
          this.selectedID = id;
          let selectedNode;

          if (this.chart !== undefined) {
            this.chart.getNodes().forEach(
              (n) => {
                if ( n.exID == id) {
                  selectedNode = n;
                  console.log(selectedNode);
                }
              }
            );
            this.chart
              .unselectAllElements()
              .setSelection([selectedNode])
              .scrollIntoView([selectedNode])
              .update();
          }
        }
      ))

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
        title: { text: '用户年用电量统计' },
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

      // this.options6 = {
      //   title: { text: '用户年用电量统计' },
      //   chart: {
      //     zoomType: 'x',
      //     height: 200,
      //     width: 600
      //   },
      //   xAxis: {
      //     categories: []
      //   },
      //   yAxis: {
      //     title: {
      //       text: " "
      //     }
      //   },
      //   series: [{
      //     showInLegend: false,
      //     type: 'column',
      //     data: [0],
      //     allowPointSelect: false
      //   }
      //   ]
      // };
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
        { field: 'businesstype', header: '行业类别'},
      ]

  }

  @ViewChild('mainScreen') elementView: ElementRef;
  @ViewChild('rightScreen') rightView: ElementRef;
  @ViewChild('chartView') chartView: ElementRef;

  viewHeight: number;
  viewWidth: number;

  style_left: object = {};
  style_right: object = {};
  style_top: object = {};
  style_bottom: object = {};
  // style_uig_left: object = {};
  style_uig_left: {width: string} = {width: '50%'};
  style_uig_right: {width: string} = {width:'50%'};

  displayedColumns = ['name', 'category', 'quantity', 'onhand', 'intransit'];

  dataSource: any;
  // dataSource = new MatTableDataSource(this.data);

  @ViewChild(MatSort) sort: MatSort;
  // @ViewChild(GlobalMapComponent) myMap;

  ngOnInit() {
    // console.log('fiterbusiness ran hahaha...');
    // this.resizeMap();
  }

  validate_left(event: ResizeEvent): boolean {
    const MIN_DIMENSIONS_PX: number = 250;
    if (
      event.rectangle.width &&
      event.rectangle.height &&
      (event.rectangle.width < MIN_DIMENSIONS_PX ||
        event.rectangle.height < MIN_DIMENSIONS_PX)
    ) {
      return false;
    }
    return true;
  }

  resizeMap() {
    setTimeout(()=>{
      this.dlMap.map.resize();
    }, 50);
  }

  onResize(event) {
    console.log(event);
  }

  onResizeEnd_left(event: ResizeEvent): void {
    this.viewHeight = this.elementView.nativeElement.offsetHeight;
    this.viewWidth = this.elementView.nativeElement.offsetWidth;
    var left_width_perc = event.rectangle.width / this.viewWidth * 100
    this.style_left = {
      width: left_width_perc + '%'
    };
    var right_width_perc = 100 - left_width_perc;
    this.style_right = {
      width: right_width_perc + '%'
    };
    var percent = parseFloat(this.style_uig_right['width'].slice(0, this.style_uig_right['width'].length));
    var width = (this.viewWidth - event.rectangle.width) *  percent / 100;
    // console.log(percent);
    // console.log(width);
    // console.log(this.chartView.nativeElement.offsetWidth);
    // this.options6["chart"]["width"]= width;
    this.useranalysisserive.PlotCategorySummary().then(
      line => {
        this.options1 = line
        this.options1["chart"]["width"] = width;
      })

    this.useranalysisserive.PlotCategoryUserUsage("公司").then(
      line => {
        this.options6 = line[0];
        console.log(this.options6);
        this.loadrank = line[1];
        this.options6["chart"]["width"] = width;
        
      }
    )
    console.log(width);

    this.resizeMap();
  }

  onResizeEnd_uig_left(event: ResizeEvent): void {
    this.viewHeight = this.rightView.nativeElement.offsetHeight;
    this.viewWidth = this.rightView.nativeElement.offsetWidth;
    var left_width_perc = event.rectangle.width / this.viewWidth * 100
    this.style_uig_left = {
      width: left_width_perc + '%'
    };
    var right_width_perc = 100 - left_width_perc;
    this.style_uig_right = {
      width: right_width_perc + '%'
    };

    var width = (this.viewWidth - event.rectangle.width);

    console.log(this.viewWidth);
    console.log(event.rectangle.width);
    console.log(width);
    this.useranalysisserive.PlotCategorySummary().then(
      line => {
        this.options1 = line;
        this.options1["chart"]["width"] = width;
      })

    this.useranalysisserive.PlotCategoryUserUsage("公司").then(
      line => {
        this.options6 = line[0];
        this.options6["chart"]["width"] = width;        
      }
    )

    this.resizeMap();
  }

  onResizeEnd_top(event: ResizeEvent): void {
    this.viewHeight = this.elementView.nativeElement.offsetHeight;
    this.viewWidth = this.elementView.nativeElement.offsetWidth;
    var top_height_perc = event.rectangle.height / this.viewHeight * 100
    this.style_top = {
      height: top_height_perc + '%'
    };
    var bottom_height_perc = 100 - top_height_perc;
    this.style_bottom = {
      height: bottom_height_perc + '%'
    };
    this.resizeMap();
  }

  ngAfterViewInit() {
    this.dataQualityService.GetIDList().then(
      line => {
        line.result.forEach(element => {
          this.meters.push({ label: element, value: element })
        });
        // console.log(this.meters);
      }
    )

    this.plot_category_summary();
    // this.plot_category_user_usage();

    this.useranalysisserive.PlotCategoryUserUsage("公司").then(
      line => {
        this.options6 = line[0];
        console.log(this.options6);
        this.loadrank = line[1];
        // this.options6["chart"]["height"]=this.iH*4/8;
        this.options6["chart"]["height"]=this.iH*0.35;
        this.options6["chart"]["width"]=this.iW/4*3/2;
        
        this.resizeMap();
      }
    )

    console.log(this.options6["chart"]["height"]);
    console.log(this.iH);
    this.resizeMap();

    // var height = this.elementView.nativeElement.offsetHeight - 101;
    // var height_per = height / this.elementView.nativeElement.offsetHeight;
    // this.style_main = {
    //   height: height_per + '%'
    // }
  }

  plot_category_summary(): void {
    console.log("plot_category_summary");
    this.useranalysisserive.PlotCategorySummary().then(
      line => {
        this.options1 = line
        this.options1["chart"]["height"]=this.iH*0.35;
        this.options1["chart"]["width"]=this.iW/4*3/2;
        this.resizeMap();
      })
  }



  // for marking the selected id
  onMapUpdate(selectedmeter: number) {
    console.log('map update, new selected id ' + selectedmeter);
    console.log(this.map);
    let flag = 0;
    let latNew, lngNew, newlayer, newlayer2;
    this.map.eachLayer((layer) => {
            // console.log(layer);
            if (layer['options']['attribution'] !== 'Open Street Map') {
                    // console.log(layer);
                    if (layer['_latlng']) {
                            // console.log(layer);
                            if (layer['options']['name']) {
                                    if (layer['options']['name'] === selectedmeter) {
                                            console.log('change flag ' + selectedmeter);
                                            flag = 1;
                                            latNew = layer['_latlng']['lat'];
                                            lngNew = layer['_latlng']['lng'];
                                            layer.remove();

                                    }
                            }
                    }
            }
    })
    if (flag === 1) {
            this.map.eachLayer((layer) => {
                    if (layer['options']['color'] === 'chocolate') {
                            this.id = layer['options']['name'];
                            this.lat = layer['_latlng']['lat'];
                            this.lng = layer['_latlng']['lng'];
                            layer.remove();
                    }
            })
    }
    if (flag === 1) {
            newlayer = L.circle([latNew, lngNew], {
                    radius: 2000,
                    name: selectedmeter,
                    color: 'chocolate',
                    fillColor: 'chocolate',
                    opacity: 0.8,
                    fillOpacity: 0.8
            }).addTo(this.map);
            newlayer.bindTooltip('ID: ' + String(selectedmeter) + ' Lat: ' + String(latNew) + ', Lng: ' + String(lngNew));
            this.map.addLayer(newlayer);

            newlayer2 = L.circle([this.lat, this.lng], {
                    radius: 1000,
                    name: selectedmeter,
                    color: 'darkcyan',
                    fillColor: 'darkcyan',
                    opacity: 0.8,
                    fillOpacity: 0.8
            }).addTo(this.map);
            newlayer2.bindTooltip('ID: ' + String(selectedmeter) + ' Lat: ' + String(this.lat) + ', Lng: ' + String(this.lng));
            this.map.addLayer(newlayer2);
    }
}


  onPointSelect(e) {

    this.useranalysisserive.PlotCategoryUserUsage(e.context.category).then(
      line => {
        this.options6 = line[0];
        // this.loadrank = line[1];
        // this.options6["chart"]["height"]=this.iH*4/8;
        this.options6["chart"]["width"]=this.iW/4*3/2;
        // this.resizeMap();
      }
    )
  }

  onRowSelect(e){
    console.log(e);
    console.log(e.data.user);
    // this.bigdataserive.PlotEvery(e.data.user, this.sdate.getTime(), this.edate.getTime())
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
    

  }

  siteName = "";
  selectedSiteList = [];


  changeid () {
    this.idService.announceId(this.selectedmeter);
    console.log(this.selectedmeter);
  }


  addToSelectedSiteList() {
    if (this.selectedSiteList.indexOf(this.siteName) < 0) {
      this.selectedSiteList.push(this.siteName);
    }
    this.siteName = "";
    console.log(this.selectedSiteList);
  }

  deleteFromSelectedSiteList(name) {
    var index = this.selectedSiteList.indexOf(name)
    this.selectedSiteList.splice(index, index + 1);
  }

  reset() {
    this.apply = 0;
    this.siteName = "";
    this.selectedSiteList = [];

    this.loadrank = [];

    this.businessList[0].value = false;
    this.businessList[1].value = false;
    this.businessList[2].value = false;
    this.businessList[3].value = false;
    this.businessList[4].value = false;
    this.businessList[5].value = false;
    this.businessList[6].value = false;
    this.businessList[7].value = false;
    this.businessList[8].value = false;

    this.satList[0].value = false;
    this.satList[1].value = false;
    this.satList[2].value = false;
    this.satList[3].value = false;
    this.satList[4].value = false;

    this.matList[0].value = false;
    this.matList[1].value = false;

    this.clsList[0].value = false;
    this.clsList[1].value = false;
    this.clsList[2].value = false;
    this.clsList[3].value = false;
    this.clsList[4].value = false;

    this.retList[0].value = false;
    this.retList[1].value = false;
  }

  getParams(): string {
    var params = [];
    // this.selectedSiteList.forEach(element => {
    //   params.push("seeds=elecotest:site:" + element);
    // });

    // join the list of selected business---Jack
    if (this.businessList[1].value || this.businessList[2].value || this.businessList[3].value || this.businessList[4].value || this.businessList[5].value || this.businessList[6].value || this.businessList[7].value || this.businessList[8].value) {
      if (this.businessList[1].value) {
        params.push("公司");
      }
      if (this.businessList[2].value) {
        params.push("房产开发");
      }
      if (this.businessList[3].value) {
        params.push("小区");
      }
      if (this.businessList[4].value) {
        params.push("工业");
      }
      if (this.businessList[5].value) {
        params.push("政府");
      }
      if (this.businessList[6].value) {
        params.push("路灯");
      }
      if (this.businessList[7].value) {
        params.push("公寓");
      }
      if (this.businessList[8].value) {
        params.push("宿舍");
      }
    } else {
      params.push("公司");
      params.push("房产开发");
      params.push("小区");
      params.push("工业");
      params.push("政府");
      params.push("路灯");
      params.push("公寓");
      params.push("宿舍");
    }



   
    return params.join(",");
    // this.parameters = params.join(",");
  }

  loadData(param?: any) {

    this.apply = 1;

    // this.mybusiness = this.getParams();

    this.useranalysisserive.PlotCategoryUserUsageList(this.getParams()).then(
      line => {
        // this.options3 = line[0];
        this.loadrank = line;
        console.log(this.loadrank);
        
      });
   

    // merge the loadDataList into a big data-table
    console.log("loadDataList_length:"+this.loadDataList.length);
    console.log("loadDataList:"+this.loadDataList);

  }
  Clicked()
  {
    console.log("123")
    this.showValue=!this.showValue;
  }
  itemClicked2(item2){
    this.listtype=item2;
    if(this.listtype=="all")
    {
        for(var i=1;i<9;i++)
        {
          this.businessList[i]["value"]=true;
        }
     }
  }
}

