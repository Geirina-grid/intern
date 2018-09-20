import { Component, OnInit, ViewChild, AfterViewInit, OnChanges, SimpleChanges } from '@angular/core';
import { Router } from '@angular/router';
import { ConfirmDialogModule, ConfirmationService } from 'primeng/primeng';
import { RetrieveCaseService } from '../../services/retrieve-case.service';
import { environment } from '../../../../environments/environment';
import { NgForm } from '@angular/forms';
import { WholeGraphService } from '../../../app-shared/services/whole-graph.service';
const uploadUrl = environment.pyPre + '/graph/';

@Component({
  selector: 'app-retrieve-case',
  templateUrl: './retrieve-case.component.html',
  styleUrls: ['./retrieve-case.component.css'],
  providers: [RetrieveCaseService]
})
export class RetrieveCaseComponent implements OnInit {

  cases: any[];
  types: any[];
  selectedType: any;
  selectedCase: any;
  constructor(
    private router: Router,
    private retrieveCaseService: RetrieveCaseService,
    private confirmationService: ConfirmationService,
    private wholeGraphService: WholeGraphService
  ) {
    this.cases = [];
    this.cases.push({ label: 'IEEE 13 Bus ', value: { id: 1, name: 'load_IEEE13Bus' } });
    this.cases.push({ label: 'IEEE 123 Bus', value: { id: 2, name: 'load_IEEE123Bus' } });
    this.types = [];
    this.types.push({ label: '全系统数据', value: { id: 1, name: '' } });
    this.types.push({ label: '配网潮流', value: { id: 2, name: '' } });
    this.types.push({ label: '拓扑辨识', value: { id: 3, name: '' } });
    this.types.push({ label: '负荷预测', value: { id: 4, name: '' } });
    this.types.push({ label: '设备管理', value: { id: 5, name: '' } });
  }

  ngOnInit() {
  }

  retrieveCase() {
    this.retrieveCaseService.retrieveCase(JSON.stringify({ case: this.selectedCase.name }))
      .then(response => console.log(response));
  }
  Confirm() {
    this.confirmationService.confirm({
      message: 'Are you sure that you want to retrieve' + this.selectedCase.name + 'case ?',
      accept: () => {
        this.retrieveCase();
      }
    });
  }
  postGraphData(form) {
    const vertexFile = (<any>document.getElementById('node')).files[0];
    const edgeFile = (<any>document.getElementById('edge')).files[0];
    const dataFile = (<any>document.getElementById('data')).files[0];
    const curFile = (<any>document.getElementById('cur')).files[0];
    const voltFile = (<any>document.getElementById('volt')).files[0];
    const pfFile = (<any>document.getElementById('pf')).files[0];
    const reacFile = (<any>document.getElementById('reac')).files[0];
    const readFile = (<any>document.getElementById('read')).files[0];
    const formData: FormData = new FormData();
    formData.append('node', vertexFile, vertexFile.name);
    formData.append('edge', edgeFile, edgeFile.name);
    if (dataFile) { formData.append('data', dataFile, dataFile.name); }
    if (curFile) { formData.append('cur', curFile, curFile.name) };
    if (voltFile) { formData.append('volt', voltFile, voltFile.name); }
    if (pfFile) { formData.append('pf', pfFile, pfFile.name); }
    if (readFile) { formData.append('read', readFile, readFile.name); }
    if (reacFile) { formData.append('reac', reacFile, reacFile.name); }
    this.wholeGraphService.uploadGraph(formData);
  }
}
