import { Component, OnInit, ViewChild, AfterViewInit, OnChanges, SimpleChanges, OnDestroy } from '@angular/core';
import { Router } from '@angular/router';
import { Capacitor } from '../../models/capacitor';
import { CapacitorService } from '../../services/capacitor.service';
import { WholeGraphService } from '../../../app-shared/services/whole-graph.service';
import { GraphChartService } from '../../../app-shared/services/graph-chart.service';
import { Subscription } from 'rxjs/Subscription';
import { GraphChart } from 'gvis'
// import * as gvis from 'gvis';
// const GraphChart = (<any>gvis).GraphChart;
// import { TableData } from './table-data';

@Component({
    selector: 'app-capacitor-table',
    templateUrl: './capacitor.component.html',
    styleUrls: ['./capacitor.component.css'],
    providers: [CapacitorService]

})
export class CapacitorTableComponent implements OnInit, AfterViewInit, OnDestroy {
    // @ViewChild('container') container: any
    public chart: any;
    public capacitors: Array<any>;
    private graph;
    selectedCPs: Capacitor[] = [];
    cpIDs: any[] = [];
    startnode: string[] = [];
    subscription: Subscription;
    public constructor(
        private router: Router,
        private capacitorService: CapacitorService,
        private graphChartService: GraphChartService
    ) {
        this.subscription = this.graphChartService.graphChart$.subscribe(
            chart => {
                this.chart = chart;
                this.highLightAll();
            }
        )
        this.graphChartService.announceSubscription();
    }

    public ngOnInit(): void {
        this.capacitorService.getCapacitors()
            .then(capacitors => {
                this.capacitors = capacitors;
                this.capacitors.forEach(cp => {
                    this.cpIDs.push(cp.bus_name);
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
        this.subscription.unsubscribe();
    }

    staticLayout(): void {
        this.chart.getNodes().forEach(n => {
            n.x = n.attrs.pos_x;
            n.y = n.attrs.pos_y;
        });
        this.chart.runLayout('static');
    }

    onRowSelect(event) {
        let selectedID = [];
        let selectedNode = [];
        this.selectedCPs.forEach(g => {
            selectedID.push(g.bus_name);
        });
        selectedID.push(event.data.bus_name);
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
        let selectedID = [];
        let selectedNode = [];
        this.selectedCPs.forEach(g => {
            selectedID.push(g.bus_name);
        });
        let idx = selectedID.indexOf(event.data.bus_name);
        // console.log(idx);
        if (idx != -1) {
            selectedID.splice(idx, 1);
        }
        // console.log(selectedID);
        if (selectedID.length === 0) {
            this.highLightAll();
            this.chart.getNodes().forEach(n => {
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
        // highlight all capacitors
        console.log(this.chart)
        console.log(this.cpIDs)
        let selectedNode = [];
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
        //      this.chart.scrollIntoView(selectedNode);
        this.chart.update();
        console.log('highlight capacitors');
    }
}
