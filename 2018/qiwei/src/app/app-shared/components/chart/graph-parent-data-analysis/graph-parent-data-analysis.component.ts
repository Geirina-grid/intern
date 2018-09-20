import { Component, OnInit, ViewChild, AfterViewInit } from '@angular/core';
import { GraphChart } from 'gvis';
import { GraphChartComponent } from '../graph-chart/graph-chart.component';
import { Router } from '@angular/router';
import { Subscription } from 'rxjs/Subscription';
import { GraphChartService } from '../../../services/graph-chart.service';
import { LeafletmapComponent } from '../leafletmap/leafletmap.component';

@Component({
  selector: 'app-graph-parent-data-analysis',
  templateUrl: './graph-parent-data-analysis.component.html',
  styleUrls: ['./graph-parent-data-analysis.component.css']
})
export class GraphParentDataAnalysisComponent implements OnInit, AfterViewInit {
  @ViewChild(GraphChartComponent) chart_component: GraphChartComponent;
  @ViewChild(LeafletmapComponent) map_component: LeafletmapComponent;
  subscriptions: Subscription[] = [];
  graphStats: boolean;
  newSub: boolean;
  mapStats: boolean;
  constructor(
    private graphChartService: GraphChartService
  ) {
    this.subscriptions.push(this.graphChartService.graphInitiated$.subscribe(
      (status) => {
        this.graphStats = status;
console.log(this.chart_component);
        this.graphChartService.announceGraphchart(this.chart_component.chart);
      }
    ));
    this.subscriptions.push(this.graphChartService.mapInitiated$.subscribe(
      (status) => {
        this.mapStats = status;
        this.graphChartService.announceMap(this.chart_component.map.map);
      }
    ));
    this.subscriptions.push(this.graphChartService.newSubscription$.subscribe(
      (status) => {
        console.log('new subscription');
        console.log('graph stats' + this.graphStats);
        if (this.graphStats) {
          console.log(this.chart_component.chart);
          this.graphChartService.announceGraphchart(this.chart_component.chart);
        }
      }
    ));
    this.subscriptions.push(this.graphChartService.newMapSubscription$.subscribe(
      (status) => {
        console.log('new map subscription');
        console.log('map stats ' + this.mapStats);
        if (this.mapStats) {
          console.log(this.chart_component.map.map);
          this.graphChartService.announceMap(this.chart_component.map.map);
        }
      }
    ));
    this.subscriptions.push(this.graphChartService.mapUpdate$.subscribe(
      (status) => {
        console.log('map updated');
        console.log('nap stats ' + this.mapStats);
        if (this.mapStats) {
          console.log('New map ' + this.chart_component.map.map);
          this.graphChartService.announceMap(this.chart_component.map.map);
        }
      }
    ));
  }

  ngOnInit() {
  }

  ngAfterViewInit() {
    // this.chart_component.annouceGraph();
  }

}
