import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { WelcomeTableComponent } from './components/welcome/welcome.component';

import { DataTableModule, SharedModule, ButtonModule, GrowlModule, TooltipModule, CheckboxModule, SliderModule } from 'primeng/primeng';
import { ConfirmDialogModule, ConfirmationService, CalendarModule, TabViewModule } from 'primeng/primeng';
import { MenuModule, MenuItem, DropdownModule, InputSwitchModule, ToolbarModule, DialogModule, ListboxModule } from 'primeng/primeng';
import { MultiSelectModule, ToggleButtonModule, RadioButtonModule, ProgressSpinnerModule } from 'primeng/primeng';
import { LeafletModule } from '@asymmetrik/ngx-leaflet';
import { platformBrowserDynamic } from '@angular/platform-browser-dynamic';
import { BrowserAnimationsModule } from '@angular/platform-browser/animations';
import { FormsModule, ReactiveFormsModule } from '@angular/forms';
import { APP_BASE_HREF } from '@angular/common';
import { HttpModule } from '@angular/http';
import { RouterModule } from '@angular/router';
import { ChartModule } from 'angular2-highcharts';
import { HighchartsStatic } from 'angular2-highcharts/dist/HighchartsService';


@NgModule({
  imports: [
    CommonModule,
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
    ChartModule
  ],
  declarations: [
    WelcomeTableComponent
  ],
  providers: [
    {
      provide: HighchartsStatic,
      useFactory: highchartsFactory
    },
  ]
})
export class HomeModule { }

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

