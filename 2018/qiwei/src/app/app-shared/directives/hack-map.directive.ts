import { Directive, OnInit } from '@angular/core';
import { LeafletDirective } from '@asymmetrik/ngx-leaflet';
import * as L from 'leaflet';

@Directive({
  selector: '[appHackMap]'
})
export class HackMapDirective implements OnInit {
  leaflefDirective: LeafletDirective;
  map: any;

  constructor(leafletDirective: LeafletDirective) {
    this.leaflefDirective = leafletDirective;
  }

  ngOnInit() {
    this.leaflefDirective.getMap();
    this.map = this.leaflefDirective.getMap();
    // this.map.invalidateSize();
  }
}
