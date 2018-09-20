import { Component, OnInit, ViewChild, OnDestroy, AfterViewInit } from '@angular/core';
import { Router } from '@angular/router';
import { CapResult } from '../../models/capResult';
import { CapResultService } from '../../services/capResult.service';
import { GraphChartService } from '../../../app-shared/services/graph-chart.service';
import { Subscription } from 'rxjs/Subscription';
import { GraphChart } from 'gvis';
//import * as gvis from 'gvis';
//const GraphChart = (<any>gvis).GraphChart;
// import { TableData } from './table-data';

@Component({
    selector: 'app-cap-result-table',
    templateUrl: './capResult.component.html',
    styleUrls: ['./capResult.component.css'],
    providers: [CapResultService]

})
export class CapResultTableComponent implements OnInit, OnDestroy,AfterViewInit {
    //@ViewChild('container') container: any
    public chart: any;
    public capResults: Array<any>;
    private graph;
    selectedCPs: CapResult[] = [];
    cpIDs: string[] = [];
    startnode: string[]=[];
    subscription: Subscription;
    

    public constructor(
        private router: Router,
        private capResultService: CapResultService,
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
        this.capResultService.getCapResults()
            .then(capResults => {
                this.capResults = capResults;
                this.capResults.forEach(cp => {
                    this.cpIDs.push(cp.Bus);
                    this.startnode.push(cp.startnode);
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
        let selectedNode = [];
        this.selectedCPs.forEach(g => {
            selectedID.push(g.Bus);
        });
        selectedID.push(event.data.Bus);
         console.log(selectedID);
        this.chart.getNodes().forEach(n => {
            if (selectedID.includes(String(n.exID))) {
                this.chart.unLowlightNode(n.exType, n.exID);
                selectedNode.push(n);
            } else {
                this.chart.lowlightNode(n.exType, n.exID);
            }
        });
        this.chart.getLinks().forEach(l => {
            this.chart.lowlightLink(l.exType, l.source.exType, l.source.exID, l.target.exType, l.target.exID);
        });
        this.chart.scrollIntoView(selectedNode);
        this.chart.update();
    }
    onRowUnselect(event) {
        // console.log(event);
        let selectedID = [];
        let selectedNode = [];
        this.selectedCPs.forEach(g => {
            selectedID.push(g.Bus);
        });
        console.log(selectedID);
        let idx = selectedID.indexOf(event.data.Bus);
        // console.log(idx);
        if (idx != -1) {
            selectedID.splice(idx, 1);
        }
        // console.log(selectedID);
        if (selectedID.length === 0) {
            console.log('highlight all');
            this.highLightAll();
            this.chart.getNodes().forEach(n =>{    
                    selectedNode.push(n);    
            });
            this.chart.scrollIntoView(selectedNode);
        } else {
            this.chart.getNodes().forEach(n => {
                if (selectedID.includes(String(n.exID))) {
                    this.chart.unLowlightNode(n.exType, n.exID);
                    selectedNode.push(n);
                } else {
                    this.chart.lowlightNode(n.exType, n.exID);
                }
            });
            this.chart.getLinks().forEach(l => {
                this.chart.lowlightLink(l.exType, l.source.exType, l.source.exID, l.target.exType, l.target.exID);
            });
            this.chart.scrollIntoView(selectedNode);              
            this.chart.update();
        }
    }

    highLightAll(): void {
        let selectedNode = [];
        // highlight all capResults
        this.chart.getNodes().forEach(n => {
            if (this.cpIDs.includes(String(n.exID))) {
                this.chart.unLowlightNode(n.exType, n.exID);
                selectedNode.push(n);
            } else {
                this.chart.lowlightNode(n.exType, n.exID);
            }
        });
        // lowlight all edges
        this.chart.getLinks().forEach(l => {
            this.chart.lowlightLink(l.exType, l.source.exType, l.source.exID, l.target.exType, l.target.exID);
        });
       // this.chart.scrollIntoView(selectedNode);
        this.chart.update();
        console.log('highlight all capResults');
    }
}
