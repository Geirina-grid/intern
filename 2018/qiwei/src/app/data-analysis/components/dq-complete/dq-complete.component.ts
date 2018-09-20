import { Component, OnInit, ViewChild, AfterViewInit, OnChanges, SimpleChanges } from '@angular/core';
import { Router } from '@angular/router';
import { WholeGraphService } from '../../../app-shared/services/whole-graph.service';
import { ConfirmDialogModule, ConfirmationService } from 'primeng/primeng';
import { GraphManageService } from '../../../app-shared/services/graph-manage.service';
import { DqCompleteService } from '../../services/dq-complete.service'
import * as hc from 'highcharts';
import * as gvis from 'gvis';
const GraphChart = (<any>gvis).GraphChart;
const PieChart = (<any>gvis).PieChart;
import { platformBrowserDynamic } from '@angular/platform-browser-dynamic';
import { NgModule } from '@angular/core';
import { BrowserModule } from '@angular/platform-browser';
import { ChartModule } from 'angular2-highcharts';

@Component({
  selector: 'app-dq-complete',
  templateUrl: './dq-complete.component.html',
  styleUrls: ['./dq-complete.component.css'],
  providers: [DqCompleteService]
})
export class DqCompleteComponent implements OnInit, AfterViewInit {
  options1: any;
  constructor(
    private router: Router,
    private dqcompleteservice: DqCompleteService    
  ) {
    this.options1 = {
      chart:{width:400},
      yAxis:{title:{text:'MegaWatt'}},
      title: { text: '' },
      series: [{
        name: 'Load Profile',
        showInLegend: false,
        data: []
      }]
    };
   }

  ngOnInit() {

  }
  
  ngAfterViewInit(){
    this.dqcompleteservice.get_peak().then(
      res=>{
        console.log(res);
        this.options1 = res[0];
      }
    )
  }
}
