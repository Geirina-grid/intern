import { Component, OnInit, ViewChild, ElementRef, ChangeDetectorRef, NgZone, AfterViewInit } from '@angular/core';

//import { datapfModule } from './data-pf.module'

import { MatTableDataSource, MatSort,MatAutocomplete } from '@angular/material';

import { PfResultService } from '../pf-result/pf-result.service';
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

@Component({
  selector: 'app-pf-result',
  templateUrl: './pf-result.component.html',
  styleUrls: ['./pf-result.component.css']
})
export class PfResultComponent implements OnInit, AfterViewInit {
  
@ViewChild('chart') idc: any;
@ViewChild(DataTableComponent) myTable;
@ViewChild(DrawLineComponent) dlMap;
@ViewChild('mainScreen') elementView: ElementRef;
@ViewChild('rightScreen') rightView: ElementRef;
@ViewChild('chartView') chartView: ElementRef;
@ViewChild('mapre') View: ElementRef;
@ViewChild('contentScreen') contentView: ElementRef;
//chart
showingTable = true;
showingChart = true;
showmap=true;
showTchart=true;
options1:any;
options2:any;
private busPAdata:any;
private busPBdata:any;
private busPCdata:any;
private busQAdata:any;
private busQBdata:any;
private busQCdata:any;
sdate: Date;
edate: Date;
defaultDates = new Date('01/01/2018');
defaultDatee = new Date('12/31/2018');
nodeid: string;
phase: string;
phaseNO: number[];
name: any;

private Cheight=450;
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
  style_topleft: object = {};
  style_topright: object = {};
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
  columns: any[] = [{ label: '请点击过滤器选择数据列表', value: '请点击过滤器选择数据列表' }];
  displayedColumns = ['请点击过滤器选择数据列表'];

  lineList=[{label:'全选',value:false, group:'line',type:'all'},
  {label:"越限",value:false,group:'line', type:'overhead line'},
  {label:"未越限",value:false,group:'line', type:'underground cable'}
];
  regulatorList=[{label:'全选',value:false,group:'regulator'}];

  SVCList=[{label:'全选',value:false,group:'SVC',type:'all'},
  // {label:'电压控制',value:false,group:'SVC',type:'voltage'},
  // {label:'电流控制',value:false,group:'SVC',type:'current'},
  //{label:'迟滞控制',value:false,group:'SVC',type:'droop'},
];

  switchList=[{label:'全选',value:false,group:'switch'}];

  busList=[{label:'全选',value:false,group:'bus',type:'all'},
  {label:"A相过电压",value:false,group:'bus', type:'phase A high'},
  {label:"A相低电压",value:false,group:'bus', type:'phase A low'},
  {label:"B相过电压",value:false,group:'bus', type:'phase B high'},
  {label:"B相低电压",value:false,group:'bus', type:'phase B low'},
  {label:"C相过电压",value:false,group:'bus', type:'phase C high'},
  {label:"C相低电压",value:false,group:'bus', type:'phase C low'},
  {label:"安全节点",value:false,group:'bus', type:'normal'}
];
batteryList=[{label:'全部',value:false,group:'battery',type:'all'},
{label:'风电',value:false,group:'battery',type:'wind'},
{label:'太阳能',value:false,group:'battery',type:'PV'},
{label:'储能',value:false,group:'battery',type:'battery'}];
  Newdata:any;
  Typeflg:number;//determine which box is selected;
  showRegulator=false;
  showSVC=false;
  showLine=false;
  showBus=false;
  showBattery=false;
  showSwitch=false;

  style_main = {};

  constructor(private pfreService:PfResultService, 
    private ref: ChangeDetectorRef,
    private zone: NgZone,
    private graphChartService: GraphChartService,
    ) {
      this.sdate = this.defaultDates;
      this.edate = this.defaultDatee;



  }






  dataSource: any;
  // dataSource = new MatTableDataSource(this.data);
  ngOnInit() 
  {
    

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
    var width = (this.viewWidth - event.rectangle.width);
    this.Cwidth= width-520;
    console.log(this.Cwidth)
    this.ppclick();
    console.log(width);

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
    this.Cheight= event.rectangle.height-130;



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
    this.Cheight= event.rectangle.height-130;

    this.ppclick();

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


    var height = (this.viewHeight - event.rectangle.height);
    this.Cheight= event.rectangle.height-130;
    this.ppclick();
    this.resizeMap();
  }

  ngAfterViewInit(): void {

    // this.chart=this.idc.chart;
    // throw new Error("Method not implemented.");
  }
  ppclickR(){
    
    var Cwidth1= this.elementView.nativeElement.offsetWidth;
    var Cheight1=this.elementView.nativeElement.offsetHeight;
    this.Cwidth=Cwidth1-this.View.nativeElement.offsetWidth-400;
    this.Cheight=this.View.nativeElement.offsetHeight-80;
    this.ppclick();
  }
  ppclick(){
    console.log(this.dlMap)
      console.log(this.dlMap.featureO[0]["properties"].ID);
      console.log(this.dlMap.featureO[0]["properties"].FROM_ID + '_' + this.dlMap.featureO[0]["properties"].TO_ID );
      this.nodeid=this.dlMap.featureO[0]["properties"].ID;
      this.name=Object.keys(this.dlMap.featureO[0]["properties"]);
      if (this.name[0]==="ID"){
          this.phase=this.dlMap.featureO[0]["properties"].PHASE;
          if (this.phase === 'ABCN' || this.phase === 'ABC'){
              this.phaseNO=[];
              this.phaseNO.push(1);
              this.phaseNO.push(1);
              this.phaseNO.push(1);
          }else if (this.phase === 'ABN'){
              this.phaseNO=[];
              this.phaseNO.push(1);
              this.phaseNO.push(1);
              this.phaseNO.push(0);
          }else if (this.phase === 'ACN'){
            this.phaseNO=[];
            this.phaseNO.push(1);
            this.phaseNO.push(0);
            this.phaseNO.push(1);
          }else if (this.phase === 'BCN'){
              this.phaseNO=[];
              this.phaseNO.push(0);
              this.phaseNO.push(1);
              this.phaseNO.push(1);
          }else if (this.phase === 'AN'){
            this.phaseNO=[];
            this.phaseNO.push(1);
            this.phaseNO.push(0);
            this.phaseNO.push(0);
          }else if (this.phase === 'BN'){
            this.phaseNO=[];
            this.phaseNO.push(0);
            this.phaseNO.push(1);
            this.phaseNO.push(0);
          }else if (this.phase === 'CN'){
            this.phaseNO=[];
            this.phaseNO.push(0);
            this.phaseNO.push(0);
            this.phaseNO.push(1);
          }
          console.log(this.phaseNO);

          this.pfreService.PlotTSD(this.nodeid, this.sdate.getTime(), this.edate.getTime(), this.phaseNO)
              .then(line => {
                // console.log(line[0]);
              // this.calculating = false;



                line[0].chart.height = this.Cheight;
                line[0].chart.width = this.Cwidth;
                // console.log(line[0]);
                // console.log(line[0].series[0].data[0][1]);
                
                this.options1 = line[0];
                // this.loadrank = line[1];
                // this.basicstats = line[3];
                // this.loadlowrank = line[4];
                (<any>window).opt = this.options1;
              })
              .catch(this.handleError);
        }
    } 
  private handleError(error: any) {
    console.error('An error occurred', error); // for demo purposes only
    alert('please select a node');
  }

  reset() {
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
    for(var i=0;i<this.busList.length;i++)
    {
      this.busList[i].value=false;
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
            for(var i=0;i<this.busList.length;i++)
            {
              this.busList[i].value=false;
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
            for(var i=0;i<this.busList.length;i++)
            {
              this.busList[i].value=false;
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
            for(var i=0;i<this.busList.length;i++)
            {
              this.busList[i].value=false;
            }
            for(var i=0;i<this.batteryList.length;i++)  
            { 
              this.batteryList[i].value=false;
            }
        break;
      case "bus":
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
            for(var i=0;i<this.busList.length;i++)
            {
              this.busList[i].value=false;
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
            for(var i=0;i<this.busList.length;i++)
            {
              this.busList[i].value=false;
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
            for(var i=0;i<this.busList.length;i++)
            {
              this.busList[i].value=false;
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
            for(var i=0;i<this.busList.length;i++)
            {
              this.busList[i].value=false;
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
            for(var i=0;i<this.busList.length;i++)
            {
              this.busList[i].value=false;
            }
            for(var i=0;i<this.batteryList.length;i++)  
            { 
              this.batteryList[i].value=false;
            }
        break;
      case "bus":
            for(var i=0;i<this.busList.length;i++)
            {
              this.busList[i].value=true;
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
            for(var i=0;i<this.busList.length;i++)
            {
              this.busList[i].value=false;
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
            for(var i=0;i<this.busList.length;i++)
            {
              this.busList[i].value=false;
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

        if (this.SVCList[0].value)
        {
          this.Change('SVC');
          params.push('capResult_D');
          this.Typeflg=0;
        }
        if (this.lineList[0].value||this.lineList[1].value||this.lineList[2].value)
        {
          this.Change('line');
          params.push('lineResult_D');
          this.Typeflg=1;
        }
        if (this.regulatorList[0].value)
        {
          this.Change('regulator');
          params.push('regulatorRes_D');
          this.Typeflg=2;
        }
        if (this.busList[0].value||this.busList[1].value||this.busList[2].value||this.busList[3].value||this.busList[4].value||this.busList[5].value||this.busList[6].value||this.busList[7].value)
        {
          this.Change('bus');
          params.push('busvoltage_D');
          this.Typeflg=3;
        }
        if (this.batteryList[0].value)
        {
          this.Change('battery');
          params.push('');
          this.Typeflg=4;
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
      var url = this.pfreService.getURL(this.getParams()); 
    this.pfreService.loadData(url).subscribe(data => {
      var V=this.GetcheckValue();
      this.pfreService.processData(data,V);
 //     this.Highlight();
      this.pfreService.getData().subscribe(data => {
        this.zone.run(() => {
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
              this.columns = [{ label: '电容器名', value: 'SVCname' }, { label: '节点', value: 'SVCBus' }, { label: '无功输出(KVar)', value: 'SVCoutput' },
              { label: '相位', value: 'SVCphase' }];
              this.displayedColumns = ['SVCname', 'SVCBus', 'SVCoutput', 'SVCphase'];
            }
            if (id=='line')
            {
              this.columns = [{ label: '线路名', value: 'linename' }, { label: '始端节点', value: 'startBus' }, 
              { label: '末端节点', value: 'endBus' },
              { label: 'A相有功(KW)', value: 'phaseAP' },
              { label: 'B相有功(KW)', value: 'phaseBP' },
              { label: 'C相有功(KW)', value: 'phaseCP' },
              { label: 'A相无功(KVar)', value: 'phaseAQ' },
              { label: 'B相无功(KVar)', value: 'phaseBQ' },
              { label: 'C相无功(KVar)', value: 'phaseCQ' },
              { label: 'A相电流(A)', value: 'phaseACu' },
              { label: 'B相电流(A)', value: 'phaseBCu' },
              { label: 'C相电流(A)', value: 'phaseCCu' },
              { label: 'A相电流相角(度)', value: 'phaseACan' },
              { label: 'B相电流相角(度)', value: 'phaseBCan' },
              { label: 'C相电流相角(度)', value: 'phaseCCan' },
              { label: 'A相线损(KW)', value: 'phaseAloss' },
              { label: 'B相线损(KW)', value: 'phaseBloss' },
              { label: 'C相线损(KW)', value: 'phaseCloss' },

            ];

              this.displayedColumns = ['linename', 'startBus', 'endBus', 'phaseAP', 'phaseBP',
              'phaseCP','phaseAQ','phaseBQ','phaseCQ','phaseACu',
              'phaseBCu','phaseCCu','phaseACan','phaseBCan','phaseCCan','phaseAloss','phaseBloss',
              'phaseCloss'


            ];
            };
            if (id=='regulator')
            {
              this.columns = [{ label: '调压器名', value: 'rename' }, 
              {label:'节点',value:'rebus'},
              { label: '相位', value: 'phase' },

              { label: '电阻电压(V)', value: 'resistorV' },
              { label: '电抗电压(V)', value: 'reactanceV' },

              { label: 'PT变比', value: 'PTratio' },
              { label: 'CT额定电流(A)', value: 'CTcurrent' },
              { label: '当前档位', value: 'CurrentS' },],

              this.displayedColumns = ['rename', 'rebus', 'phase','resistorV',
              'reactanceV','PTratio','CTcurrent','CurrentS'
            ];
            }

            if (id=='bus')
            {
              this.columns = [{ label: '节点ID', value: 'loadID' }, 
              {label:'节点',value:'lbus'},
              { label: 'A相电压(KV/p.u)', value: 'phaseAV' },
              { label: 'B相电压(KV/p.u)', value: 'phaseBV' },
              { label: 'C相电压(KV/p.u)', value: 'phaseCV' },
              { label: 'A相相角(度)', value: 'phaseAang' },
              { label: 'B相相角(度)', value: 'phaseBang' },
              { label: 'C相相角(度)', value: 'phaseCang' },
              { label: '不平衡度', value: 'unbalan' },
            ]

              this.displayedColumns = ['loadID', 'lbus', 'phaseAV', 'phaseBV', 'phaseCV',
              'phaseAang','phaseBang','phaseCang','unbalan'];
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
            if (id=='battery')
            {
              this.columns = [{ label: 'baname', value: 'baname' }, 
              {label:'batype',value:'batype'},
              { label: 'P', value: 'P' },
              { label: 'Q', value: 'Q' },
              { label: 'Qmax', value: 'Qmax' },
              { label: 'Qmin', value: 'Qmin' },
              { label: 'controlM', value: 'controlM' },
              { label: 'phase', value: 'phase' }]

              this.displayedColumns = ['baname', 'batype', 'P', 'Q', 'Qmax',
              'Qmin','controlM','phase'];
            }



      }
  





      BusHighLightAll(ID:string[]): void {
        let selectedNode = [];
        // highlight all loads

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


    showTable() {
      this.showingTable = true;
      console.log('show table')
      this.showmap=!this.showmap;
    }
  
    hideTable() {
      this.showingTable = false;
      console.log('hide table')
      this.showmap=!this.showmap;
    }
  
    showChart() {
      this.showingChart = true;
      console.log('show chart')
      this.showTchart=!this.showTchart;
    }
  
    hideChart() {
      this.showingChart = false;
      console.log('hide chart')
      this.showTchart=!this.showTchart;
    }



    itemClicked2(item2){
      this.listtype=item2;
    }
    itemClicked(item){
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
            this.PartAllTrue("SVC")
          }
          else
          {
            this.Partreset("SVC");
          }
          break;
        case "bus":
            if(this.listtype=="all") 
            {
/*               this.reset();
              this.busList[0].value=true; */
              if( this.busList[0].value==true)
              {
                this.PartAllTrue("bus");
              }
              else
              {
                this.reset();
              }

            }
            else
            {
              this.Partreset("bus");
            }
              break;
        case "battery":
        if(this.listtype=="all") 
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
          this.Partreset("battery");
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
      this.Newdata=this.pfreService.getData();
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

      for(var i=0;i<this.busList.length;i++)
      {
        Vchain.push(this.busList[i].value);
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
      this.showBus=!this.showBus;
    }
    batteryClicked(){
      this.showBattery=!this.showBattery;
    }
    switchClicked(){
      this.showSwitch=!this.showSwitch;
    }





}



