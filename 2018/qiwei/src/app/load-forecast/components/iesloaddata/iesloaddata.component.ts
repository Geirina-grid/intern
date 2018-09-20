import { Component, OnInit, ViewChild, ChangeDetectorRef, NgZone } from '@angular/core';

import { MatTableDataSource, MatSort } from '@angular/material';

import { FiltertypicalService, Product } from '../../../app-shared/services/filtertypical.service';

import { FormControl } from '@angular/forms';

import { Observable } from 'rxjs/Observable';
import { startWith } from 'rxjs/operators/startWith';
import { map } from 'rxjs/operators/map';
import { Subscription } from 'rxjs/Subscription';

import { DomSanitizer } from '@angular/platform-browser';
import { MatIconRegistry } from '@angular/material';

import { GlobalIdService } from '../../../app-shared/services/global-id.service';
import { NewelasticityService } from '../../../app-shared/services/newelasticity.service';
import { DataQualityService } from '../../../data-analysis/services/data-quality.service';
import { GraphChartService } from '../../../app-shared/services/graph-chart.service';

import { UserAnalysisService } from '../../../data-analysis/services/user-analysis.service'
import { BigDataService } from '../../../data-analysis/services/big-data.service'

// for marking the selected id
import { icon, latLng, Layer, marker, circle, tileLayer } from 'leaflet';
import { HackMapDirective } from '../../../app-shared/directives/hack-map.directive';
import { LeafletMapLatLngService } from '../../../app-shared/services/leafletmap.service';

import { DrawLineComponent } from '../../../app-shared/add-a-map/component/draw-line.component';

import * as gvis from 'gvis';
import * as L from 'leaflet';

//Bowen


import { LoaddatagraphService } from '../../../app-shared/services/loaddatagraph.service';
const GraphChart = (<any>gvis).GraphChart;


@Component({
  selector: 'app-iesloaddata',
  templateUrl: './iesloaddata.component.html',
  styleUrls: ['./iesloaddata.component.css'],
  providers: [FiltertypicalService, NewelasticityService, GlobalIdService, DataQualityService, BigDataService]
})
export class IesloaddataComponent implements OnInit {

  subscriptions: Subscription[] = [];
  public chart: any;
  idsubscription: Subscription;
  map: any;
  sdate: Date;
  edate: Date;
  defaultDates = new Date('01/01/2013');
  defaultDatee = new Date('12/31/2013');
  meters: any;
  subscription: Subscription;
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
  selectedmeter: any;
  selectedID: number;
  options1: Object;
  options2: Object;
  options3: Object;
  options4: Object;
  options5: any[];
  options6: object;
  cols: any[];
  loadrank: any[];
  iH = window.innerHeight-120;
  iW = Math.min(window.innerWidth,2000);

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
              private loaddataService: LoaddatagraphService,
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
             /*
             this.options1 = {
                 title: { text: '负荷分析-1' },
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
              this.options6 = {
                  title: { text: '负荷分析-2' },
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
                     allowPointSelect: false
                  }
                 ]
              };
              */
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
    displayedColumns = ['name', 'category', 'quantity', 'onhand', 'intransit'];
    dataSource: any;
    @ViewChild(MatSort) sort: MatSort;
    ngOnInit() {
    }

    ngAfterViewInit() {

       this.loaddataService.getMeterID("80", "meterid").then(
            line => {
                line.results.forEach(element => {
                   //this.meters.push({ label: element, value: element })
                   console.log(element);
                   console.log(element.meterids);
                   element.meterids.forEach(meters => {
                       console.log(meters.attributes);
                       console.log(meters.attributes["meterids.@dataset_meterid"]);
                       meters.attributes["meterids.@dataset_meterid"].forEach(meter=>{
                           this.meters.push({label: meter, value: meter})
                       })
                   })
                });
                //console.log(line);
             }
       )
       /*
       this.dataQualityService.GetIDList().then(
       line => {
           line.result.forEach(element => {
              this.meters.push({ label: element, value: element })
            });
        }
        )
        */
        this.plot_category_summary();

        this.useranalysisserive.PlotCategoryUserUsage("公司").then(
        line => {
            this.options6 = line[0];
            console.log(this.options6);
            this.loadrank = line[1];
            this.options6["chart"]["height"]=this.iH*4/8;
            this.options6["chart"]["width"]=this.iW/4*3/2;
        }
       )
     }

    plot_category_summary(): void {
        console.log("plot_category_summary");
        this.useranalysisserive.PlotCategorySummary().then(
          line => {
            this.options1 = line
            this.options1["chart"]["height"]=this.iH*4/8;
            this.options1["chart"]["width"]=this.iW/4*3/2;
      })
    }
 
    onMapUpdate(selectedmeter: number) {
        console.log('map update, new selected id ' + selectedmeter);
        console.log(this.map);
        let flag = 0;
        let latNew, lngNew, newlayer, newlayer2;
        this.map.eachLayer((layer) => {
            if (layer['options']['attribution'] !== 'Open Street Map') {
                    if (layer['_latlng']) {
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
    
    changeid () {
        this.idService.announceId(this.selectedmeter);
        console.log(this.selectedmeter);
    }

         

}
