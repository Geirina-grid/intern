import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { TimeseriesVisComponent } from './components/time-series/timeseries-vis.component';
import { TimeseriesVisService } from './services/timeseries-vis.service';
import { LineTableComponent } from './components/line/line.component';
import { GeneratorTableComponent } from './components/generator/generator.component';
import { RegulatorTableComponent } from './components/regulator/regulator.component';
import { RegulatorResTableComponent } from './components/regulatorRes/regulatorRes.component';
import { CapacitorTableComponent } from './components/capacitor/capacitor.component';
import { CapResultTableComponent } from './components/capResult/capResult.component';
import { LineResultTableComponent } from './components/lineResult/lineResult.component';
import { LoadDisplayTableComponent } from './components/loadDisplay/loadDisplay.component';
import { BusvoltageTableComponent } from './components/busvoltage/busvoltage.component';
import { SummaryTableComponent } from './components/summary/summary.component';
import { SwitchTableComponent } from './components/switch/switch/switch.component';
import { GraphSwitchFormComponent } from './components/switch/graph-swich-form/graph-switch-form.component';

import { DataTableModule, SharedModule, ButtonModule, GrowlModule, TooltipModule, CheckboxModule, SliderModule } from 'primeng/primeng';
import { ConfirmDialogModule, ConfirmationService, CalendarModule, TabViewModule } from 'primeng/primeng';
import { MenuModule, MenuItem, DropdownModule, InputSwitchModule, ToolbarModule, DialogModule, ListboxModule } from 'primeng/primeng';
import { MultiSelectModule, ToggleButtonModule, RadioButtonModule, ProgressSpinnerModule, ProgressBarModule } from 'primeng/primeng';
import { LeafletModule } from '@asymmetrik/ngx-leaflet';
import { platformBrowserDynamic } from '@angular/platform-browser-dynamic';
import { BrowserAnimationsModule } from '@angular/platform-browser/animations';
import { FormsModule, ReactiveFormsModule } from '@angular/forms';
import { APP_BASE_HREF } from '@angular/common';
import { HttpModule } from '@angular/http';
import { RouterModule } from '@angular/router';

import { AppSharedModule } from '../app-shared/app-shared.module';
import { ChartModule } from 'angular2-highcharts';
import { HighchartsStatic } from 'angular2-highcharts/dist/HighchartsService';


@NgModule({
  imports: [
    CommonModule,
    FormsModule,
    ChartModule,
    ReactiveFormsModule,
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
    ProgressBarModule,
    AppSharedModule
  ],
  declarations: [
    TimeseriesVisComponent,
    LineTableComponent,
    GeneratorTableComponent,
    RegulatorTableComponent,
    RegulatorResTableComponent,
    CapacitorTableComponent,
    CapResultTableComponent,
    RegulatorTableComponent,
    LineResultTableComponent,
    LoadDisplayTableComponent,
    BusvoltageTableComponent,
    SummaryTableComponent,
    SwitchTableComponent,
    GraphSwitchFormComponent,
  ],
  providers: [{
    provide: HighchartsStatic,
    useFactory: highchartsFactory
  },
   TimeseriesVisService,
  ]
})
export class GDataVisModule { }
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