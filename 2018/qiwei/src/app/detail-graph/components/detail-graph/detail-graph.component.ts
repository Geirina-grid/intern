import { Component, OnInit, ViewChild, AfterViewInit, OnChanges, SimpleChanges } from '@angular/core';
import { Router } from '@angular/router';
import { WholeGraphService } from '../../../app-shared/services/whole-graph.service';
import { DetailGraphService } from '../../services/detail-graph.service';
import { ConfirmDialogModule, ConfirmationService } from 'primeng/primeng';

import * as gvis from 'gvis';
const GraphChart = (<any>gvis).GraphChart;

@Component({
  selector: 'app-detail-graph',
  templateUrl: './detail-graph.component.html',
  styleUrls: ['./detail-graph.component.css'],
  providers: [DetailGraphService]
})
export class DetailGraphComponent implements OnInit, AfterViewInit {

  @ViewChild('container') container: any;
  @ViewChild('detailContainer') detaulContainer: any;
  public chart: any;
  public detailChart: any;
  private graph;
  private detailGraph;
  constructor(
    private router: Router,
    private detailGraphService: DetailGraphService
  ) { }

  ngOnInit() {
  }
  ngAfterViewInit(): void {
    this.chart = new GraphChart({
      render: {
        container: this.container.nativeElement,
        assetsUrlBase: './assets/gvis'
      }
    });
    this.detailChart = new GraphChart({
      render: {
        container: this.detaulContainer.nativeElement,
        assetsUrlBase: './assets/gvis'
      },
      style: {
        node: [
          {
            condition: (node, chart) => {
              return node.exType === 'Busbar';
            },
            style: {
              display: 'rectangle',
              acpectRatio: 0.35
            }
          }
        ]
      }
      // icon: {
      //   'CB': 'icons.006',
      //   'load_node': 'icons.010'

      //  }
    });
    (<any>window).gsql = this.chart;
    (<any>window).detail = this.detailChart;
    this.detailGraphService.getGraph()
      .then(graph => {
        this.graph = graph;
        console.log(this.graph);
        this.chart
          .addData(this.graph, 'gsql');
        this.staticLayout();

        this.chart.update();

        this.chart.on('onClick', (item) => {
          // Do something here. event call back.
          // Example:
          if (item.isNode) {
            // console.log(item);
            this.gotoDetail(item.exID);
          } else if (item.isLink) {
            console.log(`Link ${item.id} is clicked.`);
          }
        })
        this.chart.on('onRightClick', (item) => {
          // Do something here. event call back.
          // Example:
          if (item.isNode) {
            this.iconDetail(item.exID);
          } else if (item.isLink) {
            console.log(`Link ${item.id} is clicked.`);
          }
        })

      });
  }
  staticLayout(): void {
    this.chart.getNodes().forEach(n => {
      n.x = n.attrs.pos_x;
      n.y = n.attrs.pos_y;
    });
    this.chart.runLayout('static');
  }

  gotoDetail(id: any): void {
    this.detailGraphService.getDetail(Number(id))
      .then(detail => {
        console.log(detail);
        this.detailGraph = detail;
        this.detailChart.resetData(this.detailGraph, 'gsql')
          .runLayout('tree');
        this.detailChart.getNodes().filter(n =>
          n.attrs.Breaker_point === 1).forEach(n =>
            n.styles.base.icon = 'Closed Breaker');
        this.detailChart.getNodes().filter(n =>
          n.attrs.Breaker_point === 0).forEach(n =>
            n.styles.base.icon = 'Open Breaker');
        this.detailChart.getNodes().filter(n =>
          n.attrs.DS_pt === 1).forEach(n =>
            n.styles.base.icon = 'Closed Disconnector');
        this.detailChart.getNodes().filter(n =>
          n.attrs.DS_pt === 0).forEach(n =>
            n.styles.base.icon = 'Open Disconnector');
        this.detailChart.getNodes().filter(n =>
          n.exType === 'Load_node').forEach(n =>
            n.styles.base.icon = 'Load');
        this.detailChart.getNodes().filter(n =>
          n.exType === 'Generator').forEach(n =>
            n.styles.base.icon = 'Generator');
        this.detailChart.update();
      });
  }
  iconDetail(id: any): void {

  }

}

