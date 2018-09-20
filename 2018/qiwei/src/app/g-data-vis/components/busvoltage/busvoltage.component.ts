import { Component, OnInit, OnDestroy, ViewChild, AfterViewInit } from '@angular/core';
import { Router } from '@angular/router';
import { Busvoltage } from '../../models/busvoltage';
import { BusvoltageService } from '../../services/busvoltage.service';
import { GraphChartService } from '../../../app-shared/services/graph-chart.service';
import { Subscription } from 'rxjs/Subscription';
import { GraphChart } from 'gvis';
//import * as gvis from 'gvis';
//const GraphChart = (<any>gvis).GraphChart;
// import { TableData } from './table-data';

@Component({
    selector: 'app-busvoltage-table',
    templateUrl: './busvoltage.component.html',
    styleUrls: ['./busvoltage.component.css'],
    providers: [BusvoltageService]

})
export class BusvoltageTableComponent implements OnInit, OnDestroy, AfterViewInit {
    // @ViewChild('container') container: any
    public chart: any;
    public busvoltages: Array<any>;
    private graph;
    selectedVolVios: Busvoltage[] = [];
    busvolIDs: any[] = [];
    startnode: String[] = [];
    perunit: string[] = [];
    subscription: Subscription;


    public constructor(
        private router: Router,
        private busvoltageService: BusvoltageService,
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
        this.busvoltageService.getBusvoltagespu()
            .then(busvoltages => {
                this.busvoltages = busvoltages;
                this.busvoltages.forEach(busvol => {
                    this.busvolIDs.push(busvol.Bus_name);
                    this.startnode.push(busvol.startnode);
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
        this.selectedVolVios.forEach(g => {
            selectedID.push(g.Bus_name);
        });
        selectedID.push(event.data.Bus_name);
        // console.log(selectedID);
        this.chart.getNodes().forEach(n => {
            if (selectedID.includes(n.exID)) {
                this.chart.unLowlightNode(n.exType, n.exID);
                selectedNode.push(n);
            } else {
                this.chart.lowlightNode(n.exType, n.exID);
            }
        });
        this.chart.getLinks().forEach(l => {
            this.chart.lowlightLink(l.exType, l.source.exType, l.source.exID, l.target.exType, l.target.exID);
        });
        console.log(selectedNode);
        this.chart.scrollIntoView(selectedNode);
        this.chart.update();
    }
    onRowUnselect(event) {
        // console.log(event);
        let selectedID = [];
        let selectedNode = [];
        this.selectedVolVios.forEach(g => {
            selectedID.push(g.Bus_name);
        });
        console.log(selectedID);
        let idx = selectedID.indexOf(event.data.Bus_name);
        // console.log(idx);
        if (idx != -1) {
            selectedID.splice(idx, 1);
        }
        // console.log(selectedID);
        if (selectedID.length === 0) {
            console.log('highlight all');
            this.highLightAll();
            this.chart.getNodes().forEach(n => {
                selectedNode.push(n);
            });
            this.chart.scrollIntoView(selectedNode);
        } else {
            this.chart.getNodes().forEach(n => {
                if (selectedID.includes(n.exID)) {
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
    perunit_value() {
        this.busvoltages = [];
        this.busvoltageService.getBusvoltagespu()
            .then(busvoltages => {
                this.busvoltages = busvoltages;
            });
    }
    actual_value() {

        this.busvoltages = [];
        this.busvoltageService.getBusvoltages()
            .then(busvoltages => {
                this.busvoltages = busvoltages;
            });

    }
    highLightAll(): void {
        let selectedNode = [];
        this.chart.getNodes().forEach(n => {
            if (this.busvolIDs.includes(n.exID)) {
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
        console.log('highlight all generators');
    }



}
