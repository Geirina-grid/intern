import { LeafletModule } from '@asymmetrik/ngx-leaflet';
import { platformBrowserDynamic } from '@angular/platform-browser-dynamic';
import { BrowserAnimationsModule } from '@angular/platform-browser/animations';
import { NgModule } from '@angular/core';
import { FormsModule, ReactiveFormsModule } from '@angular/forms';
import { APP_BASE_HREF } from '@angular/common';
import { HttpModule } from '@angular/http';
import { RouterModule } from '@angular/router';
import { CommonModule } from '@angular/common';


// import primeNG modules here

import { DataTableModule, SharedModule, ButtonModule, GrowlModule, TooltipModule, CheckboxModule, SliderModule } from 'primeng/primeng';
import { ConfirmDialogModule, ConfirmationService, CalendarModule, TabViewModule } from 'primeng/primeng';
import { MenuModule, MenuItem, DropdownModule, InputSwitchModule, ToolbarModule, DialogModule, ListboxModule } from 'primeng/primeng';
import { MultiSelectModule, ToggleButtonModule, RadioButtonModule, ProgressSpinnerModule } from 'primeng/primeng';

// Test by Jack
// import { PopupComponent } from '../data-analysis/components/popup/popup.component';
// import { DataTableModule, SharedModule, ButtonModule, GrowlModule, TooltipModule, CheckboxModule, SliderModule } from 'primeng/primeng';
// import { ConfirmDialogModule, ConfirmationService, CalendarModule, TabViewModule } from 'primeng/primeng';
// import { MenuModule, MenuItem, InputSwitchModule, ToolbarModule, DialogModule, ListboxModule } from 'primeng/primeng';
// import { MultiSelectModule, ToggleButtonModule, DropdownModule, RadioButtonModule, ProgressSpinnerModule, ProgressBarModule } from 'primeng/primeng';
// import highchart
import { ChartModule } from 'angular2-highcharts';
import { HighchartsStatic } from 'angular2-highcharts/dist/HighchartsService';

// import @angular/mateiral

import { MatTabsModule } from '@angular/material/tabs';
import { MatSidenavModule } from '@angular/material';

// Angular Material Modules
import { MatButtonModule, MatCardModule, MatListModule, MatInputModule, MatSortModule} from '@angular/material';
import { MatCheckboxModule } from '@angular/material/checkbox';
import { MatTableModule } from '@angular/material/table';
import { MatAutocompleteModule } from '@angular/material/autocomplete';
import { MatChipsModule } from '@angular/material/chips';
import { MatIconModule } from '@angular/material/icon';


// components
import { GraphManageComponent } from './components/graph-manage/graph-manage/graph-manage.component';
import { GraphManageQuestionComponent } from './components/graph-manage/graph-manage-question/graph-manage-question.component';
import { GraphManageFormComponent } from './components/graph-manage/graph-manage-form/graph-manage-form.component';
import { GraphChartComponent } from './components/chart/graph-chart/graph-chart.component'
import { GraphParentComponent } from './components/chart/graph-parent/graph-parent.component';
import { GraphParentDataAnalysisComponent } from './components/chart/graph-parent-data-analysis/graph-parent-data-analysis.component';
import { BackLogComponent } from './components/log-view/back-log/back-log.component';
import { AppLogComponent } from './components/log-view/app-log/app-log.component';
import { UwsgiLogComponent } from './components/log-view/uwsgi-log/uwsgi-log.component';
import { ReqLogComponent } from './components/log-view/req-log/req-log.component';
import { LeafletmapComponent } from './components/chart/leafletmap/leafletmap.component';
import { LoadingIndicatorComponent } from './components/loading-indicator/loading-indicator.component';
import { HackMapDirective } from './directives/hack-map.directive';


// services

import { WholeGraphService } from './services/whole-graph.service';
import { GraphManageService } from './services/graph-manage.service';
import { EditGraphService } from './services/edit-graph.service';
import { GraphChartService } from './services/graph-chart.service';
import { LeafletMapLatLngService } from './services/leafletmap.service';
import { GlobalIdService } from './services/global-id.service';
import { ModelingComponent } from '../modeling/modeling.component';
import { ModelingService } from './services/modeling.service';
import { ModelingFormComponent } from '../modeling/modeling-form/modeling-form.component';
import { ModelingQuestionComponent } from '../modeling/modeling-question/modeling-question.component';
import { LoadTableComponent } from '../load-table/load-table.component';
import { LoadTableService } from './services/load-table.service';

import { NewclusteringComponent } from '../newclustering/newclustering.component';
import { NewtypicalComponent } from './newtypical/newtypical.component';
import { NewpopupComponent } from '../newpopup/newpopup.component';
import { NewelasticityComponent } from '../newelasticity/newelasticity.component';
import { DataQualityService } from '../data-analysis/services/data-quality.service';
import { FiltertypicalComponent } from '../filtertypical/filtertypical.component';
//import { ChecklistComponent } from '../shared/components/checklist/checklist.component';
// import { DataTableComponent } from './components/data-table/data-table.component';
import { FilterbusinessComponent } from '../filterbusiness/filterbusiness.component';

import { MySharedModule } from '../shared/shared.module';
import { MapboxModule } from './add-a-map/mapbox.module';

import { ResizableModule } from 'angular-resizable-element';


@NgModule({
  declarations: [
    GraphManageComponent,
    GraphManageQuestionComponent,
    GraphManageFormComponent,
    GraphChartComponent,
    GraphParentComponent,
    GraphParentDataAnalysisComponent,
    BackLogComponent,
    AppLogComponent,
    UwsgiLogComponent,
    ReqLogComponent,
    LeafletmapComponent,
    LoadingIndicatorComponent,
    HackMapDirective,
    ModelingComponent,
    ModelingFormComponent,
    ModelingQuestionComponent,
    LoadTableComponent,

    NewclusteringComponent,
    NewtypicalComponent,
    NewpopupComponent,
    NewelasticityComponent,
    FiltertypicalComponent,
    // ChecklistComponent,
    // DataTableComponent,
    FilterbusinessComponent,
 
  ],
  imports: [
    ChartModule,
    CommonModule,
    BrowserAnimationsModule,
    FormsModule,
    HttpModule,
    DataTableModule,
    SharedModule,
    MySharedModule,
    ButtonModule,
    SliderModule,
    ConfirmDialogModule,
    DropdownModule, InputSwitchModule, ToolbarModule, CalendarModule,
    GrowlModule,
    TabViewModule, DialogModule,
    ReactiveFormsModule, TooltipModule, CheckboxModule,
    MatSidenavModule,
    ListboxModule,
    MultiSelectModule,
    ToggleButtonModule,
    RadioButtonModule,
    LeafletModule.forRoot(),
    ProgressSpinnerModule,
    MatTabsModule,
    RouterModule,
    MatButtonModule, MatCheckboxModule, MatCardModule, MatListModule, MatInputModule, MatSortModule,
    MatTableModule, MatAutocompleteModule, MatChipsModule, MatIconModule,
    MapboxModule,
    ResizableModule
  ],
  providers: [
    {
      provide: HighchartsStatic,
      useFactory: highchartsFactory
    },
    WholeGraphService,
    GraphManageService,
    ModelingService,
    LoadTableService,
    ConfirmationService,
    GraphChartService, EditGraphService,
    LeafletMapLatLngService,
    GlobalIdService,
    DataQualityService
  ],
  exports: [
    GraphManageComponent,
    GraphManageQuestionComponent,
    GraphManageFormComponent,
    GraphChartComponent,
    GraphParentComponent,
    GraphParentDataAnalysisComponent,
    BackLogComponent,
    AppLogComponent,
    UwsgiLogComponent,
    ReqLogComponent,
    LeafletmapComponent,
    LoadingIndicatorComponent,
    HackMapDirective,
    ModelingComponent,
    ModelingFormComponent,
    ModelingQuestionComponent,

    // ChecklistComponent,
    //  DataTableComponent,
    // FilterbusinessComponent
  ]
})
export class AppSharedModule { }
// platformBrowserDynamic().bootstrapModule(AppModule);

export function highchartsFactory() {
  const highstock = require('highcharts/highstock');
  const highcharts = require('highcharts')
  highstock.setOptions({
    annotationsOptions: { enabledButtons: false },
    global: {
      useUTC: false
    }
  });
  highcharts.setOptions({
    annotationsOptions: { enabledButtons: false },
    global: {
      useUTC: false
    }
  });
  require('highcharts-annotations')(highstock);
  require('highcharts-annotations')(highcharts);

  return highstock;
}

