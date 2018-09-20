import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';

import { LeafletModule } from '@asymmetrik/ngx-leaflet';
import { platformBrowserDynamic } from '@angular/platform-browser-dynamic';
import { BrowserAnimationsModule } from '@angular/platform-browser/animations';
import { BrowserModule } from '@angular/platform-browser';
import { FormsModule, ReactiveFormsModule } from '@angular/forms';
import { APP_BASE_HREF } from '@angular/common';
import { HttpModule } from '@angular/http';
import { RouterModule } from '@angular/router'

import { DataQualityComponent } from './components/data-quality/data-quality.component';
import { CrossValidComponent } from './components/cross-valid/cross-valid.component';
import { MovableLoadComponent } from './components/movable-load/movable-load.component';
import { BigDataComponent } from './components/big-data/big-data.component';
import { BigDataService } from './services/big-data.service';
import { CvPeComponent } from './components/cv-pe/cv-pe.component';
import { CvPquiComponent } from './components/cv-pqui/cv-pqui.component';
import { ClusteringComponent } from './components/clustering/clustering.component';
import { DqCompleteComponent } from './components/dq-complete/dq-complete.component';
import { AllTlComponent } from './components/all-tl/all-tl.component';
import { PopupComponent } from './components/popup/popup.component';
import { UserAnalysisComponent } from './components/user-analysis/user-analysis.component';
import { UserAnalysisService } from './services/user-analysis.service';
import { TimeVisComponent } from './components/time-vis/time-vis.component';
import { DataCalculationComponent } from './components/data-calculation/data-calculation.component';
import { PrecisionMarketingComponent } from './components/precision-marketing/precision-marketing.component';
import { PrecisionMarketingService } from './services/precision-marketing.service';
import { PrecisionMarketingUserComponent } from './components/precision-marketing-user/precision-marketing-user.component';
import { PrecisionMarketingUserService } from './services/precision-marketing-user.service';

import { DataTableModule, SharedModule, ButtonModule, GrowlModule, TooltipModule, CheckboxModule, SliderModule } from 'primeng/primeng';
import { ConfirmDialogModule, ConfirmationService, CalendarModule, TabViewModule } from 'primeng/primeng';
import { MenuModule, MenuItem, InputSwitchModule, ToolbarModule, DialogModule, ListboxModule } from 'primeng/primeng';
import { MultiSelectModule, ToggleButtonModule, DropdownModule, RadioButtonModule, ProgressSpinnerModule, ProgressBarModule } from 'primeng/primeng';
import { ChartModule } from 'angular2-highcharts';
import { HighchartsStatic } from 'angular2-highcharts/dist/HighchartsService';

@NgModule({
  imports: [
    FormsModule,
    ChartModule,
    CommonModule,
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
    RouterModule,

  ],
  declarations: [
    DataQualityComponent,
    CrossValidComponent,
    MovableLoadComponent,
    BigDataComponent,
    CvPeComponent,
    CvPquiComponent,
    TimeVisComponent,
    ClusteringComponent,
    DqCompleteComponent,
    AllTlComponent,
    PopupComponent,
    UserAnalysisComponent,
    DataCalculationComponent,
    PrecisionMarketingUserComponent,
    PrecisionMarketingComponent
  ],
  providers: [{
    provide: HighchartsStatic,
    useFactory: highchartsFactory
  },
    BigDataService,
    UserAnalysisService,
    PrecisionMarketingService,
    PrecisionMarketingUserService,

  ]
})
export class DataAnalysisModule { }
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
