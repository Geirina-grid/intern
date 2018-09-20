import { Component, OnInit,OnDestroy, ViewChild, AfterViewInit } from '@angular/core';
import { Router } from '@angular/router';
import { LineResult } from '../../models/lineResult';
import { LineResultService } from '../../services/lineResult.service';
import { GraphChartService } from '../../../app-shared/services/graph-chart.service';
import { Subscription } from 'rxjs/Subscription';
import { GraphChart } from 'gvis';
// import { TableData } from './table-data';

//import * as gvis from 'gvis';
//const GraphChart = (<any>gvis).GraphChart;

@Component({
    selector: 'app-lineResult-table',
    templateUrl: './lineResult.component.html',
    styleUrls: ['./lineResult.component.css'],
    providers: [LineResultService]
})
export class LineResultTableComponent implements OnInit,OnDestroy, AfterViewInit {
    //@ViewChild('container') container: any
    private chart: any;
    public lineResults: Array<any> = [];
    private graph;
    // selectedLineResults = [];
    //  lineResultIDs: any[] = [];
    selectedLineResults: LineResult[] = [];
    lineResultEdges: any[] = [];
    startnode: string[]=[];
    subscription: Subscription;
    
    public constructor(
        private router: Router,
        private lineResultService: LineResultService,
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
        this.lineResultService.getLineResults()
            .then(lineResults => {
                this.lineResults = lineResults;
                this.lineResults.forEach(lineR => {
                    this.lineResultEdges.push(lineR.edge_name);
                    this.startnode.push(lineR.startnode);
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
            this.selectedLineResults.forEach(g => {
                selectedID.push(g.edge_name);
            });
            selectedID.push(event.data.edge_name);
            // console.log(selectedID);
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
        // console.log(this.selectedLines[0].tap_bus_type);
                // console.log(this.selectedLines);

    }
    onRowUnselect(event) {
        // console.log(this.selectedLines);
        // console.log(event);
        let selectedID = [];
        let selectedNodes = [];
        this.selectedLineResults.forEach(g => {
            selectedID.push(g.edge_name);
        });
        //console.log(selectedEdge);
        let idx = selectedID.indexOf(event.data.edge_name);
        // console.log(idx);
        if (idx != -1) {
            selectedID.splice(idx, 1);
        }
        // console.log(selectedID);
        if (selectedID.length === 0) {
            console.log('highlight all');
            this.highLightAll();
            this.chart.getNodes().forEach(n =>{    
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
        let selectedNodes = [];
        // highlight all lines
        this.chart.getLinks().forEach(l => {
            if (this.lineResultEdges.includes(String(l.attrs.edge_name))) {
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
       // this.chart.scrollIntoView(selectedNodes);
        this.chart.update();
        console.log('highlight all lines');
    }
}
