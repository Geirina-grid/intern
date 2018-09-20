import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common'
import { RouterModule } from '@angular/router'

import { MainInterfaceComponent } from './components/MainInterface/MainInterface.component';

import { NgxEchartsModule } from 'ngx-echarts';

@NgModule({
    imports: [

      NgxEchartsModule,
      RouterModule
    ],
  declarations: [
    MainInterfaceComponent
  ]
})

export class IESMainModule { }
