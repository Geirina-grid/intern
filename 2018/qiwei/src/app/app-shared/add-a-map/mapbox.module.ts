import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';

import { AddAMapComponent } from './component/add-a-map.component';
import { DrawLineComponent } from './component/draw-line.component';
import { DemodataService } from './services/demodata.service'

@NgModule({
  imports: [
    CommonModule
  ],
  providers: [DemodataService],
  declarations: [AddAMapComponent, DrawLineComponent],
  exports: [AddAMapComponent, DrawLineComponent]
})
export class MapboxModule { }