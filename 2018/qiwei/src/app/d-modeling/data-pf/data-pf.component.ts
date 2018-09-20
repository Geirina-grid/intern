import { Component, OnInit, ViewChild,ElementRef, ChangeDetectorRef, NgZone, AfterViewInit } from '@angular/core';

//import { datapfModule } from './data-pf.module'

import { MatTableDataSource, MatSort,MatAutocomplete } from '@angular/material';

import { DfserService } from '../data-pf/dfser.service';
//import { GlobalMapComponent } from '../global-map/global-map.component';

import { FormControl } from '@angular/forms';

import { Observable } from 'rxjs/Observable';
import { startWith } from 'rxjs/operators/startWith';
import { map } from 'rxjs/operators/map';

import { DomSanitizer } from '@angular/platform-browser';
import { MatIconRegistry } from '@angular/material';
import { environment } from '../../../environments/environment';

import { GraphChart } from 'gvis';
import { GraphChartService } from '../../app-shared/services/graph-chart.service';
import { Subscription } from 'rxjs/Subscription';
import value from '*.json';
import { DataTableComponent } from '../../shared/components/data-table/data-table.component'
import { ResizeEvent } from 'angular-resizable-element';
import { DrawLineComponent } from '../../app-shared/add-a-map/component/draw-line.component';

import * as gvis from 'gvis';
const PieChart = (<any>gvis).PieChart;
const LineChart = (<any>gvis).LineChart;
const BarChart = (<any>gvis).BarChart;
@Component({
  selector: 'app-data-pf',
  templateUrl: './data-pf.component.html',
  styleUrls: ['./data-pf.component.css'],
  providers: [DfserService]
})
export class DataPfComponent implements OnInit, AfterViewInit {
  
@ViewChild('chart') idc: any;
@ViewChild(DataTableComponent) myTable;
@ViewChild(DrawLineComponent) dlMap;
@ViewChild('mainScreen') elementView: ElementRef;
@ViewChild('rightScreen') rightView: ElementRef;
@ViewChild('chartView') chartView: ElementRef;
@ViewChild('container1') container1: any;
@ViewChild('mapre') View: ElementRef;

//chart 
public loadPie: any;
options1:any;
private lineXAdata:any;
private lineXBdata:any;
private lineXCdata:any;
private capC=[];
private Cheight=300;
private Cwidth=900;
// for business data-table---Jack



  iH = window.innerHeight-120;
  iW = Math.min(window.innerWidth,2000);
  tab_sh:string;
  scroll_sh:string;

  viewHeight: number;
  viewWidth: number;

  style_left: object = {};
  style_right: object = {};
  style_top: object = {};
  style_bottom: object = {};
  // style_uig_left: object = {};
  style_uig_left: {width: string} = {width: '50%'};
  style_uig_right: {width: string} = {width:'50%'};


  checkboxValue=false;
  public chart: any;
  subscription: Subscription;
  regResIDs: string[] = [];
  ID:string[]=[];
  HID:string[]=[];
  lineEdges: any[] = [];
  listtype:string;
  columns: any[] = [/* { label: 'Name', value: 'name' }, 
  { label: 'Category', value: 'category' }, 
  { label: 'Quantity', value: 'quantity' },
  { label: 'On Hand', value: 'onhand' }, */
  { label: '请点击过滤器选择数据列表', value: '请点击过滤器选择数据列表' }]
  displayedColumns = ['请点击过滤器选择数据列表'];
  // displayedColumns = ['name', 'category', 'quantity', 'onhand', 'intransit'];

  lineList=[{label:'全部',value:false, group:'line',type:'all'},
  {label:"架空线",value:false,group:'line', type:'overhead line'},
  {label:"地下电缆",value:false,group:'line', type:'underground cable'}
];
  regulatorList=[{label:'全部',value:false,group:'regulator'}];

  SVCList=[{label:'全部',value:false,group:'SVC',type:'all'},
  {label:'电压控制',value:false,group:'SVC',type:'voltage'},
  {label:'电流控制',value:false,group:'SVC',type:'current'},
//  {label:'迟滞控制',value:false,group:'SVC',type:'droop'},
];

  switchList=[{label:'全部',value:false,group:'switch'}];
  loadList=[{label:'全部',value:false,group:'load',type:'loadV'}];

  batteryList=[{label:'新能源概况',value:false,group:'battery',type:'res'},
  {label:'风电',value:false,group:'battery',type:'wind'},
  {label:'太阳能',value:false,group:'battery',type:'PV'},
  {label:'储能',value:false,group:'battery',type:'battery'}];
  Newdata:any;
  Typeflg:number;//determine which box is selected;
  showRegulator=false;
  showSVC=false;
  showLine=false;
  showLoad=false;
  showBattery=false;
  showSwitch=false;
  ifshowS=true;
  NoPV=0;
  Nowind=0;
  Nobattery=0;
  constructor(private productsService: DfserService, 
    private ref: ChangeDetectorRef,
    private zone: NgZone,
    private graphChartService: GraphChartService,
    private DfserService:DfserService 
    ) {
      this.productsService.getDmodles().then(data=>{
      // let event: ResizeEvent;
      // this.viewWidth = this.elementView.nativeElement.offsetWidth;
      // this.viewHeight = this.rightView.nativeElement.offsetWidth;
      // var left_width_perc = event.rectangle.width / this.viewWidth * 100
      // this.style_left = {
      //   width: left_width_perc + '%'
      // };
      // var right_width_perc = 100 - left_width_perc;
      // this.style_right = {
      //   width: right_width_perc + '%'
      // };
      // var percent = parseFloat(this.style_uig_right['width'].slice(0, this.style_uig_right['width'].length));
      // var width = (this.viewWidth - event.rectangle.width) *  percent / 100;
      // console.log(this.viewWidth)
      // console.log(this.viewHeight)
      this.Nobattery=data[0]["NoES"];
      this.Nowind=data[0]["NoWG"];
      this.NoPV=data[0]["NoPV"];
      $('#containerS').highcharts({
        title: { text: '系统总览' },
        chart: {
            zoomType: 'x',

        },
        xAxis: {
            categories: []
        },
        yAxis: {
            title: {
                text: "个数"
            }
        },
        series: [{
            name: '节点数量',
            // showInLegend: true,
            type: 'column',
            data: [data[0]["NoBS"]],
            // allowPointSelect: true
        }, {
            name: '线路数量',
            // showInLegend: true,
            type: 'column',
            data: [data[0]["NoLN"]],
            // allowPointSelect: true
        }, {
            name: 'PV数量',
            // showInLegend: true,
            type: 'column',
            data: [data[0]["NoPV"]],
            // allowPointSelect: true
        },
        {
          name: 'WG数量',
          // showInLegend: true,
          type: 'column',
          data: [data[0]["NoWG"]],
          // allowPointSelect: true
      },
        {
          name: 'ES数量',
          // showInLegend: true,
          type: 'column',
          data: [data[0]["NoES"]],
          // allowPointSelect: true
         },
        {
          name: '负荷数量',
          // showInLegend: true,
          type: 'column',
          data: [data[0]["NoLD"]],
          // allowPointSelect: true
        }
        ]
    });

  });

  }


  test(){
    if(this.myTable.highlightedRows!=0)
    {
      var keys = Object.keys(this.myTable.highlightedRows[0]); 
      switch(keys[0])
      {
        case "SVCname":
        {
          for (var i=0;i<this.myTable.highlightedRows.length;i++)
          {
            this.HID.push(this.myTable.highlightedRows[i]["SVCBus"]);
          }
          this.BusHighLightAll(this.HID);
          this.HID=[];
          break;
        }

        case "linename":
        {
          for (var i=0;i<this.myTable.highlightedRows.length;i++)
          {
            this.HID.push(this.myTable.highlightedRows[i]["linename"]);
           
          }
          this.LineHighLightAll(this.HID);
          console.log(this.HID);
          this.HID=[];
          break;
        }

        case "rename":
        {
          for (var i=0;i<this.myTable.highlightedRows.length;i++)
          {
            this.HID.push(this.myTable.highlightedRows[i]["rebus"]);
            console.log(this.myTable.highlightedRows[i]["rebus"]);
          }
          this.BusHighLightAll(this.HID);
          this.HID=[];
          break;
        }

        case "loadID":
        {
          for (var i=0;i<this.myTable.highlightedRows.length;i++)
          {
            this.HID.push(this.myTable.highlightedRows[i]["lbus"]);
            console.log(this.myTable.highlightedRows[i]["lbus"]);
          }
          this.BusHighLightAll(this.HID);
          this.HID=[];
          break;
        }

/*         case "baname":
        {
          for (var i=0;i<this.myTable.highlightedRows.length;i++)
          {
            this.HID.push(this.myTable.highlightedRows[i]["SVCbus"]);
            console.log(this.myTable.highlightedRows[i]["SVCbus"]);
          }
          this.BusHighLightAll(this.HID);
          break;
        } */
/*         case "Swname":
        {
          for (var i=0;i<this.myTable.highlightedRows.length;i++)
          {
            this.HID.push(this.myTable.highlightedRows[i]["SVCbus"]);
            console.log(this.myTable.highlightedRows[i]["SVCbus"]);
          }
          this.BusHighLightAll(this.HID);
          break;
        } */

      }

    }
    else
    {
      var id=[];
      this.BusHighLightAll(id);
    }

  }



  dataSource: any;
  public sumarrys: Array<any>;
  DmIDs: any[] = [];
  ngOnInit() 
  {
    this.productsService.getDmodles()
    .then(modlearrys => {
        this.sumarrys = modlearrys;
        this.sumarrys.forEach(summ => {
            this.DmIDs.push(summ.Sys_id);
        });
    });
  }

  summaryinfor(){
console.log(this.dlMap.featureO);



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
    var width = (this.viewWidth - event.rectangle.width);
    this.Cwidth= width-520;
    this.InteractCWG(this.listtype);

    this.resizeMap();
  }

  onResizeEnd_uig_left(event: ResizeEvent): void {
    this.viewHeight = this.elementView.nativeElement.offsetHeight;

    var top_height_perc = event.rectangle.height / this.viewHeight * 100
    this.style_top = {
      height: top_height_perc + '%'
    };
    var bottom_height_perc = 100 - top_height_perc;
    this.style_bottom = {
      height: bottom_height_perc + '%'
    };

    
    var height = (this.viewHeight - event.rectangle.height);
    this.Cheight= event.rectangle.height-362;



    this.viewWidth = this.rightView.nativeElement.offsetWidth;
    var left_width_perc = event.rectangle.width / this.viewWidth * 100
    this.style_uig_left = {
      width: left_width_perc + '%'
    };
    var right_width_perc = 100 - left_width_perc;
    this.style_uig_right = {
      width: right_width_perc + '%'
    };


    this.viewWidth = this.elementView.nativeElement.offsetWidth;
    var top_height_perc = event.rectangle.height / this.viewHeight * 100
    this.style_top = {
      height: top_height_perc + '%'
    };
    var bottom_height_perc = 100 - top_height_perc;
    this.style_bottom = {
      height: bottom_height_perc + '%'
    };

    var width = (this.viewWidth - event.rectangle.width);
    this.Cwidth= width-520;
        var height = (this.viewHeight - event.rectangle.height);
    this.Cheight= event.rectangle.height-362;
    this.InteractCWG(this.listtype);

    console.log(this.viewWidth);
    console.log(event.rectangle.width);
    console.log(width);
    

    this.resizeMap();
  }

  onResizeEnd_top(event: ResizeEvent): void {
    this.viewHeight = this.elementView.nativeElement.offsetHeight;

    var top_height_perc = event.rectangle.height / this.viewHeight * 100
    this.style_top = {
      height: top_height_perc + '%'
    };
    var bottom_height_perc = 100 - top_height_perc;
    this.style_bottom = {
      height: bottom_height_perc + '%'
    };

    
    var height = (this.viewHeight - event.rectangle.height);
    this.Cheight= event.rectangle.height-362;
    this.InteractCWG(this.listtype);

    console.log(this.viewHeight);
    console.log(event.rectangle.height);
    console.log(this.Cheight);

    this.resizeMap();
  }




  ngAfterViewInit(): void {

    // this.chart=this.idc.chart;
    // throw new Error("Method not implemented.");
  }



  reset() {
    this.checkboxValue=false;
    for(var i=0;i<this.lineList.length;i++)
    {
      this.lineList[i].value=false;
    }
    for(var i=0;i<this.regulatorList.length;i++)
    {
      this.regulatorList[i].value=false;
    }
    for(var i=0;i<this.SVCList.length;i++)
    {
      this.SVCList[i].value=false;
    }
    for(var i=0;i<this.switchList.length;i++)
    {
      this.switchList[i].value=false;
    }
    for(var i=0;i<this.loadList.length;i++)
    {
      this.loadList[i].value=false;
    }
    for(var i=0;i<this.batteryList.length;i++)  
    { 
      this.batteryList[i].value=false;
    }
  }

  Partreset(com:string)
  {

    switch(com)
    {
      case "line":
            for(var i=0;i<this.regulatorList.length;i++)
            {
              this.regulatorList[i].value=false;
            }
            for(var i=0;i<this.SVCList.length;i++)
            {
              this.SVCList[i].value=false;
            }
            for(var i=0;i<this.switchList.length;i++)
            {
              this.switchList[i].value=false;
            }
            for(var i=0;i<this.loadList.length;i++)
            {
              this.loadList[i].value=false;
            }
            for(var i=0;i<this.batteryList.length;i++)  
            { 
              this.batteryList[i].value=false;
            }
        break;
      case "regulator":
            for(var i=0;i<this.lineList.length;i++)
            {
              this.lineList[i].value=false;
            }
            for(var i=0;i<this.SVCList.length;i++)
            {
              this.SVCList[i].value=false;
            }
            for(var i=0;i<this.switchList.length;i++)
            {
              this.switchList[i].value=false;
            }
            for(var i=0;i<this.loadList.length;i++)
            {
              this.loadList[i].value=false;
            }
            for(var i=0;i<this.batteryList.length;i++)  
            { 
              this.batteryList[i].value=false;
            }
        break;
      case "SVC":
            for(var i=0;i<this.lineList.length;i++)
            {
              this.lineList[i].value=false;
            }
            for(var i=0;i<this.regulatorList.length;i++)
            {
              this.regulatorList[i].value=false;
            }
            for(var i=0;i<this.switchList.length;i++)
            {
              this.switchList[i].value=false;
            }
            for(var i=0;i<this.loadList.length;i++)
            {
              this.loadList[i].value=false;
            }
            for(var i=0;i<this.batteryList.length;i++)  
            { 
              this.batteryList[i].value=false;
            }
        break;
      case "load":
            for(var i=0;i<this.lineList.length;i++)
            {
              this.lineList[i].value=false;
            }
            for(var i=0;i<this.regulatorList.length;i++)
            {
              this.regulatorList[i].value=false;
            }
            for(var i=0;i<this.SVCList.length;i++)
            {
              this.SVCList[i].value=false;
            }
            for(var i=0;i<this.switchList.length;i++)
            {
              this.switchList[i].value=false;
            }
            for(var i=0;i<this.batteryList.length;i++)  
            { 
              this.batteryList[i].value=false;
            }
        break;
      case "battery":
            for(var i=0;i<this.lineList.length;i++)
            {
              this.lineList[i].value=false;
            }
            for(var i=0;i<this.regulatorList.length;i++)
            {
              this.regulatorList[i].value=false;
            }
            for(var i=0;i<this.SVCList.length;i++)
            {
              this.SVCList[i].value=false;
            }
            for(var i=0;i<this.switchList.length;i++)
            {
              this.switchList[i].value=false;
            }
            for(var i=0;i<this.loadList.length;i++)
            {
              this.loadList[i].value=false;
            }
            for(var i=0;i<this.batteryList.length;i++)
            {
              this.batteryList[i].value=false;
            }
        break;
      case "switch":
            for(var i=0;i<this.lineList.length;i++)
            {
              this.lineList[i].value=false;
            }
            for(var i=0;i<this.regulatorList.length;i++)
            {
              this.regulatorList[i].value=false;
            }
            for(var i=0;i<this.SVCList.length;i++)
            {
              this.SVCList[i].value=false;
            }
            for(var i=0;i<this.loadList.length;i++)
            {
              this.loadList[i].value=false;
            }
            for(var i=0;i<this.batteryList.length;i++)  
            { 
              this.batteryList[i].value=false;
            }
        break;
    }
  }

  PartAllTrue(com:string)
  {

    switch(com)
    {
      case "line":
            for(var i=0;i<this.regulatorList.length;i++)
            {
              this.regulatorList[i].value=false;
            }
            for(var i=0;i<this.SVCList.length;i++)
            {
              this.SVCList[i].value=false;
            }
            for(var i=0;i<this.switchList.length;i++)
            {
              this.switchList[i].value=false;
            }
            for(var i=0;i<this.loadList.length;i++)
            {
              this.loadList[i].value=false;
            }
            for(var i=0;i<this.batteryList.length;i++)  
            { 
              this.batteryList[i].value=false;
            }
            for(var i=0;i<this.lineList.length;i++)
            {
              this.lineList[i].value=true;
            }
        break;
      case "regulator":
            for(var i=0;i<this.regulatorList.length;i++)
            {
              this.regulatorList[i].value=true;
            }
            for(var i=0;i<this.lineList.length;i++)
            {
              this.lineList[i].value=false;
            }
            for(var i=0;i<this.SVCList.length;i++)
            {
              this.SVCList[i].value=false;
            }
            for(var i=0;i<this.switchList.length;i++)
            {
              this.switchList[i].value=false;
            }
            for(var i=0;i<this.loadList.length;i++)
            {
              this.loadList[i].value=false;
            }
            for(var i=0;i<this.batteryList.length;i++)  
            { 
              this.batteryList[i].value=false;
            }
        break;
      case "SVC":
            for(var i=0;i<this.SVCList.length;i++)
            {
              this.SVCList[i].value=true;
            }
            for(var i=0;i<this.lineList.length;i++)
            {
              this.lineList[i].value=false;
            }
            for(var i=0;i<this.regulatorList.length;i++)
            {
              this.regulatorList[i].value=false;
            }
            for(var i=0;i<this.switchList.length;i++)
            {
              this.switchList[i].value=false;
            }
            for(var i=0;i<this.loadList.length;i++)
            {
              this.loadList[i].value=false;
            }
            for(var i=0;i<this.batteryList.length;i++)  
            { 
              this.batteryList[i].value=false;
            }
        break;
      case "load":
            for(var i=0;i<this.loadList.length;i++)
            {
              this.loadList[i].value=true;
            }
            for(var i=0;i<this.lineList.length;i++)
            {
              this.lineList[i].value=false;
            }
            for(var i=0;i<this.regulatorList.length;i++)
            {
              this.regulatorList[i].value=false;
            }
            for(var i=0;i<this.SVCList.length;i++)
            {
              this.SVCList[i].value=false;
            }
            for(var i=0;i<this.switchList.length;i++)
            {
              this.switchList[i].value=false;
            }
            for(var i=0;i<this.batteryList.length;i++)  
            { 
              this.batteryList[i].value=false;
            }
        break;
      case "battery":
            for(var i=0;i<this.lineList.length;i++)
            {
              this.lineList[i].value=false;
            }
            for(var i=0;i<this.regulatorList.length;i++)
            {
              this.regulatorList[i].value=false;
            }
            for(var i=0;i<this.SVCList.length;i++)
            {
              this.SVCList[i].value=false;
            }
            for(var i=0;i<this.switchList.length;i++)
            {
              this.switchList[i].value=false;
            }
            for(var i=0;i<this.loadList.length;i++)
            {
              this.loadList[i].value=false;
            }
            for(var i=0;i<this.batteryList.length;i++)  
            { 
              this.batteryList[i].value=true;
            }
        break;
      case "switch":
      for(var i=0;i<this.switchList.length;i++)
      {
        this.switchList[i].value=true;
      }
            for(var i=0;i<this.lineList.length;i++)
            {
              this.lineList[i].value=false;
            }
            for(var i=0;i<this.regulatorList.length;i++)
            {
              this.regulatorList[i].value=false;
            }
            for(var i=0;i<this.SVCList.length;i++)
            {
              this.SVCList[i].value=false;
            }
            for(var i=0;i<this.loadList.length;i++)
            {
              this.loadList[i].value=false;
            }
            for(var i=0;i<this.batteryList.length;i++)  
            { 
              this.batteryList[i].value=false;
            }
        break;
    }
  }



  getParams(): String 
  {

        var params = [];

        if (this.SVCList[0].value||this.SVCList[1].value||this.SVCList[2].value)
        {
          this.Change('SVC');
          params.push('capacitor_D');
          this.Typeflg=0;
        }
        if (this.lineList[0].value||this.lineList[1].value||this.lineList[2].value)
        {
          this.Change('line');
          params.push('line_D');
          this.Typeflg=1;
        }
        if (this.regulatorList[0].value)
        {
          this.Change('regulator');
          params.push('regulator_D');
          this.Typeflg=2;
        }
        if (this.loadList[0].value)
        {
          this.Change('load');
          params.push('loadDisplay_D');
          this.Typeflg=3;
        }

        if (this.batteryList[0].value||this.batteryList[1].value||this.batteryList[2].value||this.batteryList[3].value)
        {
          if(this.batteryList[0].value==true)
          {
            this.Change('res');
            this.Typeflg=4;
          }
          else if(this.batteryList[1].value==true)
          {
            this.Change('wind');
            this.Typeflg=4;
          }
          else if(this.batteryList[2].value==true)
          {
            this.Change('PV');
            this.Typeflg=4;
          }
          else if(this.batteryList[3].value==true)
          {
            this.Change('battery');
            this.Typeflg=4;
          }

          params.push('getres_D');
        }
        

        if (this.switchList[0].value)
        {
          this.Change('switch');
          params.push('');
          this.Typeflg=5;
        }



            //let gsqlPre = environment.gsqlPre;
            //var url = gsqlPre + "/query/product_graph/getInfoMod?" + params.join("&");
            //var url = "http://localhost:3000/query/product_graph/getInfoMod?" + params.join("&");
            //console.log(url);
            return params.join("&");
  }


  loadData(param?: any) 
  {
    //var url = this.getURL();
      //  var url = this.productsService.getURL(this.getParams()); 
      var url = this.productsService.getURL(this.getParams()); 
    this.productsService.loadData(url).subscribe(data => {
      var V=this.GetcheckValue();
      this.productsService.processData(data,V);
     // this.Highlight();
      this.productsService.getData().subscribe(data => {
        this.zone.run(() => {
          console.log(data)
          this.dataSource = new MatTableDataSource(data);

          //this.myMap.reload();
          this.reset();
        });
      });
    })
  


    console.log(this.dataSource);
  }

      Change(id:string)
      {
            if (id=='SVC')
            {
              this.columns = [{ label: '电容器名', value: 'SVCname' }, { label: '节点', value: 'SVCBus' }, { label: '容量(KVar)', value: 'SVCcapacity' },
              { label: '控制方式', value: 'SVCcontrol' },
              { label: '相位', value: 'SVCphase' }];
              this.displayedColumns = ['SVCname', 'SVCBus', 'SVCcapacity', 'SVCcontrol', 'SVCphase'];
            }
            if (id=='line')
            {
              this.columns = [{ label: '线路名', value: 'linename' }, { label: '始端节点', value: 'startBus' },
              { label: '末端节点', value: 'endBus' },
              { label: '长度(ft)', value: 'lineLength' },
              { label: 'A相电阻(欧姆)', value: 'phaseAresistance' },
              { label: 'A相电抗(欧姆)', value: 'phaseAreactance' },
              { label: 'B相电阻(欧姆)', value: 'phaseBresistance' },
              { label: 'B相电抗(欧姆)', value: 'phaseBreactance' },
              { label: 'C相电阻(欧姆)', value: 'phaseCresistance' },
              { label: 'C相电抗(欧姆)', value: 'phaseCreactance' },
              { label: 'AB互感(欧姆)', value: 'phaseAB' },
              { label: 'AC互感(欧姆)', value: 'phaseAC' },
              { label: 'BC互感(欧姆)', value: 'phaseBC' },
              { label: '类型', value: 'config' },
            ];

              this.displayedColumns = ['linename', 'startBus', 'endBus', 'lineLength', 'phaseAresistance',
              'phaseAreactance','phaseBresistance','phaseBreactance','phaseCresistance','phaseCreactance',
              'phaseAB','phaseAC','phaseBC','config'


            ];
            };
            if (id=='regulator')
            {
              this.columns = [{ label: '调压器名', value: 'rename' }, 
              {label:'节点',value:'rebus'},
              { label: '相位', value: 'phase' },
              { label: '控制电压(V)', value: 'reCvoltage' },
              { label: '电阻电压(V)', value: 'resistorV' },
              { label: '电抗电压(V)', value: 'reactanceV' },
              { label: '最大档位', value: 'maxShift' },
              { label: '最小档位', value: 'minShift' },
              { label: '步长(V)', value: 'stepsize' },
              { label: 'PT变比', value: 'PTratio' },
              { label: 'CT额定电流(A)', value: 'CTcurrent' },
              { label: '带宽(V)', value: 'bandwidth' },
              { label: '电压控制相位', value: 'phaseV' }],

              this.displayedColumns = ['rename', 'rebus', 'phase', 'reCvoltage', 'resistorV',
              'reactanceV','maxShift','minShift','stepsize','PTratio','CTcurrent','bandwidth','phaseV'
            ];
            }
            if (id=='load')
            {
              this.columns = [{ label: '编号', value: 'loadID' }, 
              {label:'节点',value:'lbus'},
              { label: 'A相有功(KW)', value: 'PA' },
              { label: 'A相无功(KVar)', value: 'QA' },
              { label: 'B相有功(KW)', value: 'PB' },
              { label: 'B相无功(KVar)', value: 'QB' },
              { label: 'C相有功(KW)', value: 'PC' },
              { label: 'C相无功(KVar)', value: 'QC' }]

              this.displayedColumns = ['loadID', 'lbus', 'PA', 'QA', 'PB',
              'PC','QC'];
            }
            if (id=='switch')
            {
              this.columns = [{ label: 'Swname', value: 'Swname' }, 
              {label:'startBus',value:'startBus'},
              { label: 'endBus', value: 'endBus' },
              { label: 'status', value: 'status' },
            ]

              this.displayedColumns = ['Swname', 'startBus', 'endBus', 'status'];
            }
            if (id=='res')
            {
              this.columns = [{ label: '风机数量', value: 'windn' }, 
              {label:'PV数量',value:'PVn'},
              { label: '储能数量', value: 'batteryn' }]

              this.displayedColumns = ['windn', 'PVn', 'batteryn'];
            }
            if (id=='wind')
            {
              this.columns = [{ label: '风机名', value: 'wind_name' }, 
              {label:'状态',value:'wind_Gen_status'},
              { label: '模式', value: 'wind_Gen_mode' },
              { label: '模型', value: 'wind_Turbine_Model' }]

              this.displayedColumns = ['wind_name', 'wind_Gen_status'
              ,'wind_Gen_mode','wind_Turbine_Model'];
            }
            if (id=='PV')
            {
              this.columns = [{ label: 'PV名', value: 'solar_name' }, 
              {label:'状态',value:'solar_gen_status'},
              { label: '安装类型', value: 'solar_install_type' },
              { label: '电池板类型', value: 'solar_panel_type' },
              { label: '模式', value: 'solar_gen_mode' },
              { label: '效率', value: 'solar_efficiency' },
              { label: '日照区域', value: 'solar_area' }]

              this.displayedColumns = ['solar_name', 'solar_gen_status', 'solar_install_type',
               'solar_panel_type', 'solar_gen_mode',
              'solar_efficiency','solar_area'];
            }
            if (id=='battery')
            {
              this.columns = [{ label: '电池名', value: 'battery_name' }, 
              {label:'模式',value:'battery_generator_mode'},
              { label: '最大电压', value: 'battery_V_Max' },
              { label: '最大电流', value: 'battery_I_Max' },
              { label: '最大功率', value: 'battery_P_Max' },
              { label: '最大能量', value: 'battery_E_Max' },
              { label: '效率', value: 'battery_base_efficiency' },
              {label:'状态',value:'battery_gen_status'},
              { label: '漏电功率', value: 'battery_parasitic_power_draw' }]

              this.displayedColumns = ['battery_name', 'battery_generator_mode',
               'battery_V_Max', 'battery_I_Max', 'battery_P_Max',
              'battery_E_Max','battery_base_efficiency','battery_gen_status','battery_parasitic_power_draw'];
            }


      }
  





      BusHighLightAll(ID:string[]): void {
        let selectedNode = [];
        // highlight all loads
        console.log(ID);
        this.chart.getNodes().forEach(n => {
            if (ID.includes(String(n.exID))) {
                this.chart.unLowlightNode(n.exType, n.exID);
                selectedNode.push(n);
            } else {
                this.chart.lowlightNode(n.exType, n.exID);
            }
        });
        // lowlight all edges
        this.chart.getLinks().forEach(l => {
            this.chart.lowlightLink(l.exType, l.source.exType, l.source.exID, l.target.exType, l.target.exID);
        });
      //  this.chart.scrollIntoView(selectedNode);
        this.chart.update();
        console.log('highlight all loads');
    } 

      LineHighLightAll(ID:string[]): void {
        // highlight all lines
        if (this.chart !== undefined) {
            console.log('hightlightall');
            let selectedNodes = [];
            this.chart.getLinks().forEach(l => {
                if (ID.includes(String(l.attrs.edge_name))) {
                    this.chart.unLowlightLink(l.exType, l.source.exType, l.source.exID, l.target.exType, l.target.exID);
                    selectedNodes.push(l.source);
                    selectedNodes.push(l.target);
                } else {
                    this.chart.lowlightLink(l.exType, l.source.exType, l.source.exID, l.target.exType, l.target.exID);
                }
            });
            // lowlight all vertices
            this.chart.getNodes().forEach(n => {
            this.chart.lowlightNode(n.exType, n.exID);
            });
            //    this.chart.scrollIntoView(selectedNodes);
            this.chart.update();
            console.log('highlight all lines');
        }

  }




    Getid(data:string){
      for(var i=0;i<data['value'].length;i++)
      {
        if (this.Typeflg==0)
        {
        this.ID.push(data['value'][i]["SVCBus"]);
        }
        if (this.Typeflg==1)
        {
          var temp:string=data['value'][i]['linename'];
          this.ID.push(temp)
        }
        if (this.Typeflg==2)
        {
          this.ID.push(data['value'][i]["rename"]);
        }
        if (this.Typeflg==3)
        {
          this.ID.push(data['value'][i]["lbus"]);
        }
        if (this.Typeflg==4)
        {
        }
        if (this.Typeflg==5)
        {
        }
      }
    }

    itemClicked2(item2){
      this.listtype=item2;
      var Cwidth1= this.elementView.nativeElement.offsetWidth;
      var Cheight1=this.elementView.nativeElement.offsetHeight;
      this.Cwidth=Cwidth1-this.View.nativeElement.offsetWidth-400;
      this.Cheight=this.View.nativeElement.offsetHeight/2;
      console.log(Cwidth1);
      console.log(Cheight1);


      this.InteractCWG(this.listtype);
    }
    InteractCWG(type:string){
      var dlinechart = environment.gsqlPre + '/query/line_D';  // URL to web api
      var dcapchart = environment.gsqlPre + '/query/capacitor_D';  // URL to web api
      var dreschart = environment.gsqlPre + '/query/getres_D';  // URL to web api
      console.log(type);
      switch(type)
      {
        
        case "overhead line":
        this.DfserService.getchart(dlinechart).then(data=>{
          this.lineXAdata=[];
          this.lineXBdata=[];
          this.lineXCdata=[];

            for (var i=0;i<data.length;i++)
            {
              if(data[i]["config"]!=12)
              {
                  this.lineXAdata.push(data[i]["XA"]);
                  this.lineXBdata.push(data[i]["XB"]);
                  this.lineXCdata.push(data[i]["XC"]);
              }
            }
            let ovLine = {
                  title: { text: '架空线电抗' },
                  chart: {
                      zoomType: 'x',
                      height:this.Cheight,
                      width: this.Cwidth

                  },
                  xAxis: {
                      categories: [],
                      title: {
                          text: "线路编号"
                      }
                  },
                  yAxis: {
                      title: {
                          text: "电抗"
                      }
                  },
                  series: [{
                      name: 'A相',
                      color: '#FF0000',
                      showInLegend: true,
                      type: 'line',
                      data: [],
                      allowPointSelect: true
                  }, {
                      name: 'B相',
                      color: '#0000FF',
                      showInLegend: true,
                      type: 'line',
                      data: [],
                      allowPointSelect: true
                  }, {
                      name: 'C相',
                      color: '#00FF00',
                      showInLegend: true,
                      type: 'line',
                      data: [],
                      allowPointSelect: true
                  }
                  ]
              };
              this.lineXAdata.forEach(k => {
                ovLine.series[0].data.push(k);
            });
            this.lineXBdata.forEach(k => {
              ovLine.series[1].data.push(k);
            });
            this.lineXCdata.forEach(k => {
              ovLine.series[2].data.push(k);
            })
            this.options1=ovLine;

        });
       
        console.log(this.options1)
        break;
        case "underground cable":
        
        this.DfserService.getchart(dlinechart).then(data=>{
          this.lineXAdata=[];
          this.lineXBdata=[];
          this.lineXCdata=[];

            for (var i=0;i<data.length;i++)
            {
              if(data[i]["config"]=="12")
              {
                  this.lineXAdata.push(data[i]["XA"]);
                  this.lineXBdata.push(data[i]["XB"]);
                  this.lineXCdata.push(data[i]["XC"]);
              }
            }
            let ugLine = {
                  title: { text: '地下电缆电抗' },
                  chart: {
                      zoomType: 'x',
                      height: this.Cheight,
                      width: this.Cwidth

                  },
                  xAxis: {
                      categories: [],
                      title: {
                          text: "线路编号"
                      }
                  },
                  yAxis: {
                      title: {
                          text: "电抗"
                      }
                  },
                  series: [{
                      name: 'A相',
                      color: '#FF0000',
                      showInLegend: true,
                      type: 'line',
                      data: [],
                      allowPointSelect: true
                  }, {
                      name: 'B相',
                      color: '#0000FF',
                      showInLegend: true,
                      type: 'line',
                      data: [],
                      allowPointSelect: true
                  }, {
                      name: 'C相',
                      color: '#00FF00',
                      showInLegend: true,
                      type: 'line',
                      data: [],
                      allowPointSelect: true
                  }
                  ]
              };
            this.lineXAdata.forEach(k => {
              ugLine.series[0].data.push(k);
            });
            this.lineXBdata.forEach(k => {
              ugLine.series[1].data.push(k);
            });
            this.lineXCdata.forEach(k => {
              ugLine.series[2].data.push(k);
            })
            this.options1=ugLine;

        });

        break;
        case "voltage":

        this.DfserService.getchart(dcapchart).then(data=>{
          this.capC=[];
            for (var i=0;i<data.length;i++)
            {

              this.capC.push(data[i]["capacity"]);
              
            }
            let cap = {
                  title: { text: '电容器容量' },
                  chart: {
                      zoomType: 'x',
                      height:this.Cheight,
                      width: this.Cwidth

                  },
                  xAxis: {
                      categories: [],
                      title: {
                          text: "电容器编号"
                      }
                  },
                  yAxis: {
                      title: {
                          text: "容量"
                      }
                  },
                  series: [{
                      name: '电容器容量',
                      color: '#FF0000',
                      showInLegend: true,
                      type: 'line',
                      data: [],
                      allowPointSelect: true
                  }, 
                  ]
              };
              this.capC.forEach(k => {
                cap.series[0].data.push(k);
              });
            this.options1=cap;

        });


        break;
        case "current":

        this.DfserService.getchart(dcapchart).then(data=>{
          this.capC=[];
            for (var i=0;i<data.length;i++)
            {

              this.capC.push(data[i]["capacity"]);
              
            }
            let cap = {
                  title: { text: '电容器容量' },
                  chart: {
                      zoomType: 'x',
                      height: this.Cheight,
                      width: this.Cwidth

                  },
                  xAxis: {
                      categories: [],
                      title: {
                          text: "电容器编号"
                      }
                  },
                  yAxis: {
                      title: {
                          text: "容量"
                      }
                  },
                  series: [{
                      name: '电容器容量',
                      color: '#FF0000',
                      showInLegend: true,
                      type: 'line',
                      data: [],
                      allowPointSelect: true
                  }, 
                  ]
              };
              this.capC.forEach(k => {
                cap.series[0].data.push(k);
              });
            this.options1=cap;

        });



        break;
        case "loadV":
        break;

        case "wind":
        this.DfserService.getchart(dreschart).then(datak=>{
        var windm:string[]=[];
        var windn:string[]=[];
        var winds:string[]=[];
        var kk1=0;
        var kk2=0;
        var kk3=0;
            for (var i=0;i<datak.length;i++)
            {
              windm.push(datak[i]["wind_model"]);
              windn.push(datak[i]['from'].slice(5,datak[i]['from'].end));
              winds.push(datak[i]['windf']);
            }
            for(var i=0;i<winds.length;i++)
            {
              if(winds[i]=="ONLINE")
              {
                if(windm[i]=='GENERIC_SYNCH_SMALL')
                {
                  kk1+=1;
                 }
                 if(windm[i]=='GE_25MW')
                 {
                   kk2+=1;
                  }
                  if(windm[i]=='VESTAS_V82')
                  {
                    kk3+=1;
                  }
              }
            }


          let res=  {            
          title: { text: '风机型号' },
          chart: {
              zoomType: 'x',
              height: this.Cheight,
              width: this.Cwidth

          },
          xAxis: {
              categories: [],
              title: {
                  text: "型号"
              }
          },
          yAxis: {
              title: {
                  text: "个数"
              }
          },
          series: [{
              name: 'GE_25MW',
              // showInLegend: true,
              type: 'column',
              data:[kk2]
              // allowPointSelect: true
          }, 
          {
            name: 'VESTAS_V82',
            // showInLegend: true,
            type: 'column',
            data: [kk3]
            // allowPointSelect: true
        },
        {
          name: 'GENERIC_SYNCH_SMALL',
          // showInLegend: true,
          type: 'column',
          data: [kk1]
          // allowPointSelect: true
      }
          ]
        }

            this.options1=res;

        });

        break;
        case "PV":
        this.DfserService.getchart(dreschart).then(data=>{
          var PV_P=[];
            for (var i=0;i<data.length;i++)
            {
              PV_P.push(data[i]["inv_P"]);
            }
            let PV = {
                  title: { text: 'PV逆变器额定容量' },
                  chart: {
                      zoomType: 'x',
                      height: this.Cheight,
                      width: this.Cwidth

                  },
                  xAxis: {
                      categories: [],
                      title: {
                          text: "PV编号"
                      }
                  },
                  yAxis: {
                      title: {
                          text: "容量"
                      }
                  },
                  series: [{
                      name: '逆变器容量',
                      showInLegend: true,
                      type: 'column',
                      data: [],
                      allowPointSelect: true
                  }, 
                  ]
              };
              PV_P.forEach(k => {
                PV.series[0].data.push(k);
              });
            this.options1=PV;

        });




        break;
        case "battery":


        this.DfserService.getchart(dreschart).then(data=>{
          var battery_P=[];
            for (var i=0;i<data.length;i++)
            {
              battery_P.push(data[i]["battery_parasitic_power"]);
            }
            let battery = {
                  title: { text: '储能漏电功率' },
                  chart: {
                      zoomType: 'x',
                      height: this.Cheight,
                      width: this.Cwidth

                  },
                  xAxis: {
                      categories: [],
                      title: {
                          text: "储能编号"
                      }
                  },
                  yAxis: {
                      title: {
                          text: "漏电功率"
                      }
                  },
                  series: [{
                      name: '电池',
                      color: '#FF0000',
                      showInLegend: true,
                      type: 'column',
                      data: [],
                      allowPointSelect: true
                  }, 
                  ]
              };
              battery_P.forEach(k => {
                battery.series[0].data.push(k);
              });
            this.options1=battery;

          });
        break;


        case "res":
        console.log(this.NoPV)
            let res = {
                  title: { text: '新能源概况' },
                  chart: {
                      zoomType: 'x',
                      height: this.Cheight,
                      width: this.Cwidth

                  },
                  xAxis: {
                      categories: [],
                      title: {
                          text: "新能源"
                      }
                  },
                  yAxis: {
                      title: {
                          text: "数量"
                      }
                  },
                  series: [{
                      name: 'PV',
                      showInLegend: true,
                      type: 'column',
                      data: [this.NoPV],
                      allowPointSelect: true
                  }, 
                  {
                    name: 'battery',
                    showInLegend: true,
                    type: 'column',
                    data: [this.Nobattery],
                    allowPointSelect: true
                }, 
                {
                  name: 'wind',
                  showInLegend: true,
                  type: 'column',
                  data: [this.Nowind],
                  allowPointSelect: true
              },
                  ]
              };

            this.options1=res;

        




        break;
      }
      
    }

    itemClicked(item){
      this.ifshowS=false;
      switch(item)
      {
        case "line":
          if(this.listtype=="all") 
          {
/*             this.reset();
            this.lineList[0].value=true; */
            if( this.lineList[0].value==true)
            {
              this.PartAllTrue("line")
            }
            else
            {
              this.reset();
            }
          }
          else
          {
            this.Partreset("line");
          }
          break;
        case "regulator":
/*           this.reset();
          this.regulatorList[0].value=true; */
          this.PartAllTrue("regulator")
          break;
        case "SVC":
          if(this.listtype=="all") 
          {
/*             this.reset();
            this.SVCList[0].value=true; */
            if( this.SVCList[0].value==true)
            {
              this.PartAllTrue("SVC")
            }
            else
            {
              this.reset();
            }
          }
          else
          {
            this.Partreset("SVC");
          }
          break;
        case "load":
            if(this.listtype=="all") 
            {
/*               this.reset();
              this.busList[0].value=true; */
              if( this.loadList[0].value==true)
              {
                this.PartAllTrue("load");
              }
              else
              {
                this.reset();
              }

            }
            else
            {
              this.Partreset("load");
            }
              break;
        case "battery":
        if(this.listtype=="res") 
        {
/*             this.reset();
          this.SVCList[0].value=true; */
          if( this.batteryList[0].value==true)
          {
            this.PartAllTrue("battery")
          }
          else
          {
            this.reset();
          }
        }
        else
        {
          if(this.listtype=="wind")
          { 
            this.reset();
            this.batteryList[1].value=true;
          }
          if(this.listtype=="PV")
          { 
            this.reset();
            this.batteryList[2].value=true;
          }
          if(this.listtype=="battery")
          { 
            this.reset();
            this.batteryList[3].value=true;
          }
        }
          break;
        case "switch":
/*           this.reset();
          this.switchList[0].value=true; */
          this.PartAllTrue("switch")
          break;
      }
    }
  
  

    Highlight()
    {
      this.Newdata=this.productsService.getData();
      this.Getid(this.Newdata);
      this.BusHighLightAll(this.ID);
      if(this.Typeflg==1)
      {
        this.LineHighLightAll(this.ID);
      }
      this.ID=[];
    }
    
    GetcheckValue()
    {
      let Vchain:boolean[]=[];
      for(var i=0;i<this.lineList.length;i++)
      {
        Vchain.push(this.lineList[i].value);
      }

      for(var i=0;i<this.SVCList.length;i++)
      {
        Vchain.push(this.SVCList[i].value);
      }
      for(var i=0;i<this.batteryList.length;i++)
      {
        Vchain.push(this.batteryList[i].value);
      }


      return Vchain;
    }



    regulatorClicked(){
      this.showRegulator=!this.showRegulator;
    }
    svcClicked(){
      this.showSVC=!this.showSVC;
    }
    lineClicked(){
      this.showLine=!this.showLine;
    }
    loadClicked(){
      this.showLoad=!this.showLoad;
    }
    batteryClicked(){
      this.showBattery=!this.showBattery;
    }
    switchClicked(){
      this.showSwitch=!this.showSwitch;
    }

/*     reallClicked(value)
    {
      switch(value)
      {
        case "false":
        {
          this.PartAllTrue("regulator");
          this.checkboxValue=true;
          break;
        }
        case "true":
        {
          this.reset();
          break;
        }

      }
    } 
      lineallClicked(value)
      {
        switch(value)
        {
          case "false":
          {
            this.PartAllTrue("line");
            this.checkboxValue=true;
            break;
          }
          case "true":
          {
            this.reset();
            break;
          }
  
        }
      }
        svcallClicked(value)
        {
          switch(value)
          {
            case "false":
            {
              this.PartAllTrue("SVC");
              this.checkboxValue=true;
              break;
              
            }
            case "true":
            {
              this.reset();
              break;
            }
    
          }
        }
          loadallClicked(value)
          {
            switch(value)
            {
              case "false":
              {
                this.PartAllTrue("load");
                this.checkboxValue=true;
                break;
              }
              case "true":
              {
                this.reset();
                break;
              }
      
            }
          }
            batteryallClicked(value)
            {
              switch(value)
              {
                case "false":
                {
                  this.PartAllTrue("battery");
                  this.checkboxValue=true;
                  break;
                }
                case "true":
                {
                  this.reset();
                  break;
                }
        
              }
            }
              switchallClicked(value)
              {
                switch(value)
                {
                  case "false":
                  {
                    this.PartAllTrue("switch");
                    this.checkboxValue=true;
                    break;
                  }
                  case "true":
                  {
                    this.reset();
                    break;
                  }
          
                }

      
    } */
}



