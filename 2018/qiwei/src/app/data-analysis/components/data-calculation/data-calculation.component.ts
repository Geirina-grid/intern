import { Component, OnInit, ViewChild, AfterViewInit, OnChanges, SimpleChanges } from '@angular/core';
import { Router } from '@angular/router';
import { ConfirmDialogModule, ConfirmationService, ProgressBarModule } from 'primeng/primeng';
import { ClusteringService } from '../../services/clustering.service';
import { PrecisionMarketingService } from '../../services/precision-marketing.service';
import { MovableLoadService } from '../../services/movable-load.service';
import { DataQualityService } from '../../services/data-quality.service';

@Component({
  selector: 'app-data-calculation',
  templateUrl: './data-calculation.component.html',
  styleUrls: ['./data-calculation.component.css'],
  providers: [ClusteringService,PrecisionMarketingService,MovableLoadService,DataQualityService]
})
export class DataCalculationComponent implements OnInit {
  iH = window.innerHeight - 120;
  iW = Math.min(window.innerWidth, 2000);
  nclus: number;
  years: any[];
  selectedYear: any={ label: '2013年', value:2013 };
  display0: boolean=false;
  display1: boolean=false;
  display2: boolean=false;
  disable0: boolean=false;
  disable1: boolean=false;
  disable2: boolean=false;
  disable3: boolean=false;
  disable4: boolean=false;
  disable5: boolean=false;
  top_position: number;
  left_position: number;
  loading: boolean = false;
  processcount: any;
  chosenprocess: any;

  constructor(
    private router: Router,
    private confirmationService: ConfirmationService,
    private clusteringservice: ClusteringService,
    private precisionmarketingserive: PrecisionMarketingService,
    private movableLoadService:MovableLoadService,
    private DataQualityService: DataQualityService
  ) {
    this.nclus=30;
    this.years = [];
    this.years.push({ label: '2013年', value:2013 });
    this.top_position = this.iH*3/4;
    this.left_position = this.iW / 2-80;
    this.chosenprocess = 1;
    this.processcount = [
      { label: '1', value: 1 },
      { label: '2', value: 2 },
      { label: '4', value: 4 },
      { label: '8', value: 8 }
    ];
  }

  ngOnInit() {
  }

  runalgo(){
    console.log(this.nclus);
    console.log(this.selectedYear);
    this.display0 = true;
    this.disable0 = true;
    this.disable1 = false;
    this.disable2 = true;
    this.disable3 = true;
    this.disable4 = true;
  }

  run_cluster(){
    this.loading = true;
    this.clusteringservice.run_algo(this.nclus).then(
      res => {
        this.disable1 = true;
        this.disable2 = false;
        this.loading = false;
      }
    )
  }

  run_dataquality(){
    this.loading = true;
    this.display2=true;
    this.DataQualityService.rundq("li", this.chosenprocess).then(
      res1 => {
        this.DataQualityService.rundq("sas", this.chosenprocess).then(
          res2 => {
            this.disable2 = true;
            this.disable3 = false;
            this.display2 = false;
            this.loading = false;
          }
        )
      }
    )
  }

  run_movableload(){
    this.loading = true;
    this.display1=true;
    this.movableLoadService.RunAlgorithm(this.chosenprocess).then(
      res => {
        this.disable3 = true;
        this.disable4 = false;
        this.display1 = false;
        this.loading = false;
      }
    )
  }

  run_elasticity(){
    this.loading = true;
    this.precisionmarketingserive.RunElasticity(this.selectedYear.value).then(
      res => {
        this.disable4 = true;
        this.loading = false;
      }
    )
  }

  end(){
    this.loading = false;
    this.display0 = false;
    this.disable0 = false;
    this.display1 = false;
    this.display2 = false;
  }
}
