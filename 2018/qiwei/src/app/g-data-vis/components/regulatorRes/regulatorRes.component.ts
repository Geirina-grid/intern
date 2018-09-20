import { Component, OnInit, ViewChild,OnDestroy, AfterViewInit} from '@angular/core';
import { Router } from '@angular/router';
import { RegulatorRes } from '../../models/regulatorRes';
import { RegulatorResService } from '../../services/regulatorRes.service';
import { GraphChartService } from '../../../app-shared/services/graph-chart.service';
import { Subscription } from 'rxjs/Subscription';
import { GraphChart } from 'gvis';
//import * as gvis from 'gvis';
//const GraphChart = (<any>gvis).GraphChart;
// import { TableData } from './table-data';

@Component({
    selector: 'app-regulatorRes-table',
    templateUrl: './regulatorRes.component.html',
    styleUrls: ['./regulatorRes.component.css'],
    providers: [RegulatorResService]

})
export class RegulatorResTableComponent implements OnInit, OnDestroy,AfterViewInit {
    @ViewChild('container') container: any
    public chart: any;
    public regulatorRess: Array<any>;
    private graph;
    selectedRegRess: RegulatorRes[] = [];
    regResIDs: string[] = [];
    startnode: string[]=[];
    subscription: Subscription;

    public constructor(
        private router: Router,
        private regulatorResService: RegulatorResService,
        private graphChartService: GraphChartService
        
    ) {
        console.log('fdf')
        this.subscription = this.graphChartService.graphChart$.subscribe(
            chart => {
                this.chart = chart;
                this.highLightAll();
            }
        );
        this.graphChartService.announceSubscription();
    }

    public ngOnInit(): void {
        this.regulatorResService.getRegulatorRess()
            .then(regulatorRess => {
                this.regulatorRess = regulatorRess;
                this.regulatorRess.forEach(reg => {
                    this.regResIDs.push(reg.regulatorRes_name);
                    this.startnode.push(reg.startnode);
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
        this.selectedRegRess.forEach(g => {
            selectedID.push(g.regulatorRes_name);
        });
        selectedID.push(event.data.regulatorRes_name);
        // console.log(selectedID);
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
        this.selectedRegRess.forEach(g => {
            selectedID.push(g.regulatorRes_name);
        });
        console.log(selectedID);
        let idx = selectedID.indexOf(event.data.regulatorRes_name);
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
        // highlight all loads
        this.chart.getNodes().forEach(n => {
            if (this.regResIDs.includes(String(n.exID))) {
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
      //  this.chart.scrollIntoView(selectedNode);
        this.chart.update();
        console.log('highlight all loads');
    }



}
