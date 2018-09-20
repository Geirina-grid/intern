import { Component, OnInit } from '@angular/core';
import { Netlist } from './mock-net';
import { Net } from './triplenet';

@Component({
  selector: 'app-d-analysis',
  templateUrl: './d-analysis.component.html',
  styleUrls: ['./d-analysis.component.css']
})
export class DAnalysisComponent implements OnInit {

  nList=Netlist;
  selectN:Net;
  constructor() { }

  ngOnInit() {
  }
  
  onSelect(nn: Net): void {
    this.selectN = nn;
    
  }
}
