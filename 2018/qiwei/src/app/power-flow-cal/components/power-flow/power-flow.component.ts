import { Component, OnInit, ViewChild, AfterViewInit, OnChanges, SimpleChanges } from '@angular/core';
import { Router } from '@angular/router';
import { ConfirmDialogModule, ConfirmationService } from 'primeng/primeng';
import { PowerFlowService } from '../../services/power-flow.service';

@Component({
  selector: 'app-power-flow',
  templateUrl: './power-flow.component.html',
  styleUrls: ['./power-flow.component.css'],
  providers: [PowerFlowService]
})
export class PowerFlowComponent implements OnInit {

  iteration: number;
  maxchange: number;
  ItNo: number;
  Accuracy: number;
  VNbase: number;
  constructor(
    private router: Router,
    private powerFlowService: PowerFlowService,
    private confirmationService: ConfirmationService
  ) {
    this.iteration = 10;
    this.maxchange = 0.0001;

  }

  ngOnInit() {
  }

  runPowerFlow() {

    this.powerFlowService.runPowerFlow({ iteration: this.iteration, maxchange: this.maxchange })
      .then(data => { 
       console.log(data);
       let ItNo=[];
       let Accuracy=[];
       let VNbase=[];
        Object.keys(data[0])
          this.ItNo=data[0];
        Object.keys(data[1])
          this.Accuracy=data[1].toFixed(8);
        Object.keys(data[2])
          this.VNbase=data[2];
          console.log(this.ItNo, this.Accuracy, this.VNbase);
      });
  }
  Confirm() {
    this.confirmationService.confirm({
      message: 'Run Power Flow ?',
      accept: () => {
        this.runPowerFlow();
      }
    });
  }

}
