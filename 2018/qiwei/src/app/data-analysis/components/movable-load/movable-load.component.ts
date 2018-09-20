import { Component, OnInit, ViewChild, AfterViewInit, OnChanges, SimpleChanges, ViewEncapsulation, OnDestroy } from '@angular/core';
import { Router } from '@angular/router';
import { WholeGraphService } from '../../../app-shared/services/whole-graph.service';
// tslint:disable-next-line:max-line-length
import { ConfirmDialogModule, ConfirmationService, DropdownModule, SliderModule, TabViewModule, RadioButtonModule, ProgressSpinnerModule, InputSwitchModule} from 'primeng/primeng';
import { GraphManageService } from '../../../app-shared/services/graph-manage.service';
import { MovableLoadService } from '../../services/movable-load.service'
import { GraphChartService } from '../../../app-shared/services/graph-chart.service';
import { DataQualityService } from '../../services/data-quality.service';
import { Subscription } from 'rxjs/Subscription';
import { Message } from 'primeng/components/common/api';
import { GlobalIdService } from '../../../app-shared/services/global-id.service';
import * as L from 'leaflet';
import * as gvis from 'gvis';
const GraphChart = (<any>gvis).GraphChart;

@Component({
  selector: 'app-movable-load',
  templateUrl: './movable-load.component.html',
  styleUrls: ['./movable-load.component.css'],
  providers: [MovableLoadService, ConfirmationService, DataQualityService],
  encapsulation: ViewEncapsulation.None
})
export class MovableLoadComponent implements OnInit, AfterViewInit, OnDestroy {
  @ViewChild('topocontainer') container1: any;
  @ViewChild('t') t: any;
  iH = window.innerHeight - 120;
  iW = Math.min(window.innerWidth, 2000);
  meters: any;
  scroll_sh: string;
  slider_sh: string;
  top_position: number;
  left_position: number;
  public chart: any;
  private graph;
  checked: boolean = true;
  display1: boolean = false;
  display2: boolean = false;
  user: number;
  options1: Object;
  options2: Object;
  options5: Object;
  subscription: Subscription;
  idSubscription: Subscription;
  msgs: Message[] = [];
  types: any[];
  selectedType: number;
  selectedAlgo: number;
  selectedRadio: number;
  sdate: Date;
  edate: Date;
  defaultDates = new Date('01/01/2013');
  defaultDatee = new Date('12/31/2013');
  selectedVal0: string = '0';
  selectedVal1: string = '2';
  selectedVal2: string = '0.3';
  loadrank: any[];
  cols: any[];
  chosenprocess: any;
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
  //Ended by Bowen

  constructor(
    private router: Router,
    private wholeGraphService: WholeGraphService,
    private graphManageService: GraphManageService,
    private confirmationService: ConfirmationService,
    private movableloadService: MovableLoadService,
    private DataQualityService: DataQualityService,
    private graphChartService: GraphChartService,
    private idService: GlobalIdService
  ) {

    this.slider_sh = String(Math.round(this.iW * 3 / 4 - 30)) + "px";
    this.sdate = this.defaultDates;
    this.edate = this.defaultDatee;
    this.scroll_sh = "420px";
    this.top_position = 180;
    this.left_position = this.iW / 2;
    this.chosenprocess = 1;
    // this.user = 1;
    this.selectedType = 1;
    this.selectedAlgo = 0;
    this.selectedRadio = 0;
    this.meters = [];
    this.types = [
      { label: 'diff值', value: 1 },
      { label: '可移动负荷比例', value: 3 }
    ];
    this.cols = [
      { field: 'diff', header: '日差值' },
      { field: 'date', header: '日期' },
    ]
    this.subscription = this.graphChartService.graphChart$.subscribe(
      chart => {
        this.chart = chart;
        // console.log(this.chart);
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
            this.map.eachLayer((layer) => {
              if (layer['options']['color']) {
                if (this.changedLat == layer['_latlng']['lat'] && this.changedLng == layer['_latlng']['lng']) {
                  this.idService.announceId(layer['options']['name']);
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
        if (this.selectedAlgo == 1) {
          this.selectedRadio = Number(this.selectedVal1);
        }
        else {
          this.selectedRadio = Number(this.selectedVal2);
        }
        // tslint:disable-next-line:max-line-length
        this.show();
      }
    )

  }
  ngOnInit() {
  }
  ngOnDestroy() {
    this.map.off("click");
    this.chart.off('onClick');
    this.subscription.unsubscribe();
    this.subscriptionmap.unsubscribe();
    this.idSubscription.unsubscribe();
  }
  ngAfterViewInit(): void {
    if (this.selectedAlgo == 1) {
      this.selectedRadio = Number(this.selectedVal1);
    }
    else {
      this.selectedRadio = Number(this.selectedVal2);
    }
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

  run(): void {
    this.confirmationService.confirm({
      message: '重新运行算法需要三分钟，您要现在运行吗?',
      header: 'Confirmation',
      icon: 'fa fa-question-circle',
      accept: () => {
        this.display1 = true;
        this.movableloadService.RunAlgorithm(this.chosenprocess)
          .then(line => {
            this.display1 = false;
            if (this.selectedAlgo == 1) {
              this.selectedRadio = Number(this.selectedVal1);
            }
            else {
              this.selectedRadio = Number(this.selectedVal2);
            }
            this.show();
          })
      },
      reject: () => {
        this.msgs = [{ severity: 'info', summary: 'Rejected', detail: '您拒绝了运行算法' }];
      }
    });

  }
  onPointSelect(e) {
    this.display2 = false;
    this.idService.announceId(e.context.category)
  }
  onChange(event) {
    if (this.selectedAlgo == 1) {
      this.selectedRadio = Number(this.selectedVal1);
    }
    else {
      this.selectedRadio = Number(this.selectedVal2);
    }
    this.movableloadService.PlotUserRanking(this.selectedType, this.selectedAlgo, this.selectedRadio).then(
      line => {
        line.chart.width = 520;
        line.chart.height = 1500;
        this.options5 = line;
      }
    )
  }
  onFocus(event) {
    if (this.selectedAlgo == 1) {
      this.selectedRadio = Number(this.selectedVal1);
    }
    else {
      this.selectedRadio = Number(this.selectedVal2);
    }
    this.movableloadService.PlotUserRanking(this.selectedType, this.selectedAlgo, this.selectedRadio).then(
      line => {
        line.chart.width = 520;
        line.chart.height = 1500;
        this.options5 = line;
        this.display2 = true;
      }
    )
  }
  onBlur(event) {
    this.display2 = false;
  }

  // onChangeSlide(event) {
  //   this.rangeValues = event.values;
  //   this.sdate = new Date(this.defaultStartStr);
  //   this.sdate.setHours(this.rangeValues[0]);
  //   this.sdateStr = this.sdate.getFullYear() + '-' + (this.sdate.getMonth() + 1) + '-' + (this.sdate.getDate() + ' ' + this.sdate.getHours() + ':00:00');
  //   this.edate = new Date(this.defaultStartStr);
  //   this.edate.setHours(this.rangeValues[1]);
  //   this.edateStr = this.edate.getFullYear() + '-' + (this.edate.getMonth() + 1) + '-' + (this.edate.getDate() + ' ' + this.edate.getHours() + ':00:00');
  // }

  // onSlideEnd(event) {
  //   if (this.selectedAlgo == 1) {
  //     this.selectedRadio = Number(this.selectedVal1);
  //   }
  //   else {
  //     this.selectedRadio = Number(this.selectedVal2);
  //   }
  //   this.show();
  // }

  onTabChange(event) {
    this.selectedAlgo = event.index
    if (this.selectedAlgo == 1) {
      this.selectedRadio = Number(this.selectedVal1);
    }
    else {
      this.selectedRadio = Number(this.selectedVal2);
    }
    this.show();
  }

  onClickRadio(event) {
    if (this.selectedAlgo == 1) {
      this.selectedRadio = Number(this.selectedVal1);
    }
    else {
      this.selectedRadio = Number(this.selectedVal2);
    }
    this.show();
  }

  show():void{
    if (this.checked){
      this.movableloadService.PlotUserMovableLoad(this.user, this.sdate.getTime(), this.edate.getTime(), this.selectedAlgo, this.selectedRadio,0).then(
        line => {
          line[0].chart.width = this.iW * 9 / 16-15;
          line[0].chart.height = (this.iH - 70) / 2;
          this.options1 = line[0];
          line[1].chart.width = this.iW * 9 / 16-15;
          line[1].chart.height = (this.iH - 70) / 2;
          this.options2 = line[1];
        }
      )
      this.movableloadService.PlotStatistics(this.sdate.getTime(), this.edate.getTime(),this.user, this.selectedAlgo, this.selectedRadio).then(
        line => {
          this.t.reset();
          this.loadrank = line[0];
        }
      )
    }
    else{
      this.movableloadService.PlotSystemMovableLoad(this.sdate.getTime(), this.edate.getTime(), this.selectedAlgo, this.selectedRadio,0).then(
        line => {
          line[0].chart.width = this.iW * 9/16-15;
          line[0].chart.height = (this.iH - 70) / 2;
          this.options1 = line[0];
          line[1].chart.width = this.iW * 9/16-15;
          line[1].chart.height = (this.iH - 70) / 2;
          this.options2 = line[1];
        }
      )
      this.movableloadService.PlotStatistics(this.sdate.getTime(), this.edate.getTime(),this.user, this.selectedAlgo, this.selectedRadio).then(
        line => {
          this.t.reset();
          this.loadrank = line[1];
        }
      )
    }
  }

  switchChange(e){
    this.checked=e.checked;
    this.show();
  }

  onChangeUser(event) {
    this.idService.announceId(event.value);
  }

  onRowUnselect(e) {
    this.show();
  }

  onRowSelect(e) {
    if (this.checked){
      this.movableloadService.PlotUserMovableLoad(this.user, e.data.date, e.data.date, this.selectedAlgo, this.selectedRadio,1).then(
        line => {
          line[0].chart.width = this.iW * 9 / 16-15;
          line[0].chart.height = (this.iH - 70) / 2;
          this.options1 = line[0];
          line[1].chart.width = this.iW * 9 / 16-15;
          line[1].chart.height = (this.iH - 70) / 2;
          this.options2 = line[1];
        }
      )
    }
    else{
      this.movableloadService.PlotSystemMovableLoad(e.data.date, e.data.date, this.selectedAlgo, this.selectedRadio,1).then(
        line => {
          line[0].chart.width = this.iW * 9/16-15;
          line[0].chart.height = (this.iH - 70) / 2;
          this.options1 = line[0];
          line[1].chart.width = this.iW * 9/16-15;
          line[1].chart.height = (this.iH - 70) / 2;
          this.options2 = line[1];
        }
      )
    }
  }
}
