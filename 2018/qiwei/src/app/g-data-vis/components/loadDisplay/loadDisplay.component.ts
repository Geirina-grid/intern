import { Component, OnInit, OnDestroy, ViewChild, AfterViewInit } from '@angular/core';
import { Router } from '@angular/router';
import { LoadDisplay } from '../../models/loadDisplay';
import { CustomerDisplay } from '../../models/customerDisplay';
import { LoadDisplayService } from '../../services/loadDisplay.service';
import { GraphChartService } from '../../../app-shared/services/graph-chart.service';
import { Subscription } from 'rxjs/Subscription';
import { GraphChart } from 'gvis';
// import { TableData } from './table-data';

@Component({
    selector: 'app-load-display-table',
    templateUrl: './loadDisplay.component.html',
    styleUrls: ['./loadDisplay.component.css'],
    providers: [LoadDisplayService]

})
export class LoadDisplayTableComponent implements OnInit, AfterViewInit, OnDestroy {
    // @ViewChild('container') container: any
    iH = window.innerHeight-270;
    iW = Math.min(window.innerWidth,2000);
    h1: any;
    h2: any;
    public chart: any;
    public loadDisplays: Array<any>;
    public customerDisplays: Array<any>;
    private graph;
    selectedLoadDiss: LoadDisplay[] = [];
    selectedCustomerDiss: CustomerDisplay[] = [];
    loadDisIDs: string[] = [];
    startnode: string[]=[];
    subscription: Subscription;
    loadpoint: string[] = [];

    public constructor(
        private router: Router,
        private loadDisplayService: LoadDisplayService,
        private graphChartService: GraphChartService
    ) {
        this.customerDisplays = [];
        this.subscription = this.graphChartService.graphChart$.subscribe(
            chart => {
                this.chart = chart;
                this.highLightAll();
            }
        );
        this.graphChartService.announceSubscription();
    }

    public ngOnInit(): void {
        this.loadDisplayService.getLoadDisplays()
            .then(loadDisplays => {
                this.loadDisplays = loadDisplays;
                this.loadDisplays.forEach(loadDis => {
                    this.loadDisIDs.push(loadDis.Bus_name);
                    this.startnode.push(loadDis.startnode);
                });
            })
            .then(() => {
                if (this.chart !== undefined) {
                    this.highLightAll();
                }
            });
        
            this.h1 = this.iH*0.5+"px";
            this.h2 = this.iH*0.5+"px";  
    }
    ngAfterViewInit() {
       
    }
    ngOnDestroy() {
        this.subscription.unsubscribe();
    }

   

    onRowSelect(event) {
        let selectedID = [];
        let selectedNode = [];
        this.loadpoint =[];   
        this.customerDisplays = [];    

        this.selectedLoadDiss.forEach(g => {
            selectedID.push(g.Bus_name);
        });
        if (!selectedID.includes(event.data.Bus_name))
        {
             selectedID.push(event.data.Bus_name);         
        }
        console.log(this.selectedLoadDiss);
        console.log(selectedID);

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
        selectedID.forEach(k => {
            this.loadpoint.push(k);
        });   
        console.log(this.loadpoint);
        this.loadpoint.forEach(e => {
            this.loadDisplayService.getCustomerDisplays({loadpoint: e})
            .then(res => {
                this.customerDisplays = this.customerDisplays.concat(res);
                console.log(res);
                console.log(this.customerDisplays);
            });
        });
        // console.log(this.customerDisplays);
        // this.loadDisplayService.getCustomerDisplays({loadpoint:this.loadpoint})
        // .then(customerDisplays => {
        //     this.customerDisplays = customerDisplays;
        // })

        
    }

    onRowSelect_c(event){

    }

    onRowUnselect(event) {
        // console.log(event);
        let selectedID = [];
        let selectedNode = [];
        this.loadpoint =[]; 
        this.customerDisplays = [];

        this.selectedLoadDiss.forEach(g => {
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
        selectedID.forEach(k => {
            this.loadpoint.push(k);
        });   
        console.log(this.loadpoint);
        this.loadpoint.forEach(e => {
            this.loadDisplayService.getCustomerDisplays({loadpoint: e})
            .then(res => {
                this.customerDisplays = this.customerDisplays.concat(res);
                console.log(res);
                console.log(this.customerDisplays);
            });
        });

    }

    onRowUnselect_c(event){
        
    }

    highLightAll(): void {
        let selectedNode = [];
        // highlight all loads
        this.chart.getNodes().forEach(n => {
            if (this.loadDisIDs.includes(String(n.exID))) {
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
