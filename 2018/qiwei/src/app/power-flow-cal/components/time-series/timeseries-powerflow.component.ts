import { NgModule, Component, Input, OnInit, ViewChild, AfterViewInit, OnChanges, SimpleChanges } from '@angular/core';
import {CommonModule} from '@angular/common';
import { Router } from '@angular/router';
import { ConfirmDialogModule, ConfirmationService, ProgressBarModule} from 'primeng/primeng';
import { SeriesPowerFlowService } from '../../services/timeseries-powerflow.service';

@Component({
  selector: 'app-timeseries-powerflow',
  templateUrl: './timeseries-powerflow.component.html',
  styleUrls: ['./timeseries-powerflow.component.css'],
  providers: [SeriesPowerFlowService]
})
export class SeriesPowerFlowComponent implements OnInit {

  starttime: string;
  stoptime: string;
  defaultDates = '2018-01-01 00:00:00';
  defaultDatee = '2018-12-31 23:45:00';
  solver: any;
  selectedsolver: string;
  loading: boolean = false;
  constructor(
    private router: Router,
    private powerFlowService: SeriesPowerFlowService,
    private confirmationService: ConfirmationService
  ) {

    this.starttime = this.defaultDates;
    this.stoptime = this.defaultDatee;
    this.solver = [];
    // this.selectedsolver = '';
    this.solver.push({label: 'Backward-Forward Sweep', value: 'FBS'});
    this.solver.push({label: 'Newton-Raphson', value: 'NR'});
    this.solver.push({label: 'Gauss-Seidel', value: 'GS'});

    console.log(this.starttime, this.stoptime);
  }

  ngOnInit() {

  }

  runPowerFlow() {
    console.log(this.starttime, this.stoptime, this.selectedsolver);
    this.loading = true;
    this.powerFlowService.runPowerFlow({ starttime: this.starttime, stoptime: this.stoptime, solver: this.selectedsolver })
      .then(res => {
       console.log(res);
       if (!res.error){
       return this.powerFlowService.rungridlabd();
       }else{
       }
      })
      .then(res=>{
        if(!res.error){
          this.loading = false;
        }else{

        }
      });
  }

  // PFResults() {
  //   // this.powerFlowService.getPFResults()
  //   //   .then(data => {
  //   //    console.log(data);
  //   //    this. voltage_A = 'None';
  //   //    this.voltage_A = data['value'];
  //   //    console.log(this.voltage_A);
  //   //   });
  //   this.powerFlowService.postgridlabd();
  // }




  Confirm() {
    this.confirmationService.confirm({
      message: 'Run Power Flow ?',
      accept: () => {
        this.runPowerFlow();
      }
    });
  }

}

