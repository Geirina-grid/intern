import { Component, OnInit, ViewChild, ChangeDetectorRef, NgZone } from '@angular/core';

import { MatTableDataSource, MatSort } from '@angular/material';
// import { MatSort } from '@angular/material';

import { FiltertypicalService, Product } from '../app-shared/services/filtertypical.service';
// import { GlobalMapComponent } from '../global-map/global-map.component';

import { FormControl } from '@angular/forms';

import { Observable } from 'rxjs/Observable';
import { startWith } from 'rxjs/operators/startWith';
import { map } from 'rxjs/operators/map';

import { DomSanitizer } from '@angular/platform-browser';
import { MatIconRegistry } from '@angular/material';
// import { environment } from '../../../../environments/environment';

export class Site {
  constructor(public name: string, public type: string, public flag: string, iconRegistry: MatIconRegistry, sanitizer: DomSanitizer) { }
}

@Component({
  selector: 'app-filtertypical',
  templateUrl: './filtertypical.component.html',
  styleUrls: ['./filtertypical.component.css'],
  providers: [FiltertypicalService]
})


export class FiltertypicalComponent implements OnInit {

  //autoComplete
  siteCtrl: FormControl;
  filteredSites: Observable<any[]>;

  columns: any[] = [{ label: 'Name', value: 'name' }, { label: 'Category', value: 'category' }, { label: 'Quantity', value: 'quantity' },
  { label: 'On Hand', value: 'onhand' },
  { label: 'In Transit', value: 'intransit' }];

  elasticityList = [{label: 'index 1', value: false}, {label: 'index 2', value: false}, {label: 'index 3', value: false}, {label: 'index 4', value: false}];
  matList = [{label: '成品', value: false}, {label: '组装原件', value: false}];
  satList = [{label: '运输中', value: false}, {label: '阻塞', value: false}, {label: '过期', value: false}, {label: '质量问题', value: false}, {label: '未受限', value: false}];
  clsList = [{label: 'V1', value: false}, {label: 'V2', value: false}, {label: 'V3', value: false}, {label: 'V4', value: false}, {label: 'V5', value: false}];
  retList = [{label: '关闭', value: false}, {label: '开放', value: false}];

  sites: Site[] = [
    {
      name: 'FRA-01',
      type: '工厂',
      // https://commons.wikimedia.org/wiki/File:Flag_of_Arkansas.svg
      flag: 'https://lipis.github.io/flag-icon-css/flags/4x3/fr.svg'
    },
    {
      name: 'GER-01',
      type: '中转站',
      // https://commons.wikimedia.org/wiki/File:Flag_of_California.svg
      flag: 'https://lipis.github.io/flag-icon-css/flags/4x3/de.svg'
    },
    {
      name: 'ITA-01',
      type: '仓库',
      // https://commons.wikimedia.org/wiki/File:Flag_of_Florida.svg
      flag: 'https://lipis.github.io/flag-icon-css/flags/4x3/it.svg'
    },
    {
      name: 'MEX-01',
      type: '车间',
      // https://commons.wikimedia.org/wiki/File:Flag_of_Texas.svg
      flag: 'https://lipis.github.io/flag-icon-css/flags/4x3/mx.svg'
    },
    {
      name: 'US-01',
      type: '配送中心',
      // https://commons.wikimedia.org/wiki/File:Flag_of_Texas.svg
      flag: 'https://lipis.github.io/flag-icon-css/flags/4x3/um.svg'
    },
    {
      name: 'CA-01',
      type: '厂站',
      // https://commons.wikimedia.org/wiki/File:Flag_of_Texas.svg
      flag: 'https://lipis.github.io/flag-icon-css/flags/4x3/ca.svg'
    }
  ];

  constructor(private filtertypicalService: FiltertypicalService, private ref: ChangeDetectorRef, private zone: NgZone) {
    this.siteCtrl = new FormControl();
    this.filteredSites = this.siteCtrl.valueChanges
      .pipe(
        startWith(''),
        map(site => site ? this.filterSites(site) : this.sites.slice())
      );
  }

  filterSites(name: string) {
    return this.sites.filter(site =>
      site.name.toLowerCase().indexOf(name.toLowerCase()) === 0);
  }

  displayedColumns = ['name', 'category', 'quantity', 'onhand', 'intransit'];

  dataSource: any;
  // dataSource = new MatTableDataSource(this.data);

  @ViewChild(MatSort) sort: MatSort;
  // @ViewChild(GlobalMapComponent) myMap;

  ngOnInit() {
  }


  siteName = "";
  selectedSiteList = [];

  addToSelectedSiteList() {
    if (this.selectedSiteList.indexOf(this.siteName) < 0) {
      this.selectedSiteList.push(this.siteName);
    }
    this.siteName = "";
    console.log(this.selectedSiteList);
  }

  deleteFromSelectedSiteList(name) {
    var index = this.selectedSiteList.indexOf(name)
    this.selectedSiteList.splice(index, index + 1);
  }

  reset() {
    this.siteName = "";
    this.selectedSiteList = [];

    this.satList[0].value = false;
    this.satList[1].value = false;
    this.satList[2].value = false;
    this.satList[3].value = false;
    this.satList[4].value = false;

    this.matList[0].value = false;
    this.matList[1].value = false;

    this.clsList[0].value = false;
    this.clsList[1].value = false;
    this.clsList[2].value = false;
    this.clsList[3].value = false;
    this.clsList[4].value = false;

    this.retList[0].value = false;
    this.retList[1].value = false;
  }

  getParams(): String {
    var params = [];
    this.selectedSiteList.forEach(element => {
      params.push("seeds=elecotest:site:" + element);
    });

    if (this.satList[0].value || this.satList[1].value || this.satList[2].value || this.satList[3].value || this.satList[4].value) {
      if (this.satList[0].value) {
        params.push("seeds=inventory_status:ACTIVE");
      }
      if (this.satList[1].value) {
        params.push("seeds=inventory_status:BLOCKED");
      }
      if (this.satList[2].value) {
        params.push("seeds=inventory_status:OBSOLETE");
      }
      if (this.satList[3].value) {
        params.push("seeds=inventory_status:QUALITY_HOLD");
      }
      if (this.satList[4].value) {
        params.push("seeds=inventory_status:UNRESTRICTED");
      }
    } else {
      params.push("seeds=inventory_status:ACTIVE");
      params.push("seeds=inventory_status:BLOCKED");
      params.push("seeds=inventory_status:OBSOLETE");
      params.push("seeds=inventory_status:QUALITY_HOLD");
      params.push("seeds=inventory_status:UNRESTRICTED");
    }

    if (this.matList[0].value || this.matList[1].value) {
      if (this.matList[0].value) {
        params.push("seeds=material_type:FGI");
      }
      if (this.matList[1].value) {
        params.push("seeds=material_type:PI");
      }
    } else {
      params.push("seeds=material_type:FGI");
      params.push("seeds=material_type:PI");
    }

    if (this.clsList[0].value || this.clsList[1].value || this.clsList[2].value || this.clsList[3].value || this.clsList[4].value) {
      if (this.clsList[0].value) {
        params.push("seeds=cost_classification:V1");
      }
      if (this.clsList[1].value) {
        params.push("seeds=cost_classification:V2");
      }
      if (this.clsList[2].value) {
        params.push("seeds=cost_classification:V3");
      }
      if (this.clsList[3].value) {
        params.push("seeds=cost_classification:V4");
      }
      if (this.clsList[4].value) {
        params.push("seeds=cost_classification:V5");
      }
    } else {
      params.push("seeds=cost_classification:V1");
      params.push("seeds=cost_classification:V2");
      params.push("seeds=cost_classification:V3");
      params.push("seeds=cost_classification:V4");
      params.push("seeds=cost_classification:V5");
    }

    if (this.retList[0].value || this.retList[1].value) {
      if (this.retList[0].value) {
        params.push("seeds=returns:OPEN");
      }
      if (this.retList[1].value) {
        params.push("seeds=returns:CLOSED");
      }

    } else {
      params.push("seeds=returns:OPEN");
      params.push("seeds=returns:CLOSED");
    }



    //let gsqlPre = environment.gsqlPre;
    //var url = gsqlPre + "/query/product_graph/getInfoMod?" + params.join("&");
    //var url = "http://localhost:3000/query/product_graph/getInfoMod?" + params.join("&");
    //console.log(url);
    return params.join("&");
  }

  loadData(param?: any) {
    //var url = this.getURL();
    var url = this.filtertypicalService.getURL(this.getParams());
    this.filtertypicalService.loadData(url).subscribe(data => {
      this.filtertypicalService.processData(data);
      this.filtertypicalService.getData().subscribe(data => {
        this.zone.run(() => {
          this.dataSource = new MatTableDataSource(data);

          this.dataSource.sort = this.sort;
          // this.myMap.reload();
        });
      });
    });


    console.log(this.dataSource);
  }
}


