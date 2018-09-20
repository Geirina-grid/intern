import { Component, OnInit, ViewChild, AfterViewInit, OnChanges, SimpleChanges} from '@angular/core';
import { Router } from '@angular/router';
import { WholeGraphService } from '../app-shared/services/whole-graph.service';
import { ConfirmDialogModule, ConfirmationService, ListboxModule} from 'primeng/primeng';
import { GraphManageService } from '../app-shared/services/graph-manage.service';
import { GraphChartService } from '../app-shared/services/graph-chart.service';
import { Subscription } from 'rxjs/Subscription';
// import { PrecisionMarketingService } from '../../data-analysis/services/precision-marketing.service';
import { NewelasticityService } from '../app-shared/services/newelasticity.service';
import * as gvis from 'gvis';
import { DataQualityService } from '../data-analysis/services/data-quality.service';
import { GlobalIdService } from '../app-shared/services/global-id.service';
const GraphChart = (<any>gvis).GraphChart;

@Component({
  selector: 'app-newelasticity',
  templateUrl: './newelasticity.component.html',
  styleUrls: ['./newelasticity.component.css'],
  providers: [NewelasticityService]
})


export class NewelasticityComponent implements OnInit {
  @ViewChild('topocontainer') container1: any;
  iH = window.innerHeight-120;
  iW = Math.min(window.innerWidth,2000);
  public chart: any;
  private graph;
  date: Date;
  defaultDate = new Date('01/01/2013');
  nclus:number;
  options1: Object;
  options2: Object;
  options3: Object;
  options4: Object;
  options5: Object;
  options6: Object;
  meters: any;
  phases: any;
  selectedmeter: any;

  subscription: Subscription;
  constructor(
    private router: Router,
    private wholeGraphService: WholeGraphService,
    private DataQualityService: DataQualityService,
    private graphManageService: GraphManageService,
    private confirmationService: ConfirmationService,
    // private precisionmarketingserive: PrecisionMarketingService,
    private newelasticityservice: NewelasticityService,
    private graphChartService: GraphChartService,
    private idService: GlobalIdService
  ) {
    this.subscription = this.graphChartService.graphChart$.subscribe(
      chart => {
          this.chart = chart;
      }
    );
    this.graphChartService.announceSubscription();
    console.log(this.iH);
    console.log(this.iW);
    this.date = this.defaultDate;
    this.nclus = 1;

    this.meters = [];
   }

  ngOnInit() {
  }

  ngAfterViewInit() {

    this.DataQualityService.GetIDList().then(
      line => {
        line.result.forEach(element => {
          this.meters.push({ label: element, value: element })
        });
        console.log(this.meters);
      }
    )

    this.newelasticityservice.PlotElasticity().then(
      line => {
        line[0].chart.width = this.iW*3/4/4;
        line[0].chart.height = this.iH/2;
        this.options1 = line[0];
        line[1].chart.width = this.iW*3/4/4;
        line[1].chart.height = this.iH/2;
        this.options2 = line[1];
        line[2].chart.width = this.iW*3/4/4;
        line[2].chart.height = this.iH/2;
        this.options3 = line[2];
      }
    )
    this.newelasticityservice.PlotClusterElasticity(this.nclus).then(
      line => {
        line[0].chart.width = this.iW*3*3/4/4;
        line[0].chart.height = this.iH/2;
        this.options4 = line[0];
        line[1].chart.width = this.iW*3/4/4;
        line[1].chart.height = this.iH/2;
        this.options5 = line[1];
      }
    )
    this.newelasticityservice.PlotSystemLoad(this.date.getTime()).then(
      line => {
        line.chart.width = this.iW*3/4/4-8;
        line.chart.height = this.iH/2-15;
        this.options6 = line;
      }
    )
  }

  changeid () {
    this.idService.announceId(this.selectedmeter);
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
    this.newelasticityservice.PlotClusterElasticity(e.context.category).then(
      line => {
        line[0].chart.width = this.iW*3*3/4/4;
        line[0].chart.height = this.iH/2;
        this.options4 = line[0];
        line[1].chart.width = this.iW*3/4/4;
        line[1].chart.height = this.iH/2;
        this.options5 = line[1];
      }
    )
  }
  onSelect(event) {
    this.newelasticityservice.PlotSystemLoad(this.date.getTime()).then(
        line => {
          line.chart.width = this.iW*3/4/4-8;
          line.chart.height = this.iH/2-15;
          this.options6 = line;
        }
      )
  }
  ngOnDestroy() {
    // prevent memory leak when component destroyed
    this.subscription.unsubscribe();
  }
}
