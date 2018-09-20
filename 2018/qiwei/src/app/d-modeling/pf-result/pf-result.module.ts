
import { NgModule } from '@angular/core';
import { BrowserModule } from '@angular/platform-browser';
import { CommonModule } from '@angular/common';
import { PfResultComponent } from './pf-result.component';
// import { DevQComponent } from '../dev-q.component';
import { HttpModule } from '@angular/http';
import { HttpClientModule } from '@angular/common/http';

import { MatButtonModule, MatCheckboxModule, MatCardModule, MatListModule, MatInputModule, MatSortModule} from '@angular/material';
import { MatTableModule } from '@angular/material/table';
import { MatAutocompleteModule } from '@angular/material/autocomplete';
//import { GlobalMapComponent } from './components/global-map/global-map.component';
import { MatChipsModule } from '@angular/material/chips';
import { MatIconModule } from '@angular/material/icon';

import { FormsModule, ReactiveFormsModule } from '@angular/forms';

import { PfResultService } from './pf-result.service';
import { MySharedModule } from '../../shared/shared.module';

import {GraphChartComponent} from '../../app-shared/components/chart/graph-chart/graph-chart.component'
import { AppSharedModule } from '../../app-shared/app-shared.module';

import { MapboxModule } from '../../app-shared/add-a-map/mapbox.module';
import {DemodataService} from '../../app-shared/add-a-map/services/demodata.service';

import { ResizableModule } from 'angular-resizable-element';
import {CalendarModule, SharedModule,DataTableModule} from 'primeng/primeng';
import { ChartModule } from 'angular2-highcharts';
@NgModule({
  imports: [
    CommonModule,
    BrowserModule,
    HttpModule,
    HttpClientModule,
    MatButtonModule,
    MatCheckboxModule,
    MatCardModule,
    MatListModule,
    MatInputModule,
    MatTableModule,
    MatSortModule,
    MatAutocompleteModule,
    MatChipsModule,
    MatIconModule,
    FormsModule,
    ReactiveFormsModule,
    MySharedModule,
    AppSharedModule,
    MapboxModule,
    ResizableModule,
    ChartModule, CalendarModule, SharedModule,DataTableModule
  ],
  providers: [PfResultService],
  declarations: [PfResultComponent]
})
export class PfResultModule { }
