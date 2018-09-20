import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { RetrieveCaseComponent } from './components/retrieve-case/retrieve-case.component';

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
import { RouterModule } from '@angular/router'

@NgModule({
  imports: [
    FormsModule,
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
  ],
  declarations: [
    RetrieveCaseComponent
  ]
})
export class RetrieveCaseModule { }
