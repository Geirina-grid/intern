import { Component, OnInit, OnDestroy, ViewChild, AfterViewInit } from '@angular/core';
import { Router } from '@angular/router';
import { Line } from '../../models/line';
import { LineService } from '../../services/line.service';
import { GraphChartService } from '../../../app-shared/services/graph-chart.service';
import { Subscription } from 'rxjs/Subscription';
import { GraphChart } from 'gvis';
// import * as gvis from 'gvis';
// const GraphChart = (<any>gvis).GraphChart;
// import { TableData } from './table-data';

@Component({
    selector: 'app-line-table',
    templateUrl: './line.component.html',
    styleUrls: ['./line.component.css'],
    providers: [LineService]
})
export class LineTableComponent implements OnInit, OnDestroy, AfterViewInit {
    // @ViewChild('container') container: any
    // @ViewChild('graphchart') graphchart: any
    // public chart: any;
    // @ViewChild('table') dataTable: any;
    public lines: Array<any>;
    private graph;
    public chart: any;

    selectedLines: Line[] = [];
    lineEdges: any[] = [];
    startnode: string[] = [];
    subscription: Subscription;

    public constructor(
        private router: Router,
        private lineService: LineService,
        private graphChartService: GraphChartService
    ) {
        this.subscription = this.graphChartService.graphChart$.subscribe(
            chart => {
                this.chart = chart;
                 this.highLightAll(); 
            }
        );
        this.graphChartService.announceSubscription();
    }

    public ngOnInit(): void {
        this.lineService.getLines()
            .then(lines => {
                this.lines = lines;
                this.lines.forEach(lin => {
                    this.lineEdges.push(lin.edge_name);
                    this.startnode.push(lin.startnode);
                });
            })
            .then(() => {
                if (this.chart !== undefined) {
                    this.highLightAll();
                }
            });
    }

    ngAfterViewInit() {
    }
    ngOnDestroy() {
        // prevent memory leak when component destroyed
        this.subscription.unsubscribe();
    }

    onRowSelect(event) {
        let selectedID = [];
        let selectedNodes = [];
        //   selectedEdge.push({type:"1", srType:})
        this.selectedLines.forEach(g => {
            // selectedEdge.push(g.edge_name);
            selectedID.push(g.edge_name);
        });
        //selectedEdge.push(event.data.edge_name);
        selectedID.push(event.data.edge_name);
        // console.log(selectedID);
        console.log(this.chart);
        this.chart.getLinks().forEach(l => {
            // if (selectedEdge.includes(String(l.attrs.edge_name))) {
            if (selectedID.includes(String(l.attrs.edge_name))) {
                this.chart.unLowlightLink(l.exType, l.source.exType, l.source.exID, l.target.exType, l.target.exID);
                selectedNodes.push(l.source);
                selectedNodes.push(l.target);
            } else {
                this.chart.lowlightLink(l.exType, l.source.exType, l.source.exID, l.target.exType, l.target.exID);
            }
        });
        this.chart.getNodes().forEach(n => {
            this.chart.lowlightNode(n.exType, n.exID);
        });
        this.chart.scrollIntoView(selectedNodes);
        this.chart.update();

    }
    onRowUnselect(event) {
        //let selectedEdge = [];
        let selectedID = [];
        let selectedNodes = [];
        this.selectedLines.forEach(g => {
            //   selectedEdge.push(g.edge_name);
            selectedID.push(g.edge_name);
        });
        // console.log(selectedEdge);
        let idx = selectedID.indexOf(event.data.edge_name);
        // console.log(idx);
        if (idx != -1) {
            selectedID.splice(idx, 1);
        }
        // console.log(selectedID);
        if (selectedID.length === 0) {
            console.log('   ');
            this.highLightAll();
            this.chart.getNodes().forEach(n => {
                selectedNodes.push(n);
            });
            this.chart.scrollIntoView(selectedNodes);
        } else {
            this.chart.getLinks().forEach(l => {
                if (selectedID.includes(String(l.attrs.edge_name))) {
                    this.chart.unLowlightLink(l.exType, l.source.exType, l.source.exID, l.target.exType, l.target.exID);
                    selectedNodes.push(l.source);
                    selectedNodes.push(l.target);
                } else {
                    this.chart.lowlightLink(l.exType, l.source.exType, l.source.exID, l.target.exType, l.target.exID);
                }
            });
            this.chart.getNodes().forEach(n => {
                this.chart.lowlightNode(n.exType, n.exID);
            });
            this.chart.scrollIntoView(selectedNodes);
            this.chart.update();
        }
    }

    highLightAll(): void {
        // highlight all lines
        if (this.chart !== undefined) {
            console.log('hightlightall');
            let selectedNodes = [];
            this.chart.getLinks().forEach(l => {
                if (this.lineEdges.includes(String(l.attrs.edge_name))) {
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
}
