import { platformBrowserDynamic } from '@angular/platform-browser-dynamic';
//import { BrowserAnimationsModule } from '@angular/platform-browser/animations';
import { NgModule } from '@angular/core';
import { BrowserModule } from '@angular/platform-browser';
import { FormsModule, ReactiveFormsModule } from '@angular/forms';
import { APP_BASE_HREF } from '@angular/common';
import { HttpModule } from '@angular/http';
import { AppComponent } from './app.component';
import { AppRoutingModule } from './app-routing.module';
import { MySharedModule } from './shared/shared.module';

// import primeNG modules here

// import { DataTableModule, SharedModule, ButtonModule, GrowlModule, TooltipModule, CheckboxModule, SliderModule } from 'primeng/primeng';
// import { ConfirmDialogModule, ConfirmationService, CalendarModule, TabViewModule } from 'primeng/primeng';
// import { MenuModule, MenuItem, DropdownModule, InputSwitchModule, ToolbarModule, DialogModule, ListboxModule } from 'primeng/primeng';
// import { MultiSelectModule, ToggleButtonModule, RadioButtonModule, ProgressSpinnerModule } from 'primeng/primeng';

// import { MatTabsModule } from '@angular/material/tabs';

// import { MatSidenavModule } from '@angular/material';

// import { ChartModule } from 'angular2-highcharts';
// import { HighchartsStatic } from 'angular2-highcharts/dist/HighchartsService';
// import * as highcharts from 'highcharts';
// import * as highstock from 'highcharts/highstock';

// import { LeafletModule } from '@asymmetrik/ngx-leaflet';


// import app shared module
import { AppSharedModule } from './app-shared/app-shared.module';

// import feature modules
import { DataAnalysisModule } from './data-analysis/data-analysis.module';
import { GDataVisModule } from './g-data-vis/g-data-vis.module';
import { HomeModule } from './home/home.module';
import { LoadForecastModule } from './load-forecast/load-forecast.module';
import { RetrieveCaseModule } from './retrieve-case/retrieve-case.module';
import { PowerFlowCalModule } from './power-flow-cal/power-flow-cal.module';
import { EquipManageModule } from './equip-manage/equip-manage.module';
import { DetailGraphModule } from './detail-graph/detail-graph.module';
import { IESMainModule } from './IES/IESMain.module';
import {MatDialogModule} from '@angular/material';
import { DevQComponent } from './d-modeling/dev-q.component';
import { DAnalysisComponent } from './d-analysis/d-analysis.component';
import { EnetComponent } from './d-analysis/enet/enet.component';
import { HnetComponent } from './d-analysis/hnet/hnet.component';
import { TnetComponent } from './d-analysis/tnet/tnet.component';
import { MultiNetComponent } from './d-analysis/multi-net/multi-net.component';
// import { Filter1Component } from './filter1/filter1.component';
// import { ModelingFormComponent } from '../modeling-form/modeling-form.component';

import { MultiSelectModule, ToggleButtonModule, DropdownModule, RadioButtonModule, ProgressSpinnerModule, ProgressBarModule } from 'primeng/primeng';
import { ChartModule } from 'angular2-highcharts';
//import { DataPfComponent } from './d-modeling/data-pf/data-pf.component';
import { datapfModule } from './d-modeling/data-pf/data-pf.module';
import {PfResultModule} from './d-modeling/pf-result/pf-result.module';
//import { PfResultComponent } from './d-modeling/pf-result/pf-result.component';

import { NgxEchartsModule } from 'ngx-echarts';


// restructured component
import { FilterbusinessComponent } from './filterbusiness/filterbusiness.component';
import { MapboxModule } from './app-shared/add-a-map/mapbox.module';
import { IesloaddataComponent } from './load-forecast/components/iesloaddata/iesloaddata.component';
import { LoaddatagraphService } from './app-shared/services/loaddatagraph.service';
import { DataTableModule} from 'primeng/primeng';

//Bowen
//import { MenuModule, MenuItem, InputSwitchModule, ToolbarModule, DialogModule, ListboxModule } from 'primeng/primeng';
//import { ConfirmDialogModule, ConfirmationService, CalendarModule, TabViewModule } from 'primeng/primeng';
//import { BrowserAnimationsModule } from '@angular/platform-browser/animations';
//import { CommonModule } from '@angular/common'
//import { Daterangepicker } from 'ng2-daterangepicker';
//import { DataTableModule,SharedModule, ButtonModule, GrowlModule, TooltipModule, CheckboxModule, SliderModule } from 'primeng/primeng';

@NgModule({
  declarations: [
    AppComponent,
    DevQComponent,
    DAnalysisComponent,
    EnetComponent,
    HnetComponent,
    TnetComponent,
    MultiNetComponent,
    //IesloaddataComponent,

    //restructured component
    // FilterbusinessComponent,
  ],
  imports: [
    NgxEchartsModule,
    ChartModule,
    MatDialogModule, 
    BrowserModule,
   // BrowserAnimationsModule,
    FormsModule,
    AppRoutingModule,
    HttpModule,
    ReactiveFormsModule,
    AppSharedModule,
    IESMainModule,
    DataAnalysisModule,
    DataTableModule,
    GDataVisModule,
    HomeModule,
    LoadForecastModule,
    RetrieveCaseModule,
    PowerFlowCalModule,
    EquipManageModule,
    DetailGraphModule,
    MySharedModule,datapfModule,
    PfResultModule,
    MapboxModule,
    // DataTableModule,
    // SharedModule,
    // ButtonModule,
    // SliderModule,
    // ConfirmDialogModule,
    DropdownModule,
    //Bowen
    //ButtonModule,
    //ConfirmDialogModule,
    //SliderModule,
    //SharedModule,
    //Daterangepicker,
    //CommonModule,
    //BrowserAnimationsModule,
    //ChartModule,
    //AppRoutingModule,
    //FormsModule,
    //ReactiveFormsModule,
    //DropdownModule, InputSwitchModule, ToolbarModule, CalendarModule,
    //GrowlModule,
    //TabViewModule, DialogModule,
    //TooltipModule, CheckboxModule,
    //ListboxModule,
    //MultiSelectModule,
    //ToggleButtonModule,
    //RadioButtonModule,
    //ProgressSpinnerModule,
    //End

    //restructured modules
    // MapboxModule,
  ],
  providers: [
  //   {
  //   provide: HighchartsStatic,
  //   useFactory: highchartsFactory
  // },
    // ConfirmationService,
  LoaddatagraphService],
  bootstrap: [AppComponent],
  // entryComponents: [PreprocessTrainComponent]
})
export class AppModule { }
// platformBrowserDynamic().bootstrapModule(AppModule);

// export function highchartsFactory() {
//   const highstock = require('highcharts/highstock');
//   const highcharts = require('highcharts')
//   highstock.setOptions({
//     annotationsOptions: { enabledButtons: false },
//     global: {
//       useUTC: false
//     }
//   });
//   highcharts.setOptions({
//     annotationsOptions: { enabledButtons: false },
//     global: {
//       useUTC: false
//     }
//   });
//   require('highcharts-annotations')(highstock);
//   require('highcharts-annotations')(highcharts);

//   return highstock;
// }
