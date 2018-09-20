import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common'
import { PreprocessService } from './services/preprocess.service';
import { TrainService } from './services/train.service';
import { PredictService } from './services/predict.service';
import { NormalizerService } from './services/normalizer.service';
import { ModelService } from './services/model.service';
import { CalStatsService } from './services/cal-stats.service';
import { ReferenceService } from './services/reference.service';
import { LoadForecastComponent } from './components/load-forecast/load-forecast.component';
import { PreprocessComponent } from './components/preprocess/preprocess.component';
import { PredictComponent } from './components/predict/predict.component';
import { LoaddataComponent } from './components/loaddata/loaddata.component';
import { PreprocessTrainComponent } from './components/preprocess-train/preprocess-train.component'
import { TrainAnalysisComponent } from './components/train-analysis/train-analysis.component';
import { PreprocessTrainLogComponent } from './components/preprocess-train-log/preprocess-train-log.component';
import { TrainResultChartComponent } from './components/train-result-chart/train-result-chart.component';
import { IesloaddataComponent } from './components/iesloaddata/iesloaddata.component';

import { DataTableModule, SharedModule, ButtonModule, GrowlModule, TooltipModule, CheckboxModule, SliderModule } from 'primeng/primeng';
import { ConfirmDialogModule, ConfirmationService, CalendarModule, TabViewModule } from 'primeng/primeng';
import { MenuModule, MenuItem, InputSwitchModule, ToolbarModule, DialogModule, ListboxModule } from 'primeng/primeng';
import { MultiSelectModule, ToggleButtonModule, DropdownModule, RadioButtonModule, ProgressSpinnerModule } from 'primeng/primeng';
import { LeafletModule } from '@asymmetrik/ngx-leaflet';
import { platformBrowserDynamic } from '@angular/platform-browser-dynamic';
import { BrowserAnimationsModule } from '@angular/platform-browser/animations';
import { FormsModule, ReactiveFormsModule } from '@angular/forms';
import { APP_BASE_HREF } from '@angular/common';
import { HttpModule } from '@angular/http';
import { RouterModule } from '@angular/router';
import {MatButtonModule} from '@angular/material/button';
import { AppSharedModule } from '../app-shared/app-shared.module';
import { MapboxModule } from '../app-shared/add-a-map/mapbox.module';

// import highchart
import { ChartModule } from 'angular2-highcharts';
import { HighchartsStatic } from 'angular2-highcharts/dist/HighchartsService';
import { AppRoutingModule } from '../app-routing.module';
import { Daterangepicker } from 'ng2-daterangepicker';


@NgModule({
  imports: [
    AppRoutingModule,
    MatButtonModule,
    BrowserAnimationsModule,
    FormsModule,
    ReactiveFormsModule,
    CommonModule,
    Daterangepicker,
    DataTableModule,
    SharedModule,
    ButtonModule,
    SliderModule,
    ConfirmDialogModule,
    DropdownModule, InputSwitchModule, ToolbarModule, CalendarModule,
    GrowlModule,
    TabViewModule, DialogModule,
    TooltipModule, CheckboxModule,
    ListboxModule,
    MultiSelectModule,
    ToggleButtonModule,
    RadioButtonModule,
    ProgressSpinnerModule,
    ChartModule,
    MapboxModule
  ],
  declarations: [
    LoadForecastComponent,
    PreprocessComponent,
    PredictComponent,
    LoaddataComponent,
    TrainAnalysisComponent,
    PreprocessTrainComponent,
    PreprocessTrainLogComponent,
    TrainResultChartComponent,
    IesloaddataComponent
  ],
  providers: [
    {
      provide: HighchartsStatic,
      useFactory: highchartsFactory
    },
    PreprocessService,
    TrainService,
    PredictService, NormalizerService, ModelService, CalStatsService, ReferenceService,
  ],
  entryComponents: [PreprocessTrainComponent, TrainAnalysisComponent]
})
export class LoadForecastModule { }

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
